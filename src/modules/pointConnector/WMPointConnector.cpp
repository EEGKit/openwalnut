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

#include "core/dataHandler/WDataSetPoints.h"
#include "core/dataHandler/WDataSetFibers.h"

#include "WMPointConnector.h"
#include "WMClickHandler.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMPointConnector )

WMPointConnector::WMPointConnector():
    WModule()
{
}

WMPointConnector::~WMPointConnector()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMPointConnector::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMPointConnector() );
}

const char** WMPointConnector::getXPMIcon() const
{
    return NULL;
}

const std::string WMPointConnector::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "Point Connector";
}

const std::string WMPointConnector::getDescription() const
{
    return "Allows user to manually connect points to fibers.";
}

void WMPointConnector::connectors()
{
    // this input contains the triangle data
    m_pointInput = WModuleInputData< WDataSetPoints >::createAndAdd( shared_from_this(), "points", "The data to display" );
    m_fiberOutput = WModuleOutputData< WDataSetFibers >::createAndAdd( shared_from_this(), "fibers", "The created fibers" );

    // call WModule's initialization
    WModule::connectors();
}

void WMPointConnector::properties()
{
    WPropertyBase::PropertyChangeNotifierType notifier = boost::bind( &WMPointConnector::updateProperty, this, boost::placeholders::_1 );

    // some properties need to trigger an update
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    // setup all the properties. See header file for their meaning and purpose.
    m_size = m_properties->addProperty( "Point Size", "The size of the points.", 0.25 );
    m_size->setMin( 0.0001 );
    m_size->setMax( 10.0 );

    m_useCorrectDepth = m_properties->addProperty( "Correct Depth",
                                                   "If set, the depths of the sprites are calculated correctly. "
                                                   "You can disable this to get higher framerates at the cost of visual correctness.",
                                                   true );

    m_possibleFiberSelections = WItemSelection::SPtr( new WItemSelection() );
    m_possibleFiberSelections->addItem( ItemType::create( "Line 1", "Line 1", "", NULL ) );
    m_fiberCount++;
    m_fiberSelection = m_properties->addProperty( "Selected Line",
                                                 "The line to which the points are added", m_possibleFiberSelections->getSelectorFirst(), notifier );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_fiberSelection );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_fiberSelection );

    m_addFiber = m_properties->addProperty( "Add Line", "Add Line", WPVBaseTypes::PV_TRIGGER_READY, notifier );

    // call WModule's initialization
    WModule::properties();
}

void WMPointConnector::moduleMain()
{
    // let the main loop awake if the data changes.
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_pointInput->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // setup the main graphics-node:
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    // create the post-processing node which actually does the nice stuff to the rendered image
    m_postNode = new WGEPostprocessingNode(
        WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getCamera()
    );
    m_postNode->addUpdateCallback( new WGENodeMaskCallback( m_active ) ); // disable the m_postNode with m_active
    // provide the properties of the post-processor to the user
    m_properties->addProperty( m_postNode->getProperties() );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_postNode );

    // load the GLSL shader:
    m_shader = new WGEShader( "WMPointConnector", m_localPath );
    // set geometry shader options
    m_shader->setParameter( GL_GEOMETRY_VERTICES_OUT_EXT, 4 );
    m_shader->setParameter( GL_GEOMETRY_INPUT_TYPE_EXT, GL_POINTS );
    m_shader->setParameter( GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP );

    // insert some uniforms and defines
    m_postNode->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropDouble >( "u_pointSize", m_size ) );
    m_shader->addPreprocessor( WGEShaderPreprocessor::SPtr(
        new WGEShaderPropertyDefineOptions< WPropBool >( m_useCorrectDepth, "DEPTHWRITE_DISABLED", "DEPTHWRITE_ENABLED" ) )
    );

    osg::ref_ptr<osgViewer::View> viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getView();
    osg::ref_ptr<WMClickHandler> handler = new WMClickHandler( this );
    viewer->addEventHandler( handler.get() );

    m_vertices = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    m_colors = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array );

    m_fibers = PCFiberListSPtr( new PCFiberList() );
    m_fibers->push_back( PCFiber() );
    m_selectedFiber = 0;

    // signal ready state. The module is now ready to be used.
    ready();

    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        // Now, the moduleState variable comes into play. The module can wait for the condition, which gets fired whenever the input receives data
        // or an property changes. The main loop now waits until something happens.
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish
        if( m_shutdownFlag() )
        {
            break;
        }

        // Get data and check for invalid data.
        WDataSetPoints::SPtr points = m_pointInput->getData();
        if( !points )
        {
            debugLog() << "Invalid Data. Disabling.";
            m_postNode->clear();
            continue;
        }

        // convert point arrays to osg vec3 arrays
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

    // it is important to always remove the modules again
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_postNode );
}

void WMPointConnector::redraw()
{
    osg::ref_ptr< osg::Geometry > geometry = osg::ref_ptr< osg::Geometry >( new osg::Geometry );
    osg::ref_ptr< osg::Geode > geode( new osg::Geode() );

    osg::StateSet* state = geode->getOrCreateStateSet();
    state->setMode( GL_BLEND, osg::StateAttribute::ON );

    // combine to geometry
    geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::POINTS, 0, m_vertices->size() ) );
    geometry->setVertexArray( m_vertices );
    geometry->setColorArray( m_colors );
    geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

    // add geometry to geode
    geode->addDrawable( geometry );

    // shader and colormapping
    m_shader->apply( geode );

    // add geode to group
    m_postNode->clear();
    m_postNode->insert( geode, m_shader );
}

void WMPointConnector::handleClick( osg::Vec3 cameraPosition, osg::Vec3 direction, bool isLeftClick )
{
    float distance = 0;
    osg::MixinVector<osg::Vec3f>::size_type hitIdx = 0;
    bool hasHit = false;

    for( osg::MixinVector<osg::Vec3f>::size_type idx = 0; idx < m_vertices->size(); idx++ )
    {
        osg::Vec3 vertex = m_vertices->at( idx );

        float hit = hitVertex( cameraPosition, direction, vertex, m_size->get() );
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
