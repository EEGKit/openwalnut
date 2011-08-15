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

#ifndef WINDEXMAP_H
#define WINDEXMAP_H

// TODO( reichenbach ): maybe add a base class?

#include <string> // for size_t

#include "WGridRegular3D2.h"

class WGridRegular3D2;

/**
 * This class defines functions that convert from grid indices or coordinates to data indices and
 * needs to be specialized for every grid type.
 */ 
template< typename GridT >
class WIndexMap
{
};

/**
 * WIndexMap specialization for WGridRegular3D2.
 */
// see WIndexMap decl for complete doc
template<>
class WIndexMap< WGridRegular3D2 >
{
public:

    /**
     * Get the index of the voxel from some voxel coordinates.
     *
     * \param grid The grid.
     * \param idx The coords of the voxel.
     *
     * \return The index of the voxel.
     */
    static std::size_t getVoxelIndex( WGridRegular3D2 const& grid, WGridRegular3D2::VoxelIndex const& idx );

    /**
     * Get the index of the voxel from some voxel coordinates.
     *
     * \param grid The grid.
     * \param x The number of the voxel in x-direction.
     * \param y The number of the voxel in y-direction.
     * \param z The number of the voxel in z-direction.
     *
     * \return The index of the voxel.
     */
    static std::size_t getVoxelIndex( WGridRegular3D2 const& grid, std::size_t x, std::size_t y, std::size_t z );

    // note: we may need this to calculate positions for iterators
    /**
     * Get the voxel coords from its index.
     *
     * \param grid The grid.
     * \param index The voxel's index.
     *
     * \return An array containing the x, y and z coordinates of the voxel or an undefined result if index >= numVoxels().
     *
     * \note In debug mode, an exception gets thrown if index >= numVoxels().
     */
    static WGridRegular3D2::VoxelIndex getVoxelCoords( WGridRegular3D2 const& grid, std::size_t index );

    // note that the valueset index calculation from voxel indices does not have to be injective (though it is in this case)
};

#endif  // WINDEXMAP_H
