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

#ifndef WFIBERHANDLER_H
#define WFIBERHANDLER_H

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <osg/Geode>

#include "core/common/WItemSelectionItem.h"
#include "core/common/WItemSelectionItemTyped.h"
#include "core/common/WItemSelector.h"
#include "core/kernel/WModule.h"

#include "WMPointConnector.h"
#include "action/WActionHandler.h"

/**
 * Handles the fibers of the WMPointsConnector.
 */
class WFiberHandler
{
public:
    /**
     * A shared_ptr to this class.
     */
    typedef boost::shared_ptr< WFiberHandler > SPtr;

    /**
     * Type of selectable items.
     */
    typedef WItemSelectionItemTyped< std::string > ItemType;

    /**
     * Vector of 3D vectors, representing points
     */
    typedef std::vector< osg::Vec3 > PCFiber;

    /**
     * Vector, that could contain multiple fibers
     */
    typedef std::vector< PCFiber > PCFiberList;

    /**
     * Shared pointer to fibers-vector
     */
    typedef boost::shared_ptr< PCFiberList > PCFiberListSPtr;

    /**
     * Creates the list of fibers.
     * \param pointConnector The WMPointConnector this belongs to.
     */
    explicit WFiberHandler( WMPointConnector* pointConnector );

    /**
     * Creates the properties for this handler.
     * \param properties The PropertyGroup to add the properties to.
     */
    void createProperties( WPropertyGroup::SPtr properties );

    /**
     * Adds a vertex to the currently selected fiber.
     * \param vertex The vertex to add.
     * \param silent Whether or not this should add to the undo stack.
     */
    void addVertexToFiber( osg::Vec3 vertex, size_t fiberIdx, bool silent = false );

    /**
     * Adds a vertex to the currently selected fiber.
     * \param vertex The vertex to add.
     * \param position The position where to add the vertex.
     * \param silent Whether or not this should add to the undo stack.
     */
    void addVertexToFiberAt( osg::Vec3 vertex, size_t fiberIdx, size_t position, bool silent = false );

    /**
     * Removes a vertex from the currently selected fiber.
     * \param vertex The vertex to remove.
     * \param silent Whether or not this should add to the undo stack.
     */
    void removeVertexFromFiber( osg::Vec3 vertex, size_t fiberIdx, bool silent = false );

    void addFiber( std::string name, bool silent = false );

    void addFiberAt( std::string name, size_t position, bool silent = false, PCFiber fiber = PCFiber() );

    void removeFiber( size_t idx, bool silent = false );

    void selectFiber( size_t idx );

    /**
     * Selects the last point of the currently selected fiber.
     */
    void selectLastPoint();

    /**
     * \return PCFiberListSPtr All the fibers in this handler.
     */
    PCFiberListSPtr getFibers();

    WMPointConnector* getPointConnector();

    size_t getSelectedFiber();

private:
    /**
     * Update handler for the properties
     * \param property updated property
     */
    void updateProperty( WPropertyBase::SPtr property );

    /**
     * A reference to the WMPointConnector this belongs to.
     */
    WMPointConnector* m_pointConnector;

    /**
     * Handles the undo and redo action.
     */
    WActionHandler::SPtr m_actionHandler;

    /**
     * Stores the amount of new created fibers.
     */
    int m_fiberCount = 0;

    /**
     * Represents the index of the current active fiber.
     */
    size_t m_selectedFiber = 0;

    /**
     * The list of possible fibers, which can be selected.
     */
    boost::shared_ptr< WItemSelection > m_possibleFiberSelections;

    /**
     * Represents the current active fiber selection property.
     */
    WPropSelection m_fiberSelection;

    /**
     * Property (button) to add a new Fiber.
     */
    WPropTrigger m_addFiber;

    WPropTrigger m_removeFiber;

    WPropTrigger m_undoTrigger;

    WPropTrigger m_redoTrigger;

    /**
     * A pointer to the list of fibers.
     */
    PCFiberListSPtr m_fibers;
};

#endif  // WFIBERHANDLER_H
