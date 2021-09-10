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

#ifndef WONSCREENSELECTION_H
#define WONSCREENSELECTION_H

#include <vector>

#include <boost/function.hpp>
#include <osg/Depth>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/LineWidth>
#include <osg/Projection>

#include "core/graphicsEngine/postprocessing/WGEPostprocessingNode.h"
#include "core/graphicsEngine/shaders/WGEShader.h"
#include "core/kernel/WKernel.h"
#include "WOnscreenSelectionHandler.h"

/**
 * Forward declaration of the SelectionHandler.
 */
class WOnscreenSelectionHandler;

/**
 * Manages different types of selection.
 */
class WOnscreenSelection
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
     * A typedef for the type of the callbacks.
     */
    typedef boost::function< void ( enum WSelectionType, float, float ) > CallbackType;

    /**
     * Not only creates the Object but also sets up the drawing context.
     */
    WOnscreenSelection();

    /**
     * Cleans up all the now unneeded drawing context stuff.
     */
    ~WOnscreenSelection();

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
     * Clears the current selection.
     */
    void clear();

    /**
     * Whether a point is selected or not.
     * \param x The x position.
     * \param y The y position.
     * \param z The z position.
     * \return true The point is selected.
     * \return false The point is not selected.
     */
    bool isSelected( float x, float y, float z );

    /**
     * Checks if the positions are selected and only returns those that are.
     * \param positions The positions to check
     * \return std::vector< WPosition > A vector of selected positions
     */
    std::vector< WPosition > isSelected( std::vector< WPosition> positions );

    /**
     * Sets the current selection type.
     * \param selectionType The selection type to set.
     */
    void setSelectionType( enum WSelectionType selectionType );

    /**
     * Sets the callback for the onstart function.
     * Use NULL to reset.
     * 
     * \param onstart The Callback or NULL. 
     */
    void setOnstart( CallbackType onstart );

    /**
     * Sets the callback for the onend function.
     * Use NULL to reset.
     * 
     * \param onend The Callback or NULL. 
     */
    void setOnend( CallbackType onend );

    /**
     * Sets the callback for the onmove function.
     * Use NULL to reset.
     * 
     * \param onmove The Callback or NULL. 
     */
    void setOnmove( CallbackType onmove );

    /**
     * Returns whether this manager is currently creating a selection.
     * \return true It is creating a selection.
     * \return false It is not creating a selection.
     */
    bool isSelecting();

    /**
     * Check for the box selection.
     * \param x The x position of the point.
     * \param y The y position of the point.
     * \return true The point is inside of the box.
     * \return false The point is not inside of the box
     */
    bool boxCheck( float x, float y );

    /**
     * Check for the brush selection.
     * \param x The x position of the point.
     * \param y The y position of the point.
     * \return true The point is on the brush.
     * \return false The point is not on the brush
     */
    bool brushCheck( float x, float y );

    /**
     * Check for the lineloop selection.
     * \param x The x position of the point.
     * \param y The y position of the point.
     * \return true The point is inside of the lineloop.
     * \return false The point is inside of the lineloop
     */
    bool lineloopCheck( float x, float y );

    /**
     * Calculates the crossing number product.
     * \param x The x position of the point.
     * \param y The y position of the point.
     * \param b The start of the line.
     * \param c The end of the line.
     * \return int Either -1, 0 or 1.
     */
    int crossingNumberProduct( float x, float y, WPosition b, WPosition c );

    /**
     * Sets the click type.
     * \param clickType true for left click, false for right click.
     */
    void setClickType( bool clickType );

    /**
     * Gets the click type.
     * \return true left click.
     * \return false right click.
     */
    bool getClickType();

    /**
     * \return true The mouse has been moved.
     * \return false The mouse has not been moved.
     */
    bool hasMoved();

private:
    /**
     * Updates the current rendered data.
     */
    void updateDisplay();

    osg::ref_ptr< osg::Projection > m_projection; //!< The Projection to draw on.

    enum WSelectionType m_selectionType; //!< The current selection type.

    osg::ref_ptr< WOnscreenSelectionHandler > m_selectionHandler; //!< The SelectionHandler for this object.

    std::vector< WPosition > m_line; //!< The points that are used for the selection.

    bool m_isSelecting; //!< Whether a selection has been started or not.

    osg::ref_ptr< WGEShader > m_shader; //!< The shader for the selection.

    float m_thickness; //!< The thickness of the brush.

    bool m_clickType; //!< Whether this is a left click or not.

    bool m_moved; //!< Whether the mouse has been moved.

    CallbackType m_onstart; //!< The Callback for the start function.

    CallbackType m_onend; //!< The Callback for the end function.

    CallbackType m_onmove; //!< The Callback for the move function.
};

#endif  // WONSCREENSELECTION_H
