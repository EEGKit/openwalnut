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
#include "core/dataHandler/WGrid.h"
#include "core/dataHandler/WGridRegular3D.h"
#include "core/common/WTransferFunction.h"
#include "core/dataHandler/WDataSetFibers.h"
#include "core/dataHandler/WDataSetPoints.h"
#include "core/dataHandler/WDataSetPointsAndSizes.h"
#include "core/dataHandler/WValueSet.h"
#include "core/dataHandler/WValueSetBase.h"

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

    /**
     * Getter
     * \return shared_ptr of generated transfer function
     */
    boost::shared_ptr< WDataSetSingle > getTransferFunction();

private:
    /**
     * Stores information form csv content. Content object containing data
     */
    WMProtonData::SPtr m_protonData;

    /**
     * Stores information for the fiber display
     */
    boost::shared_ptr< WDataSetFibers > m_fibers;

    /**
     * Stores information for the point renderer
     */
    boost::shared_ptr< WDataSetPoints > m_points;

    /**
     * Stores the currently mapped transfer function
     */
    boost::shared_ptr< WDataSetSingle > m_transferFunction;

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
     * Computes gradient vector from transfer function specified in visualization properties.
     * \return shared_ptr of mapped gradim_plainColorent from transfer function in RGBA format
     */
    boost::shared_ptr< std::vector<unsigned char> > sampleTransferFunction();

    /**
     * Normalize energy deposition values to use as RGB values
     * \param edeps vector containing energy deposition values
     * \param colorArray vector containing colors per vertex
     * \param maxEdep maximum energy deposition
     */
    void normalizeEdeps( SPFloatVector edeps, SPFloatVector colorArray,  float maxEdep );


    /**
     * checks whether the requirements are fulfilled.
     * \param dataRow the row to check.
     * \return true The row can be displayed.
     * \return false The row can not be displayed.
     */
    bool checkConditionToPass( WDataSetCSV::Content::iterator dataRow );

    /**
     * Create vertex for point/fiber renderer
     *
     * \param dataRow each row of the csv file (content of row)
     */
    void addVertex( WDataSetCSV::Content::iterator dataRow );

    /**
     * Create color for point/Fiber renderer
     *
     * \param dataRow each row of the csv file (content of row)
     */
    void addColor( WColor plainColor );

    /**
     * Create edep and sizes for point/fiber renderer
     *
     * \param dataRow each row of the csv file (content of row)
     * \param maxEdep a pointer to the current maximum of the edep
     */
    void addEdepAndSize( WDataSetCSV::Content::iterator dataRow, float* maxEdep );

    /**
     * Create eventID for Fiber renderer
     *
     * \param dataRow each row of the csv file (content of row)
     */
    void addEventID( WDataSetCSV::Content::iterator dataRow );

    /**
     * calculate the property of WDataSetFiber (index, length, verticesReverse)
     */
    void calculateFibers();

    /**
     * Create the fibers for fiber renderer
     */
    void createOutputFibers();

    /**
     * Create the points for points renderer
     */
    void createOutputPoints();

    /**
     * Create outputs, so it can be displayed by the fiber display and the point renderer.
     */
    void setOutputFromCSV( );

    /**
     * Creates output for transfer function
     * \param data shared_ptr of mapped gradient from transfer function in RGBA format
     */
    void setTransferFunction( boost::shared_ptr< std::vector<unsigned char> > data );

    /**
     * the cast value from string to float
     * \return returns the cast value from string to float
     */
    bool checkIfOutputIsNull();

    /**
     * Computes the cluster size
     * \param edep energy deposition
     * \return float cluster size
     */
    float getClusterSize( float edep );

    /**
     * checks whether the string is a number  
     * \return return the casted value
     */
    float stringToFloat( std::string str );
};

#endif  // WMCSVCONVERTER_H
