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

#include "WVRController.h"

WVRController::WVRController( int role ):
    m_role( role )
{
}

void WVRController::setDeviceID( int deviceID )
{
    m_deviceID = deviceID;
}

osg::ref_ptr< osg::Node > WVRController::getNode()
{
    return m_node;
}

osg::Vec3 WVRController::getPosition()
{
    return m_position;
}

osg::Quat WVRController::getRotation()
{
    return m_rotation;
}

void WVRController::updatePose( vr::IVRSystem* vrSystem )
{
    if( vrSystem->GetControllerRoleForTrackedDeviceIndex( m_deviceID ) == m_role )
    {
        vr::TrackedDevicePose_t trackedDevicePose;
        vr::VRControllerState_t controllerState;

        vrSystem->GetControllerStateWithPose( vr::TrackingUniverseSeated, m_deviceID,
                                              &controllerState, sizeof( controllerState ), &trackedDevicePose );

        if( !trackedDevicePose.bPoseIsValid )
        {
            m_node->setNodeMask( 0 );
        }
        else
        {
            m_node->setNodeMask( 1 );

            osg::Matrixd transform = WMVRCamera::convertHmdMatrixToOSG( trackedDevicePose.mDeviceToAbsoluteTracking );

            // TODO(eschbach): refactor
            osg::Vec3 position = transform.getTrans() * 100.0;
            osg::Quat rotation = transform.getRotate();

            // switch y and z axis as openvr has different coordinate system.
            double help = position.y();
            position.y() = -position.z();
            position.z() = help;

            help = rotation.y();
            rotation.y() = -rotation.z();
            rotation.z() = help;

            osg::ref_ptr< WGEZoomTrackballManipulator > cm = osg::dynamic_pointer_cast< WGEZoomTrackballManipulator >(
            WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getCameraManipulator() );
            osg::Matrixd mainViewMatrix = cm ? cm->getMatrixWithoutZoom() :
                                                WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getCamera()->getViewMatrix();

            osg::Matrixd controllerMatrix = osg::Matrixd::rotate( rotation ) * osg::Matrixd::translate( mainViewMatrix.getTrans() + position );

            osg::MatrixTransform* mat = m_node->asTransform()->asMatrixTransform();
            mat->setMatrix( controllerMatrix );

            m_position = position;
            m_rotation = rotation;
        }
    }
}

void WVRController::createGeometry( std::string path )
{
    osg::ref_ptr< osg::Node > controllerNode = osgDB::readNodeFile( path );

    osg::ref_ptr< osg::MatrixTransform > mat = osg::ref_ptr< osg::MatrixTransform >( new osg::MatrixTransform() );
    mat->addChild( controllerNode );

    mat->setNodeMask( 0 );

    m_node = mat;
}
