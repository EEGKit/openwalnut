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

#include <list>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <osg/TexGen>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osg/Texture1D>
#include <osg/Texture2D>

#include <cmath>
#include <iostream>
#include <time.h>

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

bool WMFiberDisplay::m_fiberDisplayRunning = false;

WMFiberDisplay::WMFiberDisplay()
    : WModule(),
      m_noData( new WCondition, true ),
	  m_osgNode( osg::ref_ptr< osg::Group >() )
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

				WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->removeChild( m_osgSwitch.get() );
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

	WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_osgSwitch );
}

void WMFiberDisplay::update()
{

    if( m_noData.changed() )
    {
		if( m_osgSwitch && m_noData.get( true ) )
        {
			m_osgSwitch->setNodeMask( 0x0 );
        }
        else
        {
			m_osgSwitch->setNodeMask( 0xFFFFFFFF );
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
	osg::ref_ptr< osg::Switch > osgSwitchNew = osg::ref_ptr< osg::Switch >( new osg::Switch );

    m_tubeDrawable = osg::ref_ptr< WTubeDrawable >( new WTubeDrawable );
    m_tubeDrawable->setDataset( m_dataset );
	m_tubeDrawable->setUseDisplayList( false );
	m_tubeDrawable->setDataVariance( osg::Object::DYNAMIC );
	m_geodeTubeDrawable = osg::ref_ptr< osg::Geode >( new osg::Geode );
	m_geodeTubeDrawable->addDrawable( m_tubeDrawable );

	m_tubeGeometryPointSprite = osg::ref_ptr< WTubeGeometry >( new WTubeGeometry );
	m_tubeGeometryPointSprite->setDataset( m_dataset );
	m_tubeGeometryPointSprite->setUseDisplayList( false );
	m_tubeGeometryPointSprite->setUseVertexBufferObjects(true);
	osg::VertexBufferObject* m_vboPS = m_tubeGeometryPointSprite->getOrCreateVertexBufferObject();
	m_vboPS->setUsage (GL_STREAM_DRAW);
	m_tubeGeometryPointSprite->setDataVariance( osg::Object::DYNAMIC );
	m_geodePointSprite = osg::ref_ptr< osg::Geode >( new osg::Geode );
	m_geodePointSprite->addDrawable( m_tubeGeometryPointSprite );

	m_tubeGeometryQuadStripes = osg::ref_ptr< WTubeGeometry >( new WTubeGeometry );
	m_tubeGeometryQuadStripes->setDataset( m_dataset );
	m_tubeGeometryQuadStripes->setUseDisplayList( false );
	m_tubeGeometryQuadStripes->setUseVertexBufferObjects(true);
	osg::VertexBufferObject* m_vboQS = m_tubeGeometryQuadStripes->getOrCreateVertexBufferObject();
	m_vboQS->setUsage (GL_STREAM_DRAW);
	m_tubeGeometryQuadStripes->setDataVariance( osg::Object::DYNAMIC );
	m_geodeQuadStripes = osg::ref_ptr< osg::Geode >( new osg::Geode );
	m_geodeQuadStripes->addDrawable( m_tubeGeometryPointSprite );

	osgSwitchNew->addChild( m_geodeTubeDrawable, true );
	osgSwitchNew->addChild( m_geodePointSprite, false );
	osgSwitchNew->addChild( m_geodeQuadStripes, false );


	osgSwitchNew->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

	osgSwitchNew->setUserData( osg::ref_ptr< userData >(
        new userData( boost::shared_dynamic_cast< WMFiberDisplay >( shared_from_this() ) )
        ) );
	osgSwitchNew->addUpdateCallback( new fdNodeCallback );

    // remove previous nodes if there are any
	WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->removeChild( m_osgSwitch.get() );

	m_osgSwitch = osgSwitchNew;

    activate();

	osg::StateSet* rootState = m_osgSwitch->getOrCreateStateSet();
	initUniforms( rootState );
	createTextures( rootState );
	createTubeData();

	WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->addChild( m_osgSwitch.get() );
	std::cout << "Created /n";
	std::cout << "Created /n";
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
	if( m_osgSwitch )
    {
        if( m_active->get() )
        {
			m_osgSwitch->setNodeMask( 0xFFFFFFFF );
        }
        else
        {
			m_osgSwitch->setNodeMask( 0x0 );
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
	m_tubeThickness = m_properties->addProperty( "Tube thickness", "Adjusts the thickness of the tubes.", 30.,
            boost::bind( &WMFiberDisplay::adjustTubes, this ) );
    m_tubeThickness->setMin( 0 );
    m_tubeThickness->setMax( 300 );


	m_depthCueing = m_properties->addProperty( "Use depth cueing", "Activates depth cueing for the shaders.", false );
	m_lightModel = m_properties->addProperty( "Change lightning", "Changes lightning model", false );
	m_usePointSprites  = m_properties->addProperty( "Use PointSprites", "", false );
	m_useQuadStripes  = m_properties->addProperty( "Use QuadStripes", "", false );
	//m_lowDepthCueingFactor;
	//m_upperDepthCueingFactor;


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
	osg::StateSet* rootState = m_osgSwitch->getOrCreateStateSet();

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

			m_osgSwitch->setChildValue( m_geodeTubeDrawable, false );
			m_osgSwitch->setChildValue( m_geodePointSprite, true );
			m_osgSwitch->setChildValue( m_geodeQuadStripes, true );

			m_shaderTubesQS->apply( m_geodeQuadStripes );
			m_shaderTubesPS->apply( m_geodePointSprite );

			//m_tubeDrawable->setActiveRenderingMode(false,true);
			//m_tubeDrawablePointSprite->setActiveRenderingMode(true,false);

            rootState->addUniform( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "globalColor", 1 ) ) );
            m_uniformTubeThickness = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "u_thickness", static_cast<float>( m_tubeThickness->get() ) ) );
            rootState->addUniform( m_uniformTubeThickness );
            rootState->addUniform( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useTexture", m_useTextureProp->get() ) ) );

        }
        else if ( ( m_useTextureProp->get( true ) && !m_useTubesProp->get( true ) ) || m_activateCullBox->get( true) )
        {
			m_osgSwitch->setChildValue( m_geodeTubeDrawable, true );
			m_osgSwitch->setChildValue( m_geodePointSprite, false );
			m_osgSwitch->setChildValue( m_geodeQuadStripes, false );
			//m_tubeDrawable->setUseTubes( false );
            updateTexture();
			m_shaderTubes->deactivate( m_geodeTubeDrawable );
			m_shaderTexturedFibers->apply( m_geodeTubeDrawable );
            m_uniformUseTexture->set( m_useTextureProp->get() );
        }
        else
        {
			m_osgSwitch->setChildValue( m_geodeTubeDrawable, true );
			m_osgSwitch->setChildValue( m_geodePointSprite, false );
			m_osgSwitch->setChildValue( m_geodeQuadStripes, false );
			//m_tubeDrawable->setUseTubes( false );
			m_shaderTubes->deactivate( m_geodeTubeDrawable );
			m_shaderTexturedFibers->deactivate( m_geodeTubeDrawable );
        }
    }

    if  ( !m_useTextureProp->get( true ) && !m_useTubesProp->get( true ) )
    {
        rootState->setTextureMode( 0, GL_TEXTURE_3D, osg::StateAttribute::OFF );
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
	osg::StateSet* rootState = m_osgSwitch->getOrCreateStateSet();

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

	m_uniformDepthCueing = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useDepthCueing", false ) );
	m_uniformLightModel= osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useLightModel", false ) );
	m_uniformUsePointSprites = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "usePointSprites", false ) );
	m_uniformUseQuadStripes = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useQuadStripes", false ) );
	m_uniformLowDepthCueingFactor = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "lowerDepthCueingFactor", 0.2f ) );
	m_uniformUpperDepthCueingFactor = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "upperDepthCueingFactor", 0.8f ) );

	rootState->addUniform( m_uniformDepthCueing );
	rootState->addUniform( m_uniformLightModel );
	rootState->addUniform( m_uniformUsePointSprites );
	rootState->addUniform( m_uniformUseQuadStripes );
	rootState->addUniform( m_uniformLowDepthCueingFactor );
	rootState->addUniform( m_uniformUpperDepthCueingFactor );

	m_uniformViewportHeight = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "u_viewportHeight",
																			static_cast<int>( 0 ) ) );
	m_uniformViewportWidth = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "u_viewportWidth",
																			static_cast<int>( 0 ) ) );
	rootState->addUniform(m_uniformViewportHeight);
	rootState->addUniform(m_uniformViewportWidth);
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

