//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS
// For more information see http://www.openwalnut.org/copying
//
// This file is part of OpenWalnut.
//
// OpenWalnut is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// OpenWalnut is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with OpenWalnut. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------

#include <string>
#include <vector>

#include <osg/Geode>

#include "core/common/math/WMath.h"
#include "core/common/WLimits.h"
#include "core/graphicsEngine/shaders/WGEShader.h"
#include "core/graphicsEngine/WGEColormapping.h"
#include "core/graphicsEngine/WGEGeodeUtils.h"
#include "core/graphicsEngine/WGEManagedGroupNode.h"
#include "core/graphicsEngine/postprocessing/WGEPostprocessingNode.h"
#include "core/graphicsEngine/WGEUtils.h"
#include "core/kernel/WKernel.h"

#include "WMPointConnector.h"
#include "WMClickHandler.h"

W_LOADABLE_MODULE( WMPointConnector )

WMPointConnector::WMPointConnector():
    WModuleContainer()
{
}

WMPointConnector::~WMPointConnector()
{
    removeConnectors();
}

boost::shared_ptr< WModule > WMPointConnector::factory() const
{
    return boost::shared_ptr< WModule >( new WMPointConnector() );
}

const char** WMPointConnector::getXPMIcon() const
{
    return NULL;
}

const std::string WMPointConnector::getName() const
{
    return "Point Connector";
}

const std::string WMPointConnector::getDescription() const
{
    return "Allows user to manually connect points to fibers.";
}

void WMPointConnector::connectors()
{
    m_pointInput = WModuleInputData< WDataSetPoints >::createAndAdd( shared_from_this(), "points in", "The data to display" );
    m_fiberOutput = WModuleOutputData< WDataSetFibers >::createAndAdd( shared_from_this(), "fibers out", "The created fibers" );

    m_pointOutput = WModuleOutputData< WDataSetPoints >::create( shared_from_this(), "points out", "The data that is passed internally" );

    WModule::connectors();
}

void WMPointConnector::properties()
{
    WPropertyBase::PropertyChangeNotifierType notifier = boost::bind( &WMPointConnector::updateProperty, this, boost::placeholders::_1 );

    m_possibleFiberSelections = WItemSelection::SPtr( new WItemSelection() );
    m_possibleFiberSelections->addItem( ItemType::create( "Line 1", "Line 1", "", NULL ) );
    m_fiberSelection = m_properties->addProperty(
        "Selected Line", "The line to which the points are added", m_possibleFiberSelections->getSelectorFirst(), notifier );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_fiberSelection );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_fiberSelection );

    m_fiberCount++;

    m_addFiber = m_properties->addProperty( "Add Line", "Add Line", WPVBaseTypes::PV_TRIGGER_READY, notifier );

    WModule::properties();
}

void WMPointConnector::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_pointInput->getDataChangedCondition() );

    m_pointRenderer = createAndAdd( "Point Renderer" );
    m_pointRenderer->isReady().wait();
    m_pointOutput->connect( m_pointRenderer->getInputConnector( "points" ) );
    m_properties->addProperty( m_pointRenderer->getProperties()->getProperty( "Point Size" ) );

    osg::ref_ptr<osgViewer::View> viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getView();
    osg::ref_ptr<WMClickHandler> handler = new WMClickHandler( this );
    viewer->addEventHandler( handler.get() );

    m_vertices = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    m_colors = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array );

    m_fibers = PCFiberListSPtr( new PCFiberList() );
    m_fibers->push_back( PCFiber() );
    m_selectedFiber = 0;

    ready();

    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        if( m_shutdownFlag() )
        {
            break;
        }

        WDataSetPoints::SPtr points = m_pointInput->getData();
        if( !points )
        {
            debugLog() << "Invalid Data. Disabling.";
            continue;
        }

        m_vertices = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
        m_colors = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array );

        WDataSetPoints::VertexArray pointVertices = points->getVertices();
        WDataSetPoints::ColorArray pointColors = points->getColors();
        for( size_t pointIdx = 0; pointIdx < points->size(); ++pointIdx )
        {
            osg::Vec3 vert = points->operator[]( pointIdx );
            osg::Vec4 color = points->getColor( pointIdx );

            m_vertices->push_back( vert );
            m_colors->push_back( color );
        }

        redraw();
    }

    stop();
}

