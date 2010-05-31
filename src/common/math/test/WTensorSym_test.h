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

#ifndef WTENSORSYM_TEST_H
#define WTENSORSYM_TEST_H

#include <string>
#include <vector>
#include <algorithm>

#include <cxxtest/TestSuite.h>
#include "../WTensorSym.h"

/**
 * Test class for the WTensorSym template.
 */
class WTensorSymTest : public CxxTest::TestSuite
{
public:
    /**
     * Test access operator ().
     */
    void testAccessOperator1()
    {
        wmath::WTensorSym< 3, 2 > w;
        w( 0, 0, 0 ) = 2;
        w( 0, 0, 1 ) = 3;
        w( 0, 1, 0 ) = 0;
        w( 0, 1, 1 ) = 5;
        w( 1, 0, 0 ) = 2;
        w( 1, 0, 1 ) = 1;
        w( 1, 1, 0 ) = 8;
        w( 1, 1, 1 ) = 10;

        TS_ASSERT_EQUALS( w( 0, 0, 0 ), 2 );
        TS_ASSERT_EQUALS( w( 0, 0, 1 ), 2 );
        TS_ASSERT_EQUALS( w( 0, 1, 0 ), 2 );
        TS_ASSERT_EQUALS( w( 0, 1, 1 ), 8 );
        TS_ASSERT_EQUALS( w( 1, 0, 0 ), 2 );
        TS_ASSERT_EQUALS( w( 1, 0, 1 ), 8 );
        TS_ASSERT_EQUALS( w( 1, 1, 0 ), 8 );
        TS_ASSERT_EQUALS( w( 1, 1, 1 ), 10 );

        // test a symmetric tensor of dimension 1
        // this should not segfault
        wmath::WTensorSym< 4, 1 > t;

        t( 0, 0, 0, 0 ) = 2.0;

        TS_ASSERT_EQUALS( t( 0, 0, 0, 0 ), 2.0 );
    }

    /**
     * Test access operator [].
     */
    void testAccessOperator2()
    {
        std::vector< unsigned int > v( 3, 0 );
        wmath::WTensorSym< 3, 4 > w;

        for( v[ 0 ] = 0; v[ 0 ] < 4; ++v[ 0 ] )
        {
            for( v[ 1 ] = 0; v[ 1 ] < 4; ++v[ 1 ] )
            {
                for( v[ 2 ] = 0; v[ 2 ] < 4; ++v[ 2 ] )
                {
                    w[ v ] = v[ 0 ] + v[ 1 ] + v[ 2 ];
                    std::vector< unsigned int > v0 = v;
                    std::sort( v0.begin(), v0.end() );
                    TS_ASSERT_EQUALS( w[ v0 ], v[ 0 ] + v[ 1 ] + v[ 2 ] );
                }
            }
        }
    }

    /**
     * Test the standard constructor.
     */
    void testStandardConstructor()
    {
        // create lots of tensors
        wmath::WTensorSym< 1, 1 > t11d;
        wmath::WTensorSym< 1, 2 > t12d;
        wmath::WTensorSym< 1, 3 > t13d;
        wmath::WTensorSym< 1, 4 > t14d;
        wmath::WTensorSym< 1, 1, float > t11f;
        wmath::WTensorSym< 1, 2, int > t12i;
        wmath::WTensorSym< 1, 3, char > t13c;
        wmath::WTensorSym< 1, 4, std::string > t14s;
        wmath::WTensorSym< 2, 1 > t21d;
        wmath::WTensorSym< 2, 2 > t22d;
        wmath::WTensorSym< 2, 3 > t23d;
        wmath::WTensorSym< 2, 4 > t24d;
        wmath::WTensorSym< 2, 1, int > t21i;
        wmath::WTensorSym< 2, 2, char > t22c;
        wmath::WTensorSym< 2, 3, float > t23f;
        wmath::WTensorSym< 2, 4, float > t24f;
        wmath::WTensorSym< 3, 5 > t35d;
        wmath::WTensorSym< 4, 3 > t43d;
        wmath::WTensorSym< 5, 2 > t52d;
        wmath::WTensorSym< 6, 3 > t63d;

        TS_ASSERT_EQUALS( t35d( 0, 4, 2 ), 0.0 );
        TS_ASSERT_EQUALS( t35d( 1, 4, 0 ), 0.0 );
        TS_ASSERT_EQUALS( t35d( 0, 3, 0 ), 0.0 );
        TS_ASSERT_EQUALS( t35d( 2, 4, 1 ), 0.0 );
        TS_ASSERT_EQUALS( t35d( 0, 2, 2 ), 0.0 );
        TS_ASSERT_EQUALS( t35d( 4, 1, 4 ), 0.0 );
        TS_ASSERT_EQUALS( t35d( 4, 4, 4 ), 0.0 );
        TS_ASSERT_EQUALS( t35d( 3, 4, 3 ), 0.0 );

        TS_ASSERT_EQUALS( t11d( 0 ), 0.0 );
        TS_ASSERT_EQUALS( t22d( 0, 1 ), 0.0 );
    }

    /**
     * Test copy constructor.
     */
    void testCopyConstructor()
    {
        wmath::WTensorSym< 2, 3 > w;
        w( 0, 1 ) = 2;
        w( 2, 1 ) = 0.456;

        wmath::WTensorSym< 2, 3 > m( w );
        TS_ASSERT_EQUALS( m( 1, 0 ), 2 );
        TS_ASSERT_EQUALS( m( 1, 2 ), 0.456 );
    }

    /**
     * Test copy operator.
     */
    void testCopyOperator()
    {
        wmath::WTensorSym< 6, 2 > w;
        w( 0, 0, 1, 1, 0, 1 ) = 4.0;
        w( 1, 1, 0, 0, 0, 0 ) = 0.56;
        wmath::WTensorSym< 6, 2 > m;

        {
            m = w;
            TS_ASSERT_EQUALS( m( 0, 1, 0, 1, 0, 1 ), 4.0 );
            TS_ASSERT_EQUALS( m( 1, 0, 0, 0, 1, 0 ), 0.56 );
            TS_ASSERT_EQUALS( m( 0, 0, 0, 1, 0, 0 ), 0.0 );
        }
    }

    /**
     * Test casts to Data_T, WValue or WMatrix, depending on the order of the Tensor.
     */
    void testCastToVariousTypes()
    {
        // make sure these casts compile
        // we don't actually want to thoroughly test functionality here
        // more sophisticated tests can be found in WTensorFuncTest
        // cast to Data_T
        {
            wmath::WTensorSym< 0, 0, double > t;
            t() = 3.0;
            double d = t;
            TS_ASSERT_EQUALS( d, 3.0 );
        }
        // cast to WValue
        {
            wmath::WTensorSym< 1, 2, int > t;
            t( 0 ) = 3.0;
            wmath::WValue< int > v = t;
            TS_ASSERT_EQUALS( v[ 0 ], 3.0 );
        }
        // cast to WMatrix
        {
            wmath::WTensorSym< 2, 3, float > t;
            t( 0, 1 ) = 3.0;
            wmath::WMatrix< float > m = t;
            TS_ASSERT_EQUALS( m( 1, 0 ), 3.0 );
        }
    }
};

#endif  // WTENSORSYM_TEST_H
