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

#define _USE_MATH_DEFINES

#include <cmath>
#include <memory>
#include <string>
#include <vector>

#include "WAngleHelper.h"
#include "WConnectorData.h"
#include "WFiberHandler.h"
#include "WKeyboardHandler.h"
#include "WMPointConnector.h"

static const osg::Vec4 COLOR_SEL_POINT( 255.0 / 255.0, 190.0 / 255.0,   7.0 / 255.0, 1.0 );
static const osg::Vec4 COLOR_SEL_FIBER(  30.0 / 255.0, 136.0 / 255.0, 229.0 / 255.0, 1.0 );


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

std::shared_ptr< WModule > WMPointConnector::factory() const
{
    return std::shared_ptr< WModule >( new WMPointConnector() );
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

    m_pointOutput = WModuleOutputData< WDataSetPoints >::create( shared_from_this(), "points out", "The data that is passed internally" );
    m_fiberOutput = WModuleOutputData< WDataSetFibers >::createAndAdd( shared_from_this(), "fibers out", "The created fibers" );

    WModule::connectors();
}

void WMPointConnector::properties()
{
    m_fiberHandler->createProperties( m_properties );

    WPropertyGroup::SPtr assistanceGroup = m_properties->addPropertyGroup( "Assistance", "Property group assistance features." );

    m_enableSAPT = assistanceGroup->addProperty( "Enable SAPT", "Enable Semi-Automatic-Proton-Tracking", true );
    m_enableSizes = assistanceGroup->addProperty( "Enable sizes", "Enables the point size based on the cluster size", false,
                                                   boost::bind( &WMPointConnector::updateAll, this ) );
    m_enableAdaptiveVisibility = assistanceGroup->addProperty( "Enable adaptive visibility", "Enable adaptive visibility using a cone", true,
                                                                boost::bind( &WMPointConnector::updateAll, this ) );
    m_adaptiveVisibilityAngle = assistanceGroup->addProperty( "Adaptive visibility angle", "Adaptive visibility angle", 10.0,
                                                               boost::bind( &WMPointConnector::updateAll, this ) );
    m_adaptiveVisibilityAngle->setMin( 0.0 );
    m_adaptiveVisibilityAngle->setMax( 90.0 );

    m_hiddenOpacity = assistanceGroup->addProperty( "Hidden point opacity", "Changes the opacity of the hidden points", 0.1,
                                                     boost::bind( &WMPointConnector::updateAll, this ) );
    m_hiddenOpacity->setMin( 0.0 );
    m_hiddenOpacity->setMax( 1.0 );

    m_scaling = assistanceGroup->addProperty( "Scaling", "Changes the scaling", WPosition( 1.0, 1.0, 1.0 ),
                                               boost::bind( &WMPointConnector::updateAll, this ) );

    WModule::properties();
}

void WMPointConnector::moduleMain()
{
    createHandler();

    m_onscreenSelection = std::shared_ptr< WOnscreenSelection >( new WOnscreenSelection() );
    m_onscreenSelection->setOnend( boost::bind( &WMPointConnector::selectionEnd, this,
                                   boost::placeholders::_1, boost::placeholders::_2, boost::placeholders::_3 ) );

    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_pointInput->getDataChangedCondition() );

    createPointRenderer();
    createFiberDisplay();

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

void WMPointConnector::activate()
{
    m_pointRenderer->getProperties()->getProperty( "active" )->toPropBool()->set( m_active->get() );
    m_fiberDisplay->getProperties()->getProperty( "active" )->toPropBool()->set( m_active->get() );
}

void WMPointConnector::createPointRenderer()
{
    WPropertyGroup::SPtr pointGroup = m_properties->addPropertyGroup( "Point Renderer", "Properties passed through from the point renderer" );

    m_pointRenderer = createAndAdd( "Point Renderer" );
    m_pointRenderer->isReady().wait();
    m_pointOutput->connect( m_pointRenderer->getInputConnector( "points" ) );

    pointGroup->addProperty( "Activate", "Activates the point renderer", true,
        boost::bind( &WMPointConnector::toggleActivationOfModule, this, m_pointRenderer ) );

    pointGroup->addProperty( m_pointRenderer->getProperties()->getProperty( "Point size" ) );
}

