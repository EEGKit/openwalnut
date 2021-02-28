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

#ifndef WMFilterProtonData_H
#define WMFilterProtonData_H

#include <algorithm>

#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>

#include "core/dataHandler/WDataSetCSV.h"
#include "core/dataHandler/WDataSetFibers.h"
#include "core/kernel/WModuleOutputData.h"
#include "core/dataHandler/WDataSetPoints.h"
#include "core/dataHandler/WDataSetPointsAndSizes.h"

#include "core/common/WItemSelectionItem.h"
#include "core/common/WItemSelector.h"

#include "core/kernel/WModule.h"

#include "WMProtonData.h"

#include "WMPropertyStatus.h"
#include "WMCsvConverter.h"

/**
 * This module simply registers a given csv dataset to the csv handling mechanism.
 * This allows all outputs to be shown as a fiber-dataset or a point-dataset.
 *
 * \ingroup modules
 */
class WMFilterProtonData : public WModule
{
public:
    /**
     * represents a boost::shared_ptr to a vector containing a vector of floats.
     */
    typedef boost::shared_ptr< std::vector< float > > SPFloatVector;

    /**
     * represents a boost::shared_ptr to a vector containing a vector of size_t.
     */
    typedef boost::shared_ptr< std::vector< size_t > > SPSizeVector;

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
     * Retruns the description of this module.
     * \return description of the module.
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
     * Pointer that points to the Proton data from the CSV file  
     */
    WMProtonData::SPtr m_protonData;

    /**
     * Input connector (required for this module).
     */
    boost::shared_ptr< WModuleInputData< WDataSetCSV > > m_input;

    /**
     * WDataSetPoints output connector (required for this module).
     */
    boost::shared_ptr< WModuleOutputData< WDataSet > > m_output_points;

    /**
     * WDataSetFibers output connector (required for this module).
     */
    boost::shared_ptr< WModuleOutputData< WDataSet  > > m_output_fibers;

    /**
     * Stores information of the input-csv-data
     */
    boost::shared_ptr< WDataSetCSV > m_dataset;

    /**
     * Contains all property-groups and the subproperties 
     */
    boost::shared_ptr < WMPropertyStatus > m_propertyStatus;

    /**
     * Contains the algorithm that converts the raw CSV file into compatible WDataSets (Points, Fibers, PointsAndSizes, PointConnector etc.)
     */
    boost::shared_ptr < WMCsvConverter > m_converter;

    /**
     * Create outputs, so it can be displayed by the fiber display and the point renderer.
     *
     * \param header WDataSetCSV::Content object containing column names.
     * \param data WDataSetCSV::Content object containing data.
     */
    void setOutputFromCSV( );

    boost::shared_ptr< WModuleOutputData < WDataSet > > createOutputData(std::string headerName, std::string description);
    

};

#endif  // WMFilterProtonData_H
