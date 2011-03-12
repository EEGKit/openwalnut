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

#ifndef WMCOLORMAPPER_H
#define WMCOLORMAPPER_H

#include <string>

#include <osg/Projection>

#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../common/WPropertyVariable.h"

#include "../../dataHandler/WDataSetSingle.h"

/**
 * This module simply registers the given dataset to the texture handling mechanism. This allows all outputs to be shown as a texture.
 *
 * \ingroup modules
 */
class WMColormapper: public WModule
{
public:

    /**
     * Standard constructor.
     */
    WMColormapper();

    /**
     * Destructor.
     */
    ~WMColormapper();

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
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    boost::shared_ptr< WModuleInputData< WDataSetSingle > > m_input;  //!< Input connector required by this module.

    /**
     * This is a pointer to the dataset the module is currently working on.
     */
    boost::shared_ptr< WDataSetSingle > m_lastDataSet;

    /**
     * If true, the new texture gets placed at the position of the old one in the texture sorter
     */
    WPropBool m_replace;

    /**
     * If true, a colorbar is shown.
     */
    WPropBool m_showColorbar;

    /**
     * The projection node for the colorbar and labels
     */
    osg::ref_ptr< osg::Projection > m_barProjection;

    /**
     * The colorbar.
     */
    osg::ref_ptr< osg::Geode > m_colorBar;

    /**
     * Updates the label for the dataset name
     *
     * \param label the label to update
     */
    void updateColorbarName( osg::Drawable* label );
};

#endif  // WMCOLORMAPPER_H

