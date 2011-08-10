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

#ifndef WGRIDREGULAR3D2SPECIALIZATIONS_H
#define WGRIDREGULAR3D2SPECIALIZATIONS_H

#include <utility> // for std::pair

#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>

// for tie( x, y ) = ... ; included here so it can be used where this grid is used
#include <boost/tuple/tuple.hpp>

#include "WIndexMap.h"
#include "WDataAccess.h"
#include "WNeighborhood.h"

#include "WVoxelIterator.h"

#include "WSliceIterator.h"

// forward declare the grid
class WGridRegular3D2;

template< typename T >
class WSliceIterator;

template< typename T >
class WSliceIteratorConst;

template< typename T >
class WNeighborhoodIterator;

template< typename T >
class WNeighborhoodIteratorConst;

template< typename GridT, typename T >
class WVoxelIterator;

template< typename GridT, typename T >
class WVoxelIteratorConst;

// ########################################### WIndexMap for this grid #######################################################
// maybe move this to its own header

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

    //! The type of the values in the dataset.
    typedef ValueT ValueType;

    //! The type of the index mapper.
    typedef WIndexMap< WGridRegular3D2 > IndexMapType;

    //! A type for voxel iterators.
    typedef WVoxelIterator< WGridRegular3D2, ValueT > VoxelIterator;

    //! The type for slice iterators.
    typedef WSliceIterator< ValueT > SliceIterator;

    //! A type for voxel neighbor iterators.
    typedef WNeighborhoodIterator< ValueT > VoxelNeighborIterator;


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
     * Get a voxel iterator range. Use this to iterate all voxels in a dataset.
     *
     * \return Begin and end iterators in a std::pair.
     */
    std::pair< VoxelIterator, VoxelIterator > voxels()
    {
        return std::make_pair( VoxelIterator( m_grid.get(), m_valueSet.get(), 0 ),
                               VoxelIterator( m_grid.get(), m_valueSet.get(), m_grid->numVoxels() ) );
    }

    /**
     * Creates an iterator range for slice iteration.
     *
     * \param dir The orientation of the slice ( 0: xy, 1: xz, 2: yz ).
     * \param slice The index of the slice to iterate.
     *
     * \return A begin and an end iterator as a std::pair.
     */
    std::pair< SliceIterator, SliceIterator > slice( std::size_t dir, std::size_t slice ) const
    {
        return std::make_pair( SliceIterator( m_grid.get(), m_valueSet.get(), 0, dir, slice ),
                               SliceIterator( m_grid.get(), m_valueSet.get(), m_grid->sliceSize( dir ), dir, slice ) );
    }

private:

    /**
     * The grid instance.
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

    //! The grid type.
    typedef WGridRegular3D2 GridType;

    //! The type of the values in the dataset.
    typedef ValueT ValueType;

    //! The type of the index mapper.
    typedef WIndexMap< WGridRegular3D2 > IndexMapType;

    //! A type for voxel iterators.
    typedef WVoxelIteratorConst< WGridRegular3D2, ValueT > VoxelIterator;

    //! The type for the slice iterators.
    typedef WSliceIteratorConst< ValueT > SliceIterator;

    //! A type for voxel neighbor iterators.
    typedef WNeighborhoodIteratorConst< ValueT > VoxelNeighborIterator;

    /**
     * Constructs access object. Requires the valueset and grid.
     *
     * \param grid the grid needed to access the data
     * \param valueSet the values
     */
    WDataAccessConst( typename GridType::ConstSPtr grid, typename WValueSet< ValueType >::ConstSPtr valueSet ):
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

    /**
     * Get a voxel iterator range. Use this to iterate all voxels in a dataset.
     *
     * \return Begin and end iterators in a std::pair.
     */
    std::pair< VoxelIterator, VoxelIterator > voxels() const
    {
        return std::make_pair( VoxelIterator( m_grid.get(), m_valueSet.get(), 0 ),
                               VoxelIterator( m_grid.get(), m_valueSet.get(), m_grid->numVoxels() ) );
    }

    /**
     * Creates an iterator range for slice iteration.
     *
     * \param dir The orientation of the slice ( 0: xy, 1: xz, 2: yz ).
     * \param slice The index of the slice to iterate.
     *
     * \return A begin and an end iterator as a std::pair.
     */
    std::pair< SliceIterator, SliceIterator > slice( std::size_t dir, std::size_t slice ) const
    {
        return std::make_pair( SliceIterator( m_grid.get(), m_valueSet.get(), 0, dir, slice ),
                               SliceIterator( m_grid.get(), m_valueSet.get(), m_grid->sliceSize( dir ), dir, slice ) );
    }

private:

    /**
     * The grid instance.
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

#endif  // WGRIDREGULAR3D2SPECIALIZATIONS_H
