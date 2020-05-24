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

#include <algorithm>
#include <string>
#include <openvr.h>     // NOLINT: this is not a C system header as brainlint thinks

#include <osg/BufferObject>
#include <osg/StateAttribute>
#include <osg/Texture2D>
#include <osg/TexMat>

#include "core/graphicsEngine/postprocessing/WGEPostprocessingNode.h"
#include "core/graphicsEngine/WGEManagedGroupNode.h"
#include "core/graphicsEngine/WGECamera.h"
#include "core/graphicsEngine/WGEGeodeUtils.h"
#include "core/graphicsEngine/WGERequirement.h"

#include "core/common/WPropertyHelper.h"
#include "core/graphicsEngine/WGEUtils.h"
#include "core/graphicsEngine/WGraphicsEngine.h"
#include "core/kernel/WKernel.h"

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

    m_vrOn = m_properties->addProperty( "VR On", "Turns sending frames to Headset on or off", false );

    WModule::properties();
}

void WMVRCamera::requirements()
{
    // NOTE: Refer to WMTemplate.cpp if you do not understand these commands.

    // We need graphics to draw anything:
    m_requirements.push_back( new WGERequirement() );
}

void WMVRCamera::setupVRInterface()
{
    // Exit if we do not have an HMD present
    if(!vr::VR_IsHmdPresent())
    {
        errorLog() << "No valid HMD present!" << std::endl;
        requestStop();
    }

    // Loading the SteamVR Runtime
    vr::EVRInitError eError = vr::VRInitError_None;
    m_vrSystem = vr::VR_Init( &eError, vr::VRApplication_Scene );

    if(eError != vr::VRInitError_None )
    {
        m_vrSystem = nullptr;
        errorLog()
            << "Unable to initialize the OpenVR library." << std::endl
            << "Reason: " << vr::VR_GetVRInitErrorAsEnglishDescription( eError ) << std::endl;
        requestStop();
    }

    if( !vr::VRCompositor() )
    {
        m_vrSystem = nullptr;
        vr::VR_Shutdown();
        errorLog() << "Compositor initialization failed" << std::endl;
        requestStop();
    }

    m_vrRenderModels = ( vr::IVRRenderModels * ) vr::VR_GetGenericInterface( vr::IVRRenderModels_Version, &eError );

    if( m_vrRenderModels == nullptr )
    {
        m_vrSystem = nullptr;
        vr::VR_Shutdown();
        errorLog()
            << "Unable to get render model interface!" << std::endl
            << "Reason: " << vr::VR_GetVRInitErrorAsEnglishDescription( eError ) << std::endl;
        requestStop();
    }

    vr::VRCompositor()->SetTrackingSpace( vr::TrackingUniverseSeated );

    m_samples = 4U;
    std::string driverName = GetDeviceProperty( vr::Prop_TrackingSystemName_String );
    std::string deviceSerialNumber = GetDeviceProperty( vr::Prop_SerialNumber_String );

    debugLog() << "HMD driver name: " << driverName;
    debugLog() << "HMD device serial number: " << deviceSerialNumber;

    m_vrSystem->GetRecommendedRenderTargetSize( &m_vrRenderWidth, &m_vrRenderHeight );
}

