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

#include "WConnectorData.h"
#include "WClickHandler.h"
#include "WKeyboardHandler.h"
#include "WFiberHandler.h"

#include "WMPointConnector.h"

W_LOADABLE_MODULE( WMPointConnector )

WMPointConnector::WMPointConnector():
    WModuleContainer()
{
    m_connectorData = WConnectorData::SPtr( new WConnectorData() );
    m_fiberHandler = WFiberHandler::SPtr( new WFiberHandler( this ) );
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
    m_fiberHandler->createProperties( m_properties );
    WModule::properties();
}

void WMPointConnector::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_pointInput->getDataChangedCondition() );

    createPointRenderer();
    createHandler();

    ready();

    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        if( m_shutdownFlag() )
        {
            break;
        }

        handleInput();
    }

    stop();
}

void WMPointConnector::createPointRenderer()
{
    m_pointRenderer = createAndAdd( "Point Renderer" );
    m_pointRenderer->isReady().wait();
    m_pointOutput->connect( m_pointRenderer->getInputConnector( "points" ) );
    m_properties->addProperty( m_pointRenderer->getProperties()->getProperty( "Point Size" ) );
}

void WMPointConnector::createHandler()
{
    osg::ref_ptr< osgViewer::View > viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getView();
    osg::ref_ptr< WClickHandler > clickHandler = new WClickHandler( this );
    viewer->addEventHandler( clickHandler.get() );

    osg::ref_ptr< WKeyboardHandler > keyboardHandler = new WKeyboardHandler( this );
    viewer->addEventHandler( keyboardHandler.get() );
}

void WMPointConnector::handleInput()
{
    WDataSetPoints::SPtr points = m_pointInput->getData();
    if( !points )
    {
        debugLog() << "Invalid Data. Break.";
        return;
    }

    m_connectorData->clear();

    for( size_t pointIdx = 0; pointIdx < points->size(); ++pointIdx )
    {
        m_connectorData->addVertex( points->operator[]( pointIdx ), points->getColor( pointIdx ) );
    }

    updatePoints();
}

void WMPointConnector::updatePoints()
{
    WDataSetPoints::VertexArray vertices( new std::vector< float > );
    WDataSetPoints::VertexArray colors( new std::vector< float > );

    for( size_t idx = 0; idx < m_connectorData->getVertices()->size(); idx++ )
    {
        osg::Vec3 vertex = m_connectorData->getVertices()->at( idx );
        osg::Vec4 color = m_connectorData->getColors()->at( idx );

        vertices->push_back( vertex.x() );
        vertices->push_back( vertex.y() );
        vertices->push_back( vertex.z() );

        colors->push_back( color.x() );
        colors->push_back( color.y() );
        colors->push_back( color.z() );
        colors->push_back( color.w() );
    }

    m_pointOutput->updateData( WDataSetPoints::SPtr( new WDataSetPoints( vertices, colors ) ) );
}

void WMPointConnector::handleClick( osg::Vec3 cameraPosition, osg::Vec3 direction, bool isLeftClick )
{
    size_t hitIdx = 0;

    if( findClickedPoint( cameraPosition, direction, &hitIdx ) )
    {
        if( isLeftClick )
        {
            m_connectorData->deselectPoint();
            m_connectorData->selectPoint( hitIdx );
            m_fiberHandler->addVertexToFiber( m_connectorData->getVertices()->at( hitIdx ), m_fiberHandler->getSelectedFiber() );
        }
        else
        {
            m_connectorData->deselectPoint();
            m_fiberHandler->removeVertexFromFiber( m_connectorData->getVertices()->at( hitIdx ), m_fiberHandler->getSelectedFiber() );
            m_fiberHandler->selectLastPoint();
        }

        updatePoints();
        updateOutput();
    }
}


bool WMPointConnector::findClickedPoint( osg::Vec3 cameraPosition, osg::Vec3 direction, size_t* hitIdx )
{
    bool hasHit = false;
    float distance = 0;
    float size = m_pointRenderer->getProperties()->getProperty( "Point Size" )->toPropDouble()->get();
    for( size_t idx = 0; idx < m_connectorData->getVertices()->size(); idx++ )
    {
        osg::Vec3 vertex = m_connectorData->getVertices()->at( idx );

        float hit = hitVertex( cameraPosition, direction, vertex, size );
        if( hit > 0.0 )
        {
            float dis = ( vertex - cameraPosition ).length2();
            if( !hasHit )
            {
                hasHit = true;
                distance = dis;
                *hitIdx = idx;
            }
            else if( dis < distance )
            {
                distance = dis;
                *hitIdx = idx;
            }
        }
    }

    return hasHit;
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
        float sqrtDis = sqrt( discriminant );
        float numerator = -b - sqrtDis;
        if( numerator > 0.0 )
        {
            return numerator / ( 2.0 * a );
        }

        numerator = -b + sqrtDis;
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

void WMPointConnector::updateOutput()
{
    boost::shared_ptr< std::vector< float > > vertices = boost::shared_ptr< std::vector< float > >( new std::vector< float >() );
    boost::shared_ptr< std::vector< size_t > > lineStartIndexes = boost::shared_ptr< std::vector< size_t > >( new std::vector< size_t >() );
    boost::shared_ptr< std::vector< size_t > > lineLength = boost::shared_ptr< std::vector< size_t > >( new std::vector< size_t >() );
    boost::shared_ptr< std::vector< size_t > > verticesReverse = boost::shared_ptr< std::vector< size_t > >( new std::vector< size_t >() );

    for( size_t idx = 0; idx < m_fiberHandler->getFibers()->size(); idx++ )
    {
        WFiberHandler::PCFiber fiber = m_fiberHandler->getFibers()->at( idx );
        lineStartIndexes->push_back( vertices->size() / 3 );
        lineLength->push_back( fiber.size() );

        for( size_t vIdx = 0; vIdx < fiber.size(); vIdx++ )
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


WConnectorData::SPtr WMPointConnector::getConnectorData()
{
    return m_connectorData;
}

WFiberHandler::SPtr WMPointConnector::getFiberHandler()
{
    return m_fiberHandler;
}
