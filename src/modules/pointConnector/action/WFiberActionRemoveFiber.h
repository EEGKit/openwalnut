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

#ifndef WFIBERACTIONREMOVEFIBER_H
#define WFIBERACTIONREMOVEFIBER_H

#include <memory>
#include <string>

#include <osg/Geode>

#include "../WFiberHandler.h"
#include "WFiberAction.h"


/**
 * The action when removing a fiber.
 */
class WFiberActionRemoveFiber : public WFiberAction
{
public:
    /**
     * A shared_ptr to this class.
     */
    typedef std::shared_ptr< WFiberActionRemoveFiber > SPtr;

    /**
     * Creates this action.
     * \param name The name of the fiber.
     * \param position The position of this fiber in the fibers vector.
     * \param fiber The fiber that was removed.
     * \param hidden If the fiber was hidden.
     * \param fiberHandler The WFiberHandler of the action.
     */
    WFiberActionRemoveFiber( std::string name, size_t position, WFiberHandler::PCFiber fiber, bool hidden, WFiberHandler* fiberHandler );

    /**
     * Empty destructor.
     */
    ~WFiberActionRemoveFiber();

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
     * The name of the fiber.
     */
    std::string m_name;

    /**
     * The position of this fiber in the fibers vector.
     */
    size_t m_position;

    /**
     * The fiber that was removed.
     */
    WFiberHandler::PCFiber m_fiber;

    /**
     * Whether the fiber is hidden.
     */
    bool m_hidden;

    /**
     * The WFiberHandler of the action.
     */
    WFiberHandler* m_fiberHandler;
};

#endif  // WFIBERACTIONREMOVEFIBER_H
