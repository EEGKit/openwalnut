//---------------------------------------------------------------------------
//
// Project: OpenWalnut(   http://www.openwalnut.org   )
//
// Copyright 2009 OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS
// For more information see http://www.openwalnut.org/copying
//
// This file is part of OpenWalnut.
//
// OpenWalnut is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
//(  at your option  ) any later version.
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
//    * External Lib headers(  like OSG or Boost headers  )
//    * headers of other classes inside OpenWalnut
//    * your own header file

#include <algorithm>
#include <string>
#include <openvr.h> // NOLINT: this is not a C system header as brainlint thinks

#include <osg/BufferObject>
#include <osg/StateAttribute>
#include <osg/Texture2D>
#include <osg/TexMat>
#include <osgDB/WriteFile>
//#include <QtCore/QDir>

#include "core/graphicsEngine/postprocessing/WGEPostprocessingNode.h"
#include "core/graphicsEngine/WGEManagedGroupNode.h"
#include "core/graphicsEngine/WGECamera.h"
#include "core/graphicsEngine/WGEGeodeUtils.h"
#include "core/graphicsEngine/WGERequirement.h"
#include "core/graphicsEngine/WGEZoomTrackballManipulator.h"

#include "core/common/WPropertyHelper.h"
#include "core/graphicsEngine/WGEUtils.h"
#include "core/graphicsEngine/WGraphicsEngine.h"
#include "core/kernel/WKernel.h"

#include "WMVRCamera.h"
#include "WMVRCamera.xpm"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE(  WMVRCamera )

WMVRCamera::WMVRCamera() : WModule()
{
    // WARNING: initializing connectors inside the constructor will lead to an exception.
    // Implement WModule::initializeConnectors instead.
}

WMVRCamera::~WMVRCamera()
{
    // cleanup
    // removeConnectors();
}

std::shared_ptr<WModule> WMVRCamera::factory() const
{
    return std::shared_ptr<WModule>( new WMVRCamera() );
}

const char **WMVRCamera::getXPMIcon() const
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
    m_propCondition = std::shared_ptr<WCondition>( new WCondition() );

    m_vrOn = m_properties->addProperty( "Submit Frames", "Toggle submitting frames to OpenVR",
                                       false );
    m_VR_fpsTrigger = m_properties->addProperty( "Log Fps", "Now",
                                                WPVBaseTypes::PV_TRIGGER_READY );
    m_VR_logCameraManipulators = m_properties->addProperty( "Log Camera Manipulators", "Now",
                                                           WPVBaseTypes::PV_TRIGGER_READY );
    m_VR_screenshotTrigger = m_properties->addProperty( "Screenshot Main", "Speichern.",
                                                       WPVBaseTypes::PV_TRIGGER_READY );
    m_VR_cameraManipTrigger = m_properties->addProperty( "Apply MainView Camera Manipulation", "Apply",
                                                        WPVBaseTypes::PV_TRIGGER_READY );

    WModule::properties();
}

void WMVRCamera::requirements()
{
    // NOTE: Refer to WMTemplate.cpp if you do not understand these commands.

    // We need graphics to draw anything:
    m_requirements.push_back( new WGERequirement() );
}

