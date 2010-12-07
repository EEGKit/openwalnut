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

#include <osg/Geode>
#include <osg/Geometry>

#include "../../common/WColor.h"
#include "../../common/WLogger.h"
#include "../../common/WPathHelper.h"
#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/WDataTexture3D.h"
#include "../../dataHandler/WSubject.h"
#include "../../graphicsEngine/WGEUtils.h"
#include "../../kernel/WKernel.h"
#include "fiberdisplay2.xpm"
#include "WMFiberDisplay.h"

W_LOADABLE_MODULE( WMFiberDisplay )

WMFiberDisplay::WMFiberDisplay()
    : WModule(),
      m_noData( new WCondition, true ),
      m_osgNode( osg::ref_ptr< osg::Group >() )
{
    m_textureChanged = true;
}

WMFiberDisplay::~WMFiberDisplay()
{
}

boost::shared_ptr< WModule > WMFiberDisplay::factory() const
{
    return boost::shared_ptr< WModule >( new WMFiberDisplay() );
}

const char** WMFiberDisplay::getXPMIcon() const
{
    return fiberdisplay2_xpm;
}

void WMFiberDisplay::connectors()
{
    using boost::shared_ptr;
    typedef WModuleInputData< const WDataSetFibers > FiberInputData;  // just an alias
    typedef WModuleOutputData< WDataSetFibers > FiberOutputData;  // just an alias

    m_fiberInput = shared_ptr< FiberInputData >( new FiberInputData( shared_from_this(), "fiberInput", "A loaded fiber dataset." ) );
    m_fiberOutput = shared_ptr< FiberOutputData >( new FiberOutputData( shared_from_this(), "fiberOutput", "The selected fibers." ) );

    addConnector( m_fiberInput );
    addConnector( m_fiberOutput );

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

    m_uniformTubeThickness = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "u_thickness", static_cast<float>( m_tubeThickness->get() ) ) );
    rootState->addUniform( m_uniformTubeThickness );

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
}

void WMFiberDisplay::initCullBox()
{
    wmath::WPosition crossHairPos = WKernel::getRunningKernel()->getSelectionManager()->getCrosshair()->getPosition();
    wmath::WPosition minROIPos = crossHairPos - wmath::WPosition( 10., 10., 10. );
    wmath::WPosition maxROIPos = crossHairPos + wmath::WPosition( 10., 10., 10. );

    m_cullBox = osg::ref_ptr< WROIBox >( new WROIBox( minROIPos, maxROIPos ) );
    m_cullBox->setColor( osg::Vec4( 1.0, 0., 1.0, 0.4 ) );
}

void WMFiberDisplay::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_useTubesProp = m_properties->addProperty( "Use tubes", "Draw fiber tracts as fake tubes.", false, m_propCondition );
    m_useTextureProp = m_properties->addProperty( "Use texture", "Texture fibers with the texture on top of the list.", false, m_propCondition );
    m_tubeThickness = m_properties->addProperty( "Tube thickness", "Adjusts the thickness of the tubes.", 50., m_propCondition );
    m_tubeThickness->setMin( 0 );
    m_tubeThickness->setMax( 300 );

    m_cullBoxGroup    = m_properties->addPropertyGroup( "Box Culling",  "Properties only related to the box culling.", m_propCondition );
    m_activateCullBox = m_cullBoxGroup->addProperty( "Activate", "Activates the cull box", false, m_propCondition );
    m_showCullBox     = m_cullBoxGroup->addProperty( "Show cull box", "Shows/hides the cull box", false, m_propCondition );
    m_insideCullBox   = m_cullBoxGroup->addProperty( "Inside - outside", "Show fibers inside or outside the cull box", true, m_propCondition );

    m_propUpdateOutputTrigger = m_properties->addProperty( "Update Output", "Update!", WPVBaseTypes::PV_TRIGGER_READY, m_propCondition );
}


