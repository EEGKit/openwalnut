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

#include "../WReaderCSV.h"


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
                                 strcmp( e.what(), "CSV file is empty!" ),
                                 0 );
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
                                 strcmp( e.what(), "CSV File does not contain data!" ),
                                 0 );
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
                                 strcmp( e.what(), "Data row count does not equal header count!" ),
                                 0 );
    }

    /**
     * if a valid CSV file is loaded, no exception should be thrown
     */
    void testValidCSV()
    {
        std::string fileName = W_FIXTURE_PATH + "CSVs/valid.csv";
        std::cout << std::endl << "Test loading of " << fileName << "." << std::endl;


        WDataSetCSV::ContentSPtr testHeader = WDataSetCSV::ContentSPtr( new WDataSetCSV::Content() );
        WDataSetCSV::ContentSPtr testDataFirstRow = WDataSetCSV::ContentSPtr( new WDataSetCSV::Content() );
        WDataSetCSV::ContentSPtr testDataLastRow = WDataSetCSV::ContentSPtr( new WDataSetCSV::Content() );
        testHeader->push_back(
            {
                "PDGEncoding", "trackID", "parentID", "trackLocalTime", "time", "edep", "stepLength", "trackLength",
                "posX", "posY", "posZ", "localPosX", "localPosY", "localPosZ", "momDirX", "momDirY", "momDirZ", "baseID",
                "level1ID", "level2ID", "level3ID", "level4ID", "layerID", "photonID", "nPhantomCompton", "nCrystalCompton",
                "nPhantomRayleigh", "nCrystalRayleigh", "primaryID", "sourcePosX", "sourcePosY", "sourcePosZ", "sourceID", "eventID",
                "runID", "axialPos", "rotationAngle", "volumeID", "processName", "comptVolName", "RayleighVolName"
            }
        );
        testDataFirstRow->push_back(
            {
                "2212", "1", "0", "4.20922e-09", "4.20922e-09", "0.0199097", "0.0250083", "725.314", "-1.86984", "4.55793", "225.244", "-1.86984",
                "4.55793", "0.0125", "-0.00614681", "0.0255574", "0.999654", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "1", "-0.154322",
                "0.156973", "-500", "0", "1", "0", "277.4", "0", "[0;1;0;3;-1;-1;-1;-1;-1;-1]", "Transportation", "NULL", "NULL"
            }
        );
        testDataLastRow->push_back(
            {
                "2212", "1", "0", "5.92057e-09", "5.92057e-09", "0.3312", "0.025598", "955.947", "14.127", "28.6775", "454.394", "14.127", "28.6775",
                "0.0125", "0.216802", "0.0338896", "0.975627", "1", "22", "0", "0", "0", "0", "0", "0", "0", "0", "0", "1", "-0.480459", "0.0634152",
                "-500", "0", "2", "0", "461.8", "0", "[0;2;22;3;-1;-1;-1;-1;-1;-1]", "Transportation", "NULL", "NULL"
            }
        );

        WReaderCSV tmpCsvReader( fileName );
        TS_ASSERT_THROWS_NOTHING( tmpCsvReader.read() );
        // compare headers
        TS_ASSERT_EQUALS( tmpCsvReader.read()->getHeader()->front(), testHeader->front() );
        // compare first data rows
        TS_ASSERT_EQUALS( tmpCsvReader.read()->getData()->front(), testDataFirstRow->front() );
        // compare last data rows
        TS_ASSERT_EQUALS( tmpCsvReader.read()->getData()->back(), testDataLastRow->front() );
    }

    /**
     * check \\r line endings
     */
    void testCRLineEnding()
    {
        std::string fileName = W_FIXTURE_PATH + "CSVs/cr.csv";
        std::cout << std::endl << "Test loading of " << fileName << "." << std::endl;


        WDataSetCSV::ContentSPtr testHeader = WDataSetCSV::ContentSPtr( new WDataSetCSV::Content() );
        WDataSetCSV::ContentSPtr testDataFirstRow = WDataSetCSV::ContentSPtr( new WDataSetCSV::Content() );
        testHeader->push_back(
            {
                "header1", "header2", "header3"
            }
        );
        testDataFirstRow->push_back(
            {
                "1", "2", "3"
            }
        );

        WReaderCSV tmpCsvReader( fileName );
        TS_ASSERT_THROWS_NOTHING( tmpCsvReader.read() );
        // compare headers
        TS_ASSERT_EQUALS( tmpCsvReader.read()->getHeader()->front(), testHeader->front() );
        // compare first data rows
        TS_ASSERT_EQUALS( tmpCsvReader.read()->getData()->front(), testDataFirstRow->front() );
    }

    /**
     * check \\n line endings
     */
    void testLFLineEnding()
    {
        std::string fileName = W_FIXTURE_PATH + "CSVs/lf.csv";
        std::cout << std::endl << "Test loading of " << fileName << "." << std::endl;


        WDataSetCSV::ContentSPtr testHeader = WDataSetCSV::ContentSPtr( new WDataSetCSV::Content() );
        WDataSetCSV::ContentSPtr testDataFirstRow = WDataSetCSV::ContentSPtr( new WDataSetCSV::Content() );
        testHeader->push_back(
            {
                "header1", "header2", "header3"
            }
        );
        testDataFirstRow->push_back(
            {
                "1", "2", "3"
            }
        );

        WReaderCSV tmpCsvReader( fileName );
        TS_ASSERT_THROWS_NOTHING( tmpCsvReader.read() );
        // compare headers
        TS_ASSERT_EQUALS( tmpCsvReader.read()->getHeader()->front(), testHeader->front() );
        // compare first data rows
        TS_ASSERT_EQUALS( tmpCsvReader.read()->getData()->front(), testDataFirstRow->front() );
    }

    /**
     * check \\r\\n line endings
     */
    void testCRLFLineEnding()
    {
        std::string fileName = W_FIXTURE_PATH + "CSVs/crlf.csv";
        std::cout << std::endl << "Test loading of " << fileName << "." << std::endl;


        WDataSetCSV::ContentSPtr testHeader = WDataSetCSV::ContentSPtr( new WDataSetCSV::Content() );
        WDataSetCSV::ContentSPtr testDataFirstRow = WDataSetCSV::ContentSPtr( new WDataSetCSV::Content() );
        testHeader->push_back(
            {
                "header1", "header2", "header3"
            }
        );
        testDataFirstRow->push_back(
            {
                "1", "2", "3"
            }
        );

        WReaderCSV tmpCsvReader( fileName );
        TS_ASSERT_THROWS_NOTHING( tmpCsvReader.read() );
        // compare headers
        TS_ASSERT_EQUALS( tmpCsvReader.read()->getHeader()->front(), testHeader->front() );
        // compare first data rows
        TS_ASSERT_EQUALS( tmpCsvReader.read()->getData()->front(), testDataFirstRow->front() );
    }

    /**
     * Empty columns should not prevent the reader from reading all columns
     */
    void testEmptyColumns()
    {
        std::string fileName = W_FIXTURE_PATH + "CSVs/emptyColumns.csv";
        std::cout << std::endl << "Test loading of " << fileName << "." << std::endl;


        WDataSetCSV::ContentSPtr testHeader = WDataSetCSV::ContentSPtr( new WDataSetCSV::Content() );
        WDataSetCSV::ContentSPtr testDataFirstRow = WDataSetCSV::ContentSPtr( new WDataSetCSV::Content() );
        testHeader->push_back(
            {
                "header1", "header2", "header3"
            }
        );
        testDataFirstRow->push_back(
            {
                "", "", ""
            }
        );

        WReaderCSV tmpCsvReader( fileName );
        TS_ASSERT_THROWS_NOTHING( tmpCsvReader.read() );
        // compare headers
        TS_ASSERT_EQUALS( tmpCsvReader.read()->getHeader()->front(), testHeader->front() );
        // compare first data rows
        TS_ASSERT_EQUALS( tmpCsvReader.read()->getData()->front(), testDataFirstRow->front() );
    }
};

#endif  // WREADERCSV_TEST_H
