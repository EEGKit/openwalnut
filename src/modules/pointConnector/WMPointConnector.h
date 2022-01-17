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

#ifndef WMPOINTCONNECTOR_H
#define WMPOINTCONNECTOR_H

#include <cmath>
#include <memory>
#include <string>
#include <vector>

#include <osg/Geode>

#include "core/dataHandler/WDataSetFibers.h"
#include "core/dataHandler/WDataSetPoints.h"
#include "core/graphicsEngine/onscreenSelection/WOnscreenSelection.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleContainer.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

#include "../fiberDisplay/WMFiberDisplay.h"
#include "../pointRenderer/WMPointRenderer.h"

class WClickHandler;
class WConnectorData;
class WFiberHandler;
class WKeyboardHandler;

/**
 * This module connects the points in a point dataset.
 *
 * \ingroup modules
 */
class WMPointConnector: public WModuleContainer
{
    /**
     * Test is your best ... friend
     */
    friend class WMPointConnectorTest;

public:
    /**
     * represents a std::shared_ptr to a vector containing a vector of floats.
     */
    typedef std::shared_ptr< std::vector< float > > SPFloatVector;

    /**
     * represents a std::shared_ptr to a vector containing a vector of size_t.
     */
    typedef std::shared_ptr< std::vector< size_t > > SPSizeVector;

    /**
     * A shared_ptr to this class.
     */
    typedef std::shared_ptr< WMPointConnector > SPtr;

    /**
     * Constructor. Creates the module skeleton.
     */
    WMPointConnector();

    /**
     * Destructor.
     */
    virtual ~WMPointConnector();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description to module.
     */
    virtual const std::string getDescription() const;

    /**
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual std::shared_ptr< WModule > factory() const;

    /**
     * Get the icon for this module in XPM format.
     * \return The icon.
     */
    virtual const char** getXPMIcon() const;

    /**
     * Handles a click on the drawing area.
     * It checks all vertices and whether they are clicked.
     * \param cameraPosition The position of the camera.
     * \param direction The direction of the click.
     * \param isLeftClick Whether this click is a left or a right click.
     */
    void handleClick( osg::Vec3 cameraPosition, osg::Vec3 direction, bool isLeftClick );

    /**
     * Redraws the current vertices with their colors.
     */
    void updatePoints();

    /**
     * Updates the fiber output
     */
    void updateOutput();

    /**
     * Updates all
     */
    void updateAll();

    /**
     * \return std::shared_ptr< WConnectorData > The WConnectorData of this module.
     */
    std::shared_ptr< WConnectorData > getConnectorData();

    /**
     *
     * \return std::shared_ptr< WFiberHandler > The WFiberHandler of this module.
     */
    std::shared_ptr< WFiberHandler > getFiberHandler();

    /**
     *
     * \return shared_ptr< WOnscreenSelection > The WOnscreenSelection of this module.
     */
    std::shared_ptr< WOnscreenSelection > getOnscreenSelection();

    /**
     * 
     * \return WPropPosition The scaling of this module.
     */
    WPropPosition getScaling();

    /**
     * Pushes a function to the selection queue
     * \param func The function
     */
    void pushEventQueue( std::function< void() > func );

    /**
     * Accepts the current prediction.
     */
    void acceptPrediction();

protected:
    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void moduleMain();

    /**
     * Initialize the connectors this module is using.
     */
    virtual void connectors();

    /**
     * Initialize the properties for this module.
     */
    virtual void properties();

    /**
     * Deactivates or activates the inner modules when active changes.
     */
    virtual void activate();

private:
    /**
     * Checks if a vertex with a certain radius is hit by a ray.
     * \param rayStart The starting point of the ray.
     * \param rayDir The direction of the ray.
     * \param vertex The vertex to be checked.
     * \param radius The radius of the vertex.
     * \return < 0 if ray does not intersect. == 0 if ray is a tangent. > 0 if ray intersects.
     */
    float hitVertex( osg::Vec3 rayStart, osg::Vec3 rayDir, osg::Vec3 vertex, float radius );

    /**
     * Creates the WMPointRenderer and runs it.
     */
    void createPointRenderer();

    /**
     * Creates the WMFiberDisplay and runs it.
     */
    void createFiberDisplay();

    /**
     * Creates the WKeyboardHandler and registers them.
     */
    void createHandler();