void WMFiberDisplay::create2DTexDiffuseLightning(osg::StateSet* m_rootState) const
{
	osg::Image* image = new osg::Image;

	int noPixels = 64;

	// allocate the image data, noPixels x noPixels x 1 with 4 rgba floats - equivalent to a Vec4!
	image->allocateImage(noPixels,noPixels,1,GL_RGBA,GL_FLOAT);
	image->setInternalTextureFormat(GL_RGBA);

	float stepAlpha = M_PI/4*(float)noPixels;
	float stepLt = M_PI/2*(float)noPixels;
	float alpha, lt, diffuse = 0.0f;

	// fill in the image data.
	osg::Vec4* dataPtr = (osg::Vec4*)image->data();
	osg::Vec4 color;
	for(int i=0;i<noPixels;++i)
	{
		alpha = (float)i*stepAlpha;

		for(int j=0;j<noPixels;++j)
		{
			lt = (float)j * stepLt;
			diffuse = sqrt(1- pow(lt,2)) * (sin(alpha) + (M_PI-alpha) * cos(alpha)) / 4;
			// Einheitskreis,
			color = osg::Vec4(diffuse, 0.0f, 0.0f, 0.0f);

			*dataPtr++ = color;
		}
	}
	osg::Texture2D* texture = new osg::Texture2D;
	texture->setDataVariance(osg::Object::STATIC);
	texture->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::CLAMP);
	texture->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
	texture->setImage(image);

	m_rootState->setTextureAttribute(3,texture,osg::StateAttribute::OVERRIDE);
	m_rootState->setTextureMode(3,GL_TEXTURE_2D,osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
	m_rootState->addUniform(osg::ref_ptr<osg::Uniform>( new osg::Uniform( "textureDiff2D", 3 ) ) );

}

