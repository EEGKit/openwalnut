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

#include <osg/StateAttribute>
#include <osg/Texture2D>
#include <osg/TexMat>

#include "core/graphicsEngine/WGEManagedGroupNode.h"
#include "core/graphicsEngine/WGECamera.h"
#include "core/graphicsEngine/WGEGeodeUtils.h"
#include "core/graphicsEngine/WGERequirement.h"

#include "core/common/WPropertyHelper.h"
#include "core/graphicsEngine/WGEUtils.h"
#include "core/graphicsEngine/WGraphicsEngine.h"
#include "core/kernel/WKernel.h"

#include "OpenVRTextureBuffer.h"
#include "OpenVRDrawCallback.h"
#include "WMVRCamera.h"
#include "WMVRCamera.xpm"

#ifndef GL_TEXTURE_MAX_LEVEL
    #define GL_TEXTURE_MAX_LEVEL 0x813D
#endif

static const OSG_GLExtensions* getGLExtensions(const osg::State& state)
{
#if(OSG_VERSION_GREATER_OR_EQUAL(3, 4, 0))
    return state.get<osg::GLExtensions>();
#else
    return osg::FBOExtensions::instance(state.getContextID(), true);
#endif
}

static const OSG_Texture_Extensions* getTextureExtensions(const osg::State& state)
{
#if(OSG_VERSION_GREATER_OR_EQUAL(3, 4, 0))
    return state.get<osg::GLExtensions>();
#else
    return osg::Texture::getExtensions(state.getContextID(), true);
#endif
}

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
    m_rightEye = osg::ref_ptr< WGEGroupNode > ( new WGEGroupNode() );

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_output );

    // add for side-views
    boost::shared_ptr< WGEViewer > leftEyeView = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Left Eye View" );
    boost::shared_ptr< WGEViewer > rightEyeView = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Right Eye View" );

    leftEyeView->reset();
    rightEyeView->reset();

    // Exit if we do not have an HMD present
    if (!vr::VR_IsHmdPresent())
    {
        debugLog() << "Error: No valid HMD present!" << std::endl;
        return;
    }

    // Attach a callback to detect swap
    osg::ref_ptr<OpenVRSwapCallback> swapCallback = new OpenVRSwapCallback(this);
    leftEyeView->getCamera()->getGraphicsContext()->setSwapCallback(swapCallback);

    /*
    m_HMD = osg::ref_ptr< OpenVRDevice >(new OpenVRDevice(0.0F, 1.0F, 1.0F));
    m_HMD->setRTTCamera(OpenVRDevice::LEFT, leftEyeView->getCamera());
    m_HMD->setRTTCamera(OpenVRDevice::RIGHT, rightEyeView->getCamera());

    // Exit if we fail to initialize the HMD device
    if (!m_HMD->hmdInitialized())
    {
        // The reason for failure was reported by the constructor.
        return;
    }

    leftEyeView->getView()->setRealizeOperation(new OpenVRRealizeOperation(m_HMD));
    rightEyeView->getView()->setRealizeOperation(new OpenVRRealizeOperation(m_HMD));
    leftEyeView->getView()->addEventHandler(new OpenVREventHandler(m_HMD));
    rightEyeView->getView()->addEventHandler(new OpenVREventHandler(m_HMD));
    */
    // Loading the SteamVR Runtime
    vr::EVRInitError eError = vr::VRInitError_None;
    m_vrSystem = vr::VR_Init(&eError, vr::VRApplication_Scene);

    if (eError != vr::VRInitError_None)
    {
        m_vrSystem = nullptr;
        debugLog()
            << "Error: Unable to initialize the OpenVR library.\n"
            << "Reason: " << vr::VR_GetVRInitErrorAsEnglishDescription( eError ) << std::endl;
        return;
    }

    if ( !vr::VRCompositor() )
    {
        m_vrSystem = nullptr;
        vr::VR_Shutdown();
        debugLog() << "Error: Compositor initialization failed" << std::endl;
        return;
    }

    m_vrRenderModels = (vr::IVRRenderModels *)vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &eError);

    if (m_vrRenderModels == nullptr)
    {
        m_vrSystem = nullptr;
        vr::VR_Shutdown();
        debugLog()
            << "Error: Unable to get render model interface!\n"
            << "Reason: " << vr::VR_GetVRInitErrorAsEnglishDescription( eError ) << std::endl;
        return;
    }

    m_samples=4;
    std::string driverName = GetDeviceProperty(vr::Prop_TrackingSystemName_String);
    std::string deviceSerialNumber = GetDeviceProperty(vr::Prop_SerialNumber_String);

    debugLog() << "HMD driver name: "<< driverName << std::endl;
    debugLog() << "HMD device serial number: " << deviceSerialNumber << std::endl;

    leftEyeView->getCamera()->setInitialDrawCallback(new OpenVRInitialDrawCallback());
    rightEyeView->getCamera()->setInitialDrawCallback(new OpenVRInitialDrawCallback());

    leftEyeView->getCamera()->setPreDrawCallback(new OpenVRPreDrawCallback(leftEyeView->getCamera(), m_textureBuffer[0]));
    rightEyeView->getCamera()->setPreDrawCallback(new OpenVRPreDrawCallback(rightEyeView->getCamera(), m_textureBuffer[1]));

    leftEyeView->getCamera()->setFinalDrawCallback(new OpenVRPostDrawCallback(leftEyeView->getCamera().get(), m_textureBuffer[0]));
    rightEyeView->getCamera()->setFinalDrawCallback(new OpenVRPostDrawCallback(rightEyeView->getCamera().get(), m_textureBuffer[1]));

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

        // create the Eyes. This loop is only entered if WGEColormapper was fired or shutdown.
        // initOSG();

        debugLog() << "Waiting...";
        m_moduleState.wait();
    }

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

