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

#ifndef WNEIGHBORHOOD_H
#define WNEIGHBORHOOD_H

#include <vector>

#include <boost/array.hpp>

//############################################## WNeighborhood ##################################################

/**
 * Stores the offsets of neighbor voxels in a regular 3D-grid.
 */
class WNeighborhood
{
public:

    /**
     * Default constructor.
     */
    WNeighborhood();

    /**
     * Default destructor.
     */
    ~WNeighborhood();

    /**
     * Add a new voxel to the neighborhood. The provided coordinates should be relative coordinates.
     *
     * \param offX The offset of the neighbor in x-direction.
     * \param offY The offset of the neighbor in y-direction.
     * \param offZ The offset of the neighbor in z-direction.
     */
    void add( int offX, int offY, int offZ );

    /**
     * Returns the coordinates of a neighbor indexed by the parameter num.
     * Make sure num is smaller then the number of neighbors in this neighborhood, as returned by size().
     *
     * \param num The index of the neighbor.
     *
     * \return The relative coords.
     */
    boost::array< int, 3 > get( std::size_t num ) const;

    /**
     * Returns the number of neighbors in this neighborhood.
     *
     * \return The number of neighbors.
     */
    std::size_t size() const;

private:

    //! The offsets.
    std::vector< int > m_offsets;
};

/**
 * Create a moore-neighborhood of a given size, which contains all voxels with offsets such that
 *
 * max( |x|, |y|, |z| ) <= distance (coords relative to the initial voxel).
 *
 * However, this implementation does not include (0,0,0).
 *
 * \param distance The maximum distance from the initial voxel in any direction.
 *
 * \return A moore-neighborhood of the given size.
 */
WNeighborhood makeMoore( int distance );

/**
 * Create a von-Neumann-neighborhood of a given size, which contains all voxels with offsets such that
 *
 * |x| + |y| + |z| <= distance (coords relative to the initial voxel).
 *
 * However, this implementation does not include (0,0,0).
 *
 * \param distance The maximum manhattan distance from the initial voxel.
 *
 * \return A von-Neumann-neighborhood of the given size.
 */
WNeighborhood makeVonNeumann( int distance );

#endif  // WNEIGHBORHOOD_H
