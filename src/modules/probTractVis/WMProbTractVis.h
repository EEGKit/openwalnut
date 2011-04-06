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

#ifndef WMPROBTRACTVIS_H
#define WMPROBTRACTVIS_H

#include <string>

#include <osg/Geode>
#include <osg/Node>
#include <osg/Uniform>

#include "../../graphicsEngine/shaders/WGEShader.h"

#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../kernel/WModuleOutputData.h"

//may need forward declaration later
class WDataSetVector;
class WDataSetScalar;

/** 
 * Someone should add some documentation here.
 * Probably the best person would be the module's
 * creator, i.e. "berres".
 *
 * This is only an empty template for a new module. For
 * an example module containing many interesting concepts
 * and extensive documentation have a look at "src/modules/template"
 *
 * \ingroup modules
 */
class WMProbTractVis: public WModule
{
public:

    /**
     *
     */
    WMProbTractVis();

    /**
     *
     */
    virtual ~WMProbTractVis();

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
    * Scalar dataset representing the probability field either in real numbers in [0,1], or gray values, or the
    * connectivity score (#visits per voxel).
    */
    boost::shared_ptr< WModuleInputForwardData< WDataSetScalar > > m_input;

    /**
     * The gradient field input
     */
    boost::shared_ptr< WModuleInputData< WDataSetVector > > m_gradients;

    /**
     * The Isovalue used in the case m_isoSurface is true.
     */
    WPropDouble m_isoValue;

    /**
     * The color used when in isosurface mode for blending.
     */
    WPropColor m_isoColor;

    /**
     * The number of steps to walk along the ray.
     */
    WPropInt m_stepCount;

    /**
     * The alpha transparency used for the rendering
     */
    WPropDouble m_alpha;

    /**
     * The ratio between colormap and normal surface color.
     */
    WPropDouble m_colormapRatio;

    /**
     * Some special coloring mode emphasizing the cortex.
     */
    WPropBool m_cortexMode;

    /**
     * If true, per-pixel-phong shading is applied to the surface.
     */
    WPropBool m_phongShading;

    /**
     * If true, the ray-tracer uses stochastic jitter to improve image quality.
     */
    WPropBool m_stochasticJitter;

    /**
     * If true, a certain border area can be clipped. Very useful for non-peeled noisy data.
     */
    WPropBool m_borderClip;

    /**
     * The distance used for clipping.
     */
    WPropDouble m_borderClipDistance;

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
    * Shader
    */
    osg::ref_ptr< WGEShader > m_shader;
};

#endif  // WMPROBTRACTVIS_H
