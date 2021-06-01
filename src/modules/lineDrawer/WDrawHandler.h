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

#ifndef WDRAWHANDLER_H
#define WDRAWHANDLER_H

#include <osg/Viewport>
#include <osgGA/GUIEventHandler>
#include <osgViewer/Viewer>

#include "WMLineDrawer.h"

class WMLineDrawer;

class WDrawHandler : public osgGA::GUIEventHandler
{
public:
    explicit WDrawHandler( WMLineDrawer* drawer );

    bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );

private:
    WMLineDrawer* m_drawer;
    bool m_mousePressed;
    bool m_shiftPressed;
};

#endif  // WDRAWHANDLER_H