void WMFiberDisplay::create2DTexSpecularLightning(osg::StateSet* m_rootState) const
{
	osg::Image* image = new osg::Image;

	int noPixels = 64;
	int noExponents = 64;

	//vector<float> sn;

	// allocate the image data, noPixels x noPixels x 1 with 4 rgba floats - equivalent to a Vec4!
	image->allocateImage(noPixels,noPixels,1,GL_RGBA,GL_FLOAT);
	image->setInternalTextureFormat(GL_RGBA);

	float stepAlpha = M_PI/4*(float)noPixels;
	float stepLt = M_PI/2*(float)noPixels;
	float alpha, lt, diffuse = 0.0f;

	// fill in the image data.
	osg::Vec4* dataPtr = (osg::Vec4*)image->data();
	osg::Vec4 color;
	for(int i=0;i<noPixels;++i)
	{
		alpha = (float)i*stepAlpha;

		for(int j=0;j<noPixels;++j)
		{
			lt = (float)j * stepLt;
			diffuse = sqrt(1- pow(lt,2)) * (sin(alpha) + (M_PI-alpha) * cos(alpha)) / 4;
			// Einheitskreis,
			color = osg::Vec4(diffuse, 0.0f, 0.0f, 0.0f);

			*dataPtr++ = color;
		}
	}
	osg::Texture2D* texture = new osg::Texture2D;
	texture->setDataVariance(osg::Object::STATIC);
	texture->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::CLAMP);
	texture->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
	texture->setImage(image);

	m_rootState->setTextureAttribute(4,texture,osg::StateAttribute::OVERRIDE);
	m_rootState->setTextureMode(4,GL_TEXTURE_2D,osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
	m_rootState->addUniform(osg::ref_ptr<osg::Uniform>( new osg::Uniform( "textureSpec2D", 4 ) ) );

}

