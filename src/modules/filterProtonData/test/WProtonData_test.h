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

#ifndef WPROTONDATA_TEST_H
#define WPROTONDATA_TEST_H

#include <string>
#include <vector>

#include <cxxtest/TestSuite.h>

#include <core/common/test/WException_test.h>

#include "../WProtonData.h"


/**
 * Test class of WMProtonData class
 */
class WProtonDataTest : public CxxTest::TestSuite
{
    /**
     * A sample of column names
     */
    WDataSetCSV::Content sampleColumnNames =
    {
        {
            "PDGEncoding", "trackID", "parentID", "trackLocalTime", "time", "edep", "stepLength", "trackLength",
            "posX", "posY", "posZ", "localPosX", "localPosY", "localPosZ", "momDirX", "momDirY", "momDirZ",
            "baseID", "level1ID", "level2ID", "level3ID", "level4ID", "layerID", "photonID", "nPhantomCompton",
            "nCrystalCompton", "nPhantomRayleigh", "nCrystalRayleigh", "primaryID", "sourcePosX", "sourcePosY",
            "sourcePosZ", "sourceID", "eventID", "runID", "axialPos", "rotationAngle", "volumeID", "processName",
            "comptVolName", "RayleighVolName"
        }
    };

    /**
     * A sample of data rows
     */
    WDataSetCSV::Content sampleDataRows =
    {
        {
            "2212", "1", "0", "4.20922e-09", "4.20922e-09", "0.0199097", "0.0250083", "725.314", "-1.86984", "4.55793",
            "225.244", "-1.86984", "4.55793", "0.0125", "-0.00614681", "0.0255574", "0.999654", "0", "0", "0", "0", "0", "0",
            "0", "0", "0", "0", "0", "1", "-0.154322", "0.156973", "-500", "0", "1", "0", "277.4", "0",
            "[0;1;0;3;-1;-1;-1;-1;-1;-1]", "Transportation", "NULL", "NULL"
        },
        {
            "2212", "1", "0", "4.54012e-09", "4.54012e-09", "0.0179175", "0.0250066", "777.726", "-2.04548", "5.6447",
            "277.644", "-2.04548", "5.6447", "0.0125", "-0.00435673", "0.0234654", "0.999715", "0", "1", "0", "0", "0", "0",
            "0", "0", "0", "0", "0", "1", "-0.154322", "0.156973", "-500", "0", "1", "0", "277.4", "0",
            "[0;1;1;3;-1;-1;-1;-1;-1;-1]", "Transportation", "NULL", "NULL"
        },
        {
            "2212", "1", "0", "4.89277e-09", "4.89277e-09", "0.143599", "0.0250046", "833.49", "-2.26821", "6.88172",
            "333.394", "-2.26821", "6.88172", "0.0125", "-0.00791184", "0.0166431", "0.99983", "1", "0", "0", "0", "0", "0",
            "0", "0", "0", "0", "0", "1", "-0.154322", "0.156973", "-500", "0", "1", "0", "461.8", "0",
            "[0;2;0;3;-1;-1;-1;-1;-1;-1]", "Transportation", "NULL", "NULL"
        },
        {
            "2212", "1", "0", "4.92791e-09", "4.92791e-09", "0.0204459", "0.0250016", "838.991", "-2.30588", "6.96663",
            "338.894", "-2.30588", "6.96663", "0.0125", "0.0113033", "0.00148302", "0.999935", "1", "1", "0", "0", "0", "0",
            "0", "0", "0", "0", "0", "1", "-0.154322", "0.156973", "-500", "0", "1", "0", "461.8", "0",
            "[0;2;1;3;-1;-1;-1;-1;-1;-1]", "Transportation", "NULL", "NULL"
        },
        {
            "2212", "1", "0", "4.96345e-09", "4.96345e-09", "0.0160251", "0.0250035", "844.492", "-2.25045", "6.96272",
            "344.394", "-2.25045", "6.96272", "0.0125", "0.0167816", "-0.00606", "0.999841", "1", "2", "0", "0", "0", "0",
            "0", "0", "0", "0", "0", "1", "-0.154322", "0.156973", "-500", "0", "1", "0", "461.8", "0",
            "[0;2;2;3;-1;-1;-1;-1;-1;-1]", "Transportation", "NULL", "NULL"
        }
    };
public:
    /**
     * Test the constructor of WProtonData, so no exception is thrown, when input parameters are correct
     */
    void testConstructor()
    {
        TS_ASSERT_THROWS_NOTHING(
            WProtonData( boost::make_shared< WDataSetCSV::Content >( sampleColumnNames ),
                          boost::make_shared< WDataSetCSV::Content >( sampleDataRows ) );
        );
    }

    /**
     * Tests, if setCSVHeader throws an exception, when the new header is empty
     */
    void testSetCSVHeaderEmpty()
    {
        WProtonData protonData( boost::make_shared< WDataSetCSV::Content >( sampleColumnNames ),
                                 boost::make_shared< WDataSetCSV::Content >( sampleDataRows ) );

        WDataSetCSV::ContentSPtr tmpEmptyHeader = WDataSetCSV::ContentSPtr( new WDataSetCSV::Content() );
        TS_ASSERT_THROWS_EQUALS(
            protonData.setCSVHeader( tmpEmptyHeader ),
            WException &e,
            e.what(),
            "No empty header allowed!"
        );
    }

    /**
     * Tests, if setCSVHeader throws an exception, when parameter is a null pointer
     */
    void testSetCSVHeaderNull()
    {
        WProtonData protonData( boost::make_shared< WDataSetCSV::Content >( sampleColumnNames ),
                                 boost::make_shared< WDataSetCSV::Content >( sampleDataRows ) );

        WDataSetCSV::ContentSPtr tmpHeaderNullptr = nullptr;
        TS_ASSERT_THROWS_EQUALS(
            protonData.setCSVHeader( tmpHeaderNullptr ),
            WException &e,
            e.what(),
            "Can not set header! No header specified!"
        );
    }