void WMPointConnector::createFiberDisplay()
{
    WPropertyGroup::SPtr fiberGroup = m_properties->addPropertyGroup( "Fiber Display", "Properties passed through from the fiber display" );

    m_fiberDisplay = createAndAdd( "Fiber Display" );
    m_fiberDisplay->isReady().wait();
    m_fiberOutput->connect( m_fiberDisplay->getInputConnector( "fibers" ) );

    fiberGroup->addProperty( "Activate", "Activates the fiber display", true,
        boost::bind( &WMPointConnector::toggleActivationOfModule, this, m_fiberDisplay ) );

    fiberGroup->addProperty( m_fiberDisplay->getProperties()->getProperty( "Line Rendering" )->toPropGroup()->getProperty( "Line width" ) );
}

void WMPointConnector::toggleActivationOfModule( WModule::SPtr mod )
{
    WPropBool active = mod->getProperties()->getProperty( "active" )->toPropBool();
    active->set( !active->get() );
}

void WMPointConnector::createHandler()
{
    osg::ref_ptr< osgViewer::View > viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getView();

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

    SPFloatVector edeps;
    SPSizeVector eventIDs;
    std::shared_ptr< WValueSet< float > > clusterSizes;

    if( points->getData().type() == typeid( std::tuple< SPFloatVector > ) )
    {
        edeps = std::get< 0 >( std::any_cast< std::tuple< SPFloatVector > >( points->getData() ) );
    }
    else if( points->getData().type() == typeid( std::tuple< SPSizeVector > ) )
    {
        eventIDs = std::get< 0 >( std::any_cast< std::tuple< SPSizeVector > >( points->getData() ) );
    }
    else if( points->getData().type() == typeid( std::tuple< SPFloatVector, SPSizeVector > ) )
    {
        edeps = std::get< 0 >( std::any_cast< std::tuple< SPFloatVector, SPSizeVector > >( points->getData() ) );
        eventIDs = std::get< 1 >( std::any_cast< std::tuple< SPFloatVector, SPSizeVector > >( points->getData() ) );
    }
    else if( points->getData().type() == typeid( std::shared_ptr< WValueSet< float > > ) )
    {
        clusterSizes = std::any_cast< std::shared_ptr< WValueSet< float > > >( points->getData() );
    }

    m_connectorData->clear();

    m_fiberHandler->clear();

    WFiberHandler::PCFiberListSPtr fibers = m_fiberHandler->getFibers();

    for( size_t pointIdx = 0; pointIdx < points->size(); ++pointIdx )
    {
        osg::Vec3 vertex = points->operator[]( pointIdx );
        m_connectorData->addVertex( vertex,
                                    points->getColor( pointIdx ),
                                    edeps ? edeps->at( pointIdx ) : 0.0,
                                    clusterSizes ? clusterSizes->getScalar( pointIdx ) : 0.0 );

        if( eventIDs )
        {
            size_t eventID = eventIDs->at( pointIdx );

            while( fibers->size() <= eventID )
            {
                m_fiberHandler->addFiber( "Track " + boost::lexical_cast< std::string >( fibers->size() ), true, false );
            }

            ( fibers->begin() + eventID )->push_back( vertex );
        }
    }

    for( size_t idx = fibers->size() - 1; idx > 0; idx-- )
    {
        if( fibers->at( idx ).size() <= 1 )
        {
            m_fiberHandler->removeFiber( idx, true, false );
        }
    }

    m_fiberHandler->setFiberCount( fibers->size() );
    m_fiberHandler->selectorUpdate( fibers->size() - 1 );

    m_fiberHandler->sortVertices();

    if( clusterSizes )
    {
        m_connectorData->setHasClusterSize();
    }

    updateAll();
}

void WMPointConnector::updateAll()
{
    updatePoints();
    updateOutput();
}