bool WMVRCamera::setupVRInterface()
{
    // Exit if we do not have an HMD present
    if( !vr::VR_IsHmdPresent() )
    {
        errorLog() << "No valid HMD present!" << std::endl;
        return false;
        //requestStop();
    }

    // Loading the OpenVR Runtime
    vr::EVRInitError eError = vr::VRInitError_None;
    m_vrSystem = vr::VR_Init( &eError, vr::VRApplication_Scene );

    if( eError != vr::VRInitError_None )
    {
        m_vrSystem = nullptr;
        errorLog()
            << "Unable to initialize the OpenVR library." << std::endl
            << "Reason: " << vr::VR_GetVRInitErrorAsEnglishDescription( eError ) << std::endl;
        return false;
        //requestStop();
    }

    if( !vr::VRCompositor() )
    {
        m_vrSystem = nullptr;
        vr::VR_Shutdown();
        errorLog() << "Compositor initialization failed" << std::endl;
        return false;
        //requestStop();
    }

    m_vrRenderModels =( vr::IVRRenderModels * )vr::VR_GetGenericInterface( vr::IVRRenderModels_Version, &eError );

    if( m_vrRenderModels == nullptr )
    {
        m_vrSystem = nullptr;
        vr::VR_Shutdown();
        errorLog()
            << "Unable to get render model interface!" << std::endl
            << "Reason: " << vr::VR_GetVRInitErrorAsEnglishDescription( eError ) << std::endl;
        return false;
        //requestStop();
    }

    vr::VRCompositor()->SetTrackingSpace( vr::TrackingUniverseSeated );
    m_vrSystem->GetRecommendedRenderTargetSize( &m_vrRenderWidth, &m_vrRenderHeight );

    m_grabber = vr::k_unTrackedDeviceIndexInvalid;

    std::string driverName = getDeviceProperty( vr::Prop_TrackingSystemName_String );
    std::string deviceSerialNumber = getDeviceProperty( vr::Prop_SerialNumber_String );
    debugLog() << "HMD driver name: " << driverName;
    debugLog() << "HMD device serial number: " << deviceSerialNumber;

    return true;
}

