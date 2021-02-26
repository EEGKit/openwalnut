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
#include <string>
#include <vector>

#include "core/common/math/linearAlgebra/WVectorFixed.h"
#include "core/common/WItemSelectionItem.h"
#include "core/common/WItemSelector.h"
#include "core/common/WItemSelectionItemTyped.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"


class WDataSetPoints;
class WDataSetFibers;
class WGEShader;
class WGEManagedGroupNode;
class WGEPostprocessingNode;

/**
 * This module connects the points in a point dataset.
 *
 * \ingroup modules
 */
class WMPointConnector: public WModule
{
    /**
     * Test is your best ... friend
     */
    friend class WMPointConnectorTest;

public:
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
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     * Get the icon for this module in XPM format.
     * \return The icon.
     */
    virtual const char** getXPMIcon() const;

    /**
     * Redraws the current vertices with their colors.
     */
    void redraw();

    /**
     * Handles a click on the drawing area.
     * It checks all vertices and whether they are clicked.
     * \param cameraPosition The position of the camera.
     * \param direction The direction of the click.
     */
    void handleClick( osg::Vec3 cameraPosition, osg::Vec3 direction );

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

private:
    /**
     * Type of selectable items.
     */
    typedef WItemSelectionItemTyped< std::string > ItemType;

    /**
     * Vector of 3D vectors, representing points
     */
    typedef std::vector< osg::Vec3 >            PCFiber;
    /**
     * Vector, that could contain multiple fibers
     */
    typedef std::vector< PCFiber >              PCFiberList;
    /**
     * Shared pointer to fibers-vector
     */
    typedef boost::shared_ptr< PCFiberList >    PCFiberListSPtr;


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
     * Update handler for the properties
     * \param property updated property
     */
    void updateProperty( WPropertyBase::SPtr property );

    /**
     * Updates the fiber output
     */
    void updateOutput();

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * An input connector used to get points from other modules.
     */
    boost::shared_ptr< WModuleInputData< WDataSetPoints > > m_pointInput;

    /**
     * An output connector used to provide fibers to other modules.
     */
    boost::shared_ptr< WModuleOutputData< WDataSetFibers > > m_fiberOutput;

    /**
     * The shader for the points
     */
    osg::ref_ptr< WGEShader > m_shader;

    /**
     * The size of a point on screen.
     */
    WPropDouble m_size;

    /**
     * Slower but correct depth calculation.
     */
    WPropBool m_useCorrectDepth;

    /**
     * The vertices that are drawn.
     */
    osg::ref_ptr< osg::Vec3Array > m_vertices;

    /**
     * The color of the vertices that are drawn.
     */
    osg::ref_ptr< osg::Vec4Array > m_colors;

    /**
     * The postprocessing node of the shader.
     */
    osg::ref_ptr< WGEPostprocessingNode > m_postNode;

    /**
     * The index of the selected vertex.
     */
    osg::MixinVector<osg::Vec3f>::size_type m_selectedIndex;

    /**
     * The color of the vertex before it has been selected.
     */
    osg::Vec4 m_selectedOldColor;

    /**
     * Whether a selection has been done or not.
     */
    bool m_hasSelected = false;

    /**
     * Stores the amount of new created fibers.
     */
    int m_fiberCount = 0;

    /**
     * Represents the index of the current active fiber.
     */
    int m_selectedFiber = 0;

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

    /**
     * A pointer to the list of fibers.
     */
    PCFiberListSPtr m_fibers;
};

#endif  // WMPOINTCONNECTOR_H
