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

#ifndef WMTRANSFERFUNCTIONCOLORBAR_H
#define WMTRANSFERFUNCTIONCOLORBAR_H

#include <memory>
#include <string>

#include <osg/Projection>

#include "core/common/WPropertyVariable.h"
#include "core/dataHandler/WDataSetSingle.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"

/**
 * This module simply registers the given dataset to the texture handling mechanism. This allows all outputs to be shown as a texture.
 *
 * \ingroup modules
 */
class WMTransferFunctionColorBar: public WModule
{
public:
    /**
     * Standard constructor.
     */
    WMTransferFunctionColorBar();

    /**
     * Destructor.
     */
    ~WMTransferFunctionColorBar();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description of module.
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
     * A condition used to notify about changes in several properties.
     */
    std::shared_ptr< WCondition > m_propCondition;

    /**
     * Input connector required by this module.
     */
    std::shared_ptr< WModuleInputData< WDataSetSingle > > m_input;

    /**
     * The min of the current dataset. We store them here to avoid many calls to getTexture()->minimum()->get() and scale()->get().
     */
    double m_valueMin;

    /**
     * The scaling factor of the current dataset. We store them here to avoid many calls to getTexture()->minimum()->get() and scale()->get().
     */
    double m_valueScale;

    /**
     * The number of colorbar labels
     */
    WPropInt m_colorBarLabels;

    /**
     * Show colorbar name?
     */
    WPropBool m_colorBarName;

    /**
     * Show the border?
     */
    WPropBool m_colorBarBorder;

    /**
     * Maximum scaling factor of the current dataset.
     */
    WPropDouble m_maxScaleValue;

    /**
     * Minimum scaling factor of the current dataset.
     */
    WPropDouble m_minScaleValue;

    /**
     * Description of the color bar.
     */
    WPropString m_colorBarDescription;

    /**
     * The projection node for the colorbar and labels
     */
    osg::ref_ptr< osg::Projection > m_barProjection;

    /**
     * The colorbar.
     */
    osg::ref_ptr< osg::Geode > m_colorBar;

    /**
     * The colorbar scale labels
     */
    osg::ref_ptr< osg::Geode > m_scaleLabels;

    /**
     * Whether a custom min max was set.
     */
    bool m_customScale;

    /**
     * Callback which adds/removes scale labels to the colorbar.
     *
     * \param scaleLabels the geode containing the labels.
     */
    void updateColorbarScale( osg::Node* scaleLabels );
};

#endif  // WMTRANSFERFUNCTIONCOLORBAR_H
