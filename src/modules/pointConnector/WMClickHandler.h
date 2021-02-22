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

#ifndef WMCLICKHANDLER_H
#define WMCLICKHANDLER_H

#include <iostream>
#include <string>

#include <osg/Geode>
#include <osg/Viewport>
#include <osgGA/GUIEventHandler>
#include <osgViewer/viewer>

#include "WMPointConnector.h"

/**
 * The click handler for the mouse click events of WMPointConnector
 */
class WMClickHandler : public osgGA::GUIEventHandler
{
public:
    /**
     * Constructs one WMClickHandler.
     * \param connector The WMPointConnector this handler belongs to.
     */
    explicit WMClickHandler( WMPointConnector* connector );

    /**
     * The callback for the event listener.
     * \param ea The Adapter of the event.
     * \param aa The Adapter of the action.
     * \return Whether the event should be passed on.
     */
    bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );

private:
    /**
     * Converts mouse coordinates to world coordinates and returns the near and far points.
     * \param camera The camera of the scene.
     * \param mouseX The x position of the mouse.
     * \param mouseY The y position of the mouse.
     * \param near A pointer to the place to write the near point to.
     * \param far A pointer to the place to write the far point to.
     */
    void mouseToWorldCoordinates( osg::Camera* camera, double mouseX, double mouseY, osg::Vec3* near, osg::Vec3* far );

    /**
     * The WMPointConnector this handler belongs to.
     */
    WMPointConnector* m_connector;
};

#endif  // WMCLICKHANDLER_H
