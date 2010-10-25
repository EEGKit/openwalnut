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

#include <cstdlib>
#include <cmath>

#include <list>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Texture1D>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include "../../../common/WColor.h"
#include "../../../common/WLogger.h"
#include "../../../common/WPathHelper.h"
#include "../../../dataHandler/WDataHandler.h"
#include "../../../dataHandler/WDataTexture3D.h"
#include "../../../dataHandler/WSubject.h"
#include "../../../graphicsEngine/WGEUtils.h"
#include "../../../kernel/WKernel.h"
#include "fiberdisplay.xpm"
#include "WMFiberDisplay.h"

#define PI 3.141592653589793

bool WMFiberDisplay::m_fiberDisplayRunning = false;

WMFiberDisplay::WMFiberDisplay()
    : WModule(),
      m_noData( new WCondition, true ),
      m_osgNode( osg::ref_ptr< osg::Switch >() )
{
    m_shaderTubes = osg::ref_ptr< WShader > ( new WShader( "WMFiberDisplay-FakeTubes" ) );
    m_shaderTubesPS = osg::ref_ptr< WShader > ( new WShader( "WMFiberDisplay-FakeTubes-PS" ) );
    m_shaderTubesQS = osg::ref_ptr< WShader > ( new WShader( "WMFiberDisplay-FakeTubes-QS" ) );
    m_shaderTexturedFibers = osg::ref_ptr< WShader > ( new WShader( "WMFiberDisplay-Textured" ) );
    m_textureChanged = true;
}

WMFiberDisplay::~WMFiberDisplay()
{
    m_fiberDisplayRunning = false;
}

boost::shared_ptr< WModule > WMFiberDisplay::factory() const
{
    m_fiberDisplayRunning = true;
    return boost::shared_ptr< WModule >( new WMFiberDisplay() );
}

bool WMFiberDisplay::isRunning()
{
    return m_fiberDisplayRunning;
}

const char** WMFiberDisplay::getXPMIcon() const
{
    return fiberdisplay_xpm;
}

void WMFiberDisplay::moduleMain()
{
    // additional fire-condition: "data changed" flag
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_fiberInput->getDataChangedCondition() );

    // now, to watch changing/new textures use WSubject's change condition
    boost::signals2::connection con = WDataHandler::getDefaultSubject()->getChangeCondition()->subscribeSignal(
            boost::bind( &WMFiberDisplay::notifyTextureChange, this )
    );

    ready();

    while ( !m_shutdownFlag() ) // loop until the module container requests the module to quit
    {
        debugLog() << "Waiting for event";

        m_moduleState.wait(); // waits for firing of m_moduleState ( dataChanged, shutdown, etc. )

        if ( m_shutdownFlag() )
        {
            break;
        }

        initCullBox();

        /////////////////////////////////////////////////////////////////////////////////////////
        // what caused wait to return?
        /////////////////////////////////////////////////////////////////////////////////////////

        // data changed?
        if ( m_dataset != m_fiberInput->getData() )
        {
            // data has changed
            // -> recalculate
            debugLog() << "Data changed on " << m_fiberInput->getCanonicalName();

            m_dataset = m_fiberInput->getData();

            // ensure the data is valid (not NULL)
            if ( !m_fiberInput->getData().get() ) // ok, the output has been reset, so we can ignore the "data change"
            {
                m_noData.set( true );
                debugLog() << "Data reset on " << m_fiberInput->getCanonicalName() << ". Ignoring.";
                continue;
            }

            m_noData.set( false );

            infoLog() << "Fiber dataset for display with: " << m_dataset->size() << " fibers loaded.";

            if( m_dataset->size() != 0 ) // incase of an empty fiber dataset nothing is to display
            {
                boost::shared_ptr< WProgress > progress = boost::shared_ptr< WProgress >( new WProgress( "Fiber Display", 2 ) );
                m_progress->addSubProgress( progress );

                WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->removeChild( m_osgNode.get() );
                ++*progress;
                WKernel::getRunningKernel()->getRoiManager()->addFiberDataset( m_dataset );
                ++*progress;
                create();
                progress->finish();
            }
            else
            {
                warnLog() << "Nothing to display for an empty fiber dataset";
            }
        }
    }

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_osgNode );
}

void WMFiberDisplay::update()
{
    if( m_noData.changed() )
    {
        if( m_osgNode && m_noData.get( true ) )
        {
            m_osgNode->setNodeMask( 0x0 );
        }
        else
        {
            m_osgNode->setNodeMask( 0xFFFFFFFF );
        }
    }

    if( !m_showCullBox->get() )
    {
        m_cullBox->setNodeMask( 0x0 );
    }
    else
    {
        m_cullBox->setNodeMask( 0xFFFFFFFF );
    }

    float xMin = m_cullBox->getMinPos()[0];
    float yMin = m_cullBox->getMinPos()[1];
    float zMin = m_cullBox->getMinPos()[2];
    float xMax = m_cullBox->getMaxPos()[0];
    float yMax = m_cullBox->getMaxPos()[1];
    float zMax = m_cullBox->getMaxPos()[2];

    m_uniformUseCullBox->set( m_activateCullBox->get() );
    m_uniformInsideCullBox->set( m_insideCullBox->get() );

    m_uniformCullBoxLBX->set( static_cast<float>( xMin ) );
    m_uniformCullBoxLBY->set( static_cast<float>( yMin ) );
    m_uniformCullBoxLBZ->set( static_cast<float>( zMin ) );
    m_uniformCullBoxUBX->set( static_cast<float>( xMax ) );
    m_uniformCullBoxUBY->set( static_cast<float>( yMax ) );
    m_uniformCullBoxUBZ->set( static_cast<float>( zMax ) );
}

