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
    virtual boost::shared_ptr<WModule> factory() const;

    /**
     * Get the icon for this module in XPM format.
     * \return the icon.
     */
    virtual const char **getXPMIcon() const;

    /**
     * Returns a Deviceproperty from m_vrSystem as a string
     */
    std::string GetDeviceProperty( vr::TrackedDeviceProperty prop );

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
     * Callback for m_active. Overwrite this in your modules to handle m_active changes separately.
     */
    void setupVRInterface();

private:
    /**
     * Callback which aligns and renders the textures.
     */
    class SafeUpdateCallback : public osg::NodeCallback
    {
    public: // NOLINT
        /**
         * Constructor.
         *
         * \param hud just set the creating HUD as pointer for later reference.
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
         * The OpenVR SDK Interface
         */
        vr::IVRSystem *m_vrSystem;

        /**
         * Denotes whether the update callback is called the first time. It is especially useful
         * to set some initial value even if the property has not yet changed.
         */
        bool m_initialUpdate;
    };

    //! A condition for property updates.
    boost::shared_ptr<WCondition> m_propCondition;

    boost::shared_mutex m_updateLock; //!< Lock to prevent concurrent threads trying to update the osg node

    /**
     * The Geode containing all the cameras and the mesh
     */
    osg::ref_ptr<WGEManagedGroupNode> m_output;

    /**
     * The OpenVR SDK Interface
     */
    vr::IVRSystem *m_vrSystem;

    /**
     * The OpenVR RenderModel Interface
     */
    vr::IVRRenderModels *m_vrRenderModels;

    /*
     * sample width for MSAA
     */
    int m_samples;

    /*
     * texture to submit to left Eye
     */
    osg::ref_ptr<osg::Texture2D> m_leftTexture;

    /*
     * texture to submit to right Eye
     */
    osg::ref_ptr<osg::Texture2D> m_rightTexture;

    /**
     * The geode with the the left Eye geometry
     */
    osg::ref_ptr<WGEGroupNode> m_leftEye;

    /**
     * The geode with the the right Eye geometry
     */
    osg::ref_ptr<WGEGroupNode> m_rightEye;

    WPropDouble m_leftEyePos; //!< x position of the slice

    WPropDouble m_rightEyePos; //!< x position of the slice

    /**
     * A nice feature trigger for turning VR on and off
     */
    WPropBool m_vrOn;

    /**
     * The recommended texture width from vr_system
     */
    uint32_t m_vrRenderWidth;
    /**
     * The recommended texture height from vr_system
     */
    uint32_t m_vrRenderHeight;
};

#endif  // WMVRCAMERA_H