void WMPointConnector::updatePoints()
{
    if( m_pointRenderer == NULL )
    {
        return;
    }

    if( m_connectorData->getVertices()->size() == 0 )
    {
        m_pointOutput->updateData( NULL );
        return;
    }

    WDataSetPoints::VertexArray vertices( new std::vector< float > );
    WDataSetPoints::VertexArray colors( new std::vector< float > );

    for( size_t idx = 0; idx < m_connectorData->getVertices()->size(); idx++ )
    {
        osg::Vec3 vertex = m_connectorData->getVertices()->at( idx );
        osg::Vec4 color( 1.0, 1.0, 1.0, 1.0 );

        vertices->push_back( vertex.x() * m_scaling->get().x() );
        vertices->push_back( vertex.y() * m_scaling->get().y() );
        vertices->push_back( vertex.z() * m_scaling->get().z() );

        size_t tmpIdx;
        if( m_connectorData->getSelectedPoint( &tmpIdx ) && tmpIdx == idx )
        {
            color = COLOR_SEL_POINT;
        }
        else if( m_fiberHandler->getFiberOfPoint( vertex, &tmpIdx ) && m_fiberHandler->getSelectedFiber() == tmpIdx )
        {
            color = COLOR_SEL_FIBER;
        }
        else if( m_fiberHandler->isPointHidden( vertex ) || isAdaptivelyHidden( vertex ) )
        {
            color[3] = m_hiddenOpacity->get();
        }

        colors->push_back( color.x() );
        colors->push_back( color.y() );
        colors->push_back( color.z() );
        colors->push_back( color.w() );
    }

    std::any data = NULL;
    if( m_connectorData->hasClusterSize() && m_enableSizes->get() )
    {
        data = std::shared_ptr< WValueSet< float > >( new WValueSet< float >( 0, 1, m_connectorData->getClusterSizes() ) );
    }

    m_pointOutput->updateData( WDataSetPoints::SPtr( new WDataSetPoints( vertices, colors, data ) ) );
}

bool WMPointConnector::isAdaptivelyHidden( osg::Vec3 vertex )
{
    if( !m_enableAdaptiveVisibility->get() )
    {
        return false;
    }

    size_t verIdx = 0;
    if( !m_connectorData->getSelectedPoint( &verIdx ) )
    {
        return false;
    }

    osg::Vec3 selected = m_connectorData->getVertices()->at( verIdx );
    WFiberHandler::PCFiber fiber = m_fiberHandler->getFibers()->at( m_fiberHandler->getSelectedFiber() );
    auto it = std::find( fiber.begin(), fiber.end(), selected );
    osg::Vec3 before = osg::Vec3( 0.0, 0.0, 1.0 );
    if( it != fiber.end() && it != fiber.begin() )
    {
        before = *( --it );
    }

    double angle = WAngleHelper::calculateAngle( selected - before, vertex - selected );

    double layerDiff = abs( selected.z() - vertex.z() ) / 5.5; // 5.5 is the general layer thickness

    double checkAngle = m_adaptiveVisibilityAngle->get() * layerDiff;

    return angle > checkAngle && angle < ( 180.0 - checkAngle );
}

void WMPointConnector::handleClick( osg::Vec3 cameraPosition, osg::Vec3 direction, bool isLeftClick )
{
    size_t hitIdx = 0;

    if( findClickedPoint( cameraPosition, direction, &hitIdx ) )
    {
        if( isLeftClick )
        {
            size_t fiberIdx = 0;
            if( m_fiberHandler->getFiberOfPoint( m_connectorData->getVertices()->at( hitIdx ), &fiberIdx ) )
            {
                if( m_fiberHandler->getSelectedFiber() == fiberIdx )
                {
                    m_connectorData->selectPoint( hitIdx );
                }
                else
                {
                    m_fiberHandler->selectFiber( fiberIdx );
                    m_fiberHandler->selectorUpdate( fiberIdx );
                }
            }
            else
            {
                m_connectorData->deselectPoint();
                m_connectorData->selectPoint( hitIdx );
                m_fiberHandler->addVertexToFiber( m_connectorData->getVertices()->at( hitIdx ), m_fiberHandler->getSelectedFiber() );
            }
        }
        else
        {
            m_connectorData->deselectPoint();
            m_fiberHandler->removeVertexFromFiber( m_connectorData->getVertices()->at( hitIdx ), m_fiberHandler->getSelectedFiber() );
            m_fiberHandler->selectLastPoint();
        }

        updateAll();
    }
}


