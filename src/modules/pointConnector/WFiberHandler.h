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

#include <memory>
#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>
#include <osg/Geode>

#include "WMPointConnector.h"
#include "action/WActionHandler.h"
#include "core/common/WItemSelectionItem.h"
#include "core/common/WItemSelectionItemTyped.h"
#include "core/common/WItemSelector.h"
#include "core/common/WPropertyGroup.h"
#include "core/kernel/WModule.h"


/**
 * Handles the fibers of the WMPointsConnector.
 */
class WFiberHandler
{
public:
    /**
     * A shared_ptr to this class.
     */
    typedef std::shared_ptr< WFiberHandler > SPtr;

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
    typedef std::shared_ptr< PCFiberList > PCFiberListSPtr;

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
     * \param fiberIdx The index of the fiber to add the vertex to.
     * \param silent Whether or not this should add to the undo stack.
     */
    void addVertexToFiber( osg::Vec3 vertex, size_t fiberIdx, bool silent = false );

    /**
     * Adds multiple vertices to the currently selected fiber.
     * \param vertices The vertices to add.
     * \param fiberIdx The index of the fiber to add the vertex to.
     * \param silent Whether or not this should add to the undo stack.
     */
    void addVerticesToFiber( std::vector< osg::Vec3 > vertices, size_t fiberIdx, bool silent = false );

    /**
     * Adds a vertex to the currently selected fiber.
     * \param vertex The vertex to add.
     * \param fiberIdx The index of the fiber to add the vertex to.
     * \param position The position where to add the vertex.
     * \param silent Whether or not this should add to the undo stack.
     */
    void addVertexToFiberAt( osg::Vec3 vertex, size_t fiberIdx, size_t position, bool silent = false );

    /**
     * Removes a vertex from the currently selected fiber.
     * \param vertex The vertex to remove.
     * \param fiberIdx The index of the fiber to remove the vertex from.
     * \param silent Whether or not this should add to the undo stack.
     */
    void removeVertexFromFiber( osg::Vec3 vertex, size_t fiberIdx, bool silent = false );

    /**
     * Removes multiple vertices from the currently selected fiber.
     * \param vertices The vertices to remove.
     * \param fiberIdx The index of the fiber to remove the vertex from.
     * \param silent Whether or not this should add to the undo stack.
     */
    void removeVerticesFromFiber( std::vector< osg::Vec3 > vertices, size_t fiberIdx, bool silent = false );

    /**
     * Adds a new fiber.
     * \param name The name of the fiber.
     * \param silent Whether or not this should add to the undo stack.
     * \param updateSelector Whether the UI selector should be updated
     */
    void addFiber( std::string name, bool silent = false, bool updateSelector = true );

    /**
     * Adds a new or an old fiber at a specific position.
     * \param name The name of the fiber.
     * \param position The position of the fiber.
     * \param hidden Whether the fiber is hidden or not.
     * \param silent Whether or not this should add to the undo stack.
     * \param fiber The fiber to add.
     */
    void addFiberAt( std::string name, size_t position, bool hidden, bool silent = false, PCFiber fiber = PCFiber() );

    /**
     * Removes a fiber at an index in the fibers vector.
     * \param idx The index of the fiber.
     * \param silent Whether or not this should add to the undo stack.
     * \param updateSelector Whether the UI selector should be updated
     */
    void removeFiber( size_t idx, bool silent = false, bool updateSelector = true );

    /**
     * Toggles the visibility of a fiber.
     * \param idx The index of the fiber.
     * \param silent Whether or not this should add to the undo stack.
     */
    void toggleFiber( size_t idx, bool silent = false );

    /**
     * Creates a new fiber.
     */
    void createNewFiber();

    /**
     * Updates the UI selector.
     * 
     * \param idx index of the selected fiber.
     */
    void selectorUpdate( size_t idx );

    /**
     * Clears the data in this handler.
     */
    void clear();

    /**
     * Selects a fiber by its index in the fibers vector.
     * \param idx The index of the fiber.
     */
    void selectFiber( size_t idx );

    /**
     * Selects the last point of the currently selected fiber.
     */
    void selectLastPoint();

    /**
     * Sorts the vertices by their z value.
     */
    void sortVertices();

    /**
     * Checks if a fiber is hidden.
     * \param idx The index of the fiber to check.
     * \return true The fiber is hidden.
     * \return false The fiber is not hidden.
     */
    bool isHidden( size_t idx );

    /**
     * Gets the index of the fiber of a point.
     * \param vertex The point to check.
     * \param idx The index of the fiber of the point. This is a pointer and the secondary output. If this is NULL it is ignored.
     * \return true The point is in a fiber.
     * \return false The point is not in a fiber.
     */
    bool getFiberOfPoint( osg::Vec3 vertex, size_t* idx = NULL );

    /**
     * Checks whether a point is in a hidden fiber.
     * \param vertex The point to check.
     * \return true The point is hidden.
     * \return false The point is not hidden.
     */
    bool isPointHidden( osg::Vec3 vertex );

    /**
     * \return PCFiberListSPtr All the fibers in this handler.
     */
    PCFiberListSPtr getFibers();

    /**
     * \return WMPointConnector* The WMPointConnector this belongs to
     */
    WMPointConnector* getPointConnector();

    /**
     * \return size_t The currently selected fiber.
     */
    size_t getSelectedFiber();

    /**
     * \return WActionHandler::SPtr The WActionHandler of this class.
     */
    WActionHandler::SPtr getActionHandler();

    /**
     * Sets the fiber count.
     * \param fiberCount The new fiber count.
     */
    void setFiberCount( size_t fiberCount );

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
    std::shared_ptr< WItemSelection > m_possibleFiberSelections;

    /**
     * Represents the current active fiber selection property.
     */
    WPropSelection m_fiberSelection;

    /**
     * The type of the selection.
     */
    WPropSelection m_selectionType;

    /**
     * Property (button) to add a new fiber.
     */
    WPropTrigger m_addFiber;

    /**
     * Property (button) to remove a fiber.
     */
    WPropTrigger m_removeFiber;

    /**
     * Property (button) to toggle the visibility of a fiber.
     */
    WPropTrigger m_toggleFiber;

    /**
     * Property (button) to undo the last action.
     */
    WPropTrigger m_undoTrigger;

    /**
     * Property (button) to redo the last action.
     */
    WPropTrigger m_redoTrigger;

    /**
     * A pointer to the list of fibers.
     */
    PCFiberListSPtr m_fibers;

    /**
     * Vector for the visibility of the fibers.
     */
    std::shared_ptr< std::vector< char > > m_hidden;
};

#endif  // WFIBERHANDLER_H