    /**
     * Handles the input of this module.
     */
    void handleInput();

    /**
     * Finds the point that was clicked and writes it into hitIdx,
     * while return whether a point was hit.
     * \param cameraPosition The position of the camera.
     * \param direction The direction to check.
     * \param hitIdx A pointer to a variable where the index of the clicked point is written.
     * \return true A point was clicked.
     * \return false A point was not clicked.
     */
    bool findClickedPoint( osg::Vec3 cameraPosition, osg::Vec3 direction, size_t* hitIdx );

    /**
     * Toggles the activation of a module.
     * \param mod THe module to change the activation of.
     */
    void toggleActivationOfModule( WModule::SPtr mod );

    /**
     * The end callback of m_onscreenSelection.
     * \param type  The type of the selection.
     * \param x     The x position of the selection.
     * \param y     The y position of the selection.
     */
    void selectionEnd( WOnscreenSelection::WSelectionType type, float x, float y );

    /**
     * Checks whether a vertex is adaptively hidden.
     * \param vertex The vertex to check.
     * \param from Optional parameter for vertex to check from.
     * \return true The vertex is hidden.
     * \return false The vertex is visible.
     */
    bool isAdaptivelyHidden( osg::Vec3 vertex, osg::Vec3* from = NULL );

    /**
     * Handles a selection with only one click
     * \param clickType The type of click (true -> left, false -> right).
     * \param x The x position.
     * \param y The y position.
     */
    void handleClickSelection( bool clickType, double x, double y );

    /**
     * Handles the selection of a left click.
     * \param positions The positions in the selection.
     */
    void handleLeftSelection( std::vector< WPosition > positions );

    /**
     * Handles the selection of a right click.
     * \param positions The positions in the selection.
     */
    void handleRightSelection( std::vector< WPosition > positions );

    /**
     * Creates a track continuation prediction.
     */
    void createPrediction();

    /**
     * The WMPointRenderer associated with this module.
     */
    WModule::SPtr m_pointRenderer;

    /**
     * The WMFiberDisplay associated with this module.
     */
    WModule::SPtr m_fiberDisplay;

    /**
     * The data of this module.
     */
    std::shared_ptr< WConnectorData > m_connectorData;

    /**
     * The WFiberHandler of this module.
     */
    std::shared_ptr< WFiberHandler > m_fiberHandler;

    /**
     * An input connector used to get points from other modules.
     */
    std::shared_ptr< WModuleInputData< WDataSetPoints > > m_pointInput;

    /**
     * An output connector used to provide fibers to other modules.
     */
    std::shared_ptr< WModuleOutputData< WDataSetFibers > > m_fiberOutput;

    /**
     * The internal pointOutput to pass data to the WMPointRenderer
     */
    std::shared_ptr< WModuleOutputData< WDataSetPoints > > m_pointOutput;

    /**
     * Enables possibility for multiselection of points.
     */
    std::shared_ptr< WOnscreenSelection > m_onscreenSelection;

    /**
     * A vector for the events.
     */
    std::vector< std::function< void() > > m_eventQueue;

    /**
     * A mutex for the vector to make it thread-safe.
     */
    std::mutex m_eventMutex;

    /**
     * The current prediction.
     */
    std::vector< WPosition > m_prediction;

    /**
     * A condition notifying when something was added to the event queue.
     */
    WCondition::SPtr m_eventCondition;

    /**
     * The keyboard handler for this point connector.
     */
    osg::ref_ptr< WKeyboardHandler > m_keyboardHandler;

    /**
     * Property to enable the adjusted dijkstra.
     */
    WPropBool m_enableSAPT;

    /**
     * Property to enable adaptive visibility.
     */
    WPropBool m_enableAdaptiveVisibility;

    /**
     * Enables the prediction.
     */
    WPropBool m_enablePrediction;

    /**
     * Property to set the angle for the adaptive visibility.
     */
    WPropDouble m_adaptiveVisibilityAngle;

    /**
     * Property to set the opacity of the hidden points.
     */
    WPropDouble m_hiddenOpacity;

    /**
     * Property for the scaling as Vector.
     */
    WPropPosition m_scaling;

    /**
     * Property to enable the sizes.
     */
    WPropBool m_enableSizes;
};

#endif  // WMPOINTCONNECTOR_H
