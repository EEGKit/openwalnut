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

#ifndef WMLINEDRAWER_H
#define WMLINEDRAWER_H

#include <string>
#include <vector>

#include "core/common/WItemSelectionItem.h"
#include "core/common/WItemSelectionItemTyped.h"
#include "core/common/WItemSelector.h"
#include "core/kernel/WModule.h"
#include "WSelectionManager.h"

/**
 * This module allows to draw in the two-dimensional space.
 * 
 * \ingroup modules
 */
class WMLineDrawer : public WModule
{
public:
    /**
     * A shared_ptr to this class.
     */
    typedef boost::shared_ptr< WMLineDrawer > SPtr;

    /**
     * Type of selectable items.
     */
    typedef WItemSelectionItemTyped< std::string > ItemType;

    /**
     * Constructor. Creates the module skeleton.
     */
    WMLineDrawer();

    /**
     * Desctructor.
     */
    virtual ~WMLineDrawer();

    /**
     * Gives back the name of this module.
     * \return const std::string the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back the description of this module.
     * \return const std::string description of the module.
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
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * The property for the selection type.
     */
    WPropSelection m_selection;
};

#endif  // WMLINEDRAWER_H
