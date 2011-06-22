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

#ifndef WGRIDREGULAR3D2_H
#define WGRIDREGULAR3D2_H

#include <boost/array.hpp>

// this is a simple regular grid

// TODO( reichenbach ):
// - more constructors
// - add transform
// - ....

/**
 * A regular 3D grid that maps from its axis-parallel grid space to the world space.
 * Best used with medical regular 3D image data as produced by MRI scanners.
 */
class WGridRegular3D2
{
public:
    /**
     * Construct an axis-aligned grid from the number of voxels as well as data points per axis.
     *
     * \param nx The number of voxels on the x-axis.
     * \param ny The number of voxels on the y-axis.
     * \param nz The number of voxels on the z-axis.
     */
    WGridRegular3D2( std::size_t nx, std::size_t ny, std::size_t nz );

    /**
     * Standard destructor.
     */
    ~WGridRegular3D2();

    /**
     * Calculate the number of voxels for this grid. This equals the number of data points in this grid.
     *
     * \return The number of voxels in this grid.
     */
    std::size_t numVoxels();

    /**
     * Get the index of the voxel from some voxel coordinates.
     *
     * \param x The number of the voxel in x-direction.
     * \param y The number of the voxel in y-direction.
     * \param z The number of the voxel in z-direction.
     *
     * \return The index of the voxel.
     */
    std::size_t getVoxelIndex( std::size_t x, std::size_t y, std::size_t z );

    /**
     * Get the voxel coords from its index.
     *
     * \param index The voxel's index.
     *
     * \return An array containing the x, y and z coordinates of the voxel or an undefined result if index >= numVoxels().
     *
     * \note In debug mode, an exception gets thrown if index >= numVoxels().
     */
    boost::array< std::size_t, 3 > getVoxelCoords( std::size_t index );

    /**
     * Returns the number of voxels in x-direction.
     *
     * \return The number of voxels in x-direction.
     */
    std::size_t getNbVoxelsX();

    /**
     * Returns the number of voxels in y-direction.
     *
     * \return The number of voxels in y-direction.
     */
    std::size_t getNbVoxelsY();

    /**
     * Returns the number of voxels in z-direction.
     *
     * \return The number of voxels in z-direction.
     */
    std::size_t getNbVoxelsZ();

    /**
     * Returns an array containing the number of voxels for each axis.
     *
     * \return An array containing the number of voxels in x-, y- and z-directions respectively.
     */
    boost::array< std::size_t, 3 > getNbVoxelsXYZ();

private:

    //! The number of voxels or data points in x-direction.
    std::size_t m_coordsX;

    //! The number of voxels or data points in y-direction.
    std::size_t m_coordsY;

    //! The number of voxels or data points in z-direction.
    std::size_t m_coordsZ;
};

#endif  // WGRIDREGULAR3D2_H
