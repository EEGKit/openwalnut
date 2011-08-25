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

#include "WInterpolationTypes.h"
#include "WInterpolator.h"
#include "WTypeInterpolatorStandard.h"
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

    //! The standard interpolator.
    typedef WInterpolator< WInterpolationLinear< WGridRegular3D2 >, WTypeInterpolatorStandard< ValueT > > StandardInterpolator;

    /**
     * Constructs access object. Requires the valueset and grid.
     *
     * \param grid the grid needed to access the data
     * \param valueSet the values
     */
    WDataAccess( typename GridType::ConstSPtr grid, typename WValueSet2< ValueType >::SPtr valueSet ):
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
     * Creates a standard (tri-)linear interpolator.
     *
     * \return The interpolator object.
     */
    StandardInterpolator createStandardInterpolator() const
    {
        return StandardInterpolator( m_grid, m_valueSet );
    }

    /**
     * Creates a user-defined interpolator object. You can specify via template parameters
     * the interpolation method/precicion (for example nearest neighbor, linear, quadratic)
     * and how the weights and values should be combined.
     *
     * \tparam InterpolationPrecisionT A template specialized for each grid type that implements the method of interpolation.
     * \tparam TypeInterpolatorT A template specialized for the data type that implements calculating the resulting interpolated value.
     *
     * \return The interpolator object.
     */
    template< template< typename > class InterpolationPrecisionT,
              template< typename > class TypeInterpolatorT >
    WInterpolator< InterpolationPrecisionT< WGridRegular3D2 >, TypeInterpolatorT< ValueT > > createInterpolator() const
    {
        return WInterpolator< InterpolationPrecisionT< WGridRegular3D2 >, TypeInterpolatorT< ValueT > >( m_grid, m_valueSet );
    }

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
    typename WValueSet2< ValueType >::SPtr m_valueSet;
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

    //! The standard interpolator.
    typedef WInterpolator< WInterpolationLinear< WGridRegular3D2 >, WTypeInterpolatorStandard< ValueT > > StandardInterpolator;

    /**
     * Constructs access object. Requires the valueset and grid.
     *
     * \param grid the grid needed to access the data
     * \param valueSet the values
     */
    WDataAccessConst( typename GridType::ConstSPtr grid, typename WValueSet2< ValueType >::ConstSPtr valueSet ):
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
     * Creates a standard (tri-)linear interpolator.
     *
     * \return The interpolator object.
     */
    StandardInterpolator createStandardInterpolator() const
    {
        return StandardInterpolator( m_grid, m_valueSet );
    }

    /**
     * Creates a user-defined interpolator object. You can specify via template parameters
     * the interpolation method/precicion (for example nearest neighbor, linear, quadratic)
     * and how the weights and values should be combined.
     *
     * \tparam InterpolationPrecisionT A template specialized for each grid type that implements the method of interpolation.
     * \tparam TypeInterpolatorT A template specialized for the data type that implements calculating the resulting interpolated value.
     *
     * \return The interpolator object.
     */
    template< template< typename > class InterpolationPrecisionT,
              template< typename > class TypeInterpolatorT >
    WInterpolator< InterpolationPrecisionT< WGridRegular3D2 >, TypeInterpolatorT< ValueT > > createInterpolator() const
    {
        return WInterpolator< InterpolationPrecisionT< WGridRegular3D2 >, TypeInterpolatorT< ValueT > >( m_grid, m_valueSet );
        // I'd really like to provide a convenient template typedef for the interpolator (return) type, but we'd need
        // std-c++0x for that.
    }

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
    typename WValueSet2< ValueType >::ConstSPtr m_valueSet;
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

// ########################################### linear interpolation for this grid ###################################################

/**
 * Defines linear interpolation on a regular 3D-grid. Calculates weights and sample values.
 */
template<>
class WInterpolationLinear< WGridRegular3D2 >
{
public:

    //! The type of the grid.
    typedef WGridRegular3D2 GridType;