void WMFiberDisplay::create()
{
    // create new node
    osg::ref_ptr< osg::Switch > osgNodeNew = osg::ref_ptr< osg::Switch >( new osg::Switch );

    m_tubeDrawable = osg::ref_ptr< WTubeDrawable >( new WTubeDrawable );
    m_tubeDrawable->setDataset( m_dataset );
    m_tubeDrawable->setUseDisplayList( false );
    m_tubeDrawable->setDataVariance( osg::Object::DYNAMIC );
    m_geodeTubeDrawable = osg::ref_ptr< osg::Geode >( new osg::Geode );
    m_geodeTubeDrawable->addDrawable( m_tubeDrawable );

    m_tubeGeometryPointSprite = osg::ref_ptr< WTubeGeometry >( new WTubeGeometry );
    m_tubeGeometryPointSprite->setDataset( m_dataset );
    m_tubeGeometryPointSprite->setUseDisplayList( false );
    m_tubeGeometryPointSprite->setUseVertexBufferObjects( true );
    osg::VertexBufferObject* m_vboPS = m_tubeGeometryPointSprite->getOrCreateVertexBufferObject();
    m_vboPS->setUsage( GL_STATIC_DRAW );
    m_tubeGeometryPointSprite->setDataVariance( osg::Object::DYNAMIC );
    m_geodePointSprite = osg::ref_ptr< osg::Geode >( new osg::Geode );
    m_geodePointSprite->addDrawable( m_tubeGeometryPointSprite );

    m_tubeGeometryQuadStripes = osg::ref_ptr< WTubeGeometry >( new WTubeGeometry );
    m_tubeGeometryQuadStripes->setDataset( m_dataset );
    m_tubeGeometryQuadStripes->setUseDisplayList( false );
    m_tubeGeometryQuadStripes->setUseVertexBufferObjects( true );
    osg::VertexBufferObject* m_vboQS = m_tubeGeometryQuadStripes->getOrCreateVertexBufferObject();
    m_vboQS->setUsage( GL_STATIC_DRAW );
    m_tubeGeometryQuadStripes->setDataVariance( osg::Object::DYNAMIC );
    m_geodeQuadStripes = osg::ref_ptr< osg::Geode >( new osg::Geode );
    m_geodeQuadStripes->addDrawable( m_tubeGeometryQuadStripes );

    osgNodeNew->addChild( m_geodeTubeDrawable, true );
    osgNodeNew->addChild( m_geodePointSprite, false );
    osgNodeNew->addChild( m_geodeQuadStripes, false );

    osgNodeNew->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

    osgNodeNew->setUserData( osg::ref_ptr< userData >(
        new userData( boost::shared_dynamic_cast< WMFiberDisplay >( shared_from_this() ) )
        ) );
    osgNodeNew->addUpdateCallback( new fdNodeCallback );

    // remove previous nodes if there are any
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->removeChild( m_osgNode.get() );

    m_osgNode = osgNodeNew;

    activate();
    osg::StateSet* rootState = m_osgNode->getOrCreateStateSet();
    initUniforms( rootState );
    createTextures( rootState );

    createTubeData();

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->addChild( m_osgNode.get() );
}

void WMFiberDisplay::connectors()
{
    using boost::shared_ptr;
    typedef WModuleInputData< const WDataSetFibers > FiberInputData;  // just an alias

    m_fiberInput = shared_ptr< FiberInputData >( new FiberInputData( shared_from_this(), "fiberInput", "A loaded fiber dataset." ) );

    typedef WModuleOutputData< WFiberCluster > ClusterOutputData; // just an alias

    m_clusterOC = shared_ptr< ClusterOutputData >( new ClusterOutputData( shared_from_this(), "clusterOut", "FiberCluster of current selection" ) );

    addConnector( m_fiberInput );
    addConnector( m_clusterOC );

    WModule::connectors();  // call WModules initialization
}

void WMFiberDisplay::activate()
{
    if( m_osgNode )
    {
        if( m_active->get() )
        {
            m_osgNode->setNodeMask( 0xFFFFFFFF );
        }
        else
        {
            m_osgNode->setNodeMask( 0x0 );
        }
    }

    WModule::activate();
}

