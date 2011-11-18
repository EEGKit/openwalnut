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

#ifndef WMMULTISURFACERAYTRACER_H
#define WMMULTISURFACERAYTRACER_H

#include <string>

#include <osg/Geode>
#include <osg/Node>
#include <osg/Uniform>

#include "../../core/graphicsEngine/shaders/WGEShader.h"

#include "../../core/kernel/WModule.h"
#include "../../core/kernel/WModuleInputData.h"
#include "../../core/kernel/WModuleOutputData.h"

//may need forward declaration later
class WDataSetVector;
class WDataSetScalar;

/**
 * This module draws a scalar dataset as a set of up to four isosurfaces.
 * The transparency of the surface (automatically computed) represents the relative value
 * of the scalar in comparison to the whole dataset.
 * To improve depth perception, the saturation can be reduced with increasing distance from
 * the viewer.
 *
 * \ingroup modules
 */
class WMMultiSurfaceRaytracer: public WModule
{
public:

    /**
     *
     */
    WMMultiSurfaceRaytracer();

    /**
     *
     */
    virtual ~WMMultiSurfaceRaytracer();

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
    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_input;

    boost::shared_ptr< WModuleInputData< WDataSetVector > > m_gradients; //!< The gradient field input

    osg::ref_ptr< WGEShader > m_shader; //!< The shader.

    boost::shared_ptr< WCondition > m_propCondition; //!< A condition used to notify about changes in several properties.

    WPropInt m_surfCount; //!< The number of isosurfaces drawn.

    WPropColor m_isoColor1; //!< The color used for the first isosurface.
    WPropColor m_isoColor2; //!< The color used for the second isosurface.
    WPropColor m_isoColor3; //!< The color used for the third isosurface.
    WPropColor m_isoColor4; //!< The color used for the fourth isosurface.

    WPropDouble m_isoValue1; //!< The value used for the first isosurface.
    WPropDouble m_isoValue2; //!< The value used for the second isosurface.
    WPropDouble m_isoValue3; //!< The value used for the third isosurface.
    WPropDouble m_isoValue4; //!< The value used for the fourth isosurface.

    WMatrixFixed< double, 4, 1 > m_isoVals; //!< All isovalues in one vector.

    WPropInt m_stepCount; //!< The number of steps to walk along the ray.

    WPropDouble m_colormapRatio; //!< The ratio between colormap and normal surface color.

    WPropDouble m_isoEpsilon; //!< The deviation tolerated for the m_isoValue.

    WPropDouble m_saturation; //!< The transfer function for depth dependent saturation.

    WPropBool m_manualAlpha; //!< If true, the manual alpha slider is used rather than automatic, isovalue-dependent alpha.

    WPropBool m_phong; //!< If true, per-pixel-phong shading is applied to the surface.

    WPropBool m_jitter; //!< If true, the ray-tracer uses stochastic jitter to improve image quality.

    WPropBool m_cortexMode; //!< Some special coloring mode emphasizing the cortex.
};

#endif  // WMMULTISURFACERAYTRACER_H
