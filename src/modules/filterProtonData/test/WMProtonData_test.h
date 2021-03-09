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

#ifndef WMPROTONDATA_TEST_H
#define WMPROTONDATA_TEST_H

#include <string>
#include <cxxtest/TestSuite.h>

#include "modules/data/io/WReaderCSV.h"
#include "../WMProtonData.h"

/**
 * Test class of WMProtonData class
 */

class WMProtonDataTest : public CxxTest::TestSuite
{
public:
    /**
     * if an empty CSV file is loaded, a "CSV file is empty!" exception should be thrown
     */
    void testConstructor()
    {
        boost::shared_ptr< WDataSetCSV > csvDataSet = WReaderCSV( W_FIXTURE_PATH + "CSVs/valid.csv" ).read();
    }
};

#endif  // WMPROTONDATA_TEST_H