void WMFiberDisplay::properties()
{
    m_customColoring = m_properties->addProperty( "Custom coloring", "Switches the coloring between custom and predefined.", false );
    m_coloring = m_properties->addProperty( "Global or local coloring", "Switches the coloring between global and local.", true );

    m_useTubesProp = m_properties->addProperty( "Use tubes", "Draw fiber tracts as fake tubes.", false );
    m_useTextureProp = m_properties->addProperty( "Use texture", "Texture fibers with the texture on top of the list.", false );
    m_tubeThickness = m_properties->addProperty( "Tube thickness", "Adjusts the thickness of the tubes.", 50.,
            boost::bind( &WMFiberDisplay::adjustTubes, this ) );
    m_tubeThickness->setMin( 0 );
    m_tubeThickness->setMax( 300 );

    m_depthCueing = m_properties->addProperty( "Use depth cueing", "Activates depth cueing for the shaders.", true );
    m_lightModel = m_properties->addProperty( "Change lighting", "Changes lighting model", false );
    m_usePointSprites  = m_properties->addProperty( "Use PointSprites", "", true );
    m_useQuadStripes  = m_properties->addProperty( "Use QuadStripes", "", true );
    m_useProjection  = m_properties->addProperty( "Use projection", "", false );
    m_lowDepthCueingFactor  = m_properties->addProperty( "Lower DCF", "Adjusts the thickness of the tubes.", 50. );
    m_lowDepthCueingFactor->setMin( 0 );
    m_lowDepthCueingFactor->setMax( 100 );
    m_upperDepthCueingFactor = m_properties->addProperty( "Upper DCF", "Adjusts the thickness of the tubes.", 85. );
    m_upperDepthCueingFactor->setMin( 0 );
    m_upperDepthCueingFactor->setMax( 100 );

    m_save = m_properties->addProperty( "Save", "Saves the selected fiber bundles.", false, boost::bind( &WMFiberDisplay::saveSelected, this ) );
    m_saveFileName = m_properties->addProperty( "File name", "", WPathHelper::getAppPath() );
    m_updateOC = m_properties->addProperty( "Update output conn.",
                                            "Updates the output connector with the currently selected fibers",
                                            WPVBaseTypes::PV_TRIGGER_READY,
                                            boost::bind( &WMFiberDisplay::updateOutput, this ) );

    m_cullBoxGroup = m_properties->addPropertyGroup( "Box Culling",  "Properties only related to the box culling." );
    m_activateCullBox = m_cullBoxGroup->addProperty( "Activate", "Activates the cull box", false );
    m_showCullBox = m_cullBoxGroup->addProperty( "Show cull box", "Shows/hides the cull box", false );
    m_insideCullBox = m_cullBoxGroup->addProperty( "Inside - outside", "Show fibers inside or outside the cull box", true );
}

void WMFiberDisplay::updateRenderModes()
{
    osg::StateSet* rootState = m_osgNode->getOrCreateStateSet();

    if ( m_textureChanged )
    {
        m_textureChanged = false;
        updateTexture();
    }

    if( m_useTubesProp->changed() || m_useTextureProp->changed() || m_activateCullBox->changed() )
    {
        if ( m_useTubesProp->get( true ) )
        {
            updateTexture();
            m_useTextureProp->get( true );

            m_osgNode->setChildValue( m_geodeTubeDrawable, false );
            m_osgNode->setChildValue( m_geodePointSprite, true );
            m_osgNode->setChildValue( m_geodeQuadStripes, true );

            m_shaderTubesQS->apply( m_geodeQuadStripes );
            m_shaderTubesPS->apply( m_geodePointSprite );

            rootState->addUniform( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "globalColor", 1 ) ) );
            m_uniformTubeThickness = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "u_thickness", static_cast<float>( m_tubeThickness->get() ) ) );
            rootState->addUniform( m_uniformTubeThickness );
            rootState->addUniform( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useTexture", m_useTextureProp->get() ) ) );
        }
        else if ( ( m_useTextureProp->get( true ) && !m_useTubesProp->get( true ) ) || m_activateCullBox->get( true) )
        {
            m_osgNode->setChildValue( m_geodeTubeDrawable, true );
            m_osgNode->setChildValue( m_geodePointSprite, false );
            m_osgNode->setChildValue( m_geodeQuadStripes, false );

            //m_tubeDrawable->setUseTubes( false );
            updateTexture();
            m_shaderTubes->deactivate( m_geodeTubeDrawable );
            m_shaderTexturedFibers->apply( m_geodeTubeDrawable );
            m_uniformUseTexture->set( m_useTextureProp->get() );
        }
        else
        {
            m_osgNode->setChildValue( m_geodeTubeDrawable, true );
            m_osgNode->setChildValue( m_geodePointSprite, false );
            m_osgNode->setChildValue( m_geodeQuadStripes, false );

            //m_tubeDrawable->setUseTubes( false );
            m_shaderTubes->deactivate( m_geodeTubeDrawable );
            m_shaderTexturedFibers->deactivate( m_geodeTubeDrawable );
        }
    }

    if  ( !m_useTextureProp->get( true ) && !m_useTubesProp->get( true ) )
    {
        rootState->setTextureMode( 0, GL_TEXTURE_3D, osg::StateAttribute::OFF );
    }

    if ( m_depthCueing->changed() )
    {
        m_uniformDepthCueing->set( m_depthCueing->get( true ) );
    }

    if ( m_upperDepthCueingFactor->changed() )
    {
        m_uniformUpperDepthCueingFactor->set( static_cast<float>( m_upperDepthCueingFactor->get( true ) * 0.01 ) );
    }

    if ( m_lowDepthCueingFactor->changed() )
    {
        m_uniformLowDepthCueingFactor->set( static_cast<float>( m_lowDepthCueingFactor->get( true ) * 0.01 ) );
    }

    if ( m_usePointSprites->changed() && m_useTubesProp->get() )
    {
        m_uniformUsePointSprites->set( m_usePointSprites->get( true ) );
        m_osgNode->setChildValue( m_geodePointSprite, m_usePointSprites->get() );
    }

    if ( m_useQuadStripes->changed() && m_useTubesProp->get() )
    {
        m_uniformUseQuadStripes->set( m_useQuadStripes->get( true ) );
        m_osgNode->setChildValue( m_geodeQuadStripes, m_useQuadStripes->get() );
    }

    if ( m_useProjection->changed() )
    {
        m_uniformUseProjection->set( m_useProjection->get( true ) );
    }
}

