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

#ifndef WMWRITETRACTS_H
#define WMWRITETRACTS_H

#include <string>

#include <osg/Geode>

#include "../../dataHandler/datastructures/WFiberCluster.h"
#include "../../dataHandler/WDataSetFibers.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../kernel/WModuleOutputData.h"

/**
 * This module writes the tracts of either a fiber cluster or directly out of a
 * WDataSetFibers at its connector to a file.
 *
 * \ingroup modules
 */
class WMWriteTracts: public WModule
{
public:

    /**
     * Constructs an instance to write tracts to a file.
     */
    WMWriteTracts();

    /**
     * Destructs this instance.
     */
    virtual ~WMWriteTracts();

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
     * Input connector for writing the tracts out of a WFiberCluster to a file.
     */
    boost::shared_ptr< WModuleInputData< const WFiberCluster > > m_clusterIC;

    /**
     * Input connector for writing directly tracts to a file.
     */
    boost::shared_ptr< WModuleInputData< const WDataSetFibers > > m_tractIC;

    WPropFilename m_savePath; //!< Path where tracts should be stored
};

#endif  // WMWRITETRACTS_H