    /**
     * Tests, if setCSVHeader throws an exception, when parameter is a null pointer
     */
    void testSetCSVDataNull()
    {
        WProtonData protonData( boost::make_shared< WDataSetCSV::Content >( sampleColumnNames ),
                                 boost::make_shared< WDataSetCSV::Content >( sampleDataRows ) );

        WDataSetCSV::ContentSPtr tmpDataNullptr = nullptr;
        TS_ASSERT_THROWS_EQUALS(
                protonData.setCSVData( tmpDataNullptr ),
                WException &e,
                e.what(),
                "Can not set data! No data specified!"
        );
    }

    /**
     * Tests the setter and getter for the member variable -m_ColumnnMapSelectedIndex-
     */
    void testSetterGetterStateIndex()
    {
        WProtonData protonData( boost::make_shared< WDataSetCSV::Content >( sampleColumnNames ),
                                boost::make_shared< WDataSetCSV::Content >( sampleDataRows ) );

        std::tuple< std::string, int > refData1( "test 1", 0 );
        std::tuple< std::string, int > refData2( "test 2", 1 );
        std::tuple< std::string, int > refData3( "test 3", 2 );
        std::tuple< std::string, int > refData4( "test 4", 3 );

        protonData.setStateIndex( std::get<0>( refData1 ), std::get<1>( refData1 ) );
        protonData.setStateIndex( std::get<0>( refData2 ), std::get<1>( refData2 ) );
        protonData.setStateIndex( std::get<0>( refData3 ), std::get<1>( refData3 ) );
        protonData.setStateIndex( std::get<0>( refData4 ), std::get<1>( refData4 ) );

        TS_ASSERT_EQUALS( protonData.getColumnIndexBySelection( std::get<0>( refData1 ) ), std::get<1>( refData1 ) );
        TS_ASSERT_EQUALS( protonData.getColumnIndexBySelection( std::get<0>( refData2 ) ), std::get<1>( refData2 ) );
        TS_ASSERT_EQUALS( protonData.getColumnIndexBySelection( std::get<0>( refData3 ) ), std::get<1>( refData3 ) );
        TS_ASSERT_EQUALS( protonData.getColumnIndexBySelection( std::get<0>( refData4 ) ), std::get<1>( refData4 ) );
    }

    /**
     * Tests the availability of the single selections
     */
    void testIsColumnAvailable()
    {
        WProtonData protonData( boost::make_shared< WDataSetCSV::Content >( sampleColumnNames ),
                                boost::make_shared< WDataSetCSV::Content >( sampleDataRows ) );

        std::tuple< std::string, int > refData1( "test 1", 0 );
        std::tuple< std::string, int > refData2( "test 2", 1 );
        std::tuple< std::string, int > refData3( "test 3", 2 );
        std::tuple< std::string, int > refData4( "test 4", 3 );

        //-1 stands for no selection
        std::tuple< std::string, int > refData5( "test 5", -1 );
        std::tuple< std::string, int > refData6( "test 6", -1 );

        protonData.setStateIndex( std::get<0>( refData1 ), std::get<1>( refData1 ) );
        protonData.setStateIndex( std::get<0>( refData2 ), std::get<1>( refData2 ) );
        protonData.setStateIndex( std::get<0>( refData3 ), std::get<1>( refData3 ) );
        protonData.setStateIndex( std::get<0>( refData4 ), std::get<1>( refData4 ) );
        protonData.setStateIndex( std::get<0>( refData5 ), std::get<1>( refData5 ) );
        protonData.setStateIndex( std::get<0>( refData6 ), std::get<1>( refData6 ) );

        TS_ASSERT_EQUALS( protonData.isColumnAvailable( std::get<0>( refData1 ) ), true );
        TS_ASSERT_EQUALS( protonData.isColumnAvailable( std::get<0>( refData2 ) ), true );
        TS_ASSERT_EQUALS( protonData.isColumnAvailable( std::get<0>( refData3 ) ), true );
        TS_ASSERT_EQUALS( protonData.isColumnAvailable( std::get<0>( refData4 ) ), true );
        TS_ASSERT_EQUALS( protonData.isColumnAvailable( std::get<0>( refData5 ) ), false );
        TS_ASSERT_EQUALS( protonData.isColumnAvailable( std::get<0>( refData6 ) ), false );

        //name which is not in the m_ColumnnMapSelectedIndex
        TS_ASSERT_EQUALS( protonData.isColumnAvailable( "No Name" ), false );
        TS_ASSERT_EQUALS( protonData.isColumnAvailable( "unavailable" ), false );
    }

    /**
    * Tests the content of -m_columnMap-
    */
    void testGetColumnIndex()
    {
        WProtonData protonData( boost::make_shared< WDataSetCSV::Content >( sampleColumnNames ),
                                boost::make_shared< WDataSetCSV::Content >( sampleDataRows ) );

        std::vector< std::string > header = sampleColumnNames.at( 0 );

        for( size_t headerCounter = 0; headerCounter < header.size(); headerCounter++ )
        {
            TS_ASSERT_EQUALS( protonData.getColumnIndex( header.at( headerCounter ) ), headerCounter );
        }

        //name which is not in the m_columnMap
        TS_ASSERT_EQUALS( protonData.getColumnIndex( "No Name" ), -1 );
        TS_ASSERT_EQUALS( protonData.getColumnIndex( "unavailable" ), -1 );
    }
};

#endif  // WPROTONDATA_TEST_H
