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

#ifndef WGRIDTRANSFORMORTHO_TEST_H
#define WGRIDTRANSFORMORTHO_TEST_H

#include <cstdio>
#include <sstream>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <cxxtest/TestSuite.h>

#include "../common/exceptions/WPreconditionNotMet.h"

#include "../WGridTransformOrtho.h"

/**
 * Tests the WGridTransform class.
 */
class WGridTransformTest : public CxxTest::TestSuite
{
public:
    /**
     * Test if all data fields get initialized correctly. Constructors should throw
     * a WPreconditionNotMet exception if any input values are invalid.
     */
    void testInstantiation()
    {
        {
            TS_ASSERT_THROWS_NOTHING( WGridTransformOrtho v() );
            WGridTransformOrtho v;
            TS_ASSERT_EQUALS( v.getOffsetX(), 1.0 );
            TS_ASSERT_EQUALS( v.getOffsetY(), 1.0 );
            TS_ASSERT_EQUALS( v.getOffsetZ(), 1.0 );
            TS_ASSERT_EQUALS( v.getUnitDirectionX(), WVector3D( 1.0, 0.0, 0.0 ) );
            TS_ASSERT_EQUALS( v.getUnitDirectionY(), WVector3D( 0.0, 1.0, 0.0 ) );
            TS_ASSERT_EQUALS( v.getUnitDirectionZ(), WVector3D( 0.0, 0.0, 1.0 ) );
            TS_ASSERT_EQUALS( v.getDirectionX(), WVector3D( 1.0, 0.0, 0.0 ) );
            TS_ASSERT_EQUALS( v.getDirectionY(), WVector3D( 0.0, 1.0, 0.0 ) );
            TS_ASSERT_EQUALS( v.getDirectionZ(), WVector3D( 0.0, 0.0, 1.0 ) );
            TS_ASSERT_EQUALS( v.getOrigin(), WVector3D( 0.0, 0.0, 0.0 ) );
        }
        {
            TS_ASSERT_THROWS_NOTHING( WGridTransformOrtho v( 2.2, 3.3, -1.0 ) );
            TS_ASSERT_THROWS( WGridTransformOrtho v( 0.0, 0.0, 1.0 ), WPreconditionNotMet );
            TS_ASSERT_THROWS( WGridTransformOrtho v( 0.0, 2.0, 1.0 ), WPreconditionNotMet );
            TS_ASSERT_THROWS( WGridTransformOrtho v( 1.0, 1.0, 0.0 ), WPreconditionNotMet );
        }
        {
            WGridTransformOrtho v( 2.2, 3.3, -1.0 );
            TS_ASSERT_EQUALS( v.getOffsetX(), 2.2 );
            TS_ASSERT_EQUALS( v.getOffsetY(), 3.3 );
            TS_ASSERT_EQUALS( v.getOffsetZ(), 1.0 );
            TS_ASSERT_EQUALS( v.getUnitDirectionX(), WVector3D( 1.0, 0.0, 0.0 ) );
            TS_ASSERT_EQUALS( v.getUnitDirectionY(), WVector3D( 0.0, 1.0, 0.0 ) );
            TS_ASSERT_EQUALS( v.getUnitDirectionZ(), WVector3D( 0.0, 0.0, -1.0 ) );
            TS_ASSERT_EQUALS( v.getDirectionX(), WVector3D( 2.2, 0.0, 0.0 ) );
            TS_ASSERT_EQUALS( v.getDirectionY(), WVector3D( 0.0, 3.3, 0.0 ) );
            TS_ASSERT_EQUALS( v.getDirectionZ(), WVector3D( 0.0, 0.0, -1.0 ) );
            TS_ASSERT_EQUALS( v.getOrigin(), WVector3D( 0.0, 0.0, 0.0 ) );
        }
        {
            WMatrix< double > mat( 4, 4 );
            mat.makeIdentity();
            mat( 0, 0 ) = 2.2;
            mat( 1, 1 ) = 3.3;
            mat( 2, 2 ) = 0.0;

            TS_ASSERT_THROWS( WGridTransformOrtho v( mat ), WPreconditionNotMet );
        }
        {
            WMatrix< double > mat( 4, 4 );
            mat.makeIdentity();
            mat( 0, 0 ) = 2.2;
            mat( 1, 0 ) = 0.1;
            mat( 1, 1 ) = 3.3;
            mat( 2, 2 ) = 1.0;

            TS_ASSERT_THROWS( WGridTransformOrtho v( mat ), WPreconditionNotMet );
        }
        {
            WMatrix< double > mat( 4, 4 );
            mat.makeIdentity();
            mat( 0, 0 ) = 2.0;
            mat( 1, 0 ) = 2.0;
            mat( 1, 1 ) = 3.0;
            mat( 0, 1 ) = -3.0;
            mat( 2, 2 ) = 4.4;
            mat( 0, 3 ) = 1.0;
            mat( 1, 3 ) = 2.0;
            mat( 2, 3 ) = 0.5;

            WGridTransformOrtho v( mat );
            TS_ASSERT_EQUALS( v.getOffsetX(), sqrt( 8.0 ) );
            TS_ASSERT_EQUALS( v.getOffsetY(), sqrt( 18.0 ) );
            TS_ASSERT_EQUALS( v.getOffsetZ(), 4.4 );
            TS_ASSERT_DELTA( ( v.getUnitDirectionX() - WVector3D( 0.5 * sqrt( 2.0 ), 0.5 * sqrt( 2.0 ), 0.0 ) ).norm(), 0.0, 1e-13 );
            TS_ASSERT_DELTA( ( v.getUnitDirectionY() - WVector3D( -0.5 * sqrt( 2.0 ), 0.5 * sqrt( 2.0 ), 0.0 ) ).norm(), 0.0, 1e-13 );
            TS_ASSERT_EQUALS( v.getUnitDirectionZ(), WVector3D( 0.0, 0.0, 1.0 ) );
            TS_ASSERT_EQUALS( v.getDirectionX(), WVector3D( 2.0, 2.0, 0.0 ) );
            TS_ASSERT_EQUALS( v.getDirectionY(), WVector3D( -3.0, 3.0, 0.0 ) );
            TS_ASSERT_EQUALS( v.getDirectionZ(), WVector3D( 0.0, 0.0, 4.4 ) );
            TS_ASSERT_EQUALS( v.getOrigin(), WVector3D( 1.0, 2.0, 0.5 ) );
        }
    }

