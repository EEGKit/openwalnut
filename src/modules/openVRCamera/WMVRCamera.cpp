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

#include "core/graphicsEngine/WGEManagedGroupNode.h"

#include "WMVRCamera.h"
#include "WMVRCamera.xpm"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMVRCamera )

WMVRCamera::WMVRCamera() : WModule()
{
}

WMVRCamera::~WMVRCamera()
{
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
    WModule::connectors();
}

void WMVRCamera::properties()
{
    m_propCondition = std::shared_ptr<WCondition>( new WCondition() );

    m_vrOn = m_properties->addProperty( "Submit Frames", "Toggle submitting frames to OpenVR", false );
    m_VR_fpsTrigger = m_properties->addProperty( "Log Fps", "Now", WPVBaseTypes::PV_TRIGGER_READY );
    m_VR_logCameraViewMatrix = m_properties->addProperty( "Log Camera View Matrix", "Now", WPVBaseTypes::PV_TRIGGER_READY );
    m_VR_resetHMDPosition = m_properties->addProperty( "Reset position", "Reset", WPVBaseTypes::PV_TRIGGER_READY );

    WModule::properties();
}

bool WMVRCamera::setupVRInterface()
{
    // Exit if we do not have an HMD present
    if( !vr::VR_IsHmdPresent() )
    {
        errorLog() << "No valid HMD present!" << std::endl;
        return false;
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
    }

    if( !vr::VRCompositor() )
    {
        m_vrSystem = nullptr;
        vr::VR_Shutdown();
        errorLog() << "Compositor initialization failed" << std::endl;
        return false;
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
    }

    vr::VRCompositor()->SetTrackingSpace( vr::TrackingUniverseSeated );
    m_vrSystem->GetRecommendedRenderTargetSize( &m_vrRenderWidth, &m_vrRenderHeight );

    // Check for connected controllers
    updateDeviceIDs();

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

    osg::View* mainView = WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getView();
    osg::GraphicsContext* gc = mainView->getCamera()->getGraphicsContext();
    osg::ref_ptr< WGEGroupNode > scene = WKernel::getRunningKernel()->getGraphicsEngine()->getScene();

    osg::Matrix projLeft = convertHmdMatrixToOSG( m_vrSystem->GetProjectionMatrix( vr::Eye_Left, 1.0, 1000.0 ) );
    osg::Matrix projRight = convertHmdMatrixToOSG( m_vrSystem->GetProjectionMatrix( vr::Eye_Right, 1.0, 1000.0 ) );

    m_leftEyeCamera = new WRTTCamera( m_vrRenderWidth, m_vrRenderHeight, gc, projLeft, scene, m_localPath );
    m_rightEyeCamera = new WRTTCamera( m_vrRenderWidth, m_vrRenderHeight, gc, projRight, scene, m_localPath );

    mainView->addSlave( m_leftEyeCamera, false );
    mainView->addSlave( m_rightEyeCamera, false );

    // updatecallback for submitting frames
    gc->setSwapCallback( new WSwapUpdateCallback( this ) );

    m_leftController = std::make_unique< WVRController >( vr::TrackedControllerRole_LeftHand );
    m_rightController = std::make_unique< WVRController >( vr::TrackedControllerRole_RightHand );

    m_leftController->createGeometry( ( m_localPath / "VIVEController.obj" ).string() );
    m_rightController->createGeometry( ( m_localPath / "VIVEController.obj" ).string() );

    m_leftEyeCamera->getGeometryNode()->insert( m_leftController->getNode() );
    m_rightEyeCamera->getGeometryNode()->insert( m_leftController->getNode() );
    m_leftEyeCamera->getGeometryNode()->insert( m_rightController->getNode() );
    m_rightEyeCamera->getGeometryNode()->insert( m_rightController->getNode() );

    ResetHMDPosition();

    // Now, we can mark the module ready.
    ready();
    debugLog() << "Starting...";

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

    // remove update callback
    gc->setSwapCallback( nullptr );

    // Shut down OpenVR
    m_vrSystem = nullptr;
    vr::VR_Shutdown();

    debugLog() << "Shutting down openVRCamera";
}

void WMVRCamera::activate()
{
    if( m_active->get() )
    {
        debugLog() << "Activate.";
    }
    else
    {
        debugLog() << "Deactivate.";
    }

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
    switch( vrEvent.eventType )
    {
    case vr::VREvent_ButtonPress:
        // check for any changes regarding controller device IDs on any button press
        updateDeviceIDs();

        if( vrEvent.data.controller.button == vr::EVRButtonId::k_EButton_SteamVR_Trigger )
        {
            if( vrEvent.trackedDeviceIndex == vr::TrackedControllerRole_LeftHand )
            {
                m_leftController->setTriggered( true );
            }
            else if( vrEvent.trackedDeviceIndex == vr::TrackedControllerRole_RightHand )
            {
                m_rightController->setTriggered( true );
            }
        }
        break;
    case vr::VREvent_ButtonUnpress:
        if( vrEvent.data.controller.button == vr::EVRButtonId::k_EButton_SteamVR_Trigger )
        {
            if( vrEvent.trackedDeviceIndex == vr::TrackedControllerRole_LeftHand )
            {
                m_leftController->setTriggered( false );
            }
            else if( vrEvent.trackedDeviceIndex == vr::TrackedControllerRole_RightHand )
            {
                m_rightController->setTriggered( false );
            }
        }
        break;
    default:
        break;
    }
}

