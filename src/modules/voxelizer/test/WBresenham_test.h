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

#ifndef WBRESENHAM_TEST_H
#define WBRESENHAM_TEST_H

#include <vector>

#include <cxxtest/TestSuite.h>

#include "../../../common/WLogger.h"
#include "../WBresenham.h"

static WLogger logger;

/**
 * Unit tests the Bresenham algorithm.
 */
class WBresenhamTest : public CxxTest::TestSuite
{
public:
    /**
     * Creates a member variable with a WBresenham instance which you may use
     * for testing.
     */
    void setUp( void )
    {
        boost::shared_ptr< WGridRegular3D > grid;
        grid = boost::shared_ptr< WGridRegular3D >( new WGridRegular3D( 3, 3, 3, 0, 0, 0, 1, 1, 1 ) );
        m_algo = boost::shared_ptr< WBresenham >( new WBresenham( grid, false ) );
    }

    void tearDown( void )
    {
        m_algo.reset();
    }

    /**
     * If a line segments starts and ends on the same point only its voxel
     * should be selected.
     */
    void testLineSegementWithSameStartAndEndPoint( void )
    {
        wmath::WLine l;
        l.push_back( wmath::WPosition( 0.5, 0.5, 0.5 ) );
        l.push_back( wmath::WPosition( 0.5, 0.5, 0.5 ) );
        m_algo->raster( l );
        std::vector< double > expected( 27, 0.0 );
        expected[13] = 1.0;
        TS_ASSERT_EQUALS( m_algo->m_values, expected );
    }

    /**
     * Multiple segments in one voxel should also mark only this voxel.
     */
    void testPolySegmentOneVoxelRastering( void )
    {
        wmath::WLine l;
        l.push_back( wmath::WPosition( 0.5, 0.5, 0.5 ) );
        l.push_back( wmath::WPosition( 0.6, 0.6, 0.6 ) );
        l.push_back( wmath::WPosition( 0.7, 0.7, 0.7 ) );
        m_algo->raster( l );
        std::vector< double > expected( 27, 0.0 );
        expected[13] = 1.0;
        TS_ASSERT_EQUALS( m_algo->m_values, expected );
    }

    /**
     * Lines like WFibers consisting out of multiple line segements should
     * be traced segment by segment.
     */
    void testPolyLineRastering( void )
    {
        wmath::WLine l;
        l.push_back( wmath::WPosition( 0.4, 0.4, 0.4 ) );
        l.push_back( wmath::WPosition( 0.6, 0.6, 0.6 ) );
        l.push_back( wmath::WPosition( 1.7, 1.7, 1.7 ) );
        m_algo->raster( l );
        std::vector< double > expected( 27, 0.0 );
        expected[0] = 1.0;
        expected[13] = 1.0;
        expected[26] = 1.0;
        TS_ASSERT_EQUALS( m_algo->m_values, expected );
    }

    /**
     * Lines starting in a voxel A and ending in voxel B are rastered exactly
     * the same way as all lines starting in A and ending in B as well as
     * the line starting in the center of A and ending in the center of B.
     */
    void testExactLineIsRasteredTheSameWayAsMidpointLines( void )
    {
        wmath::WLine l;
        l.push_back( wmath::WPosition( 0.49, 0.0, 0.0 ) );
        l.push_back( wmath::WPosition( 1.49, 1.99, 0.0 ) );
        m_algo->raster( l );
        std::vector< double > expected( 27, 0.0 );
        expected[0] = 1.0;
        expected[3] = 1.0; // voxel three, since the midpoint may choose between 3 and 4
        expected[7] = 1.0;
        TS_ASSERT_EQUALS( m_algo->m_values, expected );

        // reset the algo
        m_algo->m_values[0] = 0.0;
        m_algo->m_values[3] = 0.0;
        m_algo->m_values[7] = 0.0;

        wmath::WLine k;
        k.push_back( wmath::WPosition( 0.0, 0.0, 0.0 ) );
        k.push_back( wmath::WPosition( 1.0, 2.0, 0.0 ) );
        m_algo->raster( k );
        TS_ASSERT_EQUALS( m_algo->m_values, expected );
    }
private:
    boost::shared_ptr< WBresenham > m_algo;
};

#endif  // WBRESENHAM_TEST_H