bool WMPointConnector::findClickedPoint( osg::Vec3 cameraPosition, osg::Vec3 direction, size_t* hitIdx )
{
    bool hasHit = false;
    float distance = 0;
    float size = m_pointRenderer->getProperties()->getProperty( "Point size" )->toPropDouble()->get();
    for( size_t idx = 0; idx < m_connectorData->getVertices()->size(); idx++ )
    {
        osg::Vec3 vertex = m_connectorData->getVertices()->at( idx );
        vertex = osg::Vec3( vertex.x() * m_scaling->get().x(), vertex.y() * m_scaling->get().y(), vertex.z() * m_scaling->get().z() );
        float localSize = size;
        if( m_connectorData->hasClusterSize() && m_enableSizes->get() )
        {
            localSize *= m_connectorData->getClusterSizes()->at( idx );
        }

        float hit = hitVertex( cameraPosition, direction, vertex, localSize );
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
    if( m_fiberDisplay == NULL )
    {
        return;
    }

    std::shared_ptr< std::vector< float > > vertices = std::shared_ptr< std::vector< float > >( new std::vector< float >() );
    std::shared_ptr< std::vector< float > > colors = std::shared_ptr< std::vector< float > >( new std::vector< float >() );
    std::shared_ptr< std::vector< size_t > > lineStartIndexes = std::shared_ptr< std::vector< size_t > >( new std::vector< size_t >() );
    std::shared_ptr< std::vector< size_t > > lineLength = std::shared_ptr< std::vector< size_t > >( new std::vector< size_t >() );
    std::shared_ptr< std::vector< size_t > > verticesReverse = std::shared_ptr< std::vector< size_t > >( new std::vector< size_t >() );

    for( size_t idx = 0; idx < m_fiberHandler->getFibers()->size(); idx++ )
    {
        WFiberHandler::PCFiber fiber = m_fiberHandler->getFibers()->at( idx );
        if( fiber.size() < 2 )
        {
            continue;
        }

        lineStartIndexes->push_back( vertices->size() / 3 );
        lineLength->push_back( fiber.size() );

        osg::Vec4 color( 0.0, 0.0, 0.0, 1.0 );
        if( m_fiberHandler->getSelectedFiber() == idx )
        {
            color = COLOR_SEL_FIBER;
        }

        if( m_fiberHandler->isHidden( idx ) )
        {
            color[3] = m_hiddenOpacity->get();
        }

        for( size_t vIdx = 0; vIdx < fiber.size(); vIdx++ )
        {
            osg::Vec3 vertex = fiber[vIdx];
            vertices->push_back( vertex.x() * m_scaling->get().x() );
            vertices->push_back( vertex.y() * m_scaling->get().y() );
            vertices->push_back( vertex.z() * m_scaling->get().z() );

            colors->push_back( color.x() );
            colors->push_back( color.y() );
            colors->push_back( color.z() );
            colors->push_back( color.w() );

            verticesReverse->push_back( idx );
        }
    }

    WDataSetFibers::SPtr fibers(
        new WDataSetFibers(
            vertices,
            lineStartIndexes,
            lineLength,
            verticesReverse
        )
    );

    if( vertices->size() == 0 )
    {
        m_fiberOutput->updateData( NULL );
        return;
    }

    fibers->addColorScheme( colors, "Connection", "Color fibers based on their connection." );
    fibers->setSelectedColorScheme( 3 );
    m_fiberOutput->updateData( fibers );
}

WConnectorData::SPtr WMPointConnector::getConnectorData()
{
    return m_connectorData;
}

WFiberHandler::SPtr WMPointConnector::getFiberHandler()
{
    return m_fiberHandler;
}

void WMPointConnector::selectionEnd( WOnscreenSelection::WSelectionType, float x, float y )
{
    // TODO(eschbach): maybe clean up this method. It is pretty long
    if( !m_onscreenSelection->hasMoved() )
    {
        // no movement do raycast.
        float mouseX = x * 2.0 - 1.0;
        float mouseY = y * 2.0 - 1.0;

        osg::Camera* camera = WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getCamera();
        osg::Matrix VP = camera->getViewMatrix() * camera->getProjectionMatrix();

        osg::Matrix inverseVP;
        inverseVP.invert( VP );

        osg::Vec3 nearPoint( mouseX, mouseY, -1.0f );
        osg::Vec3 farPoint( mouseX, mouseY, 1.0f );
        nearPoint = nearPoint * inverseVP;
        farPoint = farPoint * inverseVP;

        osg::Vec3 direction = farPoint - nearPoint;
        direction.normalize();

        handleClick( nearPoint, direction, m_onscreenSelection->getClickType() );
        return;
    }

    std::vector< WPosition > toCheck;
    for( size_t idx = 0; idx < m_connectorData->getVertices()->size(); idx++ )
    {
        osg::Vec3 vertex = m_connectorData->getVertices()->at( idx );
        toCheck.push_back( WPosition( vertex.x() * m_scaling->get().x(), vertex.y() * m_scaling->get().y(), vertex.z() * m_scaling->get().z() ) );
    }

    toCheck = m_onscreenSelection->isSelected( toCheck );
    if( toCheck.empty() )
    {
        return;
    }

    std::vector< WPosition > positions;
    for( size_t idx = 0; idx < toCheck.size(); idx++ )
    {
        WPosition vertex = toCheck.at( idx );
        vertex = WPosition( vertex.x() / m_scaling->get().x(), vertex.y() / m_scaling->get().y(), vertex.z() / m_scaling->get().z() );
        positions.push_back( vertex );
    }

    if( !m_onscreenSelection->getClickType() ) // right click delete
    {
        size_t idx = 0;
        size_t fibIdx = m_fiberHandler->getSelectedFiber();
        for( auto vertex = positions.begin(); vertex != positions.end(); )
        {
            if( m_fiberHandler->getFiberOfPoint( *vertex, &idx ) && idx == fibIdx )
            {
                vertex++;
            }
            else
            {
                positions.erase( vertex );
            }
        }

        if( positions.empty() )
        {
            return;
        }
        m_connectorData->deselectPoint();
        m_fiberHandler->removeVerticesFromFiber( std::vector< osg::Vec3 >( positions.begin(), positions.end() ), fibIdx );
        m_fiberHandler->selectLastPoint();
    }
    else // left click add
    {
        for( auto vertex = positions.begin(); vertex != positions.end(); )
        {
            if( m_fiberHandler->getFiberOfPoint( *vertex ) || m_fiberHandler->isPointHidden( *vertex ) || isAdaptivelyHidden( *vertex ) )
            {
                positions.erase( vertex );
            }
            else
            {
                vertex++;
            }
        }
        if( m_enableSAPT->get() )
        {
            positions = WAngleHelper::findSmoothestPath( positions, m_fiberHandler->getFibers()->at( m_fiberHandler->getSelectedFiber() ) );
        }

        if( positions.empty() )
        {
            return;
        }
        m_connectorData->deselectPoint();
        m_fiberHandler->addVerticesToFiber( std::vector< osg::Vec3 >( positions.begin(), positions.end() ), m_fiberHandler->getSelectedFiber() );
        m_fiberHandler->selectLastPoint();
    }

    updateAll();
}

std::shared_ptr< WOnscreenSelection > WMPointConnector::getOnscreenSelection()
{
    return m_onscreenSelection;
}

WPropPosition WMPointConnector::getScaling()
{
    return m_scaling;
}
