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
#include <vector>

#include <boost/thread.hpp>

#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"
#include "core/kernel/WModule.h"

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
     * Initialize requirements for this module.
     */
    virtual void requirements();

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
    /**
     * Callback which submits the textures to OpenVR SDK.
     */
    class SafeUpdateCallback : public osg::NodeCallback
    {
    public: // NOLINT
        /**
         * Constructor.
         *
         * \param module just set the creating module as pointer for later reference.
         */
        explicit SafeUpdateCallback( WMVRCamera* module ): m_module( module ), m_initialUpdate( true )
        {
        };

        /**
         * operator () - called during the update traversal.
         *
         * \param node the osg node
         * \param nv the node visitor
         */
        virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );

        /**
         * Pointer used to access members of the module to modify the node.
         * Please do not use shared_ptr here as this would prevent deletion of the module as the callback contains
         * a reference to it. It is safe to use a simple pointer here as callback get deleted before the module.
         */
        WMVRCamera* m_module;

        /**
         * timestamp of the last frame
         */
        std::chrono::_V2::system_clock::time_point m_lastFrame;


        /**
         * timestamps of the last 120 frames
         */
        double m_lastFrames[120];

        /**
         * current position in m_lastFrames for saving the next timestamp
         */
        unsigned int m_frameCounter = 0;

        /**
         * current rotation
         */
        osg::Quat m_currentQuaternion;

        /**
         * rotation saved for use in one frame later
         */
        osg::Quat m_lastQuaternion;

        /**
         * rotation difference calculated fromn last and current frame
         */
        osg::Quat m_rotDifference;

        /**
         * Denotes whether the update callback is called the first time. It is especially useful
         * to set some initial value even if the property has not yet changed.
         */
        bool m_initialUpdate;
    };

    //! A condition for property updates.
    std::shared_ptr<WCondition> m_propCondition;

    /**
     * A feature toggle for submitting frames to OpenVR SDK
     */
    WPropBool m_vrOn;

    /**
     * A trigger for debugging the FPS while VR module is running.
     */
    WPropTrigger  m_VR_fpsTrigger;

    /**
     * A trigger for debugging camera manipulators of Left/Right view of VR module.
     */
    WPropTrigger  m_VR_logCameraManipulators;

    /**
     * A trigger for taking a screenshot from the framebuffer.
     */
    WPropTrigger  m_VR_screenshotTrigger;

    /**
     * A trigger for applieing the main view camera manipulation to left/right view of VR module.
     */
    WPropTrigger  m_VR_cameraManipTrigger;

    /**
     * This modules root node mostly for cameras
     */
    osg::ref_ptr<WGEManagedGroupNode> m_rootnode;

    /**
     * A node for applying left Eye tracking
     */
    osg::ref_ptr<WGEGroupNode> m_leftEyeNode;

    /**
     * A node for applying right Eye tracking
     */
    osg::ref_ptr<WGEGroupNode> m_rightEyeNode;

    /**
     * A node for holding left Eye geometry
     */
    osg::ref_ptr<WGEGroupNode> m_leftEyeGeometryNode;

    /**
     * A node for holding right Eye geometry
     */
    osg::ref_ptr<WGEGroupNode> m_rightEyeGeometryNode;

    /**
     * The OpenVR SDK Interface
     */
    vr::IVRSystem *m_vrSystem;

    /**
     * Denotes whether the OpenVR SDK was initialized successfully
     */
    bool m_vrIsInitialized;

    /**
     * The OpenVR RenderModel Interface
     */
    vr::IVRRenderModels *m_vrRenderModels;

    /**
     * texture to submit to left Eye
     */
    osg::ref_ptr<osg::Texture2D> m_leftTexture;

    /**
     * texture to submit to right Eye
     */
    osg::ref_ptr<osg::Texture2D> m_rightTexture;
    /**
     * texture to submit to left Eye
     */
    osg::ref_ptr<osg::Texture2D> m_geometryColorLeft;

    /**
     * texture to submit to right Eye
     */
    osg::ref_ptr<osg::Texture2D> m_geometryColorRight;

    /**
     * texture to submit to left Eye
     */
    osg::ref_ptr<osg::Texture2D> m_textureColorLeft;

    /**
     * texture to submit to right Eye
     */
    osg::ref_ptr<osg::Texture2D> m_textureColorRight;

    /**
     * The recommended texture width from vr_system
     */
    uint32_t m_vrRenderWidth;

    /**
     * The recommended texture height from vr_system
     */
    uint32_t m_vrRenderHeight;

    /**
     * OpenVR tracking information
     */
    vr::TrackedDevicePose_t m_poses[vr::k_unMaxTrackedDeviceCount];         // NOLINT: the size is constant

    /**
     * OpenVR current grabbing device
     */
    vr::TrackedDeviceIndex_t m_grabber;
};

#endif  // WMVRCAMERA_H
