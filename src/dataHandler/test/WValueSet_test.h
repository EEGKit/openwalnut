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

#ifndef WVALUESET_TEST_H
#define WVALUESET_TEST_H

#include <vector>

#include <cxxtest/TestSuite.h>

#include "../WValueSet.h"
#include "../WDataHandlerEnums.h"

/**
 * UnitTests the WValueSet class
 */
class WValueSetTest : public CxxTest::TestSuite
{
public:
    /**
     * An instantiation should never throw an exception
     */
    void testInstantiation( void )
    {
        double a[2] = { 0.0, 3.1415 };
        const std::vector< double > v( a, a + sizeof( a ) / sizeof( double ) );
        TS_ASSERT_THROWS_NOTHING( WValueSet< double > valueSet( 0, 1, v, W_DT_DOUBLE ) );
    }

    /**
     * The number of values retrieved is correct
     */
    void testGetNumberOfValues( void )
    {
        int a[4] = { 0, -5, 1, 2 };
        const std::vector< int8_t > v( a, a + sizeof( a ) / sizeof( int ) );
        WValueSet< int8_t > first( 0, 1, v, W_DT_INT8 );
        TS_ASSERT_EQUALS( first.size(), 4 );
        WValueSet< int8_t > second( 1, 2, v, W_DT_INT8 );
        TS_ASSERT_EQUALS( second.size(), 2 );
        WValueSet< int8_t > third( 2, 2, v, W_DT_INT8 );
        TS_ASSERT_EQUALS( third.size(), 1 );
    }

    /**
     * The raw size is the size of the number of integral elements inside
     * this ValueSet.
     */
    void testRawSize( void )
    {
        int8_t a[4] = { 0, -5, 1, 2 };
        const std::vector< int8_t > v( a, a + sizeof( a ) / sizeof( int8_t ) );
        WValueSet< int8_t > first( 0, 1, v, W_DT_INT8 );
        TS_ASSERT_EQUALS( first.rawSize(), 4 );
        WValueSet< int8_t > second( 2, 2, v, W_DT_INT8 );
        TS_ASSERT_EQUALS( first.rawSize(), 4 );
    }

    /**
     * This function should return the i-th value if the value set is scalar.
     */
    void testGetScalar( void )
    {
        int8_t a[4] = { 0, -5, 1, 2 };
        const std::vector< int8_t > v( a, a + sizeof( a ) / sizeof( int8_t ) );
        WValueSet< int8_t > set( 0, 1, v, W_DT_INT8 );
        TS_ASSERT_EQUALS( set.getScalar( 0 ), a[0] );
        TS_ASSERT_EQUALS( set.getScalar( 1 ), a[1] );
        TS_ASSERT_EQUALS( set.getScalar( 2 ), a[2] );
        TS_ASSERT_EQUALS( set.getScalar( 3 ), a[3] );
    }

    /**
     * Raw Access should provide data access to the underlying array.
     */
    void testReadOnlyRawAccess( void )
    {
        double a[2] = { 0.0, 3.1415 };
        const std::vector< double > v( a, a + sizeof( a ) / sizeof( double ) );
        WValueSet< double > valueSet( 0, 1, v, W_DT_DOUBLE );
        const double * const b = valueSet.rawData();
        TS_ASSERT_EQUALS( b[0], 0.0 );
        TS_ASSERT_EQUALS( b[1], 3.1415 );
    }
};

#endif  // WVALUESET_TEST_H