std::string WMVRCamera::GetDeviceProperty(vr::TrackedDeviceProperty prop)
{
    uint32_t bufferLen = m_vrSystem->GetStringTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, prop, NULL, 0);
    if (bufferLen == 0)
    {
        return "";
    }

    char* buffer = new char[bufferLen];
    bufferLen = m_vrSystem->GetStringTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, prop, buffer, bufferLen);
    std::string result = buffer;
    delete [] buffer;
    return result;
}

void WMVRCamera::createRenderBuffers(osg::ref_ptr<osg::State> state)
{
    uint32_t renderWidth = 0;
    uint32_t renderHeight = 0;
    m_vrSystem->GetRecommendedRenderTargetSize(&renderWidth, &renderHeight);

    for (int i = 0; i < 2; i++)
    {
        m_textureBuffer[i] = new OpenVRTextureBuffer(state, renderWidth, renderHeight, m_samples);
    }

    int mirrorWidth = 800;
    int mirrorHeight = 450;
    m_mirrorTexture = new OpenVRMirrorTexture(state, mirrorWidth, mirrorHeight);
}

bool WMVRCamera::submitFrame()
{
    vr::Texture_t leftEyeTexture = {(void*)m_textureBuffer[0]->getTexture()};
    vr::Texture_t rightEyeTexture = {(void*)m_textureBuffer[1]->getTexture()};

    debugLog()<<"test2";
    vr::EVRCompositorError lError = vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
    debugLog()<<"test2.5";
    vr::EVRCompositorError rError = vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);

    debugLog()<<"test3";
    return lError == vr::VRCompositorError_None && rError == vr::VRCompositorError_None;
}

void WMVRCamera::blitMirrorTexture(osg::GraphicsContext* gc)
{
    m_mirrorTexture->blitTexture(gc, m_textureBuffer[0], m_textureBuffer[1]);
}

