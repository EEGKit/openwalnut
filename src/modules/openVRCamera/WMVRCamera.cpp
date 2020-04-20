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

// Some rules to the inclusion of headers:
//  * Ordering:
//    * C Headers
//    * C++ Standard headers
//    * External Lib headers (like OSG or Boost headers)
//    * headers of other classes inside OpenWalnut
//    * your own header file

#include <string>

#include <osg/StateAttribute>
#include <osg/Texture2D>
#include <osg/TexMat>

#include "core/graphicsEngine/callbacks/WGELinearTranslationCallback.h"
#include "core/graphicsEngine/callbacks/WGENodeMaskCallback.h"
#include "core/graphicsEngine/callbacks/WGEPropertyUniformCallback.h"
#include "core/graphicsEngine/callbacks/WGEShaderAnimationCallback.h"
#include "core/graphicsEngine/shaders/WGEPropertyUniform.h"
#include "core/graphicsEngine/shaders/WGEShader.h"
#include "core/graphicsEngine/shaders/WGEShaderDefineOptions.h"
#include "core/graphicsEngine/shaders/WGEShaderPropertyDefineOptions.h"
#include "core/graphicsEngine/WGEManagedGroupNode.h"
#include "core/graphicsEngine/WGERequirement.h"
#include "core/graphicsEngine/WGETextureHud.h"
#include "core/graphicsEngine/WGECamera.h"
#include "core/graphicsEngine/WGEColormapping.h"
#include "core/graphicsEngine/WGEGeodeUtils.h"

#include "core/common/math/WMath.h"
#include "core/common/WPropertyHelper.h"
#include "core/graphicsEngine/WGEUtils.h"
#include "core/graphicsEngine/WGraphicsEngine.h"
#include "core/graphicsEngine/WPickHandler.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WSelectionManager.h"

#include "WDemoGeometry.h"
#include "WMVRCamera.h"
#include "WMVRCamera.xpm"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMVRCamera )

WMVRCamera::WMVRCamera():
    WModule()
{
    // WARNING: initializing connectors inside the constructor will lead to an exception.
    // Implement WModule::initializeConnectors instead.
    
}

WMVRCamera::~WMVRCamera()
{
    // cleanup
    // removeConnectors();
}

boost::shared_ptr< WModule > WMVRCamera::factory() const
{
    return boost::shared_ptr< WModule >( new WMVRCamera() );
}

const char** WMVRCamera::getXPMIcon() const
{
    return open_vr_camera_xpm;
}

const std::string WMVRCamera::getName() const
{
    return "OpenVRCamera";
}

const std::string WMVRCamera::getDescription() const
{
    return "This module will start an OpenVR session"
" right now it only adds an extra camera that renders to a texture";
}

void WMVRCamera::connectors()
{

    // call WModules initialization
    WModule::connectors();
}

void WMVRCamera::properties()
{
    // NOTE: Refer to WMTemplate.cpp if you do not understand these commands.
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_noTransparency  = m_properties->addProperty( "No transparency", "If checked, transparency is not used. This will show the complete slices.",
                                                   false );
                                                   
    m_sliceGroup      = m_properties->addPropertyGroup( "Slices",  "Slice Options." );
    // enable slices
    // Flags denoting whether the glyphs should be shown on the specific slice
    m_showonLeftEye = m_sliceGroup->addProperty( WKernel::getRunningKernel()->getSelectionManager()->getPropSagittalShow() );
    

    // The slice positions.
    m_xPos    = m_sliceGroup->addProperty( WKernel::getRunningKernel()->getSelectionManager()->getPropSagittalPos() );
    m_yPos    = m_sliceGroup->addProperty( WKernel::getRunningKernel()->getSelectionManager()->getPropCoronalPos() );
    m_zPos    = m_sliceGroup->addProperty( WKernel::getRunningKernel()->getSelectionManager()->getPropAxialPos() );
    // show hud?
    m_showHUD = m_properties->addProperty( "Show HUD", "Check to enable the debugging texture HUD.", true );

    WModule::properties();
}

void WMVRCamera::requirements()
{
    // NOTE: Refer to WMTemplate.cpp if you do not understand these commands.

    // We need graphics to draw anything:
    m_requirements.push_back( new WGERequirement() );
}

WMVRCamera::PickCallback::PickCallback( osg::ref_ptr< osg::Node > node, WPropDouble property, bool negateDirection ):
    m_node( node ),
    m_property( property ),
    m_pickUniform( new osg::Uniform( "u_picked", 0.0f ) ),
    m_dir( negateDirection ? -1.0 : 1.0 )
{
    boost::shared_ptr< WGraphicsEngine > ge = WGraphicsEngine::getGraphicsEngine();
    boost::shared_ptr< WGEViewer > viewer = ge->getViewerByName( "Main View" );
    m_camera = viewer->getCamera();
    m_pickConnection = viewer->getPickHandler()->getPickSignal()->connect( boost::bind( &WMVRCamera::PickCallback::pick, this, _1 ) );
    node->getOrCreateStateSet()->addUniform( m_pickUniform );
}

