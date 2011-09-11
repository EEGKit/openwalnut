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

#include "WGridRegular3D2.h"

#include "WIndexMap.h"

std::size_t WIndexMap< WGridRegular3D2 >::getVoxelIndex( WGridRegular3D2 const& grid, std::size_t x, std::size_t y, std::size_t z )
{
    return x + y * grid.getNbVoxelsX() + z * grid.getNbVoxelsX() * grid.getNbVoxelsY();
}

std::size_t WIndexMap< WGridRegular3D2 >::getVoxelIndex( WGridRegular3D2 const& grid, WGridRegular3D2::VoxelIndex const& idx )
{
    return idx[ 0 ] + idx[ 1 ] * grid.getNbVoxelsX() + idx[ 2 ] * grid.getNbVoxelsX() * grid.getNbVoxelsY();
}

WGridRegular3D2::VoxelIndex WIndexMap< WGridRegular3D2 >::getVoxelCoords( WGridRegular3D2 const& grid, std::size_t index )
{
#ifdef _DEBUG
    WAssert( index < grid.numVoxels(), "The index was too large for this grid." );
#endif

    WGridRegular3D2::VoxelIndex res;

    std::size_t xy = grid.getNbVoxelsX() * grid.getNbVoxelsY();

    res[ 2 ] = index / xy;
    res[ 1 ] = index % xy;
    res[ 0 ] = res[ 1 ] % grid.getNbVoxelsX();
    res[ 1 ] = res[ 1 ] / grid.getNbVoxelsX();

    return res;
}
