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

#ifndef WSCALAR_TEST_H
#define WSCALAR_TEST_H

#include <list>
#include <iostream>

#include <cxxtest/TestSuite.h>

#include "../WScalar.h"

/**
 * Tests the WScalar class.
 */
class WScalarTest : public CxxTest::TestSuite
{
public:

    /**
     * Test whether instantiation works. Especially tests all available constructors (except copy constructor).
     */
    void testInstantiation( void )
    {
        // default constructor.
        WScalar a;

        // non-explicit convert constructors.
        WScalar i = 15;
        WScalar d = 3.0;

        // the constructors should have create the instances with the correct values
        TS_ASSERT( i == WScalar( 15 ) );
        TS_ASSERT( d == WScalar( 3.0 ) );
        TS_ASSERT( i == 15 );
        TS_ASSERT( d == 3.0 );

        // default constructors create doubles with 0.0 as initial value
        TS_ASSERT( a == 0.0 );
    }

    /**
     * Test whether copying works.
     */
    void testCopy( void )
    {

    }

    /**
     * Test comparison (especially between different types).
     */
    void testComparison( void )
    {
        WScalar i = 15;
        WScalar i2 = 3;
        WScalar d = 3.0;
        WScalar d2 = 3.001; // slightly different value

        TS_ASSERT( d == i2 );
        TS_ASSERT( d != d2 );       // ensure the double is not casted to int and then compared -> this test would fail if so
        TS_ASSERT( d2 != i2 );      // ensure the double is not casted to int and then compared -> this test would fail if so

        // -> WScalarImpl instances are large large large large :-(
        std::cout << sizeof( WScalarImplDouble ) << " - " << sizeof( double ) << std::endl;
        std::cout << sizeof( WScalarImplInt32 ) << " - " << sizeof( int32_t ) << std::endl;

        std::list< double >  l1( 100000000, 1.0 );
        std::list< WScalar > l2( 100000000, WScalar( 1.0 ) );
        typedef std::list< double >::const_iterator DblIter;
        typedef std::list< WScalar >::const_iterator WScalarIter;


        size_t aCounter = 0;
        for ( DblIter it = l1.begin(); it != l1.end(); ++it )
        {
            if ( ( *it ) == 1.0 )
            {
                ++aCounter;
            }
        }
        std::cout << aCounter << std::endl;

        size_t bCounter = 0;
        WScalar comp( 1.0 );
        for ( WScalarIter it = l2.begin(); it != l2.end(); ++it )
        {
            if ( ( *it ) == comp )
            {
                ++bCounter;
            }
        }
        std::cout << bCounter << std::endl;

    }

};

#endif  // WSCALAR_TEST_H


