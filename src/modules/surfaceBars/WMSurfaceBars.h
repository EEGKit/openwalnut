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

#ifndef WMSURFACEBARS_H
#define WMSURFACEBARS_H

#include <string>
#include <utility>

#include <osg/Node>
#include <osg/Uniform>
#include <osgDB/WriteFile>

#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../kernel/WModuleOutputData.h"

/**
 * This module is a raytracing based isosurface using special methods for animating particle flow on its surface.
 * \ingroup modules
 */
class WMSurfaceBars: public WModule
{
public:

    /**
     * Default constructor.
     */
    WMSurfaceBars();

    /**
     * Destructor.
     */
    virtual ~WMSurfaceBars();

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
     */
    virtual const char** getXPMIcon() const;

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
     * Callback for m_active. Overwrite this in your modules to handle m_active changes separately.
     */
    virtual void activate();

private:

    /**
     * An input connector used to get datasets from other modules. The connection management between connectors must not be handled by the module.
     */
    boost::shared_ptr< WModuleInputData< WDataSetSingle > > m_input;

    /**
     * The input for the volumized fiber directions.
     */
    boost::shared_ptr< WModuleInputData< WDataSetSingle > > m_directionInput;

    /**
     * The input for the volumized fiber traces. Mostly, this is some kind of integrated length data.
     */
    boost::shared_ptr< WModuleInputData< WDataSetSingle > > m_tracesInput;

    /**
     * This is a pointer to the dataset the module is currently working on.
     */
    boost::shared_ptr< WDataSetSingle > m_dataSet;

    /**
     * The dataset containing the fiber traces in each voxel in m_dataSet.
     */
    boost::shared_ptr< WDataSetSingle > m_tracesDataSet;

    /////////////////////////////////////////////////////////////////////
    // The Properties
    /////////////////////////////////////////////////////////////////////

    /**
     * The Isovalue used in the case m_isoSurface is true.
     */
    WPropInt m_isoValue;

    /**
     * The number of steps to walk along the ray.
     */
    WPropInt m_stepCount;

    /**
     * The alpha transparency used for the rendering
     */
    WPropInt m_alpha;

    /**
     * The color used when in isosurface mode for blending.
     */
    WPropColor m_isoColor;

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /////////////////////////////////////////////////////////////////////
    // OSG Stuff
    /////////////////////////////////////////////////////////////////////

    /**
     * Renders the surface to an FBO.
     *
     * \param bbox the bounding box that should be used
     *
     * \return the node which contains the cube with the surface rendering
     */
    osg::ref_ptr< osg::Node > renderSurface( std::pair< wmath::WPosition, wmath::WPosition > bbox );

    /**
     * The root node used for this modules graphics. For OSG nodes, always use osg::ref_ptr to ensure proper resource management.
     */
    osg::ref_ptr< WGEGroupNode > m_rootNode;

    /**
     * the DVR shader.
     */
    osg::ref_ptr< WShader > m_shader;

    /**
     * Node callback to change the color of the shapes inside the root node. For more details on this class, refer to the documentation in
     * moduleMain().
     */
    class SafeUpdateCallback : public osg::NodeCallback
    {
    public: // NOLINT

        /**
         * Constructor.
         *
         * \param module just set the creating module as pointer for later reference.
         */
        explicit SafeUpdateCallback( WMSurfaceBars* module ): m_module( module ), m_initialUpdate( true )
        {
        };

        /**
         * operator () - called during the update traversal.
         *
         * \param node the osg node
         * \param nv the node visitor
         */
        virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );

        /**
         * Pointer used to access members of the module to modify the node.
         */
        WMSurfaceBars* m_module;

        /**
         * Denotes whether the update callback is called the first time.
         */
        bool m_initialUpdate;
    };

    /**
     * Class handling uniform update during render traversal
     */
    class SafeUniformCallback: public osg::Uniform::Callback
    {
    public:

        /**
         * Constructor.
         *
         * \param module just set the creating module as pointer for later reference.
         */
        explicit SafeUniformCallback( WMSurfaceBars* module ): m_module( module )
        {
        };

        /**
         * The callback. Called every render traversal for the uniform.
         *
         * \param uniform the uniform for which this callback is.
         * \param nv the visitor.
         */
        virtual void operator() ( osg::Uniform* uniform, osg::NodeVisitor* nv );

        /**
         * Pointer used to access members of the module to modify the node.
         */
        WMSurfaceBars* m_module;
    };
};

#endif  // WMSURFACEBARS_H

