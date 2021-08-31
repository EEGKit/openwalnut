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

#ifndef WFIBERACTION_H
#define WFIBERACTION_H

#include <memory>



/**
 * Super class for the actions.
 */
class WFiberAction
{
public:
    /**
     * A shared_ptr to this class.
     */
    typedef std::shared_ptr< WFiberAction > SPtr;

    /**
     * Empty virtual destructor.
     */
    virtual ~WFiberAction();

    /**
     * Undos this action.
     */
    virtual void undo() = 0;

    /**
     * Redos this action.
     */
    virtual void redo() = 0;
};

#endif  // WFIBERACTION_H
