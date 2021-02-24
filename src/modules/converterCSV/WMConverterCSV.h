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

#ifndef WMCONVERTERCSV_H
#define WMCONVERTERCSV_H

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
#include "core/common/WItemSelectionItemTyped.h"


#include "core/kernel/WModule.h"

/**
 * This module simply registers a given csv dataset to the csv handling mechanism.
 * This allows all outputs to be shown as a fiber-dataset or a point-dataset.
 *
 * \ingroup modules
 */
class WMConverterCSV : public WModule
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
    WMConverterCSV();

    /**
     * Standard destructor.
     */
    virtual ~WMConverterCSV();

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
     * Stores column index of data.
     */
    WDataSetCSV::Content m_csvHeader;

    /**
     * Stores data from obtained input file.
     */
    WDataSetCSV::Content m_csvData;

    /**
     * Stores information for the fiber display
     */
    boost::shared_ptr< WDataSetFibers > m_fibers;

    /**
     * Stores information for the point renderer
     */
    boost::shared_ptr< WDataSetPoints > m_points;

    /**
     * Set lower border of range of eventID selection.
     */
    WPropInt m_minCap;

    /**
     * Set upper border of range of eventID selection.
     */
    WPropInt m_maxCap;
    
    /*
     * Stores information for the index of x-header
     */
    int xPosIndex = -1;

    /**
     * Stores information for the index of y-header
     */
    int yPosIndex = -1;

    /**
     * Stores information for the index of edeps-header
     */
    int edepIndex = -1;

    /**
     * Stores information for the index of z-header
     */
    int zPosIndex = -1;

    /**
     * Stores information for the index of eventID-header
     */
    int eventIDIndex = -1;

    /**
     * Stores information for the index of trackID-header
     */
    int trackIDIndex = -1;

    /**
     * Stores information for the index of parent-header
     */
    int parentIDIndex = -1;

    /**
     * Get column number by name from header
     *
     * \param columnNameToMatch Search for the specified column name.
     * \param headerToSearchIn Search in specified header.
     * \return Column number, where columnToMatch is in headerToSearchIn
     */
    int getColumnNumberByName( std::string columnNameToMatch, std::vector<std::string> headerToSearchIn );

    /**
     * Create output, so it can be displayed by the fiber display.
     *
     * \param header WDataSetCSV::Content object containing column names.
     * \param data WDataSetCSV::Content object containing data.
     */
    void setFibersOutOfCSVData( WDataSetCSV::Content header, WDataSetCSV::Content data );

    /**
     * Create output, so it can be displayed by the point renderer.
     *
     * \param header WDataSetCSV::Content object containing column names
     * \param data WDataSetCSV::Content object containing data
     */
    void setPointsOutOfCSVData( WDataSetCSV::Content header, WDataSetCSV::Content data );

    /**
     * Update range of selected eventID rows.
     * \param minBorder Set the lower border of selection.
     * \param maxBorder Set the upper border of selection.
     */
    void updateRangeOfEventIDSelection( int minBorder, int maxBorder );

    /**
     * Update your mesh when changing properties.
     */
    void updateMesh( WPropertyBase::SPtr property );

    /**
     * Determines smalles und biggest eventID.
     */
    void determineMinMaxEventID();
    
    /**
     * Normalize energy deposition values to use as RGB values
     * \param edeps vector containing energy deposition values
     * \param colorArray vector containing colors per vertex
     * \param maxEdep maximum present energy deposition value in edeps vector
     */
    void normalizeEdeps( std::vector< float > edeps, SPFloatVector colorArray, float maxEdep );
    
    /**
     * Decides whether to hide or show primaries.
     */
    WPropBool m_showPrimaries;

    /**
     * Decides whether to hide or show secondaries.
     */
    WPropBool m_showSecondaries;

    /**
     * Reload data when properties for selection of columnnames changed
     *
     * \param property contains reference to the property which called updateProperty()
     */
    void updateProperty( WPropertyBase::SPtr property );

    /**
     * Reload data when properties for selection of primaries and secondaries changed
     *
     * \param property contains reference to the property which called updateProperty()
     */
    void updateCheckboxProperty( WPropertyBase::SPtr property );

    /**
     * A list of items that can be selected using m_aSingleSelectionUsingTypes property.
     */
    boost::shared_ptr< WItemSelection > m_possibleSelectionsUsingTypes;

    /**
     * Stores information of the input-csv-data
     */
    boost::shared_ptr< WDataSetCSV > m_dataset;

    /**
     * A property allowing the user to select ONE item. This additionally demonstrates how to use your own types/classes in selections.
     */
    WPropSelection m_singleSelectionForPosX;

    /**
     * A property allowing the user to select ONE item. This additionally demonstrates how to use your own types/classes in selections.
     */
    WPropSelection m_singleSelectionForPosY;

    /**
     * A property allowing the user to select ONE item. This additionally demonstrates how to use your own types/classes in selections.
     */
    WPropSelection m_singleSelectionForPosZ;

    /**
     * A property allowing the user to select ONE item. This additionally demonstrates how to use your own types/classes in selections.
     */
    WPropSelection m_singleSelectionForPosEdep;

    /**
     * A property allowing the user to select ONE item. This additionally demonstrates how to use your own types/classes in selections.
     */
    WPropSelection m_singleSelectionForEventID;

    /**
     * A property allowing the user to select ONE item. This additionally demonstrates how to use your own types/classes in selections.
     */
    WPropSelection m_singleSelectionForTrackID;

    /**
     * A property allowing the user to select ONE item. This additionally demonstrates how to use your own types/classes in selections.
     */
    WPropSelection m_singleSelectionForParentID;

     /**
     * represents the item type for item-selection
     */
    typedef WItemSelectionItemTyped< std::string > ItemType;
};

#endif  // WMCONVERTERCSV_H
