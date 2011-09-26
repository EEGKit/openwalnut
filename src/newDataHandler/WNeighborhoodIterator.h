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

#ifndef WNEIGHBORHOODITERATOR_H
#define WNEIGHBORHOODITERATOR_H

#include <algorithm>
#include <memory> // for auto_ptr
#include <string> // for std::size_t

//TEST
#include <iostream>

#include "WGridRegular3D2.h"
#include "WGridRegular3D2Specializations.h"

// clamping

/**
 * A boundary strategy that finds the nearest voxel in the grid and does not ignore any voxels
 * when used with non-const iterators.
 *
 * \tparam T The type of the data in the valueset.
 */
template< typename T >
class WBoundaryStrategyClamp
{
public:

    /**
     * Default constructor.
     */
    WBoundaryStrategyClamp()
    {
    }

    /**
     * Destructor.
     */
    ~WBoundaryStrategyClamp()
    {
    }

    /**
     * Checks if the voxel at the given position should be ignored. Always returns false.
     *
     * \return False.
     */
    bool ignoreVoxel( boost::array< int, 3 > const& /* currentPos */, WGridRegular3D2 const * const /* grid */ ) const
    {
        return false;
    }

    /**
     * Get a reference to the value of the current neighbor voxel or the nearest grid voxel, if the current neighbor is not
     * in the grid.
     *
     * \param currentPos The position of the current neighbor voxel.
     * \param grid The grid.
     * \param valueSet The corresponding valueset.
     *
     * \return A reference to the value.
     */
    T& getValue( boost::array< int, 3 > const& currentPos, WGridRegular3D2 const * const grid, WValueSet2< T >* valueSet )
    {
        boost::array< std::size_t, 3 > pos = { { std::max( std::min( currentPos[ 0 ], static_cast< int >( grid->getNbVoxelsX() - 1 ) ), 0 ),
                                                 std::max( std::min( currentPos[ 1 ], static_cast< int >( grid->getNbVoxelsY() - 1 ) ), 0 ),
                                                 std::max( std::min( currentPos[ 2 ], static_cast< int >( grid->getNbVoxelsZ() - 1 ) ), 0 ) } };  // NOLINT braces
        return valueSet->operator[]( WIndexMap< WGridRegular3D2 >::getVoxelIndex( *grid, pos ) );
    }

    /**
     * Get a const reference to the value of the current neighbor voxel or the nearest grid voxel, if the current neighbor is not
     * in the grid.
     *
     * \param currentPos The position of the current neighbor voxel.
     * \param grid The grid.
     * \param valueSet The corresponding valueset.
     *
     * \return A const reference to the value.
     */
    T const& getValue( boost::array< int, 3 > const& currentPos, WGridRegular3D2 const * const grid, WValueSet2< T > const* valueSet ) const
    {
        boost::array< std::size_t, 3 > pos = { { std::max( std::min( currentPos[ 0 ], static_cast< int >( grid->getNbVoxelsX() - 1 ) ), 0 ),
                                                 std::max( std::min( currentPos[ 1 ], static_cast< int >( grid->getNbVoxelsY() - 1 ) ), 0 ),
                                                 std::max( std::min( currentPos[ 2 ], static_cast< int >( grid->getNbVoxelsZ() - 1 ) ), 0 ) } };  // NOLINT braces
        return valueSet->operator[]( WIndexMap< WGridRegular3D2 >::getVoxelIndex( *grid, pos ) );
    }
};

// wrapping

/**
 * A boundary strategy that wraps around the image and does not ignore any voxels
 * when used with non-const iterators.
 *
 * \tparam T The type of the data in the valueset.
 */
template< typename T >
class WBoundaryStrategyWrap
{
public:

    /**
     * Default constructor.
     */
    WBoundaryStrategyWrap()
    {
    }

    /**
     * Destructor.
     */
    ~WBoundaryStrategyWrap()
    {
    }

    /**
     * Checks if the voxel at the given position should be ignored. Always returns false.
     *
     * \return False.
     */
    bool ignoreVoxel( boost::array< int, 3 > const& /* currentPos */, WGridRegular3D2 const * const /* grid */ ) const
    {
        return false;
    }