void WMFiberDisplay::toggleColoring()
{
    if( m_coloring->changed() || m_customColoring->changed() )
    {
        m_tubeDrawable->setColoringMode( m_coloring->get( true ) );
        m_tubeDrawable->setCustomColoring( m_customColoring->get( true ) );
    }
}

void WMFiberDisplay::adjustTubes()
{
    if ( m_tubeThickness.get() && m_useTubesProp.get() )
    {
        if ( m_tubeThickness->changed() && m_useTubesProp->get( true ) )
        {
            m_uniformTubeThickness->set( static_cast<float>( m_tubeThickness->get() ) );
        }
    }
}

void WMFiberDisplay::saveSelected()
{
    boost::shared_ptr< std::vector< bool > > active = WKernel::getRunningKernel()->getRoiManager()->getBitField();
    m_dataset->saveSelected( m_saveFileName->getAsString(), active );
}

void WMFiberDisplay::updateOutput() const
{
    std::vector< wmath::WFiber > fibs;
    std::list< size_t > indices;
    boost::shared_ptr< std::vector< bool > > active = WKernel::getRunningKernel()->getRoiManager()->getBitField();
    for( size_t i = 0; i < active->size(); ++i ) // std::vector< bool >::const_iterator cit = active->begin(); cit != active->end(); ++cit, ++index )
    {
        if( ( *active )[i] )
        {
            size_t length = ( * m_dataset->getLineLengths() )[ i ];
            wmath::WFiber f;
            f.reserve( 3 * length );
            for( size_t p = 0; p < length; ++p )
            {
                f.push_back( m_dataset->getPosition( i, p ) );
            }
            indices.push_back( fibs.size() );
            fibs.push_back( f );
        }
    }
    boost::shared_ptr< WFiberCluster > result( new WFiberCluster );
    result->setDataSetReference( boost::shared_ptr< WDataSetFiberVector >( new WDataSetFiberVector( boost::shared_ptr< std::vector< wmath::WFiber > >( new std::vector< wmath::WFiber >( fibs ) ) ) ) ); // NOLINT
    result->setIndices( indices );
    m_clusterOC->updateData( result );
    m_updateOC->set( WPVBaseTypes::PV_TRIGGER_READY, false );
}

void WMFiberDisplay::updateTexture()
{
    osg::StateSet* rootState = m_osgNode->getOrCreateStateSet();

    // grab a list of data textures
    std::vector< boost::shared_ptr< WDataTexture3D > > tex = WDataHandler::getDefaultSubject()->getDataTextures();

    if ( tex.size() > 0 )
    {
        osg::ref_ptr<osg::Texture3D> texture3D = tex[0]->getTexture();

        if ( tex[0]->isInterpolated() )
        {
            texture3D->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR );
            texture3D->setFilter( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
        }
        else
        {
            texture3D->setFilter( osg::Texture::MIN_FILTER, osg::Texture::NEAREST );
            texture3D->setFilter( osg::Texture::MAG_FILTER, osg::Texture::NEAREST );
        }
        rootState->setTextureAttributeAndModes( 0, texture3D, osg::StateAttribute::ON );


        m_uniformType->set( tex[0]->getDataType() );
        float minValue = tex[0]->getMinValue();
        float maxValue = tex[0]->getMaxValue();
        float thresh = ( tex[0]->getThreshold() - minValue ) / ( maxValue - minValue ); // rescale to [0,1]

        m_uniformThreshold->set( thresh );
        m_uniformsColorMap->set( tex[0]->getSelectedColormap() );

        m_uniformDimX->set( static_cast<int>( tex[0]->getGrid()->getNbCoordsX() ) );
        m_uniformDimY->set( static_cast<int>( tex[0]->getGrid()->getNbCoordsY() ) );
        m_uniformDimZ->set( static_cast<int>( tex[0]->getGrid()->getNbCoordsZ() ) );
    }
}

