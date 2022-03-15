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

#ifndef WMVRCAMERA_H
#define WMVRCAMERA_H

#include <string>

#include <openvr.h> // NOLINT

#include "core/common/math/WMath.h"
#include "core/kernel/WModule.h"

#include "WRTTCamera.h"
#include "WSwapUpdateCallback.h"
#include "WVRController.h"

/**
 * Forward declaration for the controlelr class.
 */
class WVRController;

/**
 * Module starting and connecting to an OpenVR session
 * \ingroup modules
 */
class WMVRCamera : public WModule
{
    /**
     * Only UnitTests may be friends.
     */
    friend class WMVRCameraTest;

    /**
     * The SwapUpdateCallback needs access to the private fields.
     */
    friend class WSwapUpdateCallback;

public:
    /**
     * Standard constructor.
     */
    WMVRCamera();

    /**
     * Destructor.
     */
    ~WMVRCamera();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description of module.
     */
    virtual const std::string getDescription() const;

    /**
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual std::shared_ptr<WModule> factory() const;

    /**
     * Get the icon for this module in XPM format.
     * \return the icon.
     */
    virtual const char **getXPMIcon() const;

    /**
     * Returns a Deviceproperty from m_vrSystem as a string
     * \param prop a openVR device property
     * \return the property value as string
     */
    std::string getDeviceProperty( vr::TrackedDeviceProperty prop );

    /**
     * Handles generic VR events
     * \param vrEvent generic vr event like mousemove or buttonpress
     */
    void handleVREvent( vr::VREvent_t vrEvent );

    /**
     * Handles controller events
     * \param vrEvent vr controller event like buttonpress
     */
    void handleControllerEvent( vr::VREvent_t vrEvent );

    /**
     * Find and save device IDs for later use (controller IDs for example).
     */
    void updateDeviceIDs();

    /**
     * Fetch and save transformation information of controller in current frame.
     */
    void updateControllerPoses();

    /**
     * Fetch and save transformation information of HMD in current frame.
     */
    void updateHMDPose();

    /**
     * Set eye view's matrices to main view matrix; also applies HMD eye offset.
     */
    void ResetHMDPosition();

    /**
     * Converts vr::HmdMatrix34_t to osg::Matrix.
     * \param mat34 The HMD Matrix.
     * \return osg::Matrix The OSG Matrix.
     */
    static osg::Matrix convertHmdMatrixToOSG( const vr::HmdMatrix34_t &mat34, bool swapAxis = false );

    /**
     * Converts vr::HmdMatrix34_t to osg::Matrix.
     * \param mat44 The HMD Matrix.
     * \return osg::Matrix The OSG Matrix.
     */
    static osg::Matrix convertHmdMatrixToOSG( const vr::HmdMatrix44_t &mat44, bool swapAxis = false );

protected:
    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void moduleMain();

    /**
     * Initialize the connectors this module is using.
     */
    virtual void connectors();

    /**
     * Initialize the properties for this module.
     */
    virtual void properties();

    /**
     * Callback for m_active. Overwrite this in your modules to handle m_active changes separately.
     */
    virtual void activate();

    /**
     * Initialize OpenVR SDK.
     * \return bool inicating success
     */
    bool setupVRInterface();

private:
    std::shared_ptr< WCondition > m_propCondition; //!< A condition for property updates.

    WPropBool m_vrOn; //!< A feature toggle for submitting frames to OpenVR SDK
    WPropTrigger m_VR_fpsTrigger; //!< A trigger for debugging the FPS while VR module is running.
    WPropTrigger m_VR_logCameraViewMatrix; //!< A trigger for debugging camera matrix of VR module.
    WPropTrigger m_VR_resetHMDPosition; //!< A trigger for resetting the camera to the seated zero position of the HMD device.

    vr::IVRSystem *m_vrSystem; //!< The OpenVR SDK Interface.
    vr::IVRRenderModels *m_vrRenderModels; //!< The OpenVR RenderModel Interface.

    bool m_vrIsInitialized; //!< Denotes whether the OpenVR SDK was initialized successfully.
    uint32_t m_vrRenderWidth; //!< The recommended texture width from vr_system.
    uint32_t m_vrRenderHeight; //!< The recommended texture height from vr_system.

    /**
     * OpenVR tracking information
     */
    vr::TrackedDevicePose_t m_poses[vr::k_unMaxTrackedDeviceCount];         // NOLINT: the size is constant

    osg::Vec3d m_HMD_position; //!< Current position of HMD device.
    osg::Quat m_HMD_rotation; //!< Current rotation of HMD device.

    osg::Vec3d m_cameraPosition; //!< The camera position of the scene (without hmd).
    osg::Quat m_cameraRotation; //!< The camera rotation of the scene (without hmd).

    osg::Vec3d m_dataPosition; //!< The position of the scene.
    osg::Matrixd m_dataRotation; //!< The rotation of the data.

    osg::ref_ptr< WRTTCamera > m_leftEyeCamera; //!< The camera for the left eye of the VR system.
    osg::ref_ptr< WRTTCamera > m_rightEyeCamera; //!< The camera for the right eye of the VR system.

    std::unique_ptr< WVRController > m_leftController; //!< The controller for the left hand.
    std::unique_ptr< WVRController > m_rightController; //!< The controller for the right hand.

    osg::ref_ptr< osg::MatrixTransform > m_sceneTransform; //!< The node for the scene transformation.
};

#endif  // WMVRCAMERA_H