    /**
     * Get a reference to the value of the current neighbor voxel where voxels out of the grid get mapped
     * to inside voxels as if the image would just repeat itself in every direction.
     *
     * \param currentPos The position of the current neighbor voxel.
     * \param grid The grid.
     * \param valueSet The corresponding valueset.
     *
     * \return A reference to the value.
     */
    T& getValue( boost::array< int, 3 > const& currentPos, WGridRegular3D2 const * const grid, WValueSet2< T >* valueSet )
    {
        boost::array< std::size_t, 3 > pos =
        { {  // NOLINT braces
            moduloWrap( currentPos[ 0 ], static_cast< int >( grid->getNbVoxelsX() ) ),
            moduloWrap( currentPos[ 1 ], static_cast< int >( grid->getNbVoxelsY() ) ),
            moduloWrap( currentPos[ 2 ], static_cast< int >( grid->getNbVoxelsZ() ) )
        } };  // NOLINT braces

        return valueSet->operator[]( WIndexMap< WGridRegular3D2 >::getVoxelIndex( *grid, pos ) );
    }

    /**
     * Get a const reference to the value of the current neighbor voxel where voxels out of the grid get mapped
     * to inside voxels as if the image would just repeat itself in every direction.
     *
     * \param currentPos The position of the current neighbor voxel.
     * \param grid The grid.
     * \param valueSet The corresponding valueset.
     *
     * \return A const reference to the value.
     */
    T const& getValue( boost::array< int, 3 > const& currentPos, WGridRegular3D2 const * const grid, WValueSet2< T > const* valueSet ) const
    {
        boost::array< std::size_t, 3 > pos =
        { {  // NOLINT braces
            moduloWrap( currentPos[ 0 ], static_cast< int >( grid->getNbVoxelsX() ) ),
            moduloWrap( currentPos[ 1 ], static_cast< int >( grid->getNbVoxelsY() ) ),
            moduloWrap( currentPos[ 2 ], static_cast< int >( grid->getNbVoxelsZ() ) )
        } };  // NOLINT braces

        return valueSet->operator[]( WIndexMap< WGridRegular3D2 >::getVoxelIndex( *grid, pos ) );
    }

private:

    /**
     * Implements the wrapping for a single direction using the modulo operation in order to avoid loops.
     *
     * \param pos The position.
     * \param size The number of voxels in the current direction.
     *
     * \return The index of the voxel in the current direction.
     */
    std::size_t moduloWrap( int pos, int size ) const
    {
        if( pos < 0 )
        {
            // the last % is needed to map size to 0 in case pos was a multiple of -size
            return static_cast< std::size_t >( size + ( pos % size ) ) % size;
        }
        else
        {
            return static_cast< std::size_t >( pos % size );
        }
    }
};

/**
 * A boundary strategy that returns a default value for voxels outside the grid.
 *
 * \tparam T The type of the data in the valueset.
 */
template< typename T >
class WBoundaryStrategyDefault
{
public:

    /**
     * Constructor.
     *
     * \param def The default value.
     */
    WBoundaryStrategyDefault( T def = T() )
        : m_default( def )
    {
    }

    /**
     * Destructor.
     */
    ~WBoundaryStrategyDefault()
    {
    }

    /**
     * Checks if the voxel at the given position should be ignored which is the case if it is not inside the grid.
     *
     * \param currentPos The position of the current neighbor voxel.
     * \param grid The grid.
     *
     * \return True, if the voxel is not in the grid.
     */
    bool ignoreVoxel( boost::array< int, 3 > const& currentPos, WGridRegular3D2 const * const grid ) const
    {
        return !isInGrid( currentPos, grid );
    }

    /**
     * Get a reference to the value of the current neighbor voxel where voxels out of the grid return the default value.
     *
     * \param currentPos The position of the current neighbor voxel.
     * \param grid The grid.
     * \param valueSet The corresponding valueset.
     *
     * \return A reference to the value.
     */
    T& getValue( boost::array< int, 3 > const& currentPos, WGridRegular3D2 const * const grid, WValueSet2< T >* valueSet )
    {
        if( isInGrid( currentPos, grid ) )
        {
            boost::array< std::size_t, 3 > pos = { { currentPos[ 0 ], currentPos[ 1 ], currentPos[ 2 ] } };  // NOLINT braces
            return valueSet->operator[]( WIndexMap< WGridRegular3D2 >::getVoxelIndex( *grid, pos ) );
        }
        else
        {
            return m_default;
        }
    }

