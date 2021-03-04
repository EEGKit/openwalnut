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

#include "WKeyboardHandler.h"

WKeyboardHandler::WKeyboardHandler( WMPointConnector* connector ):
    m_connector( connector )
{
}

bool WKeyboardHandler::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
    if( ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN )
    {
        std::cout << "DOWN KEY:\t\t\t" << ea.getKey() << std::endl;
        std::cout << "\tUnmodified Key:\t\t" << ea.getUnmodifiedKey() << std::endl;
        std::cout << "\tModKeyMask:\t\t"<< ea.getModKeyMask() << std::endl;
    }

    if( ea.getEventType() == osgGA::GUIEventAdapter::KEYUP )
    {
        std::cout << "UP KEY:\t\t\t\t" << ea.getKey() << std::endl;
        std::cout << "\tUnmodified Key:\t\t" << ea.getUnmodifiedKey() << std::endl;
        std::cout << "\tModKeyMask:\t\t"<< ea.getModKeyMask() << std::endl;
    }

    return false;
}
