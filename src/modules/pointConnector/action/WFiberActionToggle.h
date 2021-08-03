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

#ifndef WFIBERACTIONTOGGLE_H
#define WFIBERACTIONTOGGLE_H

#include <memory>
#include <string>

#include <osg/Geode>

#include "../WFiberHandler.h"
#include "WFiberAction.h"


/**
 * The action when toggling a fiber.
 */
class WFiberActionToggle : public WFiberAction
{
public:
    /**
     * A shared_ptr to this class.
     */
    typedef std::shared_ptr< WFiberActionToggle > SPtr;

    /**
     * Creates this action.
     * \param position The position of this fiber in the fibers vector.
     * \param fiberHandler The WFiberHandler of the action.
     */
    WFiberActionToggle( size_t position, WFiberHandler* fiberHandler );

    /**
     * Empty destructor.
     */
    ~WFiberActionToggle();

    /**
     * Undos this action.
     */
    virtual void undo();

    /**
     * Redos this action.
     */
    virtual void redo();

private:
    /**
     * The position of this fiber in the fibers vector.
     */
    size_t m_position;

    /**
     * The WFiberHandler of the action.
     */
    WFiberHandler* m_fiberHandler;
};

#endif  // WFIBERACTIONTOGGLE_H
