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

#include <vector>
#include <string>
#include <cxxtest/TestSuite.h>
#include <core/common/test/WException_test.h>

#include "../WMProtonData.h"

/**
 * Test class of WMProtonData class
 */

class WMProtonDataTest : public CxxTest::TestSuite
{
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
            WMProtonData( boost::make_shared< WDataSetCSV::Content >( sampleColumnNames ),
                          boost::make_shared< WDataSetCSV::Content >( sampleDataRows ) );
        );
    }

    /**
     * Tests, if setCSVHeader throws an exception, when the new header is empty
     */
    void testSetCSVHeaderEmpty()
    {
        WMProtonData protonData( boost::make_shared< WDataSetCSV::Content >( sampleColumnNames ),
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
        WMProtonData protonData( boost::make_shared< WDataSetCSV::Content >( sampleColumnNames ),
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
        WMProtonData protonData( boost::make_shared< WDataSetCSV::Content >( sampleColumnNames ),
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
     * Tests, if all entries of m_availabilityColumnMap are set to true
     */
     void testUpdateAvailabilityOfColumnsAllTrue()
    {
        WMProtonData protonData( boost::make_shared< WDataSetCSV::Content >( sampleColumnNames ),
                                 boost::make_shared< WDataSetCSV::Content >( sampleDataRows ) );
        for( auto keyValuePair : protonData.m_availabilityColumnMap )
        {
            TS_ASSERT_EQUALS( keyValuePair.second, true );
        }
    }

    /**
     * Tests, if all entries of m_availabilityColumnMap are set to false
     */
    void testUpdateAvailabilityOfColumnsAllFalse()
    {
        WDataSetCSV::Content tmpColumnNames =
        {
            {
                "PDGEncoding_", "edep_", "trackID_", "parentID_", "eventID_", "someMoreValues"
            }
        };

        WMProtonData protonData( boost::make_shared< WDataSetCSV::Content >( tmpColumnNames ),
                                 boost::make_shared< WDataSetCSV::Content >( sampleDataRows ) );

        for( auto keyValuePair : protonData.m_availabilityColumnMap )
        {
            TS_ASSERT_EQUALS( keyValuePair.second, false );
        }
    }

    /**
     * Tests, if one entries of m_availabilityColumnMap is set to false
     */
    void testUpdateAvailabilityOfColumnsOneTrue()
    {
        WDataSetCSV::Content tmpColumnNames =
        {
            {
                "_PDGEncoding", "_edep", "_trackID", "parentID", "_eventID", "someMoreValues"
            }
        };

        WMProtonData protonData( boost::make_shared< WDataSetCSV::Content >( tmpColumnNames ),
                                 boost::make_shared< WDataSetCSV::Content >( sampleDataRows ) );

        for( auto keyValuePair : protonData.m_availabilityColumnMap )
        {
            if( keyValuePair.first == "parentID" )
            {
                TS_ASSERT_EQUALS( keyValuePair.second, true );
            }
            else
            {
                TS_ASSERT_EQUALS( keyValuePair.second, false );
            }
        }
    }
};

#endif  // WMPROTONDATA_TEST_H