void OpenVRSwapCallback::swapBuffersImplementation(osg::GraphicsContext* gc)
{
    // Submit rendered frame to compositor
    m_device->submitFrame();

    // Blit mirror texture to backbuffer
    m_device->blitMirrorTexture(gc);

    // Run the default system swapBufferImplementation
    gc->swapBuffersImplementation();
}


OpenVRMirrorTexture::OpenVRMirrorTexture(osg::ref_ptr<osg::State> state, GLint width, GLint height) : 
    m_width(width),
    m_height(height)
{
    const OSG_GLExtensions* fbo_ext = getGLExtensions(*state);

    // Configure the mirror FBO
    fbo_ext->glGenFramebuffers(1, &m_mirrorFBO);
    fbo_ext->glBindFramebuffer(GL_READ_FRAMEBUFFER_EXT, m_mirrorFBO);

    glGenTextures(1, &m_mirrorTex);
    glBindTexture(GL_TEXTURE_2D, m_mirrorTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    fbo_ext->glFramebufferTexture2D(GL_READ_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_mirrorTex, 0);
    fbo_ext->glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, 0);
    fbo_ext->glBindFramebuffer(GL_READ_FRAMEBUFFER_EXT, 0);

}

void OpenVRMirrorTexture::blitTexture(osg::GraphicsContext* gc, OpenVRTextureBuffer* leftEye,  OpenVRTextureBuffer* rightEye)
{
    const OSG_GLExtensions* fbo_ext = getGLExtensions(*(gc->getState()));

    fbo_ext->glBindFramebuffer(GL_DRAW_FRAMEBUFFER_EXT, m_mirrorFBO);
    fbo_ext->glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_mirrorTex, 0);
    fbo_ext->glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, 0);

    glClearColor(1, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //--------------------------------
    // Copy left eye image to mirror
    fbo_ext->glBindFramebuffer(GL_READ_FRAMEBUFFER_EXT, leftEye->m_Resolve_FBO);
    fbo_ext->glFramebufferTexture2D(GL_READ_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D,leftEye->m_Resolve_ColorTex, 0);
    fbo_ext->glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, 0);

    fbo_ext->glBlitFramebuffer(0, 0, leftEye->m_width, leftEye->m_height,
                               0, 0, m_width / 2, m_height,
                               GL_COLOR_BUFFER_BIT, GL_NEAREST);
    //--------------------------------
    // Copy right eye image to mirror
    fbo_ext->glBindFramebuffer(GL_READ_FRAMEBUFFER_EXT, rightEye->m_Resolve_FBO);
    fbo_ext->glFramebufferTexture2D(GL_READ_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, rightEye->m_Resolve_ColorTex, 0);
    fbo_ext->glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, 0);

    fbo_ext->glBlitFramebuffer(0, 0, rightEye->m_width, rightEye->m_height,
                               m_width / 2, 0, m_width, m_height,
                               GL_COLOR_BUFFER_BIT, GL_NEAREST);
    //---------------------------------

    fbo_ext->glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);

    // Blit mirror texture to back buffer
    fbo_ext->glBindFramebuffer(GL_READ_FRAMEBUFFER_EXT, m_mirrorFBO);
    fbo_ext->glBindFramebuffer(GL_DRAW_FRAMEBUFFER_EXT, 0);
    GLint w = m_width;
    GLint h = m_height;
    fbo_ext->glBlitFramebuffer(0, 0, w, h,
                               0, 0, w, h,
                               GL_COLOR_BUFFER_BIT, GL_NEAREST);
    fbo_ext->glBindFramebuffer(GL_READ_FRAMEBUFFER_EXT, 0);
}

void OpenVRMirrorTexture::destroy(osg::GraphicsContext* gc)
{
    const OSG_GLExtensions* fbo_ext = getGLExtensions(*gc->getState());
    if (fbo_ext)
    {
        fbo_ext->glDeleteFramebuffers(1, &m_mirrorFBO);
    }
}