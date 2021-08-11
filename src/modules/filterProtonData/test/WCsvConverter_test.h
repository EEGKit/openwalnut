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

#ifndef WCSVCONVERTER_TEST_H
#define WCSVCONVERTER_TEST_H

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <cxxtest/TestSuite.h>

#include "../../data/io/WReaderCSV.h"
#include "../../transferFunctionColorBar/WMTransferFunctionColorBar.h"
#include "../WCsvConverter.h"
#include "../WMFilterProtonData.h"
#include "core/kernel/WModuleFactory.h"

/**
 * Test class of WCsvConverter class
 */
class WCsvConverterTest : public CxxTest::TestSuite
{
public:
    /**
     * The protondata
     */
    WProtonData::SPtr m_protonData = nullptr;

    /**
     * The property status
     */
    std::shared_ptr< WPropertyStatus > m_propertyStatus = nullptr;

    /**
     * the property group
     */
    std::shared_ptr< WProperties > m_properties = nullptr;

    /**
     * The color bar
     */
    WModule::SPtr m_colorBar = nullptr;

    /**
     * Setup variables and data, needed for each test.
     */
    void setUp()
    {
        WReaderCSV csvReader( W_FIXTURE_PATH + "../data/CSVs/valid.csv" );
        std::shared_ptr< WDataSetCSV > csvDataSet = csvReader.read();
        m_protonData = WProtonData::SPtr( new WProtonData( csvDataSet->getHeader(), csvDataSet->getData() ) );
        m_propertyStatus = std::shared_ptr< WPropertyStatus >( new WPropertyStatus() );

        m_propertyStatus->setColumnPropertyHandler( WColumnPropertyHandler::SPtr( new WColumnPropertyHandler( m_protonData, m_properties, NULL ) ) );

        m_propertyStatus->setFilterPropertyHandler( WFilterPropertyHandler::SPtr( new WFilterPropertyHandler( m_protonData, m_properties, NULL ) ) );

        m_propertyStatus->setVisualizationPropertyHandler( WVisualizationPropertyHandler::SPtr(
                new WVisualizationPropertyHandler( m_protonData, m_properties, NULL ) )
        );

        m_propertyStatus->setEventIDLimitationPropertyHandler( WEventIDLimitationPropertyHandler::SPtr(
                new WEventIDLimitationPropertyHandler( m_protonData, m_properties, NULL ) )
        );

        m_colorBar = WModule::SPtr( new WMTransferFunctionColorBar() );
        //m_propertyStatus->getColumnPropertyHandler()->setSelectionEventMethod( nullptr );
    }

    /**
     * Test the constructors of WCsvConverter, so no exception is thrown, when input parameters are correct
     */
    void testConstructorThrowNothing()
    {
        TS_ASSERT_THROWS_NOTHING( WCsvConverter( m_protonData, m_propertyStatus, m_colorBar ) );
    }

    /**
     * Tests the method getClusterSize()
     */
    void testGetClusterSize()
    {
        WCsvConverter tmpCsvReader( m_protonData, m_propertyStatus, m_colorBar );
        TS_ASSERT_EQUALS( tmpCsvReader.getClusterSize( 0.0f ), 0.0f );
        TS_ASSERT_DELTA( tmpCsvReader.getClusterSize( 1.0f ), 36.11878927498844f, 1e-5 );
        TS_ASSERT_DELTA( tmpCsvReader.getClusterSize( 1.0001f ), 36.1203073289856f, 1e-5 );

        // uses std::isnan instead of TS_ASSERT_IS_NAN as it is not present in the cxxtest version on the build server
        TS_ASSERT( std::isnan( tmpCsvReader.getClusterSize( -0.0001f ) ) );
        TS_ASSERT( std::isnan( tmpCsvReader.getClusterSize( -1.0f ) ) );
    }

    /**
     * Tests the method stringToDouble()
     */
    void testStringToDouble()
    {
        std::map< std::string, double > testDoubleMap;
        testDoubleMap.insert( std::pair< std::string, double >( "1000.1", 1000.1f ) );
        testDoubleMap.insert( std::pair< std::string, double >( "1.", 1.0f ) );
        testDoubleMap.insert( std::pair< std::string, double >( ".1", 0.1f ) );
        testDoubleMap.insert( std::pair< std::string, double >( "2.1", 2.1f ) );
        testDoubleMap.insert( std::pair< std::string, double >( "+1", 1.0f ) );
        testDoubleMap.insert( std::pair< std::string, double >( "-1", -1.0f ) );
        testDoubleMap.insert( std::pair< std::string, double >( "+.1", 0.1f ) );
        testDoubleMap.insert( std::pair< std::string, double >( "-.1", -0.1f ) );
        testDoubleMap.insert( std::pair< std::string, double >( "+1e-1", 0.1f ) );
        testDoubleMap.insert( std::pair< std::string, double >( "0.001e-6", 0.000000001f ) );
        testDoubleMap.insert( std::pair< std::string, double >( "0.111111111111111", 0.111111111111111f ) );

        WCsvConverter tmpCsvReader( m_protonData, m_propertyStatus, m_colorBar );

        for( std::pair< std::string, double > stringDoublePair : testDoubleMap )
        {
            TS_ASSERT_EQUALS( tmpCsvReader.stringToDouble( stringDoublePair.first ),
                              stringDoublePair.second );
        }

        TS_ASSERT_THROWS( tmpCsvReader.stringToDouble( "1.g" ), WException &e );
        TS_ASSERT_THROWS( tmpCsvReader.stringToDouble( "" ), WException &e );
        TS_ASSERT_THROWS( tmpCsvReader.stringToDouble( "Test" ), WException &e );
        TS_ASSERT_THROWS( tmpCsvReader.stringToDouble( "1.1.2" ), WException &e );
    }

    /**
     * Tests the method stringToInt()
     */
    void testStringToInt()
    {
        std::map< std::string, int > testIntMap;
        testIntMap.insert( std::pair< std::string, int >( "1", 1 ) );
        testIntMap.insert( std::pair< std::string, int >( "0", 0 ) );
        testIntMap.insert( std::pair< std::string, int >( "10", 10 ) );
        testIntMap.insert( std::pair< std::string, int >( "+1", 1 ) );
        testIntMap.insert( std::pair< std::string, int >( "-1", -1 ) );
        testIntMap.insert( std::pair< std::string, int >( "1e-1", 0 ) );
        testIntMap.insert( std::pair< std::string, int >( "1e1", 10 ) );
        testIntMap.insert( std::pair< std::string, int >( "+1e-1", 0 ) );
        testIntMap.insert( std::pair< std::string, int >( "0.001e-6", 0 ) );
        testIntMap.insert( std::pair< std::string, int >( "0.111111111111111", 0 ) );

        WCsvConverter tmpCsvReader( m_protonData, m_propertyStatus, m_colorBar );

        for( std::pair< std::string, int > stringIntPair : testIntMap )
        {
            TS_ASSERT_EQUALS( tmpCsvReader.stringToInt( stringIntPair.first ),
                              stringIntPair.second );
        }

        TS_ASSERT_THROWS( tmpCsvReader.stringToInt( "1.g" ), WException &e );
        TS_ASSERT_THROWS( tmpCsvReader.stringToInt( "" ), WException &e );
        TS_ASSERT_THROWS( tmpCsvReader.stringToInt( "Test" ), WException &e );
        TS_ASSERT_THROWS( tmpCsvReader.stringToInt( "1.1.2" ), WException &e );
    }
};

#endif  // WCSVCONVERTER_TEST_H
