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

#include <vector>

#include "WActionHandler.h"


WActionHandler::WActionHandler()
{
    m_undo = ActionStack( new std::vector< WFiberAction::SPtr >() );
    m_redo = ActionStack( new std::vector< WFiberAction::SPtr >() );
}

WActionHandler::~WActionHandler()
{
    m_undo->clear();
    m_redo->clear();
}

void WActionHandler::pushAction( WFiberAction::SPtr action )
{
    m_undo->push_back( action );
    m_redo->clear();
}

void WActionHandler::undo()
{
    if( m_undo->empty() )
    {
        return;
    }

    WFiberAction::SPtr action = m_undo->back();

    action->undo();

    m_undo->pop_back();
    m_redo->push_back( action );
}

void WActionHandler::redo()
{
    if( m_redo->empty() )
    {
        return;
    }
    WFiberAction::SPtr action = m_redo->back();

    action->redo();

    m_redo->pop_back();
    m_undo->push_back( action );
}
