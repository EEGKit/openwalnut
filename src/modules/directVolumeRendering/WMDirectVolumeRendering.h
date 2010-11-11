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

#ifndef WMDIRECTVOLUMERENDERING_H
#define WMDIRECTVOLUMERENDERING_H

#include <string>

#include <osg/Node>
#include <osg/Uniform>

#include "../../graphicsEngine/WShader.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../kernel/WModuleOutputData.h"

/**
 * This module is a basic volume renderer.
 *
 * \ingroup modules
 */
class WMDirectVolumeRendering: public WModule
{
public:

    /**
     * Default constructor.
     */
    WMDirectVolumeRendering();

    /**
     * Destructor.
     */
    virtual ~WMDirectVolumeRendering();

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
     * Initialize requirements for this module.
     */
    virtual void requirements();

private:

    /**
     * An input connector used to get datasets from other modules. The connection management between connectors must not be handled by the module.
     */
    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_input;

    /**
     * The number of steps to walk along the ray.
     */
    WPropInt m_stepCount;

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * the DVR shader.
     */
    osg::ref_ptr< WShader > m_shader;

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
        explicit SafeUniformCallback( WMDirectVolumeRendering* module ): m_module( module )
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
        WMDirectVolumeRendering* m_module;
    };
};

#endif  // WMDIRECTVOLUMERENDERING_H