    /**
     * Different constructors should not yield differently initialized
     * data fields.
     */
    void testCompareConstructors()
    {
        WMatrix< double > mat( 4, 4 );
        mat.makeIdentity();
        mat( 0, 0 ) = 2.2;
        mat( 1, 1 ) = 3.3;
        mat( 2, 2 ) = 4.4;

        WGridTransformOrtho t1( mat );
        WGridTransformOrtho t2( 2.2, 3.3, 4.4 );

        TS_ASSERT_EQUALS( t1.getOffsetX(), t2.getOffsetX() );
        TS_ASSERT_EQUALS( t1.getOffsetY(), t2.getOffsetY() );
        TS_ASSERT_EQUALS( t1.getOffsetZ(), t2.getOffsetZ() );

        TS_ASSERT_EQUALS( t1.getDirectionX(), t2.getDirectionX() );
        TS_ASSERT_EQUALS( t1.getDirectionY(), t2.getDirectionY() );
        TS_ASSERT_EQUALS( t1.getDirectionZ(), t2.getDirectionZ() );

        TS_ASSERT_EQUALS( t1.getOrigin(), t2.getOrigin() );
    }

    /**
     * Test transformation from grid space to world space.
     */
    void testTransformationToWorldSpace()
    {
        {
            // test identity transform
            WVector3D v( -7.64, 8.73, -0.0063 );
            WGridTransformOrtho t;

            TS_ASSERT_EQUALS( v, t.positionToWorldSpace( v ) );
            TS_ASSERT_EQUALS( v, t.directionToWorldSpace( v ) );
        }

        {
            WGridTransformOrtho t( 2.2, 3.3, 4.4 );
            WVector3D v( 1.0, 1.0, 1.0 );

            TS_ASSERT_EQUALS( WVector3D( 2.2, 3.3, 4.4 ), t.positionToWorldSpace( v ) );
            TS_ASSERT_EQUALS( WVector3D( 2.2, 3.3, 4.4 ), t.directionToWorldSpace( v ) );
        }

        {
            WMatrix< double > mat( 4, 4 );
            mat.makeIdentity();
            mat( 0, 0 ) = 2.2;
            mat( 1, 1 ) = 3.3;
            mat( 2, 2 ) = 4.4;
            mat( 0, 3 ) = 1.0;
            mat( 1, 3 ) = 2.0;
            mat( 2, 3 ) = 0.5;

            WGridTransformOrtho t( mat );
            WVector3D v( 1.0, 1.0, 1.0 );

            TS_ASSERT_EQUALS( WVector3D( 3.2, 5.3, 4.9 ), t.positionToWorldSpace( v ) );
            TS_ASSERT_EQUALS( WVector3D( 2.2, 3.3, 4.4 ), t.directionToWorldSpace( v ) );
        }
        {
            WMatrix< double > mat( 4, 4 );
            mat.makeIdentity();
            mat( 0, 0 ) = 2.0;
            mat( 1, 0 ) = 2.0;
            mat( 1, 1 ) = 3.0;
            mat( 0, 1 ) = -3.0;
            mat( 2, 2 ) = 4.4;
            mat( 0, 3 ) = 1.0;
            mat( 1, 3 ) = 2.0;
            mat( 2, 3 ) = 0.5;

            WGridTransformOrtho t( mat );
            WVector3D v( 1.0, 1.0, 1.0 );

            WVector3D w = t.positionToWorldSpace( v );
            TS_ASSERT_DELTA( 0.0, w[ 0 ], 0.0001 );
            TS_ASSERT_DELTA( 7.0, w[ 1 ], 0.0001 );
            TS_ASSERT_DELTA( 4.9, w[ 2 ], 0.0001 );
            TS_ASSERT_EQUALS( WVector3D( -1.0, 5.0, 4.4 ), t.directionToWorldSpace( v ) );
        }
    }