void WMFiberDisplay::initUniforms( osg::StateSet* rootState )
{
    m_uniformUseTexture = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useTexture", false ) );
    m_uniformSampler = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex", 0 ) );
    m_uniformType = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type", 0 ) );
    m_uniformThreshold = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold", 0.0f ) );
    m_uniformsColorMap = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "cMap", 0 ) );

    m_uniformDimX = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "dimX", 1 ) );
    m_uniformDimY = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "dimY", 1 ) );
    m_uniformDimZ = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "dimZ", 1 ) );

    rootState->addUniform( m_uniformUseTexture );
    rootState->addUniform( m_uniformSampler );
    rootState->addUniform( m_uniformType );
    rootState->addUniform( m_uniformThreshold );
    rootState->addUniform( m_uniformsColorMap );

    rootState->addUniform( m_uniformDimX );
    rootState->addUniform( m_uniformDimY );
    rootState->addUniform( m_uniformDimZ );

    // cull box info
    float xMin = m_cullBox->getMinPos()[0];
    float yMin = m_cullBox->getMinPos()[1];
    float zMin = m_cullBox->getMinPos()[2];
    float xMax = m_cullBox->getMaxPos()[0];
    float yMax = m_cullBox->getMaxPos()[1];
    float zMax = m_cullBox->getMaxPos()[2];

    m_uniformUseCullBox = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useCullBox", false ) );
    m_uniformInsideCullBox = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "insideCullBox", false ) );

    m_uniformCullBoxLBX = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "cullBoxLBX", static_cast<float>( xMin ) ) );
    m_uniformCullBoxLBY = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "cullBoxLBY", static_cast<float>( yMin ) ) );
    m_uniformCullBoxLBZ = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "cullBoxLBZ", static_cast<float>( zMin ) ) );
    m_uniformCullBoxUBX = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "cullBoxUBX", static_cast<float>( xMax ) ) );
    m_uniformCullBoxUBY = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "cullBoxUBY", static_cast<float>( yMax ) ) );
    m_uniformCullBoxUBZ = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "cullBoxUBZ", static_cast<float>( zMax ) ) );

    rootState->addUniform( m_uniformUseCullBox );
    rootState->addUniform( m_uniformInsideCullBox );

    rootState->addUniform( m_uniformCullBoxLBX );
    rootState->addUniform( m_uniformCullBoxLBY );
    rootState->addUniform( m_uniformCullBoxLBZ );
    rootState->addUniform( m_uniformCullBoxUBX );
    rootState->addUniform( m_uniformCullBoxUBY );
    rootState->addUniform( m_uniformCullBoxUBZ );

    m_uniformDepthCueing = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useDepthCueing", true ) );
    m_uniformLightModel= osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useLightModel", false ) );
    m_uniformUsePointSprites = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "usePointSprites", true ) );
    m_uniformUseQuadStripes = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useQuadStripes", true ) );
    m_uniformLowDepthCueingFactor = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "lowerDepthCueingFactor", 0.5f ) );
    m_uniformUpperDepthCueingFactor = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "upperDepthCueingFactor", 0.85f ) );
    m_uniformUseProjection = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useProjection", false ) );

    rootState->addUniform( m_uniformDepthCueing );
    rootState->addUniform( m_uniformLightModel );
    rootState->addUniform( m_uniformUsePointSprites );
    rootState->addUniform( m_uniformUseQuadStripes );
    rootState->addUniform( m_uniformLowDepthCueingFactor );
    rootState->addUniform( m_uniformUpperDepthCueingFactor );
    rootState->addUniform( m_uniformUseProjection );

    m_uniformViewportHeight = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "u_viewportHeight", static_cast<int>( 0 ) ) );
    m_uniformViewportWidth = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "u_viewportWidth", static_cast<int>( 0 ) ) );

    m_tubeGeometryPointSprite->setViewPortUniform( m_uniformViewportHeight, m_uniformViewportWidth );
    m_tubeGeometryQuadStripes->setViewPortUniform( m_uniformViewportHeight, m_uniformViewportWidth );

    rootState->addUniform( m_uniformViewportHeight );
    rootState->addUniform( m_uniformViewportWidth );

    m_uniformNearPos = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "nearPos", osg::Vec3( m_dataset->getBoundingBox().first[0],
                                                                                           m_dataset->getBoundingBox().first[1],
                                                                                           m_dataset->getBoundingBox().first[2] ) ) );
    m_uniformFarPos = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "farPos", osg::Vec3( m_dataset->getBoundingBox().second[0],
                                                                                         m_dataset->getBoundingBox().second[1],
                                                                                         m_dataset->getBoundingBox().second[2] ) ) );
    rootState->addUniform( m_uniformNearPos );
    rootState->addUniform( m_uniformFarPos );
}

void WMFiberDisplay::notifyTextureChange()
{
    m_textureChanged = true;
}

void WMFiberDisplay::initCullBox()
{
    wmath::WPosition crossHairPos = WKernel::getRunningKernel()->getSelectionManager()->getCrosshair()->getPosition();
    wmath::WPosition minROIPos = crossHairPos - wmath::WPosition( 10., 10., 10. );
    wmath::WPosition maxROIPos = crossHairPos + wmath::WPosition( 10., 10., 10. );

    m_cullBox = osg::ref_ptr< WROIBox >( new WROIBox( minROIPos, maxROIPos ) );
    m_cullBox->setColor( osg::Vec4( 1.0, 0., 1.0, 0.4 ) );
}

void WMFiberDisplay::userData::update()
{
    parent->update();
}

void WMFiberDisplay::userData::updateRenderModes()
{
    parent->updateRenderModes();
}

void WMFiberDisplay::userData::toggleColoring()
{
    parent->toggleColoring();
}