void WMFiberDisplay::moduleMain()
{
    // setup shaders
    m_shaderTubes = osg::ref_ptr< WShader > ( new WShader( "WMFiberDisplay2-FakeTubes", m_localPath ) );
    m_shaderTexturedFibers = osg::ref_ptr< WShader > ( new WShader( "WMFiberDisplay2-Textured", m_localPath ) );

    // additional fire-condition: "data changed" flag
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_fiberInput->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );
    m_moduleState.add( m_active->getUpdateCondition() );

    // now, to watch changing/new textures use WSubject's change condition
    boost::signals2::connection con = WDataHandler::getDefaultSubject()->getChangeCondition()->subscribeSignal(
            boost::bind( &WMFiberDisplay::notifyTextureChange, this ) );

    initCullBox();

    m_cullBox->hide();

    ready();

    while ( !m_shutdownFlag() ) // loop until the module container requests the module to quit
    {
        m_moduleState.wait(); // waits for firing of m_moduleState ( dataChanged, shutdown, etc. )

        if ( m_shutdownFlag() )
        {
            break;
        }

        // data changed?
        if ( m_dataset != m_fiberInput->getData() )
        {
            inputUpdated();
        }

        if ( m_showCullBox->changed() )
        {
            if( m_showCullBox->get() )
            {
                m_cullBox->unhide();
            }
            else
            {
                m_cullBox->hide();
            }
        }

       if ( m_propUpdateOutputTrigger->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
       {
            updateOutput();
            m_propUpdateOutputTrigger->set( WPVBaseTypes::PV_TRIGGER_READY, false );
       }
    }

    con.disconnect();

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_osgNode );
}

void WMFiberDisplay::inputUpdated()
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
        return;
    }
    infoLog() << "Fiber dataset for display with: " << m_dataset->size() << " fibers loaded.";

    if( m_dataset->size() != 0 ) // in case of an empty fiber dataset nothing is to display
    {
        boost::shared_ptr< WProgress > progress = boost::shared_ptr< WProgress >( new WProgress( "Fiber Display", 2 ) );
        m_progress->addSubProgress( progress );

        WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->removeChild( m_osgNode.get() );
        ++*progress;
        m_fiberSelector = boost::shared_ptr<WFiberSelector>( new WFiberSelector( m_dataset ) );
        ++*progress;
        create();
        progress->finish();

        m_noData.set( false );
    }
    else
    {
        warnLog() << "Nothing to display for an empty fiber dataset";
    }
}

void WMFiberDisplay::create()
{
    // create new node
    osg::ref_ptr< osg::Group > osgNodeNew = osg::ref_ptr< osg::Group >( new osg::Group );

    m_fiberDrawable = osg::ref_ptr< WFiberDrawable >( new WFiberDrawable );
    m_fiberDrawable->setBoundingBox( osg::BoundingBox( m_dataset->getBoundingBox().first[0],
                                                      m_dataset->getBoundingBox().first[1],
                                                      m_dataset->getBoundingBox().first[2],
                                                      m_dataset->getBoundingBox().second[0],
                                                      m_dataset->getBoundingBox().second[1],
                                                      m_dataset->getBoundingBox().second[2] ) );

    m_fiberDrawable->setStartIndexes( m_dataset->getLineStartIndexes() );
    m_fiberDrawable->setPointsPerLine( m_dataset->getLineLengths() );
    m_fiberDrawable->setVerts( m_dataset->getVertices() );
    m_fiberDrawable->setTangents( m_dataset->getTangents() );
    m_fiberDrawable->setColor( m_dataset->getGlobalColors() );
    m_fiberDrawable->setBitfield( m_fiberSelector->getBitfield() );

    m_fiberDrawable->setUseDisplayList( false );
    m_fiberDrawable->setDataVariance( osg::Object::DYNAMIC );

    osg::ref_ptr< osg::Geode > geode = osg::ref_ptr< osg::Geode >( new osg::Geode );
    geode->addDrawable( m_fiberDrawable );

    osgNodeNew->addChild( geode );

    osgNodeNew->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

    osgNodeNew->addUpdateCallback( new WGEFunctorCallback< osg::Node >( boost::bind( &WMFiberDisplay::updateCallback, this ) ) );
    // remove previous nodes if there are any
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->removeChild( m_osgNode.get() );

    m_osgNode = osgNodeNew;

    activate();

    osg::StateSet* rootState = m_osgNode->getOrCreateStateSet();
    initUniforms( rootState );

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->addChild( m_osgNode.get() );
}