void WMFiberDisplay::create1DTextureRectLightning(osg::StateSet* m_rootState) const
{
	osg::Image* image = new osg::Image;

	int noPixels = 64;

	// allocate the image data, noPixels x 1 x 1 with 4 rgba floats - equivalent to a Vec4!
	image->allocateImage(noPixels,1,1,GL_RGBA,GL_FLOAT);
	image->setInternalTextureFormat(GL_RGBA);

	float step = M_PI/(float)noPixels;
	float diffuse, specular = 0.0f;

	// fill in the image data.
	osg::Vec4* dataPtr = (osg::Vec4*)image->data();
	osg::Vec4 color;
	for(int i=0;i<noPixels;++i)
	{
		diffuse = sin((float)i*step);
		specular = pow(diffuse, 16);

		color = osg::Vec4(diffuse, specular, 0.0f, 1.0f);
		*dataPtr++ = color;
	}

	osg::Texture1D* texture = new osg::Texture1D;
	texture->setDataVariance(osg::Object::STATIC);
	texture->setWrap(osg::Texture1D::WRAP_S,osg::Texture1D::CLAMP);
	texture->setFilter(osg::Texture1D::MIN_FILTER,osg::Texture1D::LINEAR);
	texture->setImage(image);

	m_rootState->setTextureAttribute(1,texture,osg::StateAttribute::OVERRIDE);
	m_rootState->setTextureMode(1,GL_TEXTURE_1D,osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
	m_rootState->addUniform(osg::ref_ptr<osg::Uniform>( new osg::Uniform( "texture", 1 ) ) );
}


void WMFiberDisplay::create2DTextureCycleLightning(osg::StateSet* m_rootState) const
{
	osg::Image* image = new osg::Image;

	int noPixels = 64;

	// allocate the image data, noPixels x noPixels x 1 with 4 rgba floats - equivalent to a Vec4!
	image->allocateImage(noPixels,noPixels,1,GL_RGBA,GL_FLOAT);
	image->setInternalTextureFormat(GL_RGBA);

	float step = M_PI/(float)noPixels;
	float alpha, diffuse, specular = 0.0f;
	float check;

	const size_t resX = 64;
	osg::ref_ptr< osg::Image > randImage = new osg::Image();
	randImage->allocateImage( resX, resX, 1, GL_RGBA, GL_UNSIGNED_BYTE );
	unsigned char *randomLuminance = randImage->data();  // should be 4 megs
	for( unsigned int x = 0; x < resX; x++ )
	{

		diffuse = sin((float)x*step);
			specular = pow(diffuse, 16);
		for( unsigned int y = 0; y < resX; y++ )
		{
		  check = pow( 2.0 * ( static_cast< float >( x ) / static_cast< float >( resX ) ) - 1.0, 2.0) +
				  pow( 2.0 * ( static_cast< float >( y ) / static_cast< float >( resX ) ) - 1.0, 2.0);
		  if(check > 1.0)
		  {
			alpha = 0.0f;
		  }
		  else
		  {
			alpha = 1.0f;
		  }

			randomLuminance[ ( 4 * y * resX ) + ( 4 * x ) + 0 ] = static_cast< unsigned char >( diffuse * 255.0 );
			randomLuminance[ ( 4 * y * resX ) + ( 4 * x ) + 1 ] = static_cast< unsigned char >( specular * 255.0 );
			randomLuminance[ ( 4 * y * resX ) + ( 4 * x ) + 2 ] = static_cast< unsigned char >( alpha * 255.0 );
			randomLuminance[ ( 4 * y * resX ) + ( 4 * x ) + 3 ] = 255;
		}
	}
	osg::Texture2D* texture = new osg::Texture2D;
	texture->setDataVariance(osg::Object::STATIC);
	texture->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::CLAMP_TO_BORDER);
	texture->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::CLAMP_TO_BORDER);
	texture->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR_MIPMAP_LINEAR);
	texture->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);
	texture->setImage(randImage);

	m_rootState->setTextureAttribute(2,texture,osg::StateAttribute::OVERRIDE);
	m_rootState->setTextureMode(2,GL_TEXTURE_2D,osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
	m_rootState->addUniform(osg::ref_ptr<osg::Uniform>( new osg::Uniform( "texturePS", 2 ) ) );

}