    /**
     * Test transformation from world space to grid space.
     */
    void testTransformationToGridSpace()
    {
        {
            // test identity transform
            WVector3D v( -7.64, 8.73, -0.0063 );
            WGridTransformOrtho t;

            TS_ASSERT_EQUALS( v, t.positionToGridSpace( v ) );
            TS_ASSERT_EQUALS( v, t.directionToGridSpace( v ) );
        }

        {
            WGridTransformOrtho t( 2.2, 3.3, 4.4 );
            WVector3D v( 2.2, 3.3, 4.4 );

            TS_ASSERT_EQUALS( WVector3D( 1.0, 1.0, 1.0 ), t.positionToGridSpace( v ) );
            TS_ASSERT_EQUALS( WVector3D( 1.0, 1.0, 1.0 ), t.directionToGridSpace( v ) );
        }

        {
            WMatrix< double > mat( 4, 4 );
            mat.makeIdentity();
            mat( 0, 0 ) = 2.2;
            mat( 1, 1 ) = 3.3;
            mat( 2, 2 ) = 4.4;
            mat( 0, 3 ) = 1.0;
            mat( 1, 3 ) = 2.0;
            mat( 2, 3 ) = 0.5;

            WGridTransformOrtho t( mat );

            TS_ASSERT_EQUALS( WVector3D( 1.0, 1.0, 1.0 ), t.positionToGridSpace( WVector3D( 3.2, 5.3, 4.9 ) ) );
            TS_ASSERT_EQUALS( WVector3D( 1.0, 1.0, 1.0 ), t.directionToGridSpace( WVector3D( 2.2, 3.3, 4.4 ) ) );
        }
        {
            WMatrix< double > mat( 4, 4 );
            mat.makeIdentity();
            mat( 0, 0 ) = 2.0;
            mat( 1, 0 ) = 2.0;
            mat( 1, 1 ) = 3.0;
            mat( 0, 1 ) = -3.0;
            mat( 2, 2 ) = 4.4;
            mat( 0, 3 ) = 1.0;
            mat( 1, 3 ) = 2.0;
            mat( 2, 3 ) = 0.5;

            WGridTransformOrtho t( mat );

            TS_ASSERT_DELTA( ( WVector3D( 1.0, 1.0, 1.0 )  // NOLINT
                               - t.positionToGridSpace( WVector3D( 0.0, 7.0, 4.9 ) ) ).norm(), 0.0, 1e-13 );
            TS_ASSERT_DELTA( ( WVector3D( 1.0, 1.0, 1.0 )  // NOLINT
                               - t.directionToGridSpace( WVector3D( -1.0, 5.0, 4.4 ) ) ).norm(), 0.0, 1e-13 );
        }
    }
};

#endif  // WGRIDTRANSFORMORTHO_TEST_H