void WMFiberDisplay::create2DTexturesBlinn( osg::StateSet* m_rootState ) const
{
    double s, t;
    double lt;
    double alpha, beta;
    double diffTerm, specTerm;
    double stretchDiff, stretchSpec;
    int n = 64;
    int texDim = 64;

    osg::ref_ptr< osg::Image > imageSpec = new osg::Image;
    osg::ref_ptr< osg::Image > imageDiff = new osg::Image;
    // allocate the image data, texDim x texDim x 1 with 4 rgba floats - equivalent to a Vec4!
    imageSpec->allocateImage( texDim, texDim, 1, GL_RGBA, GL_UNSIGNED_BYTE );
    imageDiff->allocateImage( texDim, texDim, 1, GL_RGBA, GL_UNSIGNED_BYTE );

    // fill in the image data.
    unsigned char *dataPtrSpec = imageSpec->data();
    unsigned char *dataPtrDiff = imageDiff->data();

    stretchDiff = ( 4.0 / PI );

    double factor = 1.0;
    while ( n > 0 )
    {
        factor *= ( double )( n + 1 ) / ( double ) n;
        n -= 2;
    }

    if( ( n % 2 ) == 0 )
    {
        stretchSpec = factor;
    }
    else
    {
     stretchSpec = ( 2.0 / PI ) * factor;
    }

    // compute texture for blinn lighting:
    for ( int y = 0; y < texDim; y++ )
    {
        for ( int x = 0; x < texDim; x++ )
        {
            // no divisions by zero
            s = ( ( double ) x + 0.5 ) / ( double ) texDim;
            t = ( ( double ) y + 0.5 ) / ( double ) texDim;

            alpha = acos( 2.0 * s - 1.0 );
            lt = 2.0 * t - 1.0;
            beta = acos( 2.0 * t - 1.0 );

            diffTerm = stretchDiff * sqrt( 1.0 - lt * lt )* ( sin( alpha ) + ( PI - alpha ) * cos( alpha ) ) / 4.0;
            specTerm = stretchSpec * computeSpecIntegral( alpha, beta, n );

            dataPtrDiff[ ( 4 * y * texDim ) + ( 4 * x ) + 0 ] = static_cast< unsigned char >( diffTerm * 255.0 );
            dataPtrDiff[ ( 4 * y * texDim ) + ( 4 * x ) + 1 ] = static_cast< unsigned char >( 255.0 );
            dataPtrDiff[ ( 4 * y * texDim ) + ( 4 * x ) + 2 ] = static_cast< unsigned char >( 255.0 );
            dataPtrDiff[ ( 4 * y * texDim ) + ( 4 * x ) + 3 ] = static_cast< unsigned char >( 255.0 );

            dataPtrSpec[ ( 4 * y * texDim ) + ( 4 * x ) + 0 ] = static_cast< unsigned char >( specTerm * 255.0 );
            dataPtrSpec[ ( 4 * y * texDim ) + ( 4 * x ) + 1 ] = static_cast< unsigned char >( 255.0 );
            dataPtrSpec[ ( 4 * y * texDim ) + ( 4 * x ) + 2 ] = static_cast< unsigned char >( 255.0 );
            dataPtrSpec[ ( 4 * y * texDim ) + ( 4 * x ) + 3 ] = static_cast< unsigned char >( 255.0 );
        }
    }

    osg::Texture2D* textureSpec = new osg::Texture2D;
    textureSpec->setDataVariance( osg::Object::STATIC );
    textureSpec->setWrap( osg::Texture2D::WRAP_S, osg::Texture2D::CLAMP_TO_BORDER );
    textureSpec->setWrap( osg::Texture2D::WRAP_T, osg::Texture2D::CLAMP_TO_BORDER );
    textureSpec->setFilter( osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR_MIPMAP_LINEAR );
    textureSpec->setFilter( osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR );
    textureSpec->setImage( imageSpec );
    //osgDB::writeImageFile( *imageSpec, "share/fileSpec.png" );
    m_rootState->setTextureAttribute( 4, textureSpec, osg::StateAttribute::OVERRIDE );
    m_rootState->setTextureMode( 4, GL_TEXTURE_2D, osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE );
    m_rootState->addUniform( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "textureSpec2D", 4 ) ) );

    osg::Texture2D* textureDiff = new osg::Texture2D;
    textureDiff->setDataVariance( osg::Object::STATIC );
    textureDiff->setWrap( osg::Texture2D::WRAP_S, osg::Texture2D::CLAMP_TO_BORDER );
    textureDiff->setWrap( osg::Texture2D::WRAP_T, osg::Texture2D::CLAMP_TO_BORDER );
    textureDiff->setFilter( osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR_MIPMAP_LINEAR );
    textureDiff->setFilter( osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR );
    textureDiff->setImage( imageDiff );
    //osgDB::writeImageFile( *imageDiff, "share/fileDiff.png" );
    m_rootState->setTextureAttribute( 5, textureDiff, osg::StateAttribute::OVERRIDE );
    m_rootState->setTextureMode( 5, GL_TEXTURE_2D, osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE );
    m_rootState->addUniform( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "textureDiff2D", 5 ) ) );
}


double WMFiberDisplay::computeSpecIntegral( double alpha, double beta, int n ) const
{
    double lowerBorder = alpha - PI / 2.0;
    double upperBorder = PI / 2.0;

    int steps = 10;
    double currentIntPos = ( upperBorder - lowerBorder ) / ( 2.0 * steps );

    double integral = 0.0;
    double xi;

    for ( int i = 0; i < 2 * steps; i += 2 )
    {
        xi = lowerBorder + i * currentIntPos;
        integral += 2.0 * computeIntegrand( beta, n, xi );
        xi = lowerBorder + ( i + 1 ) * currentIntPos;
        integral += 4.0 * computeIntegrand( beta, n, xi );
    }
    integral += computeIntegrand( beta, n, upperBorder );

    // f(a) added twice
    integral -= computeIntegrand( beta, n, lowerBorder );

    integral *= currentIntPos / 3.0;

    return (integral);
}

double WMFiberDisplay::computeIntegrand( double beta, int n, double theta ) const
{
    double y = cos( theta - beta );

    if ( y < 0.0 )
    {
        y = 0.0;
    }

    return ( pow( y, n ) * ( cos( theta ) / 2.0 ) );
}

