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
#include "../linearAlgebra/WLinearAlgebra.h"
#include "../linearAlgebra/WLinearAlgebra.h"
#include "WVector3dTraits.h"

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
        WVector3d_2 v( 9, 10, 11 );
        WMatrix< double > m( 3, 3 );
        int i = 0;
        for( size_t r = 0; r < 3; ++r)
        {
            for( size_t c = 0; c < 3; ++c, ++i )
            {
                m( r, c ) = i;
            }
        }
        WVector3d_2 result = multMatrixWithVector3D( m, v );
        WVector3d_2 expected( 32, 122, 212 );
        TS_ASSERT_EQUALS( result, expected );
    }

    /**
     * If the matrix is not singular then an inverse should exist and be definite.
     */
    void test3x3MatrixInversion( void )
    {
        int i = 0;
        WMatrix< double > m( 3, 3 );
        for( size_t r = 0; r < 3; ++r)
        {
            for( size_t c = 0; c < 3; ++c, ++i )
            {
                m( r, c ) = i;
            }
        }
        m( 2, 1 ) = 8;
        WMatrix< double > expected( 3, 3 );
        expected( 0, 0 ) = 1./6 *  -8;
        expected( 0, 1 ) = 1./6 *   8;
        expected( 0, 2 ) = 1./6 *  -3;
        expected( 1, 0 ) = 1./6 *   6;
        expected( 1, 1 ) = 1./6 * -12;
        expected( 1, 2 ) = 1./6 *   6;
        expected( 2, 0 ) = 1./6 *   0;
        expected( 2, 1 ) = 1./6 *   6;
        expected( 2, 2 ) = 1./6 *  -3;
        TS_ASSERT_EQUALS( invertMatrix3x3( m ), expected );
    }

    /**
     * On singular matrices no inverse exists!
     */
    void test3x3MatrixInversionOnSingularMatrix( void )
    {
        int i = 0;
        WMatrix< double > m( 3, 3 );
        for( size_t r = 0; r < 3; ++r)
        {
            for( size_t c = 0; c < 3; ++c, ++i )
            {
                m( r, c ) = i;
            }
        }
        TS_ASSERT_THROWS( invertMatrix3x3( m ), WException& e );
        // ATTENTION we can't compare the messages from WAssert since there is now a path string in side which is different on any developers machine
        //           "Assertion failed: det != 0 (in file /home/lmath/repos/OpenWalnut/src/common/math/WLinearAlgebraFunctions.cpp at line 71),....
    }

    /**
     * Test the inversion of 4x4 matrices
     */
    void test4x4Inverse()
    {
        WMatrix<double> m( 4, 4 );

        for( size_t r = 0; r < 4; ++r)
        {
            for( size_t c = 0; c < 4; ++c )
            {
                m( c, r ) = r + c * 4 + 1;
            }
        }
        m( 2, 2 ) = 12;
        m( 3, 3 ) = 15;

        WMatrix<double> m_inv = invertMatrix4x4( m );

        WMatrix<double> id = WMatrix<double>( 4, 4 ).makeIdentity();

        WMatrix<double> m_m_inv = m * m_inv;

        TS_ASSERT( m_m_inv == id );
    }

    /**
     * Two vectors are linear independent if the are not parallel
     */
    void testLinearIndependeceOfTwoVectors( void )
    {
        WVector3d_2 u( 1, 0, 0 );
        WVector3d_2 v( 0, 1, 0 );
        TS_ASSERT( linearIndependent( u, v ) );
        TS_ASSERT( linearIndependent( v, u ) );
        TS_ASSERT( !linearIndependent( v, v ) );
    }

    /**
     * Two vectors are linear independent if the are not parallel
     */
    void testLinearIndependeceOfTheNullVector( void )
    {
        WVector3d_2 u( 0, 0, 0 );
        WVector3d_2 v( 0, 0, 1 );
        TS_ASSERT( !linearIndependent( u, v ) );
        TS_ASSERT( !linearIndependent( v, u ) );
        TS_ASSERT( !linearIndependent( u, u ) );
    }

    /**
     * Small changes should nothing do to correctness
     */
    void testLinearIndependenceOnNumericalStability( void )
    {
        WVector3d_2 u( wlimits::DBL_EPS, wlimits::DBL_EPS, wlimits::DBL_EPS );
        WVector3d_2 v( wlimits::DBL_EPS, wlimits::DBL_EPS, 1 );
        TS_ASSERT( !linearIndependent( u, v ) );
        TS_ASSERT( !linearIndependent( v, u ) );
        TS_ASSERT( !linearIndependent( u, u ) );
        u[0] = wlimits::DBL_EPS * 2;
        TS_ASSERT( linearIndependent( u, v ) );
        TS_ASSERT( linearIndependent( v, u ) );
        TS_ASSERT( !linearIndependent( u, u ) );
    }

    /**
     * Test SVD calculation
     */
    void testComputeSVD( void )
    {
            const size_t nbRows = 3, nbCols = 3;
            const double a = 1.2, b = 2.3, c = 3.4,
                         d = 4.5, e = 5.6, f = 6.7,
                         g = 3.4, h = 1.2, i = 7.0;
            WMatrix_2 A( nbRows, nbCols );
            A( 0, 0 ) = a;
            A( 0, 1 ) = b;
            A( 0, 2 ) = c;
            A( 1, 0 ) = d;
            A( 1, 1 ) = e;
            A( 1, 2 ) = f;
            A( 2, 0 ) = g;
            A( 2, 1 ) = h;
            A( 2, 2 ) = i;
            WMatrix_2 U( A.rows(), A.cols() );
            WMatrix_2 V( A.cols(), A.cols() );
            WVector_2 Svec( A.cols() );
            computeSVD( A, U, V, Svec );
            WMatrix_2 S( Svec.size(), Svec.size() );
            S.setZero();
            for ( int i = 0; i < Svec.size(); ++i )
            {
                S( i, i ) = Svec( i );
            }

            WMatrix_2 A2( U*S*V.transpose() );

            for ( int row = 0; row < A.rows(); ++row )
            {
                for ( int col = 0; col < A.cols(); ++col )
                {
                    TS_ASSERT_DELTA( A( row, col ), A2( row, col ), 0.0001 );
                }
            }
    }

    /**
     * Test pseudoInverse calculation
     */
    void testPseudoInverse( void )
    {
        {
            const size_t nbRows = 3, nbCols = 3;
            const double a = 1.2, b = 2.3, c = 3.4,
                         d = 4.5, e = 5.6, f = 6.7,
                         g = 3.4, h = 1.2, i = 7.0;
            WMatrix_2 A( nbRows, nbCols );

            A( 0, 0 ) = a;
            A( 0, 1 ) = b;
            A( 0, 2 ) = c;
            A( 1, 0 ) = d;
            A( 1, 1 ) = e;
            A( 1, 2 ) = f;
            A( 2, 0 ) = g;
            A( 2, 1 ) = h;
            A( 2, 2 ) = i;
            WMatrix_2 Ainvers( pseudoInverse( A ) );
            WMatrix_2 I( A*Ainvers );

            for ( size_t row = 0; row < I.rows(); row++ )
            {
                for ( size_t col = 0; col < I.cols(); col++ )
                {
                    if ( row == col )
                    {
                        TS_ASSERT_DELTA( I( row, col ), 1.0, 0.0001 );
                    }
                    else
                    {
                        TS_ASSERT_DELTA( I( row, col ), 0.0, 0.0001 );
                    }
                }
            }
        }
        {
            WMatrix_2 m( 6, 6 );
            for( int j = 0; j < 6; ++j )
            {
                for( int i = 0; i < 6; ++i )
                {
                    m( i, j ) = pow( i + 1, j );
                }
            }
            m = m * pseudoInverse( m );
            for( int j = 0; j < 6; ++j )
            {
                for( int i = 0; i < 6; ++i )
                {
                    TS_ASSERT_DELTA( m( i, j ), i == j ? 1.0 : 0.0, 0.0001 );
                }
            }
        }
    }
};

#endif  // WLINEARALGEBRAFUNCTIONS_TEST_H