void WMVRCamera::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // View Setup
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    // create a root node for this module
    m_rootnode = osg::ref_ptr<WGEManagedGroupNode>( new WGEManagedGroupNode( m_active ) );

    // create the roots for the Eyes
    m_leftEyeNode = osg::ref_ptr<WGEGroupNode>( new WGEGroupNode() );
    m_rightEyeNode = osg::ref_ptr<WGEGroupNode>( new WGEGroupNode() );
    m_leftEyeGeometryNode = osg::ref_ptr<WGEGroupNode>( new WGEGroupNode() );
    m_rightEyeGeometryNode = osg::ref_ptr<WGEGroupNode>( new WGEGroupNode() );

    // insert the created nodes
    m_leftEyeNode->insert( m_leftEyeGeometryNode );
    m_rightEyeNode->insert( m_rightEyeGeometryNode );
    m_rootnode->insert( m_leftEyeNode );
    m_rootnode->insert( m_rightEyeNode );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootnode );

    //Set Nodemask so this modules Node are culled in the Mainview
    osg::Node::NodeMask vrNodeMask = 0x10000000;
    WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getCamera()->setCullMask( ~vrNodeMask );
    m_rootnode->setNodeMask( vrNodeMask );

    // get side-views
    std::shared_ptr<WGEViewer> leftEyeView = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Left Eye View" );
    std::shared_ptr<WGEViewer> rightEyeView = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Right Eye View" );
    leftEyeView->setScene( m_leftEyeNode );
    rightEyeView->setScene( m_rightEyeNode );
    leftEyeView->reset();
    rightEyeView->reset();

    // Now, we can mark the module ready.
    ready();
    debugLog() << "Starting...";

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // HMD Setup
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    if( setupVRInterface() )
    {
        debugLog() << "OpenVR initialized successfully";
        m_vrIsInitialized = true;
    }
    else
    {
        errorLog() << "OpenVR failed to initialize";
        m_vrRenderWidth = 1736;
        m_vrRenderHeight = 1928;
        m_vrIsInitialized = false;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Render to Texture Setup
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    leftEyeView->getCamera()->setClearMask( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    leftEyeView->getCamera()->setRenderTargetImplementation( osg::Camera::FRAME_BUFFER_OBJECT );
    leftEyeView->getCamera()->setRenderOrder( osg::Camera::PRE_RENDER, vr::Eye_Left );
    leftEyeView->getCamera()->setComputeNearFarMode( osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );
    //leftEyeView->getCamera()->setNearFarRatio(  0.000001  ); Not exactly sure what this does
    leftEyeView->getCamera()->setViewport( 0, 0, m_vrRenderWidth, m_vrRenderHeight );
    leftEyeView->getCamera()->setCullMask( vrNodeMask );

    rightEyeView->getCamera()->setClearMask( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    rightEyeView->getCamera()->setRenderTargetImplementation( osg::Camera::FRAME_BUFFER_OBJECT );
    rightEyeView->getCamera()->setRenderOrder( osg::Camera::PRE_RENDER, vr::Eye_Right );
    rightEyeView->getCamera()->setComputeNearFarMode( osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );
    //rightEyeView->getCamera()->setNearFarRatio(  0.000001  ); Not exactly sure what this does
    rightEyeView->getCamera()->setViewport( 0, 0, m_vrRenderWidth, m_vrRenderHeight );
    rightEyeView->getCamera()->setCullMask( vrNodeMask );

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

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Offscreen render pipeline setup
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    osg::ref_ptr<WGEOffscreenRenderNode> offscreenRenderLeft(
        new WGEOffscreenRenderNode(
            leftEyeView->getCamera(),
            m_vrRenderWidth,
            m_vrRenderHeight,
            true ) );
    osg::ref_ptr<WGEOffscreenRenderNode> offscreenRenderRight(
        new WGEOffscreenRenderNode(
            rightEyeView->getCamera(),
            m_vrRenderWidth,
            m_vrRenderHeight,
            true ) );

    //Add Geometry to render pipeline
    osg::ref_ptr<WGEOffscreenRenderPass> geometryPassLeft =
        offscreenRenderLeft->addGeometryRenderPass( m_leftEyeGeometryNode, "GeometryPassLeft" );
    osg::ref_ptr<WGEOffscreenRenderPass> geometryPassRight =
        offscreenRenderRight->addGeometryRenderPass( m_rightEyeGeometryNode, "GeometryPassRight" );

    //Add a texture processing step to render pipeline
    osg::ref_ptr<WGEOffscreenTexturePass> texturePassLeft = offscreenRenderLeft->addTextureProcessingPass(
        new WGEShader( "WMVRCameraTestShader", m_localPath ), "TexturePassLeft" );
    osg::ref_ptr<WGEOffscreenTexturePass> texturePassRight = offscreenRenderRight->addTextureProcessingPass(
        new WGEShader( "WMVRCameraTestShader2", m_localPath ), "TexturePassRight" );

    //And a final pass
    osg::ref_ptr<WGEOffscreenFinalPass> finalPassLeft = offscreenRenderLeft->addFinalOnScreenPass( "FinalPassLeft" );
    osg::ref_ptr<WGEOffscreenFinalPass> finalPassRight = offscreenRenderRight->addFinalOnScreenPass( "FinalPassRight" );

    //attach geometry to color and depth buffer
    m_geometryColorLeft = geometryPassLeft->attach( WGECamera::COLOR_BUFFER );
    osg::ref_ptr<osg::Texture2D> geometryDepthLeft = geometryPassLeft->attach( WGECamera::DEPTH_BUFFER );
    m_geometryColorRight = geometryPassRight->attach( WGECamera::COLOR_BUFFER );
    osg::ref_ptr<osg::Texture2D> geometryDepthRight = geometryPassRight->attach( WGECamera::DEPTH_BUFFER );

    //attach texture pass to color buffer
    m_textureColorLeft = texturePassLeft->attach( WGECamera::COLOR_BUFFER );
    m_textureColorRight = texturePassRight->attach( WGECamera::COLOR_BUFFER );

    //use geometry pass as input for texture pass
    texturePassLeft->bind( m_geometryColorLeft, 0 );
    texturePassLeft->bind( geometryDepthLeft, 1 );
    texturePassRight->bind( m_geometryColorRight, 0 );
    texturePassRight->bind( geometryDepthRight, 1 );

    // attach final pass to color output
    finalPassLeft->attach( WGECamera::COLOR_BUFFER, m_leftTexture );
    finalPassRight->attach( WGECamera::COLOR_BUFFER, m_rightTexture );
    finalPassLeft->bind( m_textureColorLeft, 0 );
    finalPassRight->bind( m_textureColorRight, 0 );

    // The final pass should also blend properly:
    finalPassLeft->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
    finalPassRight->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );

    osg::ref_ptr<osg::Node> plane_left = wge::genFinitePlane(
        osg::Vec3( -100.0, 0.0, -100.0 ), // base
        osg::Vec3( 200.0, 0.0, 0.0 ),     // spanning vector a
        osg::Vec3( 0.0, 0.0, 200.0 ),     // spanning vector b
        WColor( 0.0, 1.0, 0.0, 1.0 )      // a color.
     );
    osg::ref_ptr<osg::Node> plane_right = wge::genFinitePlane(
        osg::Vec3( -100.0, 0.0, -100.0 ), // base
        osg::Vec3( 200.0, 0.0, 0.0 ),     // spanning vector a
        osg::Vec3( 0.0, 0.0, 200.0 ),     // spanning vector b
        WColor( 0.0, 0.0, 1.0, 1.0 )      // a color.
     );

    osg::ref_ptr<osg::Node> plane_left2 = wge::genFinitePlane(
        osg::Vec3( -200.0, 0.0, -200.0 ), // base
        osg::Vec3( 100.0, 0.0, 0.0 ),     // spanning vector a
        osg::Vec3( 0.0, 0.0, 100.0 ),     // spanning vector b
        WColor( 0.0, 1.0, 0.0, 1.0 )      // a color.
     );
    osg::ref_ptr<osg::Node> plane_right2 = wge::genFinitePlane(
        osg::Vec3( -200.0, 0.0, -200.0 ), // base
        osg::Vec3( 100.0, 0.0, 0.0 ),     // spanning vector a
        osg::Vec3( 0.0, 0.0, 100.0 ),     // spanning vector b
        WColor( 0.0, 0.0, 1.0, 1.0 )      // a color.
     );

    osg::ref_ptr<osg::Node> plane_main = wge::genFinitePlane(
        osg::Vec3( -100.0, 0.0, -100.0 ), // base
        osg::Vec3( 200.0, 0.0, 0.0 ),     // spanning vector a
        osg::Vec3( 0.0, 0.0, 200.0 ),     // spanning vector b
        WColor( 1.0, 0.0, 0.0, 1.0 )      // a color.
     );

    /*m_leftEyeGeometryNode->addChild(  plane_left  );
    m_rightEyeGeometryNode->addChild(  plane_right  );
    m_leftEyeGeometryNode->addChild(  plane_left2  );
    m_rightEyeGeometryNode->addChild(  plane_right2  );
    m_rootnode->addChild(  plane_main  );*/

    m_leftEyeGeometryNode->addChild( dynamic_cast<osg::Node *>(
        WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->clone( osg::CopyOp::DEEP_COPY_ALL ) ) );
    m_rightEyeGeometryNode->addChild( dynamic_cast<osg::Node *>(
        WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->clone( osg::CopyOp::DEEP_COPY_ALL ) ) );

    leftEyeView->getScene()->insert( offscreenRenderLeft );
    rightEyeView->getScene()->insert( offscreenRenderRight );

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Main loop
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    // updatecallback for submitting frames
    m_rootnode->addUpdateCallback( new SafeUpdateCallback( this ) );

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
    debugLog() << "Shutting down openVRCamera";

    //Reset CullMask
    WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getCamera()->setCullMask( 0xFFFFFFFF );

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootnode );
}