    /**
     * Get a const reference to the value of the current neighbor voxel where voxels out of the grid return the default value.
     *
     * \param currentPos The position of the current neighbor voxel.
     * \param grid The grid.
     * \param valueSet The corresponding valueset.
     *
     * \return A const reference to the value.
     */
    T const& getValue( boost::array< int, 3 > const& currentPos, WGridRegular3D2 const * const grid, WValueSet2< T > const* valueSet ) const
    {
        if( isInGrid( currentPos, grid ) )
        {
            boost::array< std::size_t, 3 > pos = { { currentPos[ 0 ], currentPos[ 1 ], currentPos[ 2 ] } };  // NOLINT braces
            return valueSet->operator[]( WIndexMap< WGridRegular3D2 >::getVoxelIndex( *grid, pos ) );
        }
        else
        {
            return m_default;
        }
    }

private:

    /**
     * Check if the current voxel is in the grid.
     *
     * \param currentPos The position of the current neighbor voxel.
     * \param grid The grid.
     *
     * \return True, if the voxel is inside the grid.
     */
    bool isInGrid( boost::array< int, 3 > const& currentPos, WGridRegular3D2 const * const grid ) const
    {
        return currentPos[ 0 ] >= 0 && currentPos[ 1 ] >= 0 && currentPos[ 2 ] >= 0
            && currentPos[ 0 ] < static_cast< int >( grid->getNbVoxelsX() )
            && currentPos[ 1 ] < static_cast< int >( grid->getNbVoxelsY() )
            && currentPos[ 2 ] < static_cast< int >( grid->getNbVoxelsZ() );
    }

    //! The default value to return for voxels outside of the grid.
    T m_default;
};

//########################################## WNeighborhoodIterator ##############################################

// forward declaration used for friend declarations
template< typename GridT, typename ValueT >
class WVoxelIterator;

// forward declaration used for friend declarations
template< typename GridT, typename ValueT >
class WVoxelIteratorConst;

// forward the test
class WNeighborhoodIteratorTest;

/**
 * A class for iterating neighbors of voxels of a dataset. For more important information
 * on iterators, see WVoxelIterator.
 *
 * \see WVoxelIterator
 *
 * A neighborhood and a boundary strategy must be provided.
 *
 * \param T The type of the data in the valueset.
 */
template< typename T, typename BoundaryStrategyT >
class WNeighborhoodIterator
{
public:

    //! WVoxelIterator is a friend.
    // voxel iterators create neighborhood iterators using a non-public constructor
    template< typename GridTT, typename ValueTT > friend class WVoxelIterator;

    // the const version is a friend
    template< typename TT, typename BoundaryStrategyTT > friend class WNeighborhoodIteratorConst;

    // the test is a friend
    friend class WNeighborhoodIteratorTest;

    /**
     * Default constructor, creates an invalid iterator.
     */
    WNeighborhoodIterator()
        : m_grid( NULL ),
          m_valueSet( NULL )
    {
    }

    /**
     * Default destructor.
     */
    ~WNeighborhoodIterator()
    {
    }

    // TODO( reichenbach ): copy constructor and operators from const nbh-iterators
    /**
     * Copy constructor.
     *
     * \param nhi The iterator to copy from.
     */
    WNeighborhoodIterator( WNeighborhoodIterator const& nhi )  // NOLINT not explicit
        : m_grid( nhi.m_grid ),
          m_valueSet( nhi.m_valueSet ),
          m_neighborhood( nhi.m_neighborhood ),
          m_boundaryStrategy( nhi.m_boundaryStrategy ),
          m_voxel( nhi.m_voxel ),
          m_currentPos( nhi.m_currentPos ),
          m_index( nhi.m_index )
    {
    }

