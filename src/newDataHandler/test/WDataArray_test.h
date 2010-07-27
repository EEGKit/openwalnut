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

#ifndef WDATAARRAY_TEST_H
#define WDATAARRAY_TEST_H

#include <cxxtest/TestSuite.h>

#include "../WDataArray.h"
#include "../WVector.h"

/**
 * Tests the WDataArray class.
 */
class WDataArrayTest : public CxxTest::TestSuite
{
public:

    /**
     * Test is everything gets initialized properly.
     */
    void testInitialization( void )
    {
        WDataArray< WVector< 3, float > > data( 3 );

        TS_ASSERT_EQUALS( data[ 0 ][ 0 ], 0.0f );
        TS_ASSERT_EQUALS( data[ 0 ][ 1 ], 0.0f );
        TS_ASSERT_EQUALS( data[ 0 ][ 2 ], 0.0f );
        TS_ASSERT_EQUALS( data[ 1 ][ 0 ], 0.0f );
        TS_ASSERT_EQUALS( data[ 1 ][ 1 ], 0.0f );
        TS_ASSERT_EQUALS( data[ 1 ][ 2 ], 0.0f );
        TS_ASSERT_EQUALS( data[ 2 ][ 0 ], 0.0f );
        TS_ASSERT_EQUALS( data[ 2 ][ 1 ], 0.0f );
        TS_ASSERT_EQUALS( data[ 2 ][ 2 ], 0.0f );
    }

    /**
     * Test if the elements of the vectors in a DataArray
     * have successive memory positions.
     */
    void testMemoryLayout( void )
    {
        // instantiate a DataArray
        WDataArray< WVector< 2, float > > data( 3 );

        data[ 0 ][ 0 ] = 1.0f;
        data[ 0 ][ 1 ] = 2.0f;
        data[ 1 ][ 0 ] = 3.0f;
        data[ 1 ][ 1 ] = 4.0f;
        data[ 2 ][ 0 ] = 5.0f;
        data[ 2 ][ 1 ] = 6.0f;

        float* f = &data[ 0 ][ 0 ];
        TS_ASSERT_EQUALS( *f, 1.0f );
        ++f;
        TS_ASSERT_EQUALS( *f, 2.0f );
        ++f;
        TS_ASSERT_EQUALS( *f, 3.0f );
        ++f;
        TS_ASSERT_EQUALS( *f, 4.0f );
        ++f;
        TS_ASSERT_EQUALS( *f, 5.0f );
        ++f;
        TS_ASSERT_EQUALS( *f, 6.0f );

        *f = 35.5f;
        TS_ASSERT_EQUALS( data[ 2 ][ 1 ], 35.5f );
    }
};

#endif  // WDATAARRAY_TEST_H

