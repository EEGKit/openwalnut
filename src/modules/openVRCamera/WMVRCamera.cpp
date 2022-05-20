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

static const osg::Matrix AXIS_SWAP_MATRIX(  1.0,  0.0,  0.0,  0.0,
                                            0.0,  0.0,  1.0,  0.0,
                                            0.0, -1.0,  0.0,  0.0,
                                            0.0,  0.0,  0.0,  1.0 );

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
    m_sceneTransform = osg::ref_ptr< osg::MatrixTransform >( new osg::MatrixTransform() );
    m_sceneTransform->addChild( scene );

    osg::Matrix projLeft = convertHmdMatrixToOSG( m_vrSystem->GetProjectionMatrix( vr::Eye_Left, 0.01, 1000.0 ) );
    osg::Matrix projRight = convertHmdMatrixToOSG( m_vrSystem->GetProjectionMatrix( vr::Eye_Right, 0.01, 1000.0 ) );

    m_leftEyeCamera = new WRTTCamera( m_vrRenderWidth, m_vrRenderHeight, gc, projLeft, m_sceneTransform, m_localPath );
    m_rightEyeCamera = new WRTTCamera( m_vrRenderWidth, m_vrRenderHeight, gc, projRight, m_sceneTransform, m_localPath );

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
    WVRController* controller = vrEvent.trackedDeviceIndex == m_leftController->getDeviceID() ? m_leftController.get() :
                                vrEvent.trackedDeviceIndex == m_rightController->getDeviceID() ? m_rightController.get() : nullptr;

    if( !controller )
    {
        return; // Not a controller
    }

    bool pressed = vrEvent.eventType == vr::VREvent_ButtonPress ? true : false;
    if( !pressed && vrEvent.eventType != vr::VREvent_ButtonUnpress )
    {
        return; // neither press nor unpress
    }

    switch( vrEvent.data.controller.button )
    {
        case vr::EVRButtonId::k_EButton_SteamVR_Trigger:
            controller->setTriggered( pressed );
            break;
        case vr::EVRButtonId::k_EButton_Grip:
            controller->setGripped( pressed );
            break;
    }
}

void WMVRCamera::updateDeviceIDs()
{
    for( uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; i++ )
    {
        vr::ETrackedControllerRole cRole = m_vrSystem->GetControllerRoleForTrackedDeviceIndex( i );

        if( cRole == vr::TrackedControllerRole_LeftHand && m_leftController )
        {
            m_leftController->setDeviceID( i );
        }
        if( cRole == vr::TrackedControllerRole_RightHand && m_rightController )
        {
            m_rightController->setDeviceID( i );
        }
        // more devices IDs can be tracked here (such as the sensors)
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

        osg::Vec3 dir( 0.0, 0.0, -1.0 );
        dir = ( m_cameraRotation * m_HMD_rotation ) * dir;
        dir *= length;

        m_cameraPosition += dir;
    }
    else if( m_rightController->isTriggered() )
    {
        // data translation
        osg::Vec3 diff = m_rightController->getPosition() - m_rightController->getPrevPosition();
        m_dataPosition += diff;

        // data rotation
        osg::Vec3 contrPosition = ( m_cameraPosition - m_dataPosition ) + m_rightController->getPosition();
        osg::Quat rotDiff = m_rightController->getPrevRotation().inverse() * m_rightController->getRotation();
        osg::Matrixd rotMatrix = osg::Matrixd::translate( -contrPosition ) * osg::Matrixd::rotate( rotDiff ) *
                                 osg::Matrixd::translate( contrPosition );

        m_dataRotation *= rotMatrix;

        m_sceneTransform->setMatrix( m_dataRotation * osg::Matrixd::translate( m_dataPosition ) );
    }

    if( m_leftController->isGripped() && m_rightController->isGripped() )
    {
        ResetHMDPosition();
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
    osg::Matrix poseTransform = convertHmdMatrixToOSG( trackedDevicePose.mDeviceToAbsoluteTracking, true );

    m_HMD_position = poseTransform.getTrans() * 100.0;
    m_HMD_rotation = poseTransform.getRotate();
}

void WMVRCamera::ResetHMDPosition()
{
    vr::VRChaperone()->ResetZeroPose( vr::TrackingUniverseSeated );

    // Calculate bounds
    osg::BoundingSphere boundingSphere;
    WGEZoomTrackballNodeVisitor cbVisitor;
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->accept( cbVisitor );
    osg::BoundingBox& bb = cbVisitor.getBoundingBox();

    if( bb.valid() )
    {
        boundingSphere.expandBy( bb );
    }
    else
    {
        boundingSphere = WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->getBound();
    }

    osg::Vec3d center = boundingSphere.center();

    // Calculate distance from camera to the center of the scene
    float pfLeft, pfRight, pfTop, pfBottom;
    m_vrSystem->GetProjectionRaw( vr::Eye_Left, &pfLeft, &pfRight, &pfTop, &pfBottom );
    float distance = ( boundingSphere.radius() * 2.0 ) / pfTop;

    m_cameraPosition = center + osg::Vec3( 0.0, distance, 0.0 );
    m_cameraRotation = osg::Quat();

    m_dataPosition = osg::Vec3();
    m_dataRotation = osg::Matrix();
    m_sceneTransform->setMatrix( osg::Matrix() );
}

osg::Matrix WMVRCamera::convertHmdMatrixToOSG( const vr::HmdMatrix34_t &mat34, bool swapAxis )
{
    osg::Matrix matrix(
        mat34.m[0][0], mat34.m[1][0], mat34.m[2][0], 0.0,
        mat34.m[0][1], mat34.m[1][1], mat34.m[2][1], 0.0,
        mat34.m[0][2], mat34.m[1][2], mat34.m[2][2], 0.0,
        mat34.m[0][3], mat34.m[1][3], mat34.m[2][3], 1.0f );

    if( swapAxis )
    {
        matrix = matrix * AXIS_SWAP_MATRIX;
    }
    return matrix;
}

osg::Matrix WMVRCamera::convertHmdMatrixToOSG( const vr::HmdMatrix44_t &mat44, bool swapAxis )
{
    osg::Matrix matrix(
        mat44.m[0][0], mat44.m[1][0], mat44.m[2][0], mat44.m[3][0],
        mat44.m[0][1], mat44.m[1][1], mat44.m[2][1], mat44.m[3][1],
        mat44.m[0][2], mat44.m[1][2], mat44.m[2][2], mat44.m[3][2],
        mat44.m[0][3], mat44.m[1][3], mat44.m[2][3], mat44.m[3][3] );

    if( swapAxis )
    {
        matrix = matrix * AXIS_SWAP_MATRIX;
    }
    return matrix;
}