    /**
     * Copy operator.
     *
     * \param nhi The iterator to copy from.
     *
     * \return *this.
     */
    WNeighborhoodIterator& operator=( WNeighborhoodIterator const nhi )
    {
        if( this == &nhi )
        {
            return *this;
        }
        m_grid = nhi.m_grid;
        m_valueSet = nhi.m_valueSet;
        m_neighborhood = nhi.m_neighborhood;
        m_boundaryStrategy = nhi.m_boundaryStrategy;
        m_voxel = nhi.m_voxel;
        m_currentPos = nhi.m_currentPos;
        m_index = nhi.m_index;
        return *this;
    }

    /**
     * Increment the iterator. Ingores voxels depending on the boundary strategy chosen.
     *
     * \return *this.
     */
    WNeighborhoodIterator& operator++()
    {
        do
        {
            ++m_index;
            calcCurrentVoxel();
        }
        while( m_boundaryStrategy->ignoreVoxel( m_currentPos, m_grid ) && m_index < m_neighborhood->size() );
        return *this;
    }

    /**
     * Decrement the iterator. Ingores voxels depending on the boundary strategy chosen.
     *
     * \return *this.
     */
    WNeighborhoodIterator& operator--()
    {
        do
        {
            --m_index;
            calcCurrentVoxel();
        }
        while( m_boundaryStrategy->ignoreVoxel( m_currentPos, m_grid ) && m_index > 0 );
        return *this;
    }

    /**
     * Increment the iterator. Ingores voxels depending on the boundary strategy chosen.
     *
     * \return The iterator before incrementing.
     */
    WNeighborhoodIterator operator++( int )
    {
        WNeighborhoodIterator it = *this;
        do
        {
            ++m_index;
            calcCurrentVoxel();
        }
        while( m_boundaryStrategy->ignoreVoxel( m_currentPos, m_grid ) && m_index < m_neighborhood->size() );
        return it;
    }

    /**
     * Decrement the iterator. Ingores voxels depending on the boundary strategy chosen.
     *
     * \return The iterator before decrementing.
     */
    WNeighborhoodIterator operator--( int )
    {
        WNeighborhoodIterator it = *this;
        do
        {
            --m_index;
            calcCurrentVoxel();
        }
        while( m_boundaryStrategy->ignoreVoxel( m_currentPos, m_grid ) && m_index > 0 );
        return it;
    }

    /**
     * Comparison operator.
     *
     * \param nb The iterator to compare to.
     *
     * \return True, if the two iterators are equal.
     */
    bool operator==( WNeighborhoodIterator const& nb ) const
    {
        // TODO( reichenbach ): better compare m_currentPos instead of the first two?
        return m_voxel == nb.m_voxel && m_index == nb.m_index && m_grid == nb.m_grid && m_valueSet == nb.m_valueSet;
    }

    /**
     * Comparison operator.
     *
     * \param nb The iterator to compare to.
     *
     * \return True, if the two iterators are equal.
     */
    bool operator==( WNeighborhoodIteratorConst< T, BoundaryStrategyT > const& nb ) const
    {
        // TODO( reichenbach ): better compare m_currentPos instead of the first two?
        return m_voxel == nb.m_voxel && m_index == nb.m_index && m_grid == nb.m_grid && m_valueSet == nb.m_valueSet;
    }

    /**
     * Comparison operator.
     *
     * \param nb The iterator to compare to.
     *
     * \return False, if the two iterators are equal.
     */
    bool operator!=( WNeighborhoodIterator const& nb ) const
    {
        return !this->operator==( nb );
    }

    /**
     * Comparison operator.
     *
     * \param nb The iterator to compare to.
     *
     * \return False, if the two iterators are equal.
     */
    bool operator!=( WNeighborhoodIteratorConst< T, BoundaryStrategyT > const& nb ) const
    {
        return !this->operator==( nb );
    }

    /**
     * Check if this iterator points to a valid grid and valueset.
     * Use it like you would use an operator bool ().
     *
     * \return A non-zero void pointer if this iterator points to a valid grid and valuset.
     */
    operator void const* () const
    {
        if( m_grid && m_valueSet )
        {
            return reinterpret_cast< void const* >( 1 );
        }
        else
        {
            return NULL;
        }
    }

