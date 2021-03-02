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

#ifndef WMCSVCONVERTER_H
#define WMCSVCONVERTER_H

#include <string>
#include <vector>
#include <iostream>

#include <boost/lexical_cast.hpp>
#include "core/dataHandler/WDataSetFibers.h"
#include "core/dataHandler/WDataSetPoints.h"
#include "core/dataHandler/WDataSetPointsAndSizes.h"

#include "WMConverterIndexes.h"
#include "WMConverterVectors.h"
#include "WMPropertyStatus.h"
#include "WMProtonData.h"

/**
 * Converts the csv data to points and fibers.
 */
class WMCsvConverter
{
public:
    /**
    * Initializes the vectors and indexes.
    * Calls setOutputFromCSV.
    * \param protonData The protondata to use.
    * \param propertyStatus The status of the properties.
    */
    WMCsvConverter( WMProtonData::SPtr protonData, boost::shared_ptr< WMPropertyStatus > propertyStatus );

    /**
     * represents a boost::shared_ptr to a vector containing a vector of floats.
     */
    typedef boost::shared_ptr< std::vector< float > > SPFloatVector;

    /**
     * represents a boost::shared_ptr to a vector containing a vector of size_t.
     */
    typedef boost::shared_ptr< std::vector< size_t > > SPSizeVector;

    /**
     * Getter
     *
     * \return shared_ptr of generated fibers from the CSV file 
     */
    boost::shared_ptr< WDataSetFibers > getFibers();

    /**
     * Getter
     *
     * \return shared_ptr of generated Points from the CSV file 
     */
    boost::shared_ptr< WDataSetPoints > getPoints();

private:
    /**
     * Stores information for the fiber display
     */
    boost::shared_ptr< WDataSetFibers > m_fibers;

    /**
     * Stores information for the point renderer
     */
    boost::shared_ptr< WDataSetPoints > m_points;

    /**
     * The fallback color to use.
     */
    WColor m_plainColor;

    /**
     * Stores points for point and fiber renderer
     */
    WMConverterVectors::SPtr m_vectors;

    /**
     * Stores indexes for fiber renderer
     */
    WMConverterIndexes::SPtr m_indexes;

    /**
     * includes all property groups 
     */
    WMPropertyStatus::SPtr m_propertyStatus;

    /**
     * Normalize energy deposition values to use as RGB values
     * \param edeps vector containing energy deposition values
     * \param colorArray vector containing colors per vertex
     * \param maxEdep maximum present energy deposition value in edeps vector
     */
    void normalizeEdeps( SPFloatVector edeps, SPFloatVector colorArray, float maxEdep );


    /**
     * checks whether the requirements are fulfilled
     * \param dataRow the row to check.
     */
    bool canShow( WDataSetCSV::Content::iterator dataRow );

    /**
     * Create vertex and color for point renderer (vertex for fiber renderer)
     *
     * \param dataRow each row of the csv file (content of row)
     */
    void addVertexAndColor( WDataSetCSV::Content::iterator dataRow );

    /**
     * Create edep and sizes for point/fiber renderer
     *
     * \param dataRow each row of the csv file (content of row)
     * \param maxEdep a pointer to the current maximum of the edep
     */
    void addEdepAndSize( WDataSetCSV::Content::iterator dataRow, float* maxEdep );

    /**
     * calculate the property of WDataSetFiber (index, length, verticesReverse)
     */
    void calculateFibers();

    /**
     * Create the fibers and points for fiber/points renderer
     */
    void createPointsAndFibers();

    /**
     * Create outputs, so it can be displayed by the fiber display and the point renderer.
     *
     * \param protonData WDataSetCSV::Content object containing data.
     */
    void setOutputFromCSV( WMProtonData::SPtr protonData );

    /**
     * Checks if output variables have to be null, if true, sets them to null
     * \return true, if there are no vertices to output; false otherwise
     */
    bool checkIfOutputIsNull();
};

#endif  // WMCSVCONVERTER_H
