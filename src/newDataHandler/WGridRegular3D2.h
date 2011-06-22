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
#include <boost/shared_ptr.hpp>

#include "WIndexMap.h"
#include "WDataAccess.h"

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

    //! A shared pointer to the const version of this grid type.
    typedef boost::shared_ptr< WGridRegular3D2 const > ConstSPtr;

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

private:

    //! The number of voxels or data points in x-direction.
    std::size_t m_coordsX;

    //! The number of voxels or data points in y-direction.
    std::size_t m_coordsY;

    //! The number of voxels or data points in z-direction.
    std::size_t m_coordsZ;
};

// ########################################### WIndexMap for this grid #######################################################
// maybe move this to its own header

/**
 * WIndexMap specialization for WGridRegular3D2.
 */
template<>
class WIndexMap< WGridRegular3D2 >
{
public:

    /**
     * Get the index of the voxel from some voxel coordinates.
     *
     * \param x The number of the voxel in x-direction.
     * \param y The number of the voxel in y-direction.
     * \param z The number of the voxel in z-direction.
     *
     * \return The index of the voxel.
     */
    static std::size_t getVoxelIndex( WGridRegular3D2 const& grid, std::size_t x, std::size_t y, std::size_t z );
    // the voxel coords in the function above may need to be changed to a single parameter

    // note: we may need this to calculate positions for iterators
    /**
     * Get the voxel coords from its index.
     *
     * \param index The voxel's index.
     *
     * \return An array containing the x, y and z coordinates of the voxel or an undefined result if index >= numVoxels().
     *
     * \note In debug mode, an exception gets thrown if index >= numVoxels().
     */
    static boost::array< std::size_t, 3 > getVoxelCoords( WGridRegular3D2 const& grid, std::size_t index );
};

std::size_t WIndexMap< WGridRegular3D2 >::getVoxelIndex( WGridRegular3D2 const& grid, std::size_t x, std::size_t y, std::size_t z )
{
    return x + y * grid.getNbVoxelsX() + z * grid.getNbVoxelsX() * grid.getNbVoxelsY();
}

boost::array< std::size_t, 3 > WIndexMap< WGridRegular3D2 >::getVoxelCoords( WGridRegular3D2 const& grid, std::size_t index )
{
#ifdef _DEBUG
    WAssert( index < grid.numVoxels(), "The index was too large for this grid." );
#endif

    boost::array< std::size_t, 3 > res;

    std::size_t xy = grid.getNbVoxelsX() * grid.getNbVoxelsY();

    res[ 2 ] = index / xy;
    res[ 1 ] = index % xy;
    res[ 0 ] = res[ 1 ] / grid.getNbVoxelsX();
    res[ 1 ] = res[ 1 ] % grid.getNbVoxelsX();

    return res;
}

// ########################################### WDataAccess for this grid #######################################################

/**
 * WDataAccess specialization for WGridRegular3D2.
 */
template< typename ValueT >
class WDataAccess< WGridRegular3D2, ValueT >
{
public:

    /**
     * The grid type.
     */
    typedef WGridRegular3D2 GridType;

    /**
     * The type of the values in the dataset
     */
    typedef ValueT ValueType;

    /**
     * The type of the index mapper.
     */
    typedef WIndexMap< WGridRegular3D2 > IndexMapType;

    /**
     * Constructs access object. Requires the valueset and grid.
     *
     * \param grid the grid needed to access the data
     * \param valueSet the values
     */
    WDataAccess( typename GridType::ConstSPtr grid, typename WValueSet< ValueType >::SPtr valueSet ):
        m_grid( grid ),
        m_valueSet( valueSet )
    {
    }

    /**
     * Get the data at a given position.
     *
     * \param x The number of the voxel in x-direction.
     * \param y The number of the voxel in y-direction.
     * \param z The number of the voxel in z-direction.
     *
     * \return A reference to the data.
     */
    ValueT& getAt( std::size_t x, std::size_t y, std::size_t z );

private:

    /**
     * The grid instance
     */
    typename GridType::ConstSPtr m_grid;

    /**
     * The valueset.
     */
    typename WValueSet< ValueType >::SPtr m_valueSet;
};

template< typename ValueT >
ValueT& WDataAccess< WGridRegular3D2, ValueT >::getAt( std::size_t x, std::size_t y, std::size_t z )
{
    std::size_t index = IndexMapType::getVoxelIndex( *m_grid.get(), x, y, z );
    return m_valueSet->operator[] ( index );
}

// ########################################### WDataAccessConst for this grid #######################################################

/**
 * WDataAccessConst specialization for WGridRegular3D2.
 */
template< typename ValueT >
class WDataAccessConst< WGridRegular3D2, ValueT >
{
public:

    /**
     * The grid type.
     */
    typedef WGridRegular3D2 GridType;

    /**
     * The type of the values in the dataset
     */
    typedef ValueT ValueType;

    /**
     * The type of the index mapper.
     */
    typedef WIndexMap< WGridRegular3D2 > IndexMapType;

    /**
     * Constructs access object. Requires the valueset and grid.
     *
     * \param grid the grid needed to access the data
     * \param valueSet the values
     */
    WDataAccessConst( typename GridType::ConstSPtr grid, typename WValueSet< ValueType >::SPtr valueSet ):
        m_grid( grid ),
        m_valueSet( valueSet )
    {
    }

    /**
     * Get the data at a given position.
     *
     * \param x The number of the voxel in x-direction.
     * \param y The number of the voxel in y-direction.
     * \param z The number of the voxel in z-direction.
     *
     * \return A reference to the data.
     */
    ValueT const& getAt( std::size_t x, std::size_t y, std::size_t z ) const;

private:

    /**
     * The grid instance
     */
    typename GridType::ConstSPtr m_grid;

    /**
     * The valueset.
     */
    typename WValueSet< ValueType >::ConstSPtr m_valueSet;
};

template< typename ValueT >
ValueT const& WDataAccessConst< WGridRegular3D2, ValueT >::getAt( std::size_t x, std::size_t y, std::size_t z ) const
{
    std::size_t index = IndexMapType::getVoxelIndex( *m_grid.get(), x, y, z );
    return m_valueSet->operator[] ( index );
}

#endif  // WGRIDREGULAR3D2_H