void WMVRCamera::PickCallback::pick( WPickInfo pickInfo )
{
    if( pickInfo.getName() == m_node->getName() )
    {
        WVector3d normal = pickInfo.getPickNormal();
        WVector2d newPixelPos = pickInfo.getPickPixel();
        // dragging was initialized earlier
        if( m_isPicked )
        {
            osg::Vec3 startPosScreen( m_oldPixelPosition[ 0 ], m_oldPixelPosition[ 1 ], 0.0 );
            osg::Vec3 endPosScreen( newPixelPos[ 0 ], newPixelPos[ 1 ], 0.0 );

            osg::Vec3 startPosWorld = wge::unprojectFromScreen( startPosScreen, m_camera );
            osg::Vec3 endPosWorld = wge::unprojectFromScreen( endPosScreen, m_camera );

            osg::Vec3 moveDirWorld = endPosWorld - startPosWorld;
            float diff = moveDirWorld * static_cast< osg::Vec3 >( normal );

            m_property->set( m_property->get() + m_dir * diff );
        }
        // this might have initialized dragging. Keep track of old position
        m_oldPixelPosition = newPixelPos;
        m_isPicked = true;
        m_pickUniform->set( 1.0f );
    }
    else    // someone else got picked.
    {
        m_isPicked = false;
        m_pickUniform->set( 0.0f );
    }
}

