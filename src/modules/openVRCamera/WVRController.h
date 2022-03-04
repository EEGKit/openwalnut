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

#ifndef WVRCONTROLLER_H
#define WVRCONTROLLER_H

#include <string>

#include <osg/Node>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>

#include <openvr.h> // NOLINT

#include "core/graphicsEngine/WGEZoomTrackballManipulator.h"
#include "core/kernel/WKernel.h"

#include "WMVRCamera.h"

/**
 * Handles a hand controller.
 */
class WVRController
{
public:
    /**
     * Creates a new controller with its role (LEFT / RIGHT hand).
     * \param role The role of the controller.
     */
    explicit WVRController( int role );

    /**
     * Sets the device id of the controller.
     * \param deviceID The device id.
     */
    void setDeviceID( int deviceID );

    /**
     * Gets the geometry node of the controller
     * \return osg::ref_ptr< osg::Node > The node.
     */
    osg::ref_ptr< osg::Node > getNode();

    /**
     * Gets the current position of the controller relative to the camera position.
     * \return osg::Vec3 The position.
     */
    osg::Vec3 getPosition();

    /**
     * Gets the current rotation of the controller.
     * \return osg::Quat The rotation.
     */
    osg::Quat getRotation();

    /**
     * Creates the geometry with a model at the given path.
     * \param path The path to the model.
     */
    void createGeometry( std::string path );

    /**
     * Updates the current pose of the controller.
     * \param vrSystem The VRSystem to get the pose from.
     */
    void updatePose( vr::IVRSystem* vrSystem );

private:
    int m_role; //!< The role of the controller
    int m_deviceID; //!< The device id of the controller

    osg::ref_ptr< osg::Node > m_node; //!< The geometry of the controller.

    osg::Vec3 m_position; //!< The current position of the controller relative to the camera position.
    osg::Quat m_rotation; //!< The current rotation of the controller.
};

#endif  // WVRCONTROLLER_H