void WMVRCamera::activate()
{
    // This method gets called, whenever the m_active property changes. Your module should always handle this if you do not use the
    // WGEManagedGroupNode for your scene. The user can(  de-  )activate modules in his GUI and you can handle this case here:
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

std::string WMVRCamera::getDeviceProperty( vr::TrackedDeviceProperty prop )
{
    uint32_t bufferLen = m_vrSystem->GetStringTrackedDeviceProperty( vr::k_unTrackedDeviceIndex_Hmd, prop, NULL, 0 );
    if( bufferLen == 0 )
    {
        return "";
    }

    char *buffer = new char[bufferLen];
    bufferLen = m_vrSystem->GetStringTrackedDeviceProperty( vr::k_unTrackedDeviceIndex_Hmd, prop, buffer, bufferLen );
    std::string result = buffer;
    delete[] buffer;
    return result;
}

void WMVRCamera::handleVREvent( vr::VREvent_t vrEvent )
{
    vr::ETrackedDeviceClass deviceClass = m_vrSystem->GetTrackedDeviceClass( vrEvent.trackedDeviceIndex );
    switch( deviceClass )
    {
    case vr::TrackedDeviceClass_Controller:
        handleControllerEvent( vrEvent );
        break;
    default:
        break;
    }
}

void WMVRCamera::handleControllerEvent( vr::VREvent_t vrEvent )
{
    //debugLog() << "Trigger is:" << vr::EVRButtonId::k_EButton_SteamVR_Trigger;
    //debugLog() << "Grip is:" << vr::EVRButtonId::k_EButton_Grip;
    switch( vrEvent.eventType )
    {
    case vr::VREvent_ButtonPress:
        debugLog() << "Pressed:" << vrEvent.data.controller.button;
        if(
            vrEvent.data.controller.button == vr::EVRButtonId::k_EButton_Grip ||
            vrEvent.data.controller.button == vr::EVRButtonId::k_EButton_SteamVR_Trigger )
        {
            m_grabber = vrEvent.trackedDeviceIndex;
        }
        break;
    case vr::VREvent_ButtonUnpress:
        debugLog() << "Unpressed:" << vrEvent.data.controller.button;
        if(
            vrEvent.data.controller.button == vr::EVRButtonId::k_EButton_Grip ||
            vrEvent.data.controller.button == vr::EVRButtonId::k_EButton_SteamVR_Trigger )
        {
            m_grabber = vr::k_unTrackedDeviceIndexInvalid;
        }
        break;
    default:
        break;
    }
}

void WMVRCamera::SafeUpdateCallback::operator()( osg::Node *node, osg::NodeVisitor *nv )
{
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    double elapsedSeconds =( now - m_lastFrame ).count() / 1000000000.0;
    m_lastFrame = now;
    m_lastFrames[m_frameCounter++] = elapsedSeconds;
    if( m_frameCounter >= 120 )
    {
        m_frameCounter = 0;
    }
    double elapsedSecondsSum = 0;
    for( int i = 0; i <= 119; i++ )
    {
        elapsedSecondsSum += m_lastFrames[i];
    }
    double averageElapsedSeconds = elapsedSecondsSum / 120.0;

    if( m_module->m_VR_fpsTrigger->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
    {
        m_module->debugLog() << elapsedSeconds << " sec since last Frame | " << 1 / elapsedSeconds << " fps";
        m_module->debugLog() << averageElapsedSeconds << " sec average Frametiming | " << 1 / averageElapsedSeconds << " fps";

        m_module->m_VR_fpsTrigger->set( WPVBaseTypes::PV_TRIGGER_READY, false );
    }

    if( m_module->m_VR_logCameraManipulators->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
    {
        std::shared_ptr<WGEViewer> leftEyeView = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Left Eye View" );
        m_module->debugLog() << "Left Eye Camera Manipulator matrix: " << leftEyeView->getCameraManipulator()->getMatrix();

        std::shared_ptr<WGEViewer> rightEyeView = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Right Eye View" );
        m_module->debugLog() << "Right Eye Camera Manipulator matrix: " << rightEyeView->getCameraManipulator()->getMatrix();

        m_module->m_VR_logCameraManipulators->set( WPVBaseTypes::PV_TRIGGER_READY, false );
    }

    if( m_module->m_VR_screenshotTrigger->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
    {
        osg::ref_ptr<osg::Image> mainImage = new osg::Image();
        mainImage->readPixels( 0, 0, m_module->m_vrRenderWidth, m_module->m_vrRenderHeight, GL_RGBA, GL_UNSIGNED_BYTE );

        m_module->debugLog() << "Breite:" << mainImage->s();
        m_module->debugLog() << "Pointer:" << mainImage->getDataPointer();
        m_module->debugLog() << "DatenMenge:" << mainImage->getTotalDataSize();

        std::string filename = "./OpenWalnut - framebuffer.png";

        osgDB::writeImageFile( *mainImage, filename );

        m_module->m_VR_screenshotTrigger->set( WPVBaseTypes::PV_TRIGGER_READY, false );
    }

    if( m_module->m_VR_cameraManipTrigger->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
    {
        std::shared_ptr<WGEViewer> leftEyeView = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Left Eye View" );
        std::shared_ptr<WGEViewer> rightEyeView = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Right Eye View" );
        osg::Matrixd mainViewMatrix = WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getCameraManipulator()->getMatrix();

        leftEyeView->getCameraManipulator()->setByMatrix( mainViewMatrix );
        rightEyeView->getCameraManipulator()->setByMatrix( mainViewMatrix );

        m_module->m_VR_cameraManipTrigger->set( WPVBaseTypes::PV_TRIGGER_READY, false );
    }

    if( m_module->m_vrOn->changed( true ) )
    {
        if( m_module->m_vrOn->get() )
        {
            m_module->debugLog() << "Start submitting frames to OpenVR";
        }
        else
        {
            m_module->debugLog() << "Stop submitting frames to OpenVR";
        }
    }

    if( m_module->m_vrOn->get() && m_module->m_vrIsInitialized )
    {
        if( !m_initialUpdate )
        {
            unsigned int leftContextID = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Left Eye View" )
            ->getCamera()->getGraphicsContext()->getState()->getContextID();
            
            unsigned int rightContextID = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Right Eye View" )
            ->getCamera()->getGraphicsContext()->getState()->getContextID();

            vr::Texture_t leftEyeTexture = {
               ( void * )( uintptr_t )m_module->m_textureColorLeft->getTextureObject( leftContextID )->id(),
                vr::TextureType_OpenGL,
                vr::ColorSpace_Gamma};
            vr::Texture_t rightEyeTexture = {
               ( void * )( uintptr_t )m_module->m_textureColorRight->getTextureObject( rightContextID )->id(),
                vr::TextureType_OpenGL,
                vr::ColorSpace_Gamma};

            vr::EVRCompositorError lError = vr::VRCompositor()->Submit( vr::Eye_Left, &leftEyeTexture );
            vr::EVRCompositorError rError = vr::VRCompositor()->Submit( vr::Eye_Right, &rightEyeTexture );

            if( lError != vr::VRCompositorError_None || rError != vr::VRCompositorError_None )
            {
                m_module->errorLog() << "Links:" << lError << "|Rechts:" << rError << std::endl;
            }
        }

        // get all OpenVR tracking information
        for( uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i )
        {
            m_module->m_poses[i].bPoseIsValid = false;
        }
        vr::VRCompositor()->WaitGetPoses( m_module->m_poses, vr::k_unMaxTrackedDeviceCount, NULL, 0 );

        // handle controller events
        vr::VREvent_t vrEvent;
        while( m_module->m_vrSystem->PollNextEvent( &vrEvent, sizeof( vr::VREvent_t ) ) )
            m_module->handleVREvent( vrEvent );

        vr::TrackedDevicePose_t trackedDevicePose;
        vr::HmdMatrix34_t poseMatrix;
        double doubleQuat[4];

        vr::VRSystem()->GetDeviceToAbsoluteTrackingPose( vr::TrackingUniverseStanding, 0, &trackedDevicePose, 1 );

        poseMatrix = trackedDevicePose.mDeviceToAbsoluteTracking;

        doubleQuat[3] = sqrt( fmax( 0, 1 + poseMatrix.m[0][0] + poseMatrix.m[1][1] + poseMatrix.m[2][2] ) ) / 2;
        doubleQuat[0] = sqrt( fmax( 0, 1 + poseMatrix.m[0][0] - poseMatrix.m[1][1] - poseMatrix.m[2][2] ) ) / 2;
        doubleQuat[1] = sqrt( fmax( 0, 1 - poseMatrix.m[0][0] + poseMatrix.m[1][1] - poseMatrix.m[2][2] ) ) / 2;
        doubleQuat[2] = sqrt( fmax( 0, 1 - poseMatrix.m[0][0] - poseMatrix.m[1][1] + poseMatrix.m[2][2] ) ) / 2;
        doubleQuat[0] = copysign( doubleQuat[0], poseMatrix.m[2][1] - poseMatrix.m[1][2] );
        doubleQuat[1] = copysign( doubleQuat[1], poseMatrix.m[0][2] - poseMatrix.m[2][0] );
        doubleQuat[2] = copysign( doubleQuat[2], poseMatrix.m[1][0] - poseMatrix.m[0][1] );

        m_currentQuaternion.set( doubleQuat[0], doubleQuat[1], doubleQuat[2], doubleQuat[3] );

        bool rotDiffSet = false;
        rotDiffSet = false;

        if( m_frameCounter > 0 )
        {
            m_rotDifference = m_currentQuaternion * m_lastQuaternion.inverse();
            rotDiffSet = true;

            m_module->debugLog() << "current: " << m_currentQuaternion.asVec4();
            m_module->debugLog() << "last: " << m_lastQuaternion.asVec4();
        }

        m_lastQuaternion = m_currentQuaternion;

        // adjust Scene according to inputs
        vr::TrackedDevicePose_t controllerPose;
        osg::ref_ptr<osgGA::TrackballManipulator> tm_MainView;
        osg::ref_ptr<osgGA::TrackballManipulator> tm_LeftView;
        osg::ref_ptr<osgGA::TrackballManipulator> tm_RightView;

        if( m_module->m_grabber != vr::k_unTrackedDeviceIndexInvalid )
        {
            controllerPose = m_module->m_poses[m_module->m_grabber];
        }

        tm_MainView = osg::dynamic_pointer_cast<osgGA::TrackballManipulator>(
            WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getCameraManipulator() );

        tm_LeftView = osg::dynamic_pointer_cast<osgGA::TrackballManipulator>(
            WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Left Eye View" )->getCameraManipulator() );

        tm_RightView = osg::dynamic_pointer_cast<osgGA::TrackballManipulator>(
            WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Right Eye View" )->getCameraManipulator() );

        // apply controller rotation to views
        if( m_module->m_grabber != vr::k_unTrackedDeviceIndexInvalid )
        {
            double angle = sqrt(
                               controllerPose.vAngularVelocity.v[0] * controllerPose.vAngularVelocity.v[0] +
                               controllerPose.vAngularVelocity.v[1] * controllerPose.vAngularVelocity.v[1] +
                               controllerPose.vAngularVelocity.v[2] * controllerPose.vAngularVelocity.v[2] ) *
                           elapsedSeconds;

            if( tm_MainView )
            {
                // Controller rotation

                osg::Quat rotFrom = tm_MainView->getRotation();
                osg::Quat rotBy = tm_MainView->getRotation();
                rotBy.makeRotate(
                    angle,
                    -controllerPose.vAngularVelocity.v[0],
                    controllerPose.vAngularVelocity.v[2],
                    -controllerPose.vAngularVelocity.v[1] );
                osg::Quat rotTo = rotFrom * rotBy;
                tm_MainView->setRotation( rotTo );

                rotFrom = tm_MainView->getRotation();
                rotTo = m_rotDifference * rotFrom;
                tm_MainView->setRotation( rotTo );
            }
            if( tm_LeftView )
            {
                osg::Quat rotFrom = tm_LeftView->getRotation();
                osg::Quat rotBy = tm_LeftView->getRotation();
                rotBy.makeRotate(
                    angle,
                    -controllerPose.vAngularVelocity.v[0],
                    controllerPose.vAngularVelocity.v[2],
                    -controllerPose.vAngularVelocity.v[1] );
                osg::Quat rotTo = rotFrom * rotBy;
                tm_LeftView->setRotation( rotTo );
            }
            if( tm_RightView )
            {
                osg::Quat rotFrom = tm_RightView->getRotation();
                osg::Quat rotBy = tm_RightView->getRotation();
                rotBy.makeRotate(
                    angle,
                    -controllerPose.vAngularVelocity.v[0],
                    controllerPose.vAngularVelocity.v[2],
                    -controllerPose.vAngularVelocity.v[1] );
                osg::Quat rotTo = rotFrom * rotBy;
                tm_RightView->setRotation( rotTo );
            }
        }

        //apply hmd rotation to viewss
        if( rotDiffSet )
        {
            if( tm_MainView )
            {
                osg::Quat rotFrom = tm_MainView->getRotation();
                osg::Quat rotTo = rotFrom * m_rotDifference;
                tm_MainView->setRotation( rotTo );
            }
            if( tm_LeftView )
            {
                osg::Quat rotFrom = tm_LeftView->getRotation();
                osg::Quat rotTo = rotFrom * m_rotDifference;
                tm_LeftView->setRotation( rotTo );
            }
            if( tm_RightView )
            {
                osg::Quat rotFrom = tm_RightView->getRotation();
                osg::Quat rotTo = rotFrom * m_rotDifference;
                tm_RightView->setRotation( rotTo );
            }
        }
    }

    m_initialUpdate = false;

    traverse( node, nv );
}