    /**
     * Get a reference to the data of the voxel pointed to by this iterator.
     *
     * \return A reference to the data.
     */
    typename WValueSet2< T >::ValueReturnType operator* ()
    {
        return m_boundaryStrategy->getValue( this->operator()(), m_grid, m_valueSet );
    }

    /**
     * Get the coordinates of this voxel in grid space.
     *
     * \return The coords of the voxel pointed to by this iterator.
     */
    boost::array< int, 3 > operator() () const
    {
        return m_currentPos;
    }

    /**
     * Get the relative coordinates of this neighbor.
     *
     * \return The relative coords of the neighbor pointed to by this iterator.
     */
    boost::array< int, 3 > relativeCoordinates() const
    {
        return m_neighborhood->get( m_index );
    }

protected:

    /**
     * Constructs valid iterators.
     *
     * \param grid The grid of the dataset.
     * \param valueSet The valueset of the dataset.
     * \param nb A neighborhood to iterate.
     * \param strat A strategy for outside voxels.
     * \param voxel The coords of the voxel who's neighbors shall be iterated.
     * \param nbIndex The index of the initial neighbor in the provided neighborhood.
     */
    WNeighborhoodIterator( WGridRegular3D2 const* grid, WValueSet2< T >* valueSet, WNeighborhood* nb,
                           BoundaryStrategyT* strat, WGridRegular3D2::VoxelIndex const& voxel, std::size_t nbIndex )
        : m_grid( grid ),
          m_valueSet( valueSet ),
          m_neighborhood( nb ),
          m_boundaryStrategy( strat ),
          m_voxel( voxel ),
          m_index( nbIndex )
    {
        calcCurrentVoxel();
        while( m_boundaryStrategy->ignoreVoxel( m_currentPos, m_grid ) && m_index < m_neighborhood->size() )
        {
            ++m_index;
            calcCurrentVoxel();
        }
    }

    /**
     * Calculates the absolute coords (in grid space) of the current neighbor voxel.
     */
    void calcCurrentVoxel()
    {
        m_currentPos[ 0 ] = m_neighborhood->get( m_index )[ 0 ] + m_voxel[ 0 ];
        m_currentPos[ 1 ] = m_neighborhood->get( m_index )[ 1 ] + m_voxel[ 1 ];
        m_currentPos[ 2 ] = m_neighborhood->get( m_index )[ 2 ] + m_voxel[ 2 ];
    }

private:

    //! The grid to use.
    WGridRegular3D2 const* m_grid;

    //! The valueset.
    WValueSet2< T >* m_valueSet;

    //! Stores the neighbor's relative coords.
    WNeighborhood* m_neighborhood;

    //! The strategy to use.
    BoundaryStrategyT* m_boundaryStrategy;

    //! The coords of the voxel that created this neighborhood iterator.
    WGridRegular3D2::VoxelIndex m_voxel;

    //! The coords of the current neighbor.
    boost::array< int, 3 > m_currentPos;

    //! The index if the current neighbor in the neighborhood.
    std::size_t m_index;
};


//######################################### WNeighborhoodIteratorConst ##########################################

/**
 * A class for iterating neighbors of voxels of a dataset, const version. For more important information
 * on iterators, see WVoxelIterator.
 *
 * \see WVoxelIterator
 *
 * A neighborhood and a boundary strategy must be provided.
 *
 * \param T The type of the data in the valueset.
 */
template< typename T, typename BoundaryStrategyT >
class WNeighborhoodIteratorConst
{
public:

    //! WVoxelIteratorConst is a friend.
    template< typename GridTT, typename ValueTT > friend class WVoxelIteratorConst;

    // the const version is a friend
    template< typename TT, typename BoundaryStrategyTT > friend class WNeighborhoodIterator;

    // the test is a friend
    friend class WNeighborhoodIteratorTest;

    /**
     * Default constructor, creates an invalid iterator.
     */
    WNeighborhoodIteratorConst()
        : m_grid( NULL ),
          m_valueSet( NULL )
    {
    }

    /**
     * Default destructor.
     */
    ~WNeighborhoodIteratorConst()
    {
    }

