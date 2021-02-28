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

#include <boost/lexical_cast.hpp>
#include "core/dataHandler/WDataSetFibers.h"
#include "core/dataHandler/WDataSetPoints.h"
#include "core/dataHandler/WDataSetPointsAndSizes.h"

#include "WMColumnPropertyHandler.h"
#include "WMConverterIndexes.h"
#include "WMConverterVectors.h"
#include "WMPropertyStatus.h"
#include "WMProtonData.h"

class WMCsvConverter
{
public:
    WMCsvConverter(WMProtonData::SPtr protonData, boost::shared_ptr< WMPropertyStatus > propertystatus);

    /**
     * represents a boost::shared_ptr to a vector containing a vector of floats.
     */
    typedef boost::shared_ptr< std::vector< float > > SPFloatVector;

    /**
     * represents a boost::shared_ptr to a vector containing a vector of size_t.
     */
    typedef boost::shared_ptr< std::vector< size_t > > SPSizeVector;

    boost::shared_ptr< WDataSetFibers > getFibers();

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

   WColor m_plainColor;

    WMConverterVectors::SPtr m_vectors;

    WMConverterIndexes::SPtr m_indexes;

    WMPropertyStatus::SPtr m_propertyStatus;

    /**
     * Normalize energy deposition values to use as RGB values
     * \param edeps vector containing energy deposition values
     * \param colorArray vector containing colors per vertex
     * \param maxEdep maximum present energy deposition value in edeps vector
     */
    void normalizeEdeps( SPFloatVector edeps, SPFloatVector colorArray, float maxEdep );

    bool canShow( WDataSetCSV::Content::iterator dataRow );

    void addVertexAndColor( WDataSetCSV::Content::iterator dataRow );

    void addEdepAndSize( WDataSetCSV::Content::iterator dataRow, float* maxEdep );

    void calculateFibers();

    void createPointsAndFibers();

    /**
     * Create outputs, so it can be displayed by the fiber display and the point renderer.
     *
     * \param protonData WDataSetCSV::Content object containing data.
     */
    void setOutputFromCSV( WMProtonData::SPtr protonData, boost::shared_ptr< WMPropertyStatus > propertystatus);
    
};

#endif  // WMCSVCONVERTER_H


