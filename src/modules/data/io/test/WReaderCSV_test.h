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

#ifndef WREADERCSV_TEST_H
#define WREADERCSV_TEST_H

#include <string>

#include <cxxtest/TestSuite.h>

#include "io/WReaderCSV.h"


/**
 * Test class of WReaderCSV class
 */

class WReaderCSVTest : public CxxTest::TestSuite
{
public:
    /**
     * if an empty CSV file is loaded, a "CSV file is empty!" exception should be thrown
     */
    void testEmptyCSV()
    {
        std::string fileName = W_FIXTURE_PATH + "CSVs/empty.csv";
        std::cout << std::endl << "Test loading of " << fileName << "." << std::endl;

        TS_ASSERT_THROWS_EQUALS( WReaderCSV( fileName ).read(),
                                 const WException &e,
                                 e.what(),
                                 "CSV file is empty!" );
    }

    /**
     * if a CSV file only contains a header row, a "CSV File does not contain data!" exception should be thrown
     */
    void testOnlyHeaderCSV()
    {
        std::string fileName = W_FIXTURE_PATH + "CSVs/onlyHeader.csv";
        std::cout << std::endl << "Test loading of " << fileName << "." << std::endl;

        TS_ASSERT_THROWS_EQUALS( WReaderCSV( fileName ).read(),
                                 const WException &e,
                                 e.what(),
                                 "CSV File does not contain data!" );
    }

    /**
     * if a CSV file contains rows, whose column count differs from the header,
     * a "Data row count does not equal header count!" exception should be thrown
     */
    void testInvalidColumnNumber()
    {
        std::string fileName = W_FIXTURE_PATH + "CSVs/invalidColumnCount.csv";
        std::cout << std::endl << "Test loading of " << fileName << "." << std::endl;

        TS_ASSERT_THROWS_EQUALS( WReaderCSV( fileName ).read(),
                                 const WException &e,
                                 e.what(),
                                 "Data row count does not equal header count!" );
    }

    /**
     * if a valid CSV file is loaded, no exception should be thrown
     */
    void testValidCSV()
    {
        std::string fileName = W_FIXTURE_PATH + "CSVs/valid.csv";
        std::cout << std::endl << "Test loading of " << fileName << "." << std::endl;

        TS_ASSERT_THROWS_NOTHING( WReaderCSV( fileName ).read() );
    }
};

#endif  // WREADERCSV_TEST_H
