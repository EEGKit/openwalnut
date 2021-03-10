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

#ifndef WDATASETCSV_TEST_H
#define WDATASETCSV_TEST_H

#include <boost/shared_ptr.hpp>

#include <cxxtest/TestSuite.h>

#include "../WDataSetCSV.h"

/**
 * test class of the WDataSetCSV class
 */
class WDataSetCSVTest : public CxxTest::TestSuite
{
    /**
     * WDataSetCSV object, which is created during the tests
     */
    WDataSetCSV csvDataSet;

    /**
     * WDataSetCSV::ContentSPtr object, represents the header, while creation of csvDataSet
     */
    WDataSetCSV::ContentSPtr m_header;

    /**
     * WDataSetCSV::ContentSPtr object, represents the data, while creation of csvDataSet
     */
    WDataSetCSV::ContentSPtr m_data;
public:
    /**
     * setUp method, runs before each test
     */
     void setUp()
    {
        std::vector< std::string > vecHeader;
        std::vector< std::string > vecData;

        m_header = WDataSetCSV::ContentSPtr( new WDataSetCSV::Content() );
        m_data = WDataSetCSV::ContentSPtr( new WDataSetCSV::Content() );

        // initialize m_header
        vecHeader.push_back( "cell00" );
        vecHeader.push_back( "cell01" );
        m_header->push_back( vecHeader );

        // initialize m_data
        for( int idx = 0; idx < 5; idx++ )
        {
            vecData.clear();
            vecData.push_back( "cell"+ std::to_string( idx ) + "0" );
            vecData.push_back( "cell"+ std::to_string( idx ) + "1" );
            m_data->push_back( vecData );
        }

        csvDataSet = WDataSetCSV( m_header, m_data );
    }

    /**
     * tearDown  method, runs after each test
     */
    void tearDown()
    {
    }

    /**
     * Test getHeader() method
     */
    void testGetHeader()
    {
        TS_ASSERT_EQUALS( csvDataSet.getHeader(), m_header );
    }

    /**
     * Test getData() method
     */
    void testGetData()
    {
        TS_ASSERT_EQUALS( csvDataSet.getData(), m_data );
    }

};

#endif  // WDATASETCSV_TEST_H