void WMVRCamera::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // View Setup
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    // create the root node for all the geometry
    m_output = osg::ref_ptr< WGEManagedGroupNode > ( new WGEManagedGroupNode( m_active ) );

    // create the roots for the Eyes
    m_leftEye = osg::ref_ptr< WGEGroupNode > ( new WGEGroupNode() );
    m_rightEye = osg::ref_ptr< WGEGroupNode > ( new WGEGroupNode() );

    // insert the created nodes
    m_output->insert( m_leftEye );
    m_output->insert( m_rightEye );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_output );

    // updatecallback for submitting frames
    m_output->addUpdateCallback( new SafeUpdateCallback( this ) );

    // add side-views
    boost::shared_ptr< WGEViewer > leftEyeView = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Left Eye View" );
    boost::shared_ptr< WGEViewer > rightEyeView = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Right Eye View" );

    leftEyeView->reset();
    rightEyeView->reset();

    //Trigger m_moduleState.wait if vr is turned on
    //m_moduleState.add(m_vrOn->getCondition());

    // Now, we can mark the module ready.
    ready();
    debugLog() << "Starting...";

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // HMD Setup
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    setupVRInterface();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Render to Texture Setup
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    leftEyeView->getCamera()->setClearMask( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    leftEyeView->getCamera()->setRenderTargetImplementation( osg::Camera::FRAME_BUFFER_OBJECT );
    leftEyeView->getCamera()->setRenderOrder( osg::Camera::PRE_RENDER, vr::Eye_Left );
    leftEyeView->getCamera()->setComputeNearFarMode( osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );

    rightEyeView->getCamera()->setClearMask( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    rightEyeView->getCamera()->setRenderTargetImplementation( osg::Camera::FRAME_BUFFER_OBJECT );
    rightEyeView->getCamera()->setRenderOrder( osg::Camera::PRE_RENDER, vr::Eye_Right );
    rightEyeView->getCamera()->setComputeNearFarMode( osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );

    m_leftTexture = new osg::Texture2D;
    m_leftTexture->setTextureSize( m_vrRenderWidth, m_vrRenderHeight );
    m_leftTexture->setInternalFormat( GL_RGBA8 );
    m_leftTexture->setFilter( osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST );
    m_leftTexture->setFilter( osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR );

    m_rightTexture = new osg::Texture2D;
    m_rightTexture->setTextureSize( m_vrRenderWidth, m_vrRenderHeight );
    m_rightTexture->setInternalFormat( GL_RGBA8 );
    m_rightTexture->setFilter( osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST );
    m_rightTexture->setFilter( osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR );

    //create nodes in the scene-graph, which will encapsulate our render pipeline:
    osg::ref_ptr< WGEOffscreenRenderNode > offscreenl(
        new WGEOffscreenRenderNode(
            leftEyeView->getCamera(),
            m_vrRenderWidth,
            m_vrRenderHeight,
            true
        )
    );
    osg::ref_ptr< WGEOffscreenRenderNode > offscreenr(
        new WGEOffscreenRenderNode(
            rightEyeView->getCamera(),
            m_vrRenderWidth,
            m_vrRenderHeight,
            true
        )
    );

    osg::ref_ptr< WGEOffscreenFinalPass > renderToTexturel = offscreenl->addFinalOnScreenPass( "Output" );

    osg::ref_ptr< WGEOffscreenFinalPass > renderToTexturer = offscreenr->addFinalOnScreenPass( "Output" );

    // attach color0 output
    renderToTexturel->attach( WGECamera::COLOR_BUFFER0, m_leftTexture );
    // attach color0 output
    renderToTexturer->attach( WGECamera::COLOR_BUFFER0, m_rightTexture );

    // The final pass should also blend properly:
    renderToTexturel->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
    // The final pass should also blend properly:
    renderToTexturer->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );

    // provide the Gbuffer input
    renderToTexturel->bind( m_leftTexture, 0 );
    // provide the Gbuffer input
    renderToTexturer->bind( m_rightTexture, 0 );

    m_leftEye->insert( offscreenl );
    m_rightEye->insert( offscreenr );

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Main loop
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        // woke up since the module is requested to finish
        if( m_shutdownFlag() )
        {
            break;
        }

        debugLog() << "Waiting...";
        m_moduleState.wait();
    }

    // Shut down OpenVR
    m_vrSystem = nullptr;
    vr::VR_Shutdown();

    // Never miss to clean up. Especially remove your OSG nodes. Everything else you add to these nodes will be removed automatically.
    debugLog() << "Shutting down VRCamera";

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_output );
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

std::string WMVRCamera::GetDeviceProperty( vr::TrackedDeviceProperty prop )
{
    uint32_t bufferLen = m_vrSystem->GetStringTrackedDeviceProperty( vr::k_unTrackedDeviceIndex_Hmd, prop, NULL, 0 );
    if(bufferLen == 0)
    {
        return "";
    }

    char* buffer = new char[ bufferLen ];
    bufferLen = m_vrSystem->GetStringTrackedDeviceProperty( vr::k_unTrackedDeviceIndex_Hmd, prop, buffer, bufferLen );
    std::string result = buffer;
    delete [] buffer;
    return result;
}

void WMVRCamera::SafeUpdateCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    if( !m_initialUpdate && m_module->m_vrOn->get() )
    {
        vr::TrackedDevicePose_t poses[vr::k_unMaxTrackedDeviceCount];           // NOLINT: the size is constant
        for(uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i) poses[i].bPoseIsValid = false;
        vr::VRCompositor()->WaitGetPoses( poses, vr::k_unMaxTrackedDeviceCount, NULL, 0 );
/*
        osg::ref_ptr< osg::Texture2D > randTexture = wge::genWhiteNoiseTexture( m_module->m_vrRenderWidth, m_module->m_vrRenderHeight, 4 );

        randTexture->setInternalFormat(GL_RGBA8);

        randTexture->generateAndAssignTextureObject(0U,randTexture->getTextureTarget());
*/
        vr::Texture_t leftEyeTexture = {
            ( void* )( uintptr_t )m_module->m_rightTexture->getTextureObject( 0U )->id(),
            vr::TextureType_OpenGL,
            vr::ColorSpace_Gamma
            };
        vr::Texture_t rightEyeTexture = {
            ( void* )( uintptr_t )m_module->m_leftTexture->getTextureObject( 0U )->id(),
            vr::TextureType_OpenGL,
            vr::ColorSpace_Gamma
            };

        vr::EVRCompositorError lError = vr::VRCompositor()->Submit( vr::Eye_Left, &leftEyeTexture );
        vr::EVRCompositorError rError = vr::VRCompositor()->Submit( vr::Eye_Right, &rightEyeTexture );

        if( lError != vr::VRCompositorError_None || rError !=vr::VRCompositorError_None )
        {
            m_module->errorLog() << "Links:" << lError << "|Rechts:" << rError << std::endl;
        }
    }
    m_initialUpdate = false;

    traverse( node, nv );
}
