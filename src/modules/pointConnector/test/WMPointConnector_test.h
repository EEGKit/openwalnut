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

#ifndef WMPOINTCONNECTOR_TEST_H
#define WMPOINTCONNECTOR_TEST_H

#include <cxxtest/TestSuite.h>

#include "../WMPointConnector.h"


/**
 * Test functionality of WMPointConnector class
 */
class WMPointConnectorTest : public CxxTest::TestSuite
{
public:
    /**
     * Setup logger and other stuff for each test.
     */
    void setUp()
    {
        WLogger::startup();
    }

    /**
     * Instatiation should throw nothing.
     */
    void testInstantiation()
    {
        TS_ASSERT_THROWS_NOTHING( WMPointConnector() );
    }

    /**
     * Test if ray trace algorithm hits vertex at least twice
     */
    void testHitVertex_Hit()
    {
        osg::Vec3 rayStart( 0.0, 0.0, 0.0 );
        osg::Vec3 rayDir( 0.0, 0.0, 1.0 );
        osg::Vec3 targetVertex( 0.0, 0.0, 40.4 );
        float targetVertexRadius = 1.0;

        TS_ASSERT_LESS_THAN( 0.0,
                             WMPointConnector().hitVertex( rayStart, rayDir, targetVertex, targetVertexRadius )
                           );
    }

    /**
     * Test if ray trace algorithm hits vertex at one point
     */
    void testHitVertex_Tangent()
    {
        osg::Vec3 rayStart( 0.0, 0.0, 0.0 );
        osg::Vec3 rayDir( 0.0, 0.0, 1.0 );
        osg::Vec3 targetVertex( 1.0, 0.0, 2.0 );
        float targetVertexRadius = 1.0;

        TS_ASSERT_EQUALS( 0.0,
                          WMPointConnector().hitVertex( rayStart, rayDir, targetVertex, targetVertexRadius )
        );
    }

    /**
     * Test if ray trace algorithm does not hit vertex in front of starting position
     */
    void testHitVertex_MissAhead()
    {
        osg::Vec3 rayStart( 0.0, 0.0, 0.0 );
        osg::Vec3 rayDir( 0.0, 0.0, 1.0 );
        osg::Vec3 targetVertex( 3.0, 2.0, 12.0 );
        float targetVertexRadius = 1.0;

        TS_ASSERT_LESS_THAN( WMPointConnector().hitVertex( rayStart, rayDir, targetVertex, targetVertexRadius ),
                             0.0
        );
    }

    /**
     * Test if ray trace algorithm does not hit vertex behind starting position
     */
    void testHitVertex_MissBehind()
    {
        osg::Vec3 rayStart( 0.0, 0.0, 0.0 );
        osg::Vec3 rayDir( 0.0, 0.0, 1.0 );
        osg::Vec3 targetVertex( 3.0, 2.0, -12.0 );
        float targetVertexRadius = 1.0;

        TS_ASSERT_LESS_THAN( WMPointConnector().hitVertex( rayStart, rayDir, targetVertex, targetVertexRadius ),
                             0.0
        );
    }
};

#endif  // WMPOINTCONNECTOR_TEST_H
