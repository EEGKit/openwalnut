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

#ifndef WMATLAB_TEST_H
#define WMATLAB_TEST_H

#include <cxxtest/TestSuite.h>

#include "../WMatlab.h"

/**
 * Unit tests for the matlab wrapper.
 */
class WMatlabTest : public CxxTest::TestSuite
{
public:
    /**
     * 
     */
    void testFoundMatlab( void )
    {
        TS_ASSERT_THROWS_NOTHING( WMatlab() );
    }

    /**
     * 
     */
    void testInAndOut( void )
    {
        WMatlab matlab;
        WMatrix<double> input( 3, 4 );
        for ( size_t i = 0; i < input.getNbRows(); ++i )
        {
            for ( size_t j = 0; j < input.getNbCols(); ++j )
            {
                input( i, j ) = static_cast<double>( i * 10 + j );
            }
        }
        matlab.putVariable( "A",  input );
        WMatrix<double> output( matlab.getVariable( "A" ) );
        TS_ASSERT_EQUALS( input, output );
    }

    /**
     * 
     */
    void testFunctionEvaluation( void )
    {
        WMatlab matlab;
        WMatrix<double> A( 3, 4 );
        WMatrix<double> B( 3, 4 );
        for ( size_t i = 0; i < A.getNbRows(); ++i )
        {
            for ( size_t j = 0; j < A.getNbCols(); ++j )
            {
                A( i, j ) = static_cast<double>( i * 10 + j );
                B( i, j ) = static_cast<double>( j * 10 + i );
            }
        }

        matlab.putVariable( "A", A );
        matlab.putVariable( "B", B );

        matlab.evalString( "C=A+B" );

        WMatrix<double> C( matlab.getVariable( "C" ) );
        TS_ASSERT_EQUALS( A+B, C );
    }
};

#endif  // WMATLAB_TEST_H
