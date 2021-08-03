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

#ifndef WACTIONHANDLER_H
#define WACTIONHANDLER_H

#include <memory>
#include <vector>

#include "WFiberAction.h"


/**
 * Handles undo and redo action.
 */
class WActionHandler
{
public:
    /**
     * A shared_ptr to this class.
     */
    typedef std::shared_ptr< WActionHandler > SPtr;

    /**
     * Creates the undo and redo vectors.
     */
    WActionHandler();

    /**
     * Clears the vectors for destruction.
     */
    ~WActionHandler();

    /**
     * Pushes an action to the undo vector and clears the redo vector.
     * \param action The ation to push.
     */
    void pushAction( WFiberAction::SPtr action );

    /**
     * Undos the last action and pushes it to the redo vector.
     */
    void undo();

    /**
     * Redos the last action and pushes it to the undo vector.
     */
    void redo();

private:
    /**
     * A shared_ptr to a vector of an action.
     */
    typedef std::shared_ptr< std::vector< WFiberAction::SPtr > > ActionStack;

    /**
     * The undo vector.
     */
    ActionStack m_undo;

    /**
     * The redo vector.
     */
    ActionStack m_redo;
};

#endif  // WACTIONHANDLER_H