    /**
     * Copy constructor.
     *
     * \param nhi The iterator to copy from.
     */
    WNeighborhoodIteratorConst( WNeighborhoodIteratorConst const& nhi )  // NOLINT not explicit
        : m_grid( nhi.m_grid ),
          m_valueSet( nhi.m_valueSet ),
          m_neighborhood( nhi.m_neighborhood ),
          m_boundaryStrategy( nhi.m_boundaryStrategy ),
          m_voxel( nhi.m_voxel ),
          m_currentPos( nhi.m_currentPos ),
          m_index( nhi.m_index )
    {
    }

    /**
     * Copy constructor.
     *
     * \param nhi The iterator to copy from.
     */
    WNeighborhoodIteratorConst( WNeighborhoodIterator< T,BoundaryStrategyT > const& nhi )  // NOLINT not explicit
        : m_grid( nhi.m_grid ),
          m_valueSet( nhi.m_valueSet ),
          m_neighborhood( nhi.m_neighborhood ),
          m_boundaryStrategy( nhi.m_boundaryStrategy ),
          m_voxel( nhi.m_voxel ),
          m_currentPos( nhi.m_currentPos ),
          m_index( nhi.m_index )
    {
    }

    /**
     * Copy operator.
     *
     * \param nhi The iterator to copy from.
     *
     * \return *this.
     */
    WNeighborhoodIteratorConst& operator=( WNeighborhoodIteratorConst const& nhi )
    {
        if( this == &nhi )
        {
            return *this;
        }
        m_grid = nhi.m_grid;
        m_valueSet = nhi.m_valueSet;
        m_neighborhood = nhi.m_neighborhood;
        m_boundaryStrategy = nhi.m_boundaryStrategy;
        m_voxel = nhi.m_voxel;
        m_currentPos = nhi.m_currentPos;
        m_index = nhi.m_index;
        return *this;
    }

    /**
     * Copy operator.
     *
     * \param nhi The iterator to copy from.
     *
     * \return *this.
     */
    WNeighborhoodIteratorConst& operator=( WNeighborhoodIterator< T, BoundaryStrategyT > const& nhi )
    {
        m_grid = nhi.m_grid;
        m_valueSet = nhi.m_valueSet;
        m_neighborhood = nhi.m_neighborhood;
        m_boundaryStrategy = nhi.m_boundaryStrategy;
        m_voxel = nhi.m_voxel;
        m_currentPos = nhi.m_currentPos;
        m_index = nhi.m_index;
        return *this;
    }

    /**
     * Increment the iterator.
     *
     * \return *this.
     */
    WNeighborhoodIteratorConst& operator++()
    {
        if( m_index < m_neighborhood->size() )
        {
            ++m_index;
            calcCurrentVoxel();
        }
        return *this;
    }

    /**
     * Decrement the iterator.
     *
     * \return *this.
     */
    WNeighborhoodIteratorConst& operator--()
    {
        if( m_index > 0 )
        {
            --m_index;
            calcCurrentVoxel();
        }
        return *this;
    }

    /**
     * Increment the iterator.
     *
     * \return The iterator before incrementing.
     */
    WNeighborhoodIteratorConst operator++( int )
    {
        WNeighborhoodIteratorConst it = *this;
        if( m_index < m_neighborhood->size() )
        {
            ++m_index;
            calcCurrentVoxel();
        }
        return it;
    }

    /**
     * Decrement the iterator.
     *
     * \return The iterator before decrementing.
     */
    WNeighborhoodIteratorConst operator--( int )
    {
        WNeighborhoodIteratorConst it = *this;
        if( m_index > 0 )
        {
            --m_index;
            calcCurrentVoxel();
        }
        return it;
    }

    /**
     * Comparison operator.
     *
     * \param nb The iterator to compare to.
     *
     * \return True, if the two iterators are equal.
     */
    bool operator==( WNeighborhoodIteratorConst const& nb ) const
    {
        return m_voxel == nb.m_voxel && m_index == nb.m_index && m_grid == nb.m_grid && m_valueSet == nb.m_valueSet;
    }

    /**
     * Comparison operator.
     *
     * \param nb The iterator to compare to.
     *
     * \return True, if the two iterators are equal.
     */
    bool operator==( WNeighborhoodIterator< T, BoundaryStrategyT > const& nb ) const
    {
        return m_voxel == nb.m_voxel && m_index == nb.m_index && m_grid == nb.m_grid && m_valueSet == nb.m_valueSet;
    }

