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

#include "WMClickHandler.h"

WMClickHandler::WMClickHandler( WMPointConnector* connector ):
    m_connector( connector )
{
}

bool WMClickHandler::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
    osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>( &aa );

    if( ea.getEventType() == osgGA::GUIEventAdapter::RELEASE && ea.getButton() != osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON )
    {
        osg::Camera* camera = viewer->getCamera();
        osg::Viewport* viewport = camera->getViewport();
        double halfWidth = viewport->width() * 0.5;
        double halfHeight = viewport->height() * 0.5;

        if( !halfWidth || !halfHeight )
        {
            return false;
        }

        double mouseX = ( ea.getX() - halfWidth ) / halfWidth;
        double mouseY = ( ea.getY() - halfHeight ) / halfHeight;

        osg::Vec3 nearP( 0.0, 0.0, 0.0 );
        osg::Vec3 farP( 0.0, 0.0, 0.0 );
        mouseToWorldCoordinates( camera, mouseX, mouseY, &nearP, &farP );

        osg::Vec3 direction = farP - nearP;
        direction.normalize();

        bool left = ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON;

        m_connector->handleClick( nearP, direction, left );
    }
    return false;
}

void WMClickHandler::mouseToWorldCoordinates( osg::Camera* camera, double mouseX, double mouseY, osg::Vec3* nearP, osg::Vec3* farP )
{
    osg::Matrix VP = camera->getViewMatrix() * camera->getProjectionMatrix();

    osg::Matrix inverseVP;
    inverseVP.invert( VP );

    // compute world near far
    osg::Vec3 nearPoint( mouseX, mouseY, -1.0f );
    osg::Vec3 farPoint( mouseX, mouseY, 1.0f );
    *nearP = nearPoint * inverseVP;
    *farP = farPoint * inverseVP;
}
