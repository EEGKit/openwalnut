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

#ifndef WSELECTIONMANAGER_H
#define WSELECTIONMANAGER_H

#include <vector>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/LineWidth>

#include "core/kernel/WKernel.h"
#include "WOverlay.h"
#include "WSelectionHandler.h"

/**
 * Forward declaration of the SelectionHandler.
 */
class WSelectionHandler;

/**
 * Manages different types of selection.
 */
class WSelectionManager
{
public:
    /**
     * The different types of selection.
     */
    enum class WSelectionType
    {
        BRUSH, LINELOOP, BOX
    };

    /**
     * Not only creates the Object but also sets up the drawing context.
     */
    WSelectionManager();

    /**
     * Cleans up all the now unneeded drawing context stuff.
     */
    ~WSelectionManager();

    /**
     * Starts the selection.
     * 
     * \param x The normalized x position of the mouse.
     * \param y The normalized y position of the mouse.
     */
    void start( float x, float y );

    /**
     * Ends the selection.
     * 
     * \param x The normalized x position of the mouse.
     * \param y The normalized y position of the mouse.
     */
    void end( float x, float y );

    /**
     * Handles mouse move while selecting.
     * 
     * \param x The normalized x position of the mouse.
     * \param y The normalized y position of the mouse.
     */
    void move( float x, float y );

    /**
     * Gets the current selection type.
     * 
     * \return enum WSelectionType The current selection type.
     */
    enum WSelectionType getSelectionType();

    /**
     * Sets the current selection type.
     * \param selectionType The selection type to set.
     */
    void setSelectionType( enum WSelectionType selectionType );

private:
    /**
     * Updates the current rendered data.
     */
    void updateDisplay();

    osg::ref_ptr< WOverlay > m_overlay; //!< The overlay to draw on.

    enum WSelectionType m_selectionType; //!< The current selection type.

    osg::ref_ptr< WSelectionHandler > m_selectionHandler; //!< The SelectionHandler for this object.

    std::vector< WPosition > m_line; //!< The points that are used for the selection.

    bool m_hasStarted; //!< Whether a selection has been started or not.
};

#endif  // WSELECTIONMANAGER_H
