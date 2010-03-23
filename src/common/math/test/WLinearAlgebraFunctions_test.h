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

#ifndef WLINEARALGEBRAFUNCTIONS_TEST_H
#define WLINEARALGEBRAFUNCTIONS_TEST_H

#include <string>

#include <cxxtest/TestSuite.h>

#include "../../WException.h"
#include "../WLinearAlgebraFunctions.h"
#include "../WMatrix.h"
#include "../WVector3D.h"
#include "WVector3DTraits.h"

using wmath::WMatrix;
/**
 * Tests for WMatrix.
 */
class WLinearAlgebraFunctionsTest : public CxxTest::TestSuite
{
public:
    /**
     * The vector multiplied with the matrix is just a new vector where
     * each component is the dot product of the corresponding row with the vector.
     */
    void testMatrixVectorMultiply( void )
    {
        wmath::WVector3D v( 9, 10, 11 );
        wmath::WMatrix< double > m( 3, 3 );
        int i = 0;
        for( size_t r = 0; r < 3; ++r)
        {
            for( size_t c = 0; c < 3; ++c, ++i )
            {
                m( r, c ) = i;
            }
        }
        wmath::WVector3D result = wmath::multMatrixWithVector3D( m, v );
        wmath::WVector3D expected( 32, 122, 212 );
        TS_ASSERT_EQUALS( result, expected );
    }

    /**
     * If the matrix is not singular then an inverse should exist and be definite.
     */
    void test3x3MatrixInversion( void )
    {
        int i = 0;
        wmath::WMatrix< double > m( 3, 3 );
        for( size_t r = 0; r < 3; ++r)
        {
            for( size_t c = 0; c < 3; ++c, ++i )
            {
                m( r, c ) = i;
            }
        }
        m( 2, 1 ) = 8;
        wmath::WMatrix< double > expected( 3, 3 );
        expected( 0, 0 ) = 1./6 *  -8;
        expected( 0, 1 ) = 1./6 *   8;
        expected( 0, 2 ) = 1./6 *  -3;
        expected( 1, 0 ) = 1./6 *   6;
        expected( 1, 1 ) = 1./6 * -12;
        expected( 1, 2 ) = 1./6 *   6;
        expected( 2, 0 ) = 1./6 *   0;
        expected( 2, 1 ) = 1./6 *   6;
        expected( 2, 2 ) = 1./6 *  -3;
        TS_ASSERT_EQUALS( wmath::invertMatrix3x3( m ), expected );
    }

    /**
     * On singular matrices no inverse exists!
     */
    void test3x3MatrixInversionOnSingularMatrix( void )
    {
        int i = 0;
        wmath::WMatrix< double > m( 3, 3 );
        for( size_t r = 0; r < 3; ++r)
        {
            for( size_t c = 0; c < 3; ++c, ++i )
            {
                m( r, c ) = i;
            }
        }
        TS_ASSERT_THROWS_EQUALS( wmath::invertMatrix3x3( m ), WException& e, std::string( e.what() ),
                "WAssert failed. Message: Determinat is zero. This matrix can not be inverted." );
    }

    /**
     * Test the inversion of 4x4 matrices
     */
    void test4x4Inverse()
    {
        wmath::WMatrix<double> m( 4, 4 );

        for( size_t r = 0; r < 4; ++r)
        {
            for( size_t c = 0; c < 4; ++c )
            {
                m( c, r ) = r + c * 4 + 1;
            }
        }
        m( 2, 2 ) = 12;
        m( 3, 3 ) = 15;

        wmath::WMatrix<double> m_inv = wmath::invertMatrix4x4( m );

        wmath::WMatrix<double> id = wmath::WMatrix<double>( 4, 4 ).makeIdentity();

        wmath::WMatrix<double> m_m_inv = m * m_inv;

        TS_ASSERT( m_m_inv == id );
    }

    /**
     * Two vectors are linear independent if the are not parallel
     */
    void testLinearIndependeceOfTwoVectors( void )
    {
        wmath::WVector3D u( 1, 0, 0 );
        wmath::WVector3D v( 0, 1, 0 );
        TS_ASSERT( wmath::linearIndependent( u, v ) );
        TS_ASSERT( wmath::linearIndependent( v, u ) );
        TS_ASSERT( !wmath::linearIndependent( v, v ) );
    }

    /**
     * Two vectors are linear independent if the are not parallel
     */
    void testLinearIndependeceOfTheNullVector( void )
    {
        wmath::WVector3D u( 0, 0, 0 );
        wmath::WVector3D v( 0, 0, 1 );
        TS_ASSERT( !wmath::linearIndependent( u, v ) );
        TS_ASSERT( !wmath::linearIndependent( v, u ) );
        TS_ASSERT( !wmath::linearIndependent( u, u ) );
    }

    /**
     * Small changes should nothing do to correctness
     */
    void testLinearIndependenceOnNumericalStability( void )
    {
        wmath::WVector3D u( wlimits::FLT_EPS, wlimits::FLT_EPS, wlimits::FLT_EPS );
        wmath::WVector3D v( wlimits::FLT_EPS, wlimits::FLT_EPS, 1 );
        TS_ASSERT( !wmath::linearIndependent( u, v ) );
        TS_ASSERT( !wmath::linearIndependent( v, u ) );
        TS_ASSERT( !wmath::linearIndependent( u, u ) );
        u[0] = wlimits::FLT_EPS * 2;
        TS_ASSERT( wmath::linearIndependent( u, v ) );
        TS_ASSERT( wmath::linearIndependent( v, u ) );
        TS_ASSERT( !wmath::linearIndependent( u, u ) );
    }
};

#endif  // WLINEARALGEBRAFUNCTIONS_TEST_H