void WMFiberDisplay::create1DTextureRectLighting( osg::StateSet* m_rootState ) const
{
    osg::Image* image = new osg::Image;

    int noPixels = 64;

    // allocate the image data, noPixels x 1 x 1 with 4 rgba floats - equivalent to a Vec4!
    image->allocateImage( noPixels, 1, 1, GL_RGBA, GL_FLOAT );

    float step = M_PI / static_cast<float>( noPixels );
    float diffuse, specular = 0.0f;

    // fill in the image data.
    osg::Vec4* dataPtr = ( osg::Vec4* )image->data();
    osg::Vec4 color;
    for( int i = 0; i < noPixels; ++i )
    {
        diffuse = sin( static_cast<float>( i * step ) );
        specular = pow( diffuse, 16 );

        color = osg::Vec4( diffuse, specular, 0.0f, 1.0f );
        *dataPtr++ = color;
    }

    osg::Texture1D* texture = new osg::Texture1D;
    texture->setDataVariance( osg::Object::STATIC );
    texture->setWrap( osg::Texture1D::WRAP_S, osg::Texture1D::CLAMP );
    texture->setFilter( osg::Texture1D::MIN_FILTER, osg::Texture1D::LINEAR );
    texture->setImage( image );

    m_rootState->setTextureAttribute( 7, texture, osg::StateAttribute::OVERRIDE );
    m_rootState->setTextureMode( 7, GL_TEXTURE_1D, osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE );
    m_rootState->addUniform( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "texture", 7 ) ) );
}


void WMFiberDisplay::create2DTextureCycleLighting( osg::StateSet* m_rootState ) const
{
    int noPixels = 64;

    // allocate the image data, noPixels x noPixels x 1 with 4 rgba floats - equivalent to a Vec4!

    float step = M_PI / static_cast<float>( noPixels );
    float alpha, diffuse, specular = 0.0f;
    float check, border;

    const size_t resX = 64;
    osg::ref_ptr< osg::Image > randImage = new osg::Image();
    randImage->allocateImage( resX, resX, 1, GL_RGBA, GL_UNSIGNED_BYTE );
    unsigned char *randomLuminance = randImage->data();
    for( unsigned int y = 0; y < resX; y++ )
    {
        diffuse = sin( static_cast<float>( y * step ) );
        specular = pow( diffuse, 16 );
        for( unsigned int x = 0; x < resX; x++ )
        {
            border = 1.0;
            check = pow( 2.0 * ( static_cast< float >( x ) / static_cast< float >( resX ) ) - 1.0, 2.0 ) +
            pow( 2.0 * ( static_cast< float >( y ) / static_cast< float >( resX ) ) - 1.0, 2.0 );
            if( check > 1.0 )
            {
                alpha = 0.0f;
            }
            else
            {
                if( check > 0.9 )
                {
                    border = 0.0;
                }
                alpha = 1.0f;
            }

            randomLuminance[ ( 4 * y * resX ) + ( 4 * x ) + 0 ] = static_cast< unsigned char >( diffuse * 255.0 );
            randomLuminance[ ( 4 * y * resX ) + ( 4 * x ) + 1 ] = static_cast< unsigned char >( specular * 255.0 );
            randomLuminance[ ( 4 * y * resX ) + ( 4 * x ) + 2 ] = static_cast< unsigned char >( alpha * 255.0 );
            randomLuminance[ ( 4 * y * resX ) + ( 4 * x ) + 3 ] = static_cast< unsigned char >( border * 255.0 );
        }
    }
    osg::Texture2D* texture = new osg::Texture2D;
    texture->setDataVariance( osg::Object::STATIC );
    texture->setWrap( osg::Texture2D::WRAP_S, osg::Texture2D::CLAMP_TO_BORDER );
    texture->setWrap( osg::Texture2D::WRAP_T, osg::Texture2D::CLAMP_TO_BORDER );
    texture->setFilter( osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR_MIPMAP_LINEAR );
    texture->setFilter( osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR );
    texture->setImage( randImage );
    //osgDB::writeImageFile( *randImage, "share/file.png" );

    m_rootState->setTextureAttribute( 6, texture, osg::StateAttribute::OVERRIDE );
    m_rootState->setTextureMode( 6, GL_TEXTURE_2D, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE );
    m_rootState->addUniform( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "texturePS", 6 ) ) );
}

void WMFiberDisplay::createTextures( osg::StateSet* m_rootState ) const
{
    create1DTextureRectLighting( m_rootState );
    create2DTextureCycleLighting( m_rootState );

    create2DTexturesBlinn( m_rootState );
}

