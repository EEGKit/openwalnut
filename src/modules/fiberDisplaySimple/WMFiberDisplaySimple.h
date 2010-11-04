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

#ifndef WMFIBERDISPLAYSIMPLE_H
#define WMFIBERDISPLAYSIMPLE_H

#include <string>

#include "../../dataHandler/WDataSetScalar.h"
#include "../../graphicsEngine/WGEManagedGroupNode.h"
#include "../../graphicsEngine/WShader.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../kernel/WModuleOutputData.h"

/**
 * This modules takes a dataset and equalizes its histogram.
 *
 * \ingroup modules
 */
class WMFiberDisplaySimple: public WModule
{
public:

    /**
     * Default constructor.
     */
    WMFiberDisplaySimple();

    /**
     * Destructor.
     */
    virtual ~WMFiberDisplaySimple();

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

private:

    /**
     * The fiber dataset which is going to be filtered.
     */
    boost::shared_ptr< WModuleInputData< WDataSetFibers > > m_fiberInput;

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * The root node used for this modules graphics. For OSG nodes, always use osg::ref_ptr to ensure proper resource management.
     */
    osg::ref_ptr< WGEManagedGroupNode > m_rootNode;

    /**
     * The properties of the fiber dataset.
     */
    WProperties::SharedPtr m_fibProps;

    /**
     * The shader used for clipping of fibers using an arbitrary plane.
     */
    osg::ref_ptr< WShader > m_shader;

    /**
     * A property group for all the clipping related props.
     */
    WPropGroup m_clipPlaneGroup;

    /**
     * Property for en-/disable clipping.
     */
    WPropBool m_clipPlaneEnabled;

    /**
     * Point on the plane. Defines the plane.
     */
    WPropPosition m_clipPlanePoint;

    /**
     * Vector of the plane. Defines the plane.
     */
    WPropPosition m_clipPlaneVector;

    /**
     * Distance from plane. Used as threshold.
     */
    WPropDouble m_clipPlaneDistance;

    /**
     * Uniform for plane point.
     */
    osg::ref_ptr< WGEPropertyUniform< WPropPosition > > m_clipPlanePointUniform;

    /**
     * Uniform for plane vector.
     */
    osg::ref_ptr< WGEPropertyUniform< WPropPosition > > m_clipPlaneVectorUniform;

    /**
     * Uniform for plane distance.
     */
    osg::ref_ptr< WGEPropertyUniform< WPropDouble > > m_clipPlaneDistanceUniform;
};

#endif  // WMFIBERDISPLAYSIMPLE_H

