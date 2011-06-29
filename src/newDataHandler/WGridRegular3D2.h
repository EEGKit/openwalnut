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

#include "WIndexMap.h"
#include "WDataAccess.h"
#include "WVoxelIterator.h"

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

    //! The grid type.
    typedef WGridRegular3D2 GridType;

    //! The type of the values in the dataset
    typedef ValueT ValueType;

    //! The type of the index mapper.
    typedef WIndexMap< WGridRegular3D2 > IndexMapType;

    //! A type for voxel iterators.
    typedef WVoxelIterator< WGridRegular3D2, ValueT > VoxelIterator;


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

    /**
     * Get the data at a given position.
     *
     * \param vox The coods of the voxel in grid-space.
     *
     * \return A reference to the data.
     */
    ValueT& getAt( WGridRegular3D2::VoxelIndex const& vox );

    /**
     * Get a voxel iterator range. Use this to iterator all voxels in a dataset.
     *
     * \return Begin and end iterators in a std::pair.
     */
    std::pair< VoxelIterator, VoxelIterator > voxels()
    {
        return std::make_pair( VoxelIterator( m_grid.get(), m_valueSet.get(), 0 ),
                               VoxelIterator( m_grid.get(), m_valueSet.get(), m_grid->numVoxels() ) );
    }

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
ValueT& WDataAccess< WGridRegular3D2, ValueT >::getAt( WGridRegular3D2::VoxelIndex const& vox )
{
    // we use the index map to convert from grid space to valueset index
    std::size_t index = IndexMapType::getVoxelIndex( *m_grid.get(), vox );
    return m_valueSet->operator[] ( index );
}

template< typename ValueT >
ValueT& WDataAccess< WGridRegular3D2, ValueT >::getAt( std::size_t x, std::size_t y, std::size_t z )
{
    WGridRegular3D2::VoxelIndex vox = { { x, y, z } }; // NOLINT not a line for every brace
    // as above
    return getAt( vox );
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

    /**
     * Get the data at a given position.
     *
     * \param vox The coods of the voxel in grid-space.
     *
     * \return A reference to the data.
     */
    ValueT const& getAt( WGridRegular3D2::VoxelIndex const& vox ) const;

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
ValueT const& WDataAccessConst< WGridRegular3D2, ValueT >::getAt( WGridRegular3D2::VoxelIndex const& vox ) const
{
    std::size_t index = IndexMapType::getVoxelIndex( *m_grid.get(), vox );
    return m_valueSet->operator[] ( index );
}

template< typename ValueT >
ValueT const& WDataAccessConst< WGridRegular3D2, ValueT >::getAt( std::size_t x, std::size_t y, std::size_t z ) const
{
    WGridRegular3D2::VoxelIndex vox = { { x, y, z } }; // NOLINT not a line for every brace
    // as above
    return getAt( vox );
}

#endif  // WGRIDREGULAR3D2_H