void WMVRCamera::initOSG()
{
    
    debugLog() << "Starting2.1...";
    // remove the old slices
    m_output->clear();
    m_leftEye->clear();

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Property Setup
    ///////////////////////////////////////////////////////////////////////////////////////////////

    debugLog() << "Starting2.5...";
    // no colormaps -> no slices
    bool empty = !WGEColormapping::instance()->size();
    if( empty )
    {
        debugLog() << "Starting2.6...";
        // hide the slider properties.
        m_xPos->setHidden();
        m_yPos->setHidden();
        m_zPos->setHidden(); 
        debugLog() << "Starting2.7...";
        return;
    }
    
    debugLog() << "Starting3...";
    // grab the current bounding box
    WBoundingBox bb = WGEColormapping::instance()->getBoundingBox();
    WVector3d minV = bb.getMin();
    WVector3d maxV = bb.getMax();
    WVector3d sizes = ( maxV - minV );
    WVector3d midBB = minV + ( sizes * 0.5 );

    
    // update the properties
    m_xPos->setMin( minV[0] );
    m_xPos->setMax( maxV[0] );
    m_yPos->setMin( minV[1] );
    m_yPos->setMax( maxV[1] );
    m_zPos->setMin( minV[2] );
    m_zPos->setMax( maxV[2] );
    // un-hide the slider properties.
    m_xPos->setHidden( false );
    m_yPos->setHidden( false );
    m_zPos->setHidden( false );

    // always update slice positions if they happen to be outside the bounding box (i.e. after shrinking the box)
    m_xPos->setRecommendedValue( midBB[0] );
    m_yPos->setRecommendedValue( midBB[1] );
    m_zPos->setRecommendedValue( midBB[2] );
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Navigation View Setup
    ///////////////////////////////////////////////////////////////////////////////////////////////

    double maxSizeInv = 1.0 / std::max( sizes[0], std::max( sizes[1], sizes[2] ) );
    m_leftEye->setMatrix(
        osg::Matrixd::translate( -midBB[0], -midBB[1], -midBB[2] ) *
        osg::Matrixd::scale( maxSizeInv, maxSizeInv, maxSizeInv ) *
        osg::Matrixd::translate( 0.0, 0.0, -0.5 )
    );

    debugLog() << "Starting4...";
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Slice Setup
    ///////////////////////////////////////////////////////////////////////////////////////////////

    // create a new geode containing the slices

    // X Slice
    osg::ref_ptr< osg::Node > leftEyeSlice = wge::genFinitePlane( minV, osg::Vec3( 0.0, sizes[1], 0.0 ),
                                                                  osg::Vec3( 0.0, 0.0, sizes[2] ) );
    leftEyeSlice->setName( "Left Eye Slice" );
    osg::Uniform* leftEyeSliceUniform = new osg::Uniform( "u_WorldTransform", osg::Matrixf::identity() );
    leftEyeSlice->getOrCreateStateSet()->addUniform( leftEyeSliceUniform );
    
    // disable culling.
    // NOTE: Somehow, this is ignore by OSG. If you know why: tell me please
    leftEyeSlice->setCullingActive( false );

    // each slice is child of an transformation node
    osg::ref_ptr< osg::MatrixTransform > mLeftEye = new osg::MatrixTransform();
    mLeftEye->addChild( leftEyeSlice );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Callback Setup
    ///////////////////////////////////////////////////////////////////////////////////////////////

    // Control transformation node by properties. We use an additional uniform here to provide the shader the transformation matrix used to
    // translate the slice.
    mLeftEye->addUpdateCallback( new WGELinearTranslationCallback< WPropDouble >( osg::Vec3( 1.0, 0.0, 0.0 ), m_xPos, leftEyeSliceUniform ) );

    // set callbacks for en-/disabling the nodes
    leftEyeSlice->addUpdateCallback( new WGENodeMaskCallback( m_showonLeftEye ) );

    // set the pick callbacks for each slice
    m_leftEyeSlicePicker = PickCallback::SPtr( new PickCallback( leftEyeSlice, m_xPos ) );

    // transparency property
    osg::ref_ptr< osg::Uniform > transparencyUniform = new osg::Uniform( "u_noTransparency", false );
    transparencyUniform->setUpdateCallback( new WGEPropertyUniformCallback< WPropBool >( m_noTransparency ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Done
    ///////////////////////////////////////////////////////////////////////////////////////////////
    osg::ref_ptr< osg::StateSet > state = m_output->getOrCreateStateSet();
    state->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

    debugLog() << "Starting5...";
    // we want some nice animations: add timer
    osg::ref_ptr< osg::Uniform > animationUniform = new osg::Uniform( "u_timer", 0 );
    state->addUniform( animationUniform );
    animationUniform->setUpdateCallback( new WGEShaderAnimationCallback() );

    // transparency property
    state->addUniform( transparencyUniform );
    m_leftEye->getOrCreateStateSet()->addUniform( transparencyUniform );

    // add the transformation nodes to the output group
    m_output->insert( mLeftEye );

    // add proxy
    m_output->insert( wge::generateCullProxy( bb ) );

    m_output->dirtyBound();

    debugLog() << "Starting6...";
    m_leftEye->insert( mLeftEye );
}

void WMVRCamera::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );

    // Now, we can mark the module ready.
    ready();
    debugLog() << "Starting...";

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Eye Setup
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    // create the root node for all the geometry
    m_output = osg::ref_ptr< WGEManagedGroupNode > ( new WGEManagedGroupNode( m_active ) );

    // create the roots for the Eyes
    m_leftEye = osg::ref_ptr< WGEGroupNode > ( new WGEGroupNode() );

    
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_output );

    // add for side-views
    boost::shared_ptr< WGEViewer > v = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Left Eye View" );
    if( v )
    {
        v->getScene()->insert( m_leftEye );
    }

    // disable the pick-coloring for the side views
    m_output->getOrCreateStateSet()->addUniform( new osg::Uniform( "u_pickColorEnabled", 1.0f ) );
    m_leftEye->getOrCreateStateSet()->addUniform( new osg::Uniform( "u_pickColorEnabled", 1.0f ) );

    m_output->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
    m_leftEye->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );

    
    // apply colormapping to all the nodes
    osg::ref_ptr< WGEShader > shader = new WGEShader( "WMNavigationSlices", m_localPath );
    WGEColormapping::NodeList nodes;
    nodes.push_back( m_output );
    nodes.push_back( m_leftEye );
    WGEColormapping::apply( nodes, shader ); // this automatically applies the shader

    // we need to be informed if the bounding box of the volume containing all the data changes.
    m_moduleState.add( WGEColormapping::instance()->getChangeCondition() );

    debugLog() << "Starting2...";
    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        // woke up since the module is requested to finish
        if( m_shutdownFlag() )
        {
            break;
        }

        
        // create the Eyes. This loop is only entered if WGEColormapper was fired or shutdown.
        initOSG();


        debugLog() << "Waiting...";

        m_moduleState.wait();
    }

    // Never miss to clean up. Especially remove your OSG nodes. Everything else you add to these nodes will be removed automatically.
    debugLog() << "Shutting down VRCamera";

    m_leftEyeSlicePicker.reset();

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_output );

    v = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Left Eye View" );
    if( v )
    {
        v->getScene()->remove( m_leftEye );
    }
}

void WMVRCamera::activate()
{
    // This method gets called, whenever the m_active property changes. Your module should always handle this if you do not use the
    // WGEManagedGroupNode for your scene. The user can (de-)activate modules in his GUI and you can handle this case here:
    if( m_active->get() )
    {
        debugLog() << "Activate.";
    }
    else
    {
        debugLog() << "Deactivate.";
    }

    // The simpler way is by using WGEManagedGroupNode which deactivates itself according to m_active. See moduleMain for details.

    // Always call WModule's activate!
    WModule::activate();
}