void WMFiberDisplay::createTextures(osg::StateSet* m_rootState) const
{
	create1DTextureRectLightning(m_rootState);
	create2DTextureCycleLightning(m_rootState);

	create2DTexDiffuseLightning(m_rootState);
	create2DTexSpecularLightning(m_rootState);

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

	m_fiberPointStartIndexes = new osg::VectorGLuint;
	m_fiberQuadStartIndexes = new osg::VectorGLuint;
	unsigned int offset = 0;

	// add vertexdata for quadstrips
	for( size_t i = 0; i < startIndices->size(); ++i )
	{
		int idx = startIndices->at( i ) * 3;
		m_fiberQuadStartIndexes->push_back( offset);
		offset += 2*pointsPerLine->at(i);
		for ( size_t k = 0; k < pointsPerLine->at( i ); ++k )
		{
			m_tubeTangents->push_back(osg::Vec3f( tangents->at( idx ), tangents->at( idx + 1 ), tangents->at( idx + 2 ) ));
			m_tubeTangents->push_back(osg::Vec3f( tangents->at( idx ), tangents->at( idx + 1 ), tangents->at( idx + 2 ) ));

			m_tubeColors->push_back(osg::Vec3f( colors->at( idx ), colors->at( idx + 1 ), colors->at( idx + 2 ) ));
			m_tubeColors->push_back(osg::Vec3f( colors->at( idx ), colors->at( idx + 1 ), colors->at( idx + 2 ) ));

			m_tubeVerts->push_back(osg::Vec3f( verts->at( idx ), verts->at( idx + 1 ), verts->at( idx + 2 ) ));
			m_tubeVerts->push_back(osg::Vec3f( verts->at( idx ), verts->at( idx + 1 ), verts->at( idx + 2 ) ));

			// TexCoords: first 1DTexturePosition, second Positionflag
			// SouthPointPosition
			m_tubeTexCoords->push_back(osg::Vec2f( -1.0f , 0.0f ));

			// NorthPointPosition
			m_tubeTexCoords->push_back(osg::Vec2f( 1.0f, 1.0f ));

			idx += 3;
		}
	}

	offset = 0;

	// add vertexdata for pointsprites
	for( size_t i = 0; i < startIndices->size(); ++i )
	{
		int idx = startIndices->at( i ) * 3;
		m_fiberPointStartIndexes->push_back( offset);
		offset += pointsPerLine->at(i);
		for ( size_t k = 0; k < pointsPerLine->at( i ); ++k )
		{
			m_pointTangents->push_back(osg::Vec3f( tangents->at( idx ), tangents->at( idx + 1 ), tangents->at( idx + 2 ) ));

			m_pointColors->push_back(osg::Vec3f( colors->at( idx ), colors->at( idx + 1 ), colors->at( idx + 2 ) ));

			m_pointVerts->push_back(osg::Vec3f( verts->at( idx ), verts->at( idx + 1 ), verts->at( idx + 2 ) ));

			// if k=0 || k=pointPerLine-1 then draw endtexture
			if(k == 0 || k == pointsPerLine->at(i)-1)
			{
				m_pointTexCoords->push_back(osg::Vec2f( 0.0f , 0.0f ));
			}
			else
			{
				m_pointTexCoords->push_back(osg::Vec2f( 0.0f , 1.0f ));
			}

			idx += 3;
		}

	}
	// setting up VBO and PrimitivSets
	m_tubeGeometryPointSprite->setVertexArray( m_pointVerts );
	m_tubeGeometryPointSprite->setColorArray( m_pointColors );
	m_tubeGeometryPointSprite->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
	m_tubeGeometryPointSprite->setNormalArray( m_pointTangents );
	m_tubeGeometryPointSprite->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );
	m_shaderTubesPS->addBindAttribLocation("psTexCoords", VERTEXBIND);
	m_tubeGeometryPointSprite->setVertexAttribArray(VERTEXBIND, m_pointTexCoords );
	m_tubeGeometryPointSprite->setVertexAttribBinding( VERTEXBIND, osg::Geometry::BIND_PER_VERTEX );

	m_tubeGeometryQuadStripes->setVertexArray( m_tubeVerts );
	m_tubeGeometryQuadStripes->setColorArray( m_tubeColors );
	m_tubeGeometryQuadStripes->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
	m_tubeGeometryQuadStripes->setNormalArray( m_tubeTangents );
	m_tubeGeometryQuadStripes->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );
	m_shaderTubesQS->addBindAttribLocation("qsTexCoords", VERTEXBIND);
	m_tubeGeometryQuadStripes->setVertexAttribArray(VERTEXBIND, m_tubeTexCoords );
	m_tubeGeometryQuadStripes->setVertexAttribBinding( VERTEXBIND, osg::Geometry::BIND_PER_VERTEX );

/*
	for ( size_t i = 0; i < pointsPerLine->size(); ++i )
	{
		m_tubeGeometryPointSprite->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::POINTS, m_fiberPointStartIndexes->at(i), (*pointsPerLine)[i] ) );
		m_tubeGeometryQuadStripes->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUADS, m_fiberQuadStartIndexes->at(i), 2 * (*pointsPerLine)[i] ) );

	}
*/
}