void WMVRCamera::updateDeviceIDs()
{
    for( uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; i++ )
    {
        vr::ETrackedControllerRole cRole = m_vrSystem->GetControllerRoleForTrackedDeviceIndex( i );

        if( cRole == vr::TrackedControllerRole_LeftHand && m_leftController ) m_leftController->setDeviceID( i );
        if( cRole == vr::TrackedControllerRole_RightHand && m_rightController ) m_rightController->setDeviceID( i );
        // more devices IDs can be tracked here (such as the sonsors)
    }
}

void WMVRCamera::updateControllerPoses()
{
    m_leftController->updatePose( m_vrSystem, m_cameraPosition );
    m_rightController->updatePose( m_vrSystem, m_cameraPosition );

    // zooming
    if( m_leftController->isTriggered() && m_rightController->isTriggered() )
    {
        double lenPrev = ( m_leftController->getPrevPosition() - m_rightController->getPrevPosition() ).length();
        double lenNow  = ( m_leftController->getPosition() - m_rightController->getPosition() ).length();
        double length = lenNow - lenPrev;

        osg::Vec3 dir( 0.0, 1.0, 0.0 );

        dir = ( m_cameraRotation * m_HMD_rotation.inverse() ) * dir;
        dir *= length;

        double help = dir.y();
        dir.y() = -dir.z();
        dir.z() = help;

        m_cameraPosition += dir;
    }
}

void WMVRCamera::updateHMDPose()
{
    updateDeviceIDs();
    updateControllerPoses();

    // get all OpenVR tracking information
    for( uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i )
    {
        m_poses[i].bPoseIsValid = false;
    }
    vr::VRCompositor()->WaitGetPoses( m_poses, vr::k_unMaxTrackedDeviceCount, NULL, 0 );

    // handle controller events
    vr::VREvent_t vrEvent;
    while( m_vrSystem->PollNextEvent( &vrEvent, sizeof( vr::VREvent_t ) ) )
        handleVREvent( vrEvent );

    vr::TrackedDevicePose_t trackedDevicePose = m_poses[vr::k_unTrackedDeviceIndex_Hmd];

    if( !trackedDevicePose.bPoseIsValid )
    {
        return;
    }

    // pose calculation
    osg::Matrix poseTransform = convertHmdMatrixToOSG( trackedDevicePose.mDeviceToAbsoluteTracking );

    osg::Vec3 position = poseTransform.getTrans() * 100.0;
    m_HMD_rotation = poseTransform.getRotate().inverse();

    // switch y and z axis as openvr has different coordinate system.
    double help = position.y();
    position.y() = -position.z();
    position.z() = help;

    m_HMD_position = position;
}

void WMVRCamera::ResetHMDPosition()
{
    vr::VRChaperone()->ResetZeroPose( vr::TrackingUniverseSeated );

    osg::ref_ptr< WGEZoomTrackballManipulator > cm = osg::dynamic_pointer_cast< WGEZoomTrackballManipulator >(
    WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getCameraManipulator() );
    osg::Matrixd mainViewMatrix = cm ? cm->getMatrixWithoutZoom() :
                                        WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getCamera()->getViewMatrix();

    // Calculate lookAt quaternation from camera to the center of the scene
    osg::Vec3d center = WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->getBound().center();
    osg::Vec3d ndir = center - mainViewMatrix.getTrans();
    osg::Vec3d camdir( 0.0, 0.0, -1.0 );
    osg::Vec3d normal = ndir ^ camdir;
    double w = sqrt( ndir.length2() * camdir.length2() ) + ndir * camdir;
    osg::Vec4d qt( normal.x(), normal.y(), normal.z(), w );
    qt.normalize();

    m_cameraPosition = mainViewMatrix.getTrans();
    m_cameraRotation = osg::Quat( qt );
}

osg::Matrix WMVRCamera::convertHmdMatrixToOSG( const vr::HmdMatrix34_t &mat34 )
{
    osg::Matrix matrix(
        mat34.m[0][0], mat34.m[1][0], mat34.m[2][0], 0.0,
        mat34.m[0][1], mat34.m[1][1], mat34.m[2][1], 0.0,
        mat34.m[0][2], mat34.m[1][2], mat34.m[2][2], 0.0,
        mat34.m[0][3], mat34.m[1][3], mat34.m[2][3], 1.0f );
    return matrix;
}

osg::Matrix WMVRCamera::convertHmdMatrixToOSG( const vr::HmdMatrix44_t &mat44 )
{
    osg::Matrix matrix(
        mat44.m[0][0], mat44.m[1][0], mat44.m[2][0], mat44.m[3][0],
        mat44.m[0][1], mat44.m[1][1], mat44.m[2][1], mat44.m[3][1],
        mat44.m[0][2], mat44.m[1][2], mat44.m[2][2], mat44.m[3][2],
        mat44.m[0][3], mat44.m[1][3], mat44.m[2][3], mat44.m[3][3] );
    return matrix;
}