void WMFiberDisplay::createTubeData()
{
    boost::shared_ptr< std::vector< size_t > > pointsPerLine = m_dataset->getLineLengths();
    boost::shared_ptr< std::vector< float > > verts = m_dataset->getVertices();
    boost::shared_ptr< std::vector< float > > tangents = m_dataset->getTangents();
    boost::shared_ptr< std::vector< float > > colors;
    boost::shared_ptr< std::vector< size_t > > startIndices = m_dataset->getLineStartIndexes();
    if ( m_customColoring )
    {
        colors = WKernel::getRunningKernel()->getRoiManager()->getCustomColors();
    }
    else
    {
        colors = ( m_coloring->get( true ) ? m_dataset->getGlobalColors() : m_dataset->getLocalColors() );
    }

    m_tubeVerts = new osg::Vec3Array;
    m_tubeTangents = new osg::Vec3Array;
    m_tubeTexCoords = new osg::Vec2Array;
    m_tubeColors = new osg::Vec3Array;

    m_pointTexCoords = new osg::Vec2Array;
    m_pointVerts  = new osg::Vec3Array;
    m_pointTangents  = new osg::Vec3Array;
    m_pointColors  = new osg::Vec3Array;

    m_fiberPointStartIndexes = boost::shared_ptr< std::vector< size_t > >( new std::vector< size_t >() );
    m_fiberQuadStartIndexes = boost::shared_ptr< std::vector< size_t > >( new std::vector< size_t >() );
    m_fiberPointStartIndexes->clear();
    m_fiberQuadStartIndexes->clear();
    unsigned int offset = 0;

    // add vertexdata for quadstrips
    for( size_t i = 0; i < startIndices->size(); ++i )
    {
        int idx = startIndices->at( i ) * 3;
        m_fiberQuadStartIndexes->push_back( offset );
        offset += 2 * pointsPerLine->at( i );
        for ( size_t k = 0; k < pointsPerLine->at( i ); ++k )
        {
            m_tubeTangents->push_back( osg::Vec3f( tangents->at( idx ), tangents->at( idx + 1 ), tangents->at( idx + 2 ) ) );
            m_tubeTangents->push_back( osg::Vec3f( tangents->at( idx ), tangents->at( idx + 1 ), tangents->at( idx + 2 ) ) );

            m_tubeColors->push_back( osg::Vec3f( colors->at( idx ), colors->at( idx + 1 ), colors->at( idx + 2 ) ) );
            m_tubeColors->push_back( osg::Vec3f( colors->at( idx ), colors->at( idx + 1 ), colors->at( idx + 2 ) ) );

            m_tubeVerts->push_back( osg::Vec3f( verts->at( idx ), verts->at( idx + 1 ), verts->at( idx + 2 ) ) );
            m_tubeVerts->push_back( osg::Vec3f( verts->at( idx ), verts->at( idx + 1 ), verts->at( idx + 2 ) ) );

            // TexCoords: first 1DTexturePosition, second Positionflag
            // SouthPointPosition
            m_tubeTexCoords->push_back( osg::Vec2f( -1.0f , 0.0f ) );

            // NorthPointPosition
            m_tubeTexCoords->push_back( osg::Vec2f( 1.0f, 1.0f ) );

            idx += 3;
        }
    }

    offset = 0;

    // add vertexdata for pointsprites
    for( size_t i = 0; i < startIndices->size(); ++i )
    {
        int idx = startIndices->at( i ) * 3;
        m_fiberPointStartIndexes->push_back( offset );
        offset += pointsPerLine->at( i );
        for ( size_t k = 0; k < pointsPerLine->at( i ); ++k )
        {
            m_pointTangents->push_back( osg::Vec3f( tangents->at( idx ), tangents->at( idx + 1 ), tangents->at( idx + 2 ) ) );

            m_pointColors->push_back( osg::Vec3f( colors->at( idx ), colors->at( idx + 1 ), colors->at( idx + 2 ) ) );

            m_pointVerts->push_back( osg::Vec3f( verts->at( idx ), verts->at( idx + 1 ), verts->at( idx + 2 ) ) );

            // if k=0 || k=pointPerLine-1 then draw endtexture
            if(k == 0 || k == pointsPerLine->at( i ) - 1 )
            {
                m_pointTexCoords->push_back( osg::Vec2f( 0.0f , 0.0f ) );
            }
            else
            {
                m_pointTexCoords->push_back( osg::Vec2f( 0.0f , 1.0f ) );
            }

            idx += 3;
        }
    }
    osg::ref_ptr<osg::StateSet> statesetPS = m_geodePointSprite->getOrCreateStateSet();
    osg::ref_ptr<osg::StateSet> statesetQS = m_geodeQuadStripes->getOrCreateStateSet();

    statesetQS->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    statesetPS->setMode( GL_VERTEX_PROGRAM_POINT_SIZE, osg::StateAttribute::ON );

    // setting up VBO and PrimitivSets
    m_tubeGeometryPointSprite->setVertexArray( m_pointVerts );
    m_tubeGeometryPointSprite->setColorArray( m_pointColors );
    m_tubeGeometryPointSprite->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
    m_tubeGeometryPointSprite->setNormalArray( m_pointTangents );
    m_tubeGeometryPointSprite->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );
    m_shaderTubesPS->addBindAttribLocation( "psTexCoords", 6 );
    m_tubeGeometryPointSprite->setVertexAttribArray( 6, m_pointTexCoords );
    m_tubeGeometryPointSprite->setVertexAttribBinding( 6, osg::Geometry::BIND_PER_VERTEX );
    m_tubeGeometryPointSprite->setStartIndices( m_fiberPointStartIndexes );

    m_tubeGeometryQuadStripes->setVertexArray( m_tubeVerts );
    m_tubeGeometryQuadStripes->setColorArray( m_tubeColors );
    m_tubeGeometryQuadStripes->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
    m_tubeGeometryQuadStripes->setNormalArray( m_tubeTangents );
    m_tubeGeometryQuadStripes->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );
    m_shaderTubesQS->addBindAttribLocation( "qsTexCoords", 7 );
    m_tubeGeometryQuadStripes->setVertexAttribArray( 7, m_tubeTexCoords );
    m_tubeGeometryQuadStripes->setVertexAttribBinding( 7, osg::Geometry::BIND_PER_VERTEX );
    m_tubeGeometryQuadStripes->setStartIndices( m_fiberQuadStartIndexes );

    for ( size_t i = 0; i < pointsPerLine->size(); ++i )
    {
        m_tubeGeometryPointSprite->addPrimitiveSet( new osg::DrawArrays(
                osg::PrimitiveSet::POINTS, m_fiberPointStartIndexes->at(i), (*pointsPerLine)[i] ) );
        m_tubeGeometryQuadStripes->addPrimitiveSet( new osg::DrawArrays(
                osg::PrimitiveSet::QUAD_STRIP, m_fiberQuadStartIndexes->at(i), 2 * (*pointsPerLine)[i] ) );
    }
}