void WMFiberDisplay::update()
{
    if( m_osgNode && m_noData.changed() )
    {
        if ( m_noData.get( true ) )
        {
            m_osgNode->setNodeMask( 0x0 );
        }
        else
        {
            m_osgNode->setNodeMask( 0xFFFFFFFF );
        }
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
            m_fiberDrawable->setUseTubes( true );
            m_shaderTubes->apply( m_osgNode );
            m_uniformUseTexture->set( m_useTextureProp->get( true ) );
        }
        else if ( ( m_useTextureProp->get( true ) && !m_useTubesProp->get() ) || m_activateCullBox->get( true) )
        {
            m_fiberDrawable->setUseTubes( false );
            updateTexture();
            m_shaderTubes->deactivate( m_osgNode );
            m_shaderTexturedFibers->apply( m_osgNode );
            m_uniformUseTexture->set( m_useTextureProp->get() );
        }
        else
        {
            m_fiberDrawable->setUseTubes( false );
            m_shaderTubes->deactivate( m_osgNode );
            m_shaderTexturedFibers->deactivate( m_osgNode );
        }
    }

    if  ( !m_useTextureProp->get() && !m_useTubesProp->get() )
    {
        rootState->setTextureMode( 0, GL_TEXTURE_3D, osg::StateAttribute::OFF );
    }
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

void WMFiberDisplay::notifyTextureChange()
{
    m_textureChanged = true;
}

void WMFiberDisplay::updateCallback()
{
    update();

    if( m_dataset )
    {
        m_fiberDrawable->setColor( m_dataset->getColorScheme()->getColor() );
    }

    if ( m_tubeThickness->changed() && m_useTubesProp->get() )
    {
        m_uniformTubeThickness->set( static_cast<float>( m_tubeThickness->get( true ) ) );
    }

    updateRenderModes();
}

void WMFiberDisplay::updateOutput()
{
    boost::shared_ptr< std::vector< bool > > active = m_fiberSelector->getBitfield();
    boost::shared_ptr< std::vector< float > >vertices = boost::shared_ptr< std::vector< float > >( new std::vector< float >() );
    boost::shared_ptr< std::vector< size_t > > lineStartIndexes = boost::shared_ptr< std::vector< size_t > > ( new std::vector< size_t >() );
    boost::shared_ptr< std::vector< size_t > > lineLengths = boost::shared_ptr< std::vector< size_t > >( new std::vector< size_t >() );
    boost::shared_ptr< std::vector< size_t > > verticesReverse = boost::shared_ptr< std::vector< size_t > >( new std::vector< size_t >() );
    std::pair< wmath::WPosition, wmath::WPosition > boundingBox = m_dataset->getBoundingBox();

    size_t countLines = 0;

    for ( size_t l = 0; l < active->size(); ++l )
    {
        if ( ( *active )[l] )
        {
            size_t pc = m_dataset->getLineStartIndexes()->at( l ) * 3;

            lineStartIndexes->push_back( vertices->size() / 3 );
            lineLengths->push_back( m_dataset->getLineLengths()->at( l ) );

            for ( size_t j = 0; j < m_dataset->getLineLengths()->at( l ); ++j )
            {
                vertices->push_back( m_dataset->getVertices()->at( pc ) );
                ++pc;
                vertices->push_back( m_dataset->getVertices()->at( pc ) );
                ++pc;
                vertices->push_back( m_dataset->getVertices()->at( pc ) );
                ++pc;
                verticesReverse->push_back( countLines );
            }
            ++countLines;
        }
    }
    boost::shared_ptr< WDataSetFibers> newOutput =
            boost::shared_ptr< WDataSetFibers>( new WDataSetFibers( vertices, lineStartIndexes, lineLengths, verticesReverse, boundingBox ) );
    m_fiberOutput->updateData( newOutput );
}
