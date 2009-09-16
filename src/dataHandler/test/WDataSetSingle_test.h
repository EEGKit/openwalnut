//---------------------------------------------------------------------------
//
// Project: OpenWalnut
//
// Copyright 2009 SomeCopyrightowner
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

#ifndef WDATASETSINGLE_TEST_H
#define WDATASETSINGLE_TEST_H

#include <vector>

#include <cxxtest/TestSuite.h>

#include "../WDataSetSingle.h"
#include "../WValueSet.hpp"
#include "../WGrid.h"

class WDataSetSingleTest : public CxxTest::TestSuite
{
public:
    boost::shared_ptr< WGrid > gridDummy;
    boost::shared_ptr< WValueSetBase > valueSetDummy;

    /**
     * Constructs unit test environment.
     */
    void setUp( void )
    {
        // create dummies, since they are needed in almost every test
        gridDummy = boost::shared_ptr< WGrid >( new WGrid( 1 ) );
        std::vector< int > data( 1, 1 );
        valueSetDummy = boost::shared_ptr< WValueSet< int > >( new WValueSet< int >( 0, 1, data ) );
    }

    /**
     * During instantiation nothing should be thrown.
     */
    void testInstantiation( void )
    {
        TS_ASSERT_THROWS_NOTHING( WDataSetSingle ds( valueSetDummy, gridDummy ) );
    }

    /**
     * Retrieving a WValueSetBase should always give the original pointer.
     */
    void testGetValueSet( void )
    {
        std::vector< double > data( 1, 3.1415 );
        boost::shared_ptr< WValueSet< double > > other;
        other = boost::shared_ptr< WValueSet< double > >( new WValueSet< double >( 0, 1, data ) );
        WDataSetSingle dataSetSingle( valueSetDummy, gridDummy );
        TS_ASSERT_EQUALS( dataSetSingle.getValueSet(), valueSetDummy );
        TS_ASSERT_DIFFERS( dataSetSingle.getValueSet(), other );
    }

    /**
     * Retrieving a WGrid should always give the original pointer.
     */
    void testGetGrid( void )
    {
        boost::shared_ptr< WGrid > other = boost::shared_ptr< WGrid >( new WGrid( 1 ) );
        WDataSetSingle dataSetSingle( valueSetDummy, gridDummy );
        TS_ASSERT_EQUALS( dataSetSingle.getGrid(), gridDummy );
        TS_ASSERT_DIFFERS( dataSetSingle.getGrid(), other );
    }
};

#endif  // WDATASETSINGLE_TEST_H
