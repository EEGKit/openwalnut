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

#include "WFiberHandler.h"
#include "WKeyboardHandler.h"


static const unsigned int CTRL_DOWN = 65507;
static const unsigned int CTRL_UP = 16777249;
static const unsigned int KEY_Z = 90;
static const unsigned int KEY_Y = 89;

WKeyboardHandler::WKeyboardHandler( WMPointConnector* connector ):
    m_connector( connector )
{
}

bool WKeyboardHandler::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
    if( ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN )
    {
        if( ea.getKey() == CTRL_DOWN )
        {
            m_isCtrl = true;
        }
        else if( ea.getKey() == KEY_Z && m_isCtrl )
        {
            m_connector->getFiberHandler()->getActionHandler()->undo();
            return true;
        }
        else if( ea.getKey() == KEY_Y && m_isCtrl )
        {
            m_connector->getFiberHandler()->getActionHandler()->redo();
            return true;
        }
    }

    if( ea.getEventType() == osgGA::GUIEventAdapter::KEYUP )
    {
        if( ea.getKey() == CTRL_UP )
        {
            m_isCtrl = false;
        }
    }

    return false;
}
