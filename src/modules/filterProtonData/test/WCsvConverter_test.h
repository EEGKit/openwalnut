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

#include <vector>
#include <string>
#include <cxxtest/TestSuite.h>

#include "core/kernel/WModuleFactory.h"
#include "../WCsvConverter.h"
#include "../WMFilterProtonData.h"
#include "../../transferFunctionColorBar/WMTransferFunctionColorBar.h"
#include "../../data/io/WReaderCSV.h"

/**
 * Test class of WCsvConverter class
 */

class WCsvConverterTest : public CxxTest::TestSuite
{
public:
    /**
     *
    */
    WProtonData::SPtr m_protonData = nullptr;

    /**
     *
     */
    boost::shared_ptr< WPropertyStatus > m_propertyStatus = nullptr;

    /**
     *
     */
    boost::shared_ptr< WProperties > m_properties = nullptr;

    /**
     *
     * @param setUp
     * @return
     */
    WModule::SPtr m_colorBar = nullptr;

    /**
     * Setup variables and data, needed for each test.
     */
    void setUp()
    {
        WReaderCSV csvReader( W_FIXTURE_PATH + "../data/CSVs/valid.csv" );
        boost::shared_ptr< WDataSetCSV > csvDataSet = csvReader.read();
        m_protonData = WProtonData::SPtr( new WProtonData( csvDataSet->getHeader(), csvDataSet->getData() ) );
        m_propertyStatus = boost::shared_ptr< WPropertyStatus >( new WPropertyStatus() );

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

        TS_ASSERT_IS_NAN( tmpCsvReader.getClusterSize( -0.0001f ) );
        TS_ASSERT_IS_NAN( tmpCsvReader.getClusterSize( -1.0f ) );
    }
};

#endif  // WCSVCONVERTER_TEST_H
