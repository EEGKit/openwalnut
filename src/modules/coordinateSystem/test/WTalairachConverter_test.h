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

#ifndef WTALAIRACHCONVERTER_TEST_H
#define WTALAIRACHCONVERTER_TEST_H

#include <iostream>

#include <cxxtest/TestSuite.h>

#include <boost/shared_ptr.hpp>

#include "../WTalairachConverter.h"

/**
 * tests the talailrach converter class
 */
class WTalairachConverterTest : public CxxTest::TestSuite
{
friend class WTalairachConverter;

public:
    /**
     * TODO(schurade): Document this!
     */
    void setUp( void )
    {
        wmath::WVector3D ac( 150, 122, 90 );
        wmath::WVector3D pc( 110, 124, 97 );
        wmath::WVector3D ihp( 140, 119, 188 );
        m_tc = boost::shared_ptr<WTalairachConverter> ( new WTalairachConverter( ac, pc, ihp ) );

        m_tc->setAp( wmath::WVector3D( 80, 0, 0 ) );
        m_tc->setPp( wmath::WVector3D( -97, 0, 0 ) );
        m_tc->setSp( wmath::WVector3D( 0, 0, 81 ) );
        m_tc->setIp( wmath::WVector3D( 0, 0, -41 ) );
        m_tc->setRp( wmath::WVector3D( 0, -60, 0 ) );
        m_tc->setLp( wmath::WVector3D( 0, 63, 0 ) );
    }

    /**
     * TODO(schurade): Document this!
     */
    void tearDown( void )
    {
        m_tc.reset();
    }


    /**
     * TODO(schurade): Document this!
     */
    void testCanonical2ACPC( void )
    {
        wmath::WVector3D testPoint( 200, 60, 130 );
        wmath::WVector3D result = m_tc->Canonical2ACPC( testPoint );

        TS_ASSERT_DELTA( result[0], 45.35, 0.05 );
        TS_ASSERT_DELTA( result[1], -57.6, 0.05 );
        TS_ASSERT_DELTA( result[2], 50.69, 0.05 );
    }

    /**
     * TODO(schurade): Document this!
     */
    void testACPC2Canonical( void )
    {
        wmath::WVector3D testPoint( 45.35, -57.6, 50.69 );
        wmath::WVector3D result = m_tc->ACPC2Canonical( testPoint );

        TS_ASSERT_DELTA( result[0], 200, 0.05 );
        TS_ASSERT_DELTA( result[1], 60, 0.05 );
        TS_ASSERT_DELTA( result[2], 130, 0.05 );
    }

    /**
     * TODO(schurade): Document this!
     */
    void testACPC2Talairach( void )
    {
        wmath::WVector3D testPoint( 12, 43, -12 );
        wmath::WVector3D result = m_tc->ACPC2Talairach( testPoint );

        TS_ASSERT_DELTA( result[0], 5.0, 0.05 );
        TS_ASSERT_DELTA( result[1], 15.85, 0.05 );
        TS_ASSERT_DELTA( result[2], -2.15, 0.05 );
    }

    /**
     * TODO(schurade): Document this!
     */
    void testTalairach2ACPC( void )
    {
        wmath::WVector3D testPoint( 10.5, 46.41, -12.29 );
        wmath::WVector3D result = m_tc->Talairach2ACPC( testPoint );

        TS_ASSERT_DELTA( result[0], 25., 0.05 );
        TS_ASSERT_DELTA( result[1], 126., 0.05 );
        TS_ASSERT_DELTA( result[2], -68.35, 0.05 );
    }


private:
    /**
     * TODO(schurade): Document this!
     */
    boost::shared_ptr<WTalairachConverter> m_tc;
};

#endif  // WTALAIRACHCONVERTER_TEST_H
