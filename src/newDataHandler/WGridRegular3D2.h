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

#include <utility> // for std::pair

#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>

// for tie( x, y ) = ... ; included here so it can be used where this grid is used
#include <boost/tuple/tuple.hpp>

// this is a simple regular grid

// TODO( reichenbach ):
// - more constructors
// - add transform
// - ....

// puts boost::tie in our namespace
using boost::tie;

/**
 * A regular 3D grid that maps from its axis-parallel grid space to the world space.
 * Best used with medical regular 3D image data as produced by MRI scanners.
 */
class WGridRegular3D2
{
public:

    //! A shared pointer to the const version of this grid type.
    typedef boost::shared_ptr< WGridRegular3D2 const > ConstSPtr;

    //! A shared pointer to this grid type.
    typedef boost::shared_ptr< WGridRegular3D2 > SPtr;

    //! The type used to index voxels in this grid.
    typedef boost::array< std::size_t, 3 > VoxelIndex;

    //! The type used for coordinates.
    typedef boost::array< double, 3 > CoordinateType;

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
    std::size_t numVoxels() const;

    /**
     * Returns the number of data points for this grid which equals the number of voxels.
     *
     * \return The number of data points in this grid.
     */
    // note: needed by several templates
    std::size_t size() const;

    /**
     * Returns the number of voxels in x-direction.
     *
     * \return The number of voxels in x-direction.
     */
    std::size_t getNbVoxelsX() const;

    /**
     * Returns the number of voxels in y-direction.
     *
     * \return The number of voxels in y-direction.
     */
    std::size_t getNbVoxelsY() const;

    /**
     * Returns the number of voxels in z-direction.
     *
     * \return The number of voxels in z-direction.
     */
    std::size_t getNbVoxelsZ() const;

    /**
     * Returns an array containing the number of voxels for each axis.
     *
     * \return An array containing the number of voxels in x-, y- and z-directions respectively.
     */
    boost::array< std::size_t, 3 > getNbVoxelsXYZ() const;

    /**
     * Return the number of voxels in a slice.
     *
     * \param dir The orientation of the slice ( 0: xy, 1: xz, 2: yz ).
     *
     * \return The number of voxel for a slice of the given orientation.
     */
    std::size_t sliceSize( std::size_t dir ) const;

    /**
     * Transforms coordinates from world space to grid space.
     *
     * \param coords The world coordinates to transform.
     * \return The transformed coords.
     */
    CoordinateType transformToGridSpace( CoordinateType const& coords ) const;

    /**
     * Transforms coordinates from grid space to world space.
     *
     * \param coords The grid coordinates to transform.
     * \return The transformed coords.
     */
    CoordinateType transformToWorldSpace( CoordinateType const& coords ) const;

    /**
     * Check if the given world coordinates are in the grid.
     *
     * \param coords The coordinates to test.
     *
     * \return true, if the coordinates were inside (or on the boundary of) the grid.
     */
    bool isInGrid( CoordinateType const& coords ) const;

private:

    //! The number of voxels or data points in x-direction.
    std::size_t m_coordsX;

    //! The number of voxels or data points in y-direction.
    std::size_t m_coordsY;

    //! The number of voxels or data points in z-direction.
    std::size_t m_coordsZ;
};

#endif  // WGRIDREGULAR3D2_H
