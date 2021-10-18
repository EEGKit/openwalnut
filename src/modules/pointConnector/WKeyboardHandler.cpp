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
static const unsigned int SHIFT_DOWN = 65505;
static const unsigned int SHIFT_UP = 16777248;

static const unsigned int KEY_A = 65;
static const unsigned int KEY_C = 67;
static const unsigned int KEY_T = 84;
static const unsigned int KEY_X = 88;
static const unsigned int KEY_Y = 89;
static const unsigned int KEY_Z = 90;

static const double SCALING_FACTOR = 2.0;

WKeyboardHandler::WKeyboardHandler( WMPointConnector* connector ):
    m_connector( connector )
{
}

bool WKeyboardHandler::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& )
{
    if( ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN )
    {
        if( m_isCtrl )
        {
            switch( ea.getKey() )
            {
                case KEY_Y:
                    m_connector->pushEventQueue( std::bind( &WActionHandler::redo, m_connector->getFiberHandler()->getActionHandler() ) );
                    return true;
                case KEY_Z:
                    m_connector->pushEventQueue( std::bind( &WActionHandler::undo, m_connector->getFiberHandler()->getActionHandler() ) );
                    return true;
            }
        }

        if( m_isShift )
        {
            switch( ea.getKey() )
            {
                case KEY_A:
                    m_connector->getFiberHandler()->createNewFiber();
                    return true;
                case KEY_C:
                    m_connector->pushEventQueue( std::bind( &WMPointConnector::acceptPrediction, m_connector ) );
                    return true;
                case KEY_T:
                    m_connector->getFiberHandler()->toggleFiber( m_connector->getFiberHandler()->getSelectedFiber() );
                    return true;
            }
        }

        WPosition scal = m_connector->getScaling()->get();
        switch( ea.getKey() )
        {
            case CTRL_DOWN:
                m_isCtrl = true;
                return true;
            case SHIFT_DOWN:
                m_isShift = true;
                return true;
            case KEY_X:
                m_connector->getScaling()->set( WPosition( scal.x() * ( m_isShift ? 1.0 / SCALING_FACTOR : SCALING_FACTOR ), scal.y(), scal.z() ) );
                return true;
            case KEY_Y:
                m_connector->getScaling()->set( WPosition( scal.x(), scal.y() * ( m_isShift ? 1.0 / SCALING_FACTOR : SCALING_FACTOR ), scal.z() ) );
                return true;
            case KEY_Z:
                m_connector->getScaling()->set( WPosition( scal.y(), scal.x(), scal.z() * ( m_isShift ? 1.0 / SCALING_FACTOR : SCALING_FACTOR ) ) );
                return true;
        }
    }

    if( ea.getEventType() == osgGA::GUIEventAdapter::KEYUP )
    {
        switch( ea.getKey() )
        {
            case CTRL_UP:
                m_isCtrl = false;
                return true;
            case SHIFT_UP:
                m_isShift = false;
                return true;
        }
    }

    return false;
}