void WMPointConnector::redraw()
{
    WDataSetPoints::VertexArray vertices( new std::vector< float > );
    WDataSetPoints::VertexArray colors( new std::vector< float > );

    for( size_t idx = 0; idx < m_vertices->size(); idx++ )
    {
        vertices->push_back( m_vertices->at( idx ).x() );
        vertices->push_back( m_vertices->at( idx ).y() );
        vertices->push_back( m_vertices->at( idx ).z() );

        colors->push_back( m_colors->at( idx ).x() );
        colors->push_back( m_colors->at( idx ).y() );
        colors->push_back( m_colors->at( idx ).z() );
        colors->push_back( m_colors->at( idx ).w() );
    }

    m_pointOutput->updateData( WDataSetPoints::SPtr( new WDataSetPoints( vertices, colors ) ) );
}

void WMPointConnector::handleClick( osg::Vec3 cameraPosition, osg::Vec3 direction, bool isLeftClick )
{
    float distance = 0;
    osg::MixinVector<osg::Vec3f>::size_type hitIdx = 0;
    bool hasHit = false;

    for( osg::MixinVector<osg::Vec3f>::size_type idx = 0; idx < m_vertices->size(); idx++ )
    {
        osg::Vec3 vertex = m_vertices->at( idx );

        float hit = hitVertex( cameraPosition, direction, vertex, 1.0 );
        if( hit > 0.0 )
        {
            float dis = ( vertex - cameraPosition ).length2();
            if( !hasHit )
            {
                hasHit = true;
                distance = dis;
                hitIdx = idx;
            }
            else if( dis < distance )
            {
                distance = dis;
                hitIdx = idx;
            }
        }
    }

    if( hasHit )
    {
        if( isLeftClick )
        {
            if( m_hasSelected )
            {
                m_colors->operator[]( m_selectedIndex ) = m_selectedOldColor;
            }

            m_selectedOldColor = m_colors->operator[]( hitIdx );
            m_selectedIndex = hitIdx;
            m_colors->operator[]( hitIdx ) = osg::Vec4( 0.0, 1.0, 0.0, 1.0 );
            m_hasSelected = true;

            m_fibers->at( m_selectedFiber ).push_back( m_vertices->at( hitIdx ) );
        }
        else
        {
            auto fiber = m_fibers->begin() + m_selectedFiber;
            osg::Vec3 vertex = m_vertices->at( hitIdx );
            for( auto it = fiber->begin(); it != fiber->end(); )
            {
                if( *it == vertex )
                {
                    fiber->erase( it );
                }
                else
                {
                    it++;
                }
            }

            if( m_hasSelected )
            {
                m_colors->operator[]( m_selectedIndex ) = m_selectedOldColor;
                m_hasSelected = false;
            }

            if( !fiber->empty() )
            {
                std::vector< osg::Vec3 >::iterator vertexIterator = std::find( m_vertices->begin(), m_vertices->end(), fiber->at( fiber->size() - 1 ) );
                size_t vIdx = std::distance( m_vertices->begin(), vertexIterator );

                m_selectedOldColor = m_colors->operator[]( vIdx );
                m_selectedIndex = vIdx;
                m_colors->operator[]( vIdx ) = osg::Vec4( 0.0, 1.0, 0.0, 1.0 );
                m_hasSelected = true;
            }

        }

        redraw();
        updateOutput();
    }
}

