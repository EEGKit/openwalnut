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

#ifndef WGRIDREGULAR3D2_TEST_H
#define WGRIDREGULAR3D2_TEST_H

#include <cxxtest/TestSuite.h>

#include "../WGridRegular3D2.h"

/**
 * Test functionality of WGridRegular3D2.
 */
class WGridRegular3D2Test : public CxxTest::TestSuite
{
public:
    /**
     * Test the mapping from voxel indices to voxel coords.
     */
    void testVoxelIndexCalculation()
    {
        WGridRegular3D2 g( 3, 4, 5 );

        TS_ASSERT_EQUALS( 4, WIndexMap< WGridRegular3D2 >::getVoxelIndex( g, 1, 1, 0 ) );
        TS_ASSERT_EQUALS( 28, WIndexMap< WGridRegular3D2 >::getVoxelIndex( g, 1, 1, 2 ) );
        TS_ASSERT_EQUALS( g.numVoxels() - 1, WIndexMap< WGridRegular3D2 >::getVoxelIndex( g, 2, 3, 4 ) );

        boost::array< std::size_t, 3 > coords = WIndexMap< WGridRegular3D2 >::getVoxelCoords( g, 28 );
        TS_ASSERT_EQUALS( coords[ 0 ], 1 );
        TS_ASSERT_EQUALS( coords[ 1 ], 1 );
        TS_ASSERT_EQUALS( coords[ 2 ], 2 );
    }
};

#endif  // WGRIDREGULAR3D2_TEST_H