    //! The type of the coordinates used.
    typedef WGridRegular3D2::CoordinateType CoordinateType;

    //! The number of values and weights needed for the interpolation.
    static int const numValues = 8;
    // this needs to be specified in your own interpolation classes

    /**
     * Calculates the weights and samples the grid. These can then be used to calculate
     * the interpolated value.
     *
     * \tparam ValueSetPtrT A pointer to a valueset.
     * \tparam WeightsArrayT The type for the weights array.
     * \tparam ValueArrayT The type for the value array.
     *
     * \param grid The grid to interpolate on.
     * \param valueSet The valueset.
     * \param coords The coordinates of the value that is to be interpolated.
     * \param success A bool flag that will indicate success.
     * \param weights An array of weights for the interpolation.
     * \param values An array of values.
     */
    template< typename ValueSetPtrT, typename WeightsArrayT, typename ValuePtrArrayT >
    inline static void getWeightsAndValues( WGridRegular3D2::ConstSPtr const& grid, ValueSetPtrT const& valueSet,
                                            CoordinateType const& coords, bool* success,
                                            WeightsArrayT& weights, ValuePtrArrayT& values )
    {
        *success = false;
        if( !grid->isInGrid( coords ) )
        {
            return;
        }

        CoordinateType c = grid->transformToGridSpace( coords );
        boost::array< double, 3 > a = { { c[ 0 ] - static_cast< int >( c[ 0 ] ),
                                          c[ 1 ] - static_cast< int >( c[ 1 ] ),
                                          c[ 2 ] - static_cast< int >( c[ 2 ] ) } };  // NOLINT

        weights[ 0 ] = ( 1.0 - a[ 0 ] ) * ( 1.0 - a[ 1 ] ) * ( 1.0 - a[ 2 ] );
        weights[ 1 ] = a[ 0 ] * ( 1.0 - a[ 1 ] ) * ( 1.0 - a[ 2 ] );
        weights[ 2 ] = a[ 0 ] * a[ 1 ] * ( 1.0 - a[ 2 ] );
        weights[ 3 ] = a[ 0 ] * a[ 1 ] * a[ 2 ];
        weights[ 4 ] = a[ 0 ] * ( 1.0 - a[ 1 ] ) * a[ 2 ];
        weights[ 5 ] = ( 1.0 - a[ 0 ] ) * ( 1.0 - a[ 1 ] ) * a[ 2 ];
        weights[ 6 ] = ( 1.0 - a[ 0 ] ) * a[ 1 ] * a[ 2 ];
        weights[ 7 ] = ( 1.0 - a[ 0 ] ) * a[ 1 ] * ( 1.0 - a[ 2 ] );

        std::size_t index = static_cast< std::size_t >( c[ 0 ] ) + static_cast< std::size_t >( c[ 1 ] ) * grid->getNbVoxelsX()
                          + static_cast< std::size_t >( c[ 2 ] ) * grid->getNbVoxelsX() * grid->getNbVoxelsY();

        values.set( 0, valueSet->operator[] ( index ) );
        index += 1;
        values.set( 1, valueSet->operator[] ( index ) );
        index += grid->getNbVoxelsX();
        values.set( 2, valueSet->operator[] ( index ) );
        index += grid->getNbVoxelsX() * grid->getNbVoxelsY();
        values.set( 3, valueSet->operator[] ( index ) );
        index -= grid->getNbVoxelsX();
        values.set( 4, valueSet->operator[] ( index ) );
        index -= 1;
        values.set( 5, valueSet->operator[] ( index ) );
        index += grid->getNbVoxelsX();
        values.set( 6, valueSet->operator[] ( index ) );
        index -= grid->getNbVoxelsX() * grid->getNbVoxelsY();
        values.set( 7, valueSet->operator[] ( index ) );

        *success = true;
    }
};

#endif  // WGRIDREGULAR3D2SPECIALIZATIONS_H