    /**
     * Comparison operator.
     *
     * \param nb The iterator to compare to.
     *
     * \return False, if the two iterators are equal.
     */
    bool operator!=( WNeighborhoodIteratorConst const& nb ) const
    {
        return !this->operator==( nb );
    }

    /**
     * Comparison operator.
     *
     * \param nb The iterator to compare to.
     *
     * \return False, if the two iterators are equal.
     */
    bool operator!=( WNeighborhoodIterator< T, BoundaryStrategyT > const& nb ) const
    {
        return !this->operator==( nb );
    }

    /**
     * Check if this iterator points to a valid grid and valueset.
     * Use it like you would use an operator bool ().
     *
     * \return A non-zero void pointer if this iterator points to a valid grid and valuset.
     */
    operator void const* () const
    {
        if( m_grid && m_valueSet )
        {
            return reinterpret_cast< void const* >( 1 );
        }
        else
        {
            return NULL;
        }
    }

    /**
     * Get a const reference to the data if the voxel pointed to by this iterator.
     *
     * \return A const reference to the data.
     */
    typename WValueSet2< T >::ValueReturnTypeConst operator* () const
    {
        return m_boundaryStrategy->getValue( this->operator()(), m_grid, m_valueSet );
    }

    /**
     * Get the coordinates of this voxel in grid space.
     *
     * \return The coords of the voxel pointed to by this iterator.
     */
    boost::array< int, 3 > operator() () const
    {
        return m_currentPos;
    }

    /**
     * Get the relative coordinates of this neighbor.
     *
     * \return The relative coords of the neighbor pointed to by this iterator.
     */
    boost::array< int, 3 > relativeCoordinates() const
    {
        return m_neighborhood->get( m_index );
    }

protected:

    /**
     * Constructs valid iterators.
     *
     * \param grid The grid of the dataset.
     * \param valueSet The valueset of the dataset.
     * \param nb A neighborhood to iterate.
     * \param strat A strategy for outside voxels.
     * \param voxel The coords of the voxel who's neighbors shall be iterated.
     * \param nbIndex The index of the initial neighbor in the provided neighborhood.
     */
    WNeighborhoodIteratorConst( WGridRegular3D2 const* grid, WValueSet2< T > const* valueSet, WNeighborhood* nb,
                                BoundaryStrategyT* strat, WGridRegular3D2::VoxelIndex const& voxel, std::size_t nbIndex )
        : m_grid( grid ),
          m_valueSet( valueSet ),
          m_neighborhood( nb ),
          m_boundaryStrategy( strat ),
          m_voxel( voxel ),
          m_index( nbIndex )
    {
        if( m_index >= m_neighborhood->size() )
        {
            m_index = m_neighborhood->size();
        }
        calcCurrentVoxel();
    }

    /**
     * Calculates the absolute coords (in grid space) of the current neighbor voxel.
     */
    void calcCurrentVoxel()
    {
        m_currentPos[ 0 ] = m_neighborhood->get( m_index )[ 0 ] + m_voxel[ 0 ];
        m_currentPos[ 1 ] = m_neighborhood->get( m_index )[ 1 ] + m_voxel[ 1 ];
        m_currentPos[ 2 ] = m_neighborhood->get( m_index )[ 2 ] + m_voxel[ 2 ];
    }

private:

    //! The grid to use.
    WGridRegular3D2 const* m_grid;

    //! The valueset.
    WValueSet2< T > const* m_valueSet;

    //! Stores the neighbor's relative coords.
    WNeighborhood* m_neighborhood;

    //! The strategy to use.
    BoundaryStrategyT* m_boundaryStrategy;

    //! The coords of the voxel that created this neighborhood iterator.
    WGridRegular3D2::VoxelIndex m_voxel;

    //! The coords of the current neighbor.
    boost::array< int, 3 > m_currentPos;

    //! The index if the current neighbor in the neighborhood.
    std::size_t m_index;
};

#endif  // WNEIGHBORHOODITERATOR_H