float WMPointConnector::hitVertex( osg::Vec3 rayStart, osg::Vec3 rayDir, osg::Vec3 vertex, float radius )
{
    osg::Vec3 oc = rayStart - vertex;
    float a = rayDir * rayDir;
    float b = 2.0 * ( oc * rayDir );
    float c = ( oc * oc ) - radius * radius;
    float discriminant = b * b - 4 * a * c;

    if( discriminant < 0.0 )
    {
        return -1.0;
    }
    else if( discriminant == 0.0 )
    {
        return 0.0;
    }
    else
    {
        float numerator = -b - sqrt( discriminant );
        if( numerator > 0.0 )
        {
            return numerator / ( 2.0 * a );
        }

        numerator = -b + sqrt( discriminant );
        if( numerator > 0.0 )
        {
            return numerator / ( 2.0 * a );
        }
        else
        {
            return -1.0;
        }
    }
}

void WMPointConnector::updateProperty( WPropertyBase::SPtr property )
{
    if(property == m_addFiber && m_addFiber->get(true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
    {
        m_addFiber->set( WPVBaseTypes::PV_TRIGGER_READY, false );

        m_fiberCount++;
        std::string name = "Line " + boost::lexical_cast< std::string >( m_fiberCount );

        m_fibers->push_back( PCFiber() );
        m_possibleFiberSelections->addItem( ItemType::create( name, name, "", NULL ) );
        m_fiberSelection->set( m_possibleFiberSelections->getSelectorLast() );
    }
    if(property == m_fiberSelection )
    {
        m_selectedFiber = m_fiberSelection->get().getItemIndexOfSelected( 0 );
        if( m_hasSelected )
        {
            m_colors->operator[]( m_selectedIndex ) = m_selectedOldColor;
            m_hasSelected = false;
        }

        PCFiber fiber = m_fibers->at( m_selectedFiber );
        if( !fiber.empty() )
        {
            std::vector< osg::Vec3 >::iterator vertexIterator = std::find( m_vertices->begin(), m_vertices->end(), fiber[fiber.size() - 1] );
            size_t vIdx = std::distance( m_vertices->begin(), vertexIterator );

            m_selectedOldColor = m_colors->operator[]( vIdx );
            m_selectedIndex = vIdx;
            m_colors->operator[]( vIdx ) = osg::Vec4( 0.0, 1.0, 0.0, 1.0 );
            m_hasSelected = true;
        }

        redraw();
    }
}

void WMPointConnector::updateOutput()
{
    boost::shared_ptr< std::vector< float > > vertices = boost::shared_ptr< std::vector< float > >( new std::vector< float >() );
    boost::shared_ptr< std::vector< size_t > > lineStartIndexes = boost::shared_ptr< std::vector< size_t > >( new std::vector< size_t >() );
    boost::shared_ptr< std::vector< size_t > > lineLength = boost::shared_ptr< std::vector< size_t > >( new std::vector< size_t >() );
    boost::shared_ptr< std::vector< size_t > > verticesReverse = boost::shared_ptr< std::vector< size_t > >( new std::vector< size_t >() );

    for( PCFiberList::size_type idx = 0; idx < m_fibers->size(); idx++ )
    {
        PCFiber fiber = m_fibers->at( idx );
        lineStartIndexes->push_back( vertices->size() / 3 );
        lineLength->push_back( fiber.size() );

        for( PCFiber::size_type vIdx = 0; vIdx < fiber.size(); vIdx++ )
        {
            osg::Vec3 vertex = fiber[vIdx];
            vertices->push_back( vertex.x() );
            vertices->push_back( vertex.y() );
            vertices->push_back( vertex.z() );

            verticesReverse->push_back( idx );
        }
    }

    m_fiberOutput->updateData( boost::shared_ptr< WDataSetFibers >(
        new WDataSetFibers(
            vertices,
            lineStartIndexes,
            lineLength,
            verticesReverse
        )
    ) );
}
