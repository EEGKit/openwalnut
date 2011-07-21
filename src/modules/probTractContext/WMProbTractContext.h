//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

#ifndef WMPROBTRACTCONTEXT_H
#define WMPROBTRACTCONTEXT_H

#include <string>

#include <osg/Geode>

#include <boost/shared_ptr.hpp>

#include "../../core/graphicsEngine/shaders/WGEShader.h"

#include "../../core/kernel/WModule.h"
#include "../../core/kernel/WModuleContainer.h"
#include "../../core/kernel/WModuleInputData.h"
#include "../../core/kernel/WModuleOutputData.h"
#include "../../core/kernel/WModuleInputForwardData.h"
#include "../../core/kernel/WModuleOutputForwardData.h"
#include "../../core/dataHandler/WDataSetScalar.h"

/** 
 * This module provides context for modules such as WMProbTractVis.
 *
 * \ingroup modules
 */
class WMProbTractContext: public WModuleContainer
{
public:

    /**
     *
     */
    WMProbTractContext();

    /**
     *
     */
    virtual ~WMProbTractContext();

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
    boost::shared_ptr< WModule > m_visModule;  //!< The ProbTractVis module used in this container

    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_input; //!< Scalar dataset.

    osg::ref_ptr< WGEShader > m_shader; //!< The shader.

    WPropDouble m_axial; //!< Axial NavSlice position.
    WPropDouble m_coronal; //!< Coronal NavSlice position.
    WPropDouble m_sagittal; //!< Sagittal NavSlice position.

    // gui properties
    boost::shared_ptr< WCondition > m_propCondition;

    WPropDouble m_isoValue; //!< The isovalue used for the context.
    WPropDouble m_alpha; //!< The context transparency
    WPropColor m_isoColor; //!< The color used for the context.
    WPropInt m_stepCount; //!< The number of steps for the raytracer.
    //TODO(aberres): separate alphas for vis and context!!
    WPropBool m_phong; //!< Enables or disables phong shading.
    WPropBool m_jitter; //!< Enables or disables stochastic jitter.
};

#endif  // WMPROBTRACTCONTEXT_H
