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

#ifndef WMFILTERPROTONDATA_H
#define WMFILTERPROTONDATA_H

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <boost/lexical_cast.hpp>

#include "WCsvConverter.h"
#include "WPropertyStatus.h"
#include "WProtonData.h"
#include "core/common/WItemSelectionItem.h"
#include "core/common/WItemSelector.h"
#include "core/dataHandler/WDataSetCSV.h"
#include "core/dataHandler/WDataSetFibers.h"
#include "core/dataHandler/WDataSetPoints.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleContainer.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"


/**
 * This module simply registers a given csv dataset to the csv handling mechanism.
 * This allows all outputs to be shown as a fiber-dataset or a point-dataset.
 *
 * \ingroup modules
 */
class WMFilterProtonData : public WModuleContainer
{
    /**
     * Only test classes may be friend
     */
     friend class WCsvConverterTest;
public:
    /**
     * represents a std::shared_ptr to a vector containing a vector of floats.
     */
    typedef std::shared_ptr< std::vector< float > > SPFloatVector;

    /**
     * represents a std::shared_ptr to a vector containing a vector of size_t.
     */
    typedef std::shared_ptr< std::vector< size_t > > SPSizeVector;

    /**
     * Standard constructor.
     */
    WMFilterProtonData();

    /**
     * Standard destructor.
     */
    virtual ~WMFilterProtonData();

    /**
     * Returns the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Returns the description of this module.
     * \return description of the module.
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
     * Toggles activation for inner module based on activation of this module
     */
    virtual void activate();

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
     * Pointer that points to the Proton data from the CSV file  
     */
    WProtonData::SPtr m_protonData;

    /**
     * Reference to the transfer function color bar module within this module container
     */
    WModule::SPtr m_colorBar;

    /**
     * Input connector (required for this module).
     */
    std::shared_ptr< WModuleInputData< WDataSetCSV > > m_input;

    /**
     * WDataSetPoints output connector (required for this module).
     */
    std::shared_ptr< WModuleOutputData< WDataSetPoints > > m_output_points;

    /**
     * WDataSetFibers output connector (required for this module).
     */
    std::shared_ptr< WModuleOutputData< WDataSetFibers > > m_output_fibers;

    /**
     * WDataSetSingle output connector to output current transfer function
     */
    std::shared_ptr< WModuleOutputData< WDataSetSingle > > m_output_transferFunction;

    /**
     * WDataSetPointsAndEventIDs output connector to output points and eventIDs for PointConn.
     */
    std::shared_ptr< WModuleOutputData< WDataSetPoints > > m_output_points_eventIds;

    /**
     * Stores information of the input-CSV-data
     */
    std::shared_ptr< WDataSetCSV > m_dataset;

    /**
     * Contains all property-groups and the subproperties
     */
    std::shared_ptr < WPropertyStatus > m_propertyStatus;

    /**
     * Contains the algorithm that converts the raw CSV file into compatible WDataSets (Points, Fibers, PointsAndSizes, PointConnector etc.)
     */
    std::shared_ptr < WCsvConverter > m_converter;

    /**
     * Creates the transfer function color bar module within this module container
     */
    void createColorBar();

    /**
     * Contains the loaded properties.
     */
    std::vector< std::pair< std::string, std::string > > m_loadedProperties;

    /**
     * Create outputs, so it can be displayed by the fiber display and the point renderer.
     */
    void setOutputFromCSV( );

    /**
     * Update added group property and subproperty
     */
    void updateProperty();

    /**
     * Notifier for the dummy properties.
     * \param group The WPropertyGroup this property belongs to.
     * \param property The property that was updated.
     */
    void loadNotifier( WPropertyGroup::SPtr group, WPropertyBase::SPtr property );

    /**
     * Sets the current properties to the state of the loaded properties.
     */
    void setToLoadedProperties();

    /**
     * Removes the property groups.
     */
    void clearProperties();
};

#endif  // WMFILTERPROTONDATA_H
