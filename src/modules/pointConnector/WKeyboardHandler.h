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

#ifndef WKEYBOARDHANDLER_H
#define WKEYBOARDHANDLER_H

#include <iostream>
#include <string>

#include <osg/Geode>
#include <osg/Viewport>
#include <osgGA/GUIEventHandler>
#include <osgViewer/Viewer>

#include "WMPointConnector.h"


/**
 * Forward declartion for the WMPointConnector
 */
class WMPointConnector;

/**
 * The keyboard handler for the keyboard events of WMPointConnector
 */
class WKeyboardHandler : public osgGA::GUIEventHandler
{
public:
    /**
     * Constructs one WKeyboardHandler.
     * \param connector The WMPointConnector this handler belongs to.
     */
    explicit WKeyboardHandler( WMPointConnector* connector );

    /**
     * The callback for the event listener.
     * \param ea The Adapter of the event.
     * \param aa The Adapter of the action.
     * \return Whether the event should be passed on.
     */
    bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );

private:
    /**
     * The WMPointConnector this handler belongs to.
     */
    WMPointConnector* m_connector;

    /**
     * Whether ctrl is pressed or not.
     */
    bool m_isCtrl = false;

    /**
     * Whether shift is pressed or not.
     */
    bool m_isShift = false;
};

#endif  // WKEYBOARDHANDLER_H
