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

#ifndef WSLICEITERATOR_H
#define WSLICEITERATOR_H

#include <string> // for std::size_t
#include <utility>

#include "../common/WException.h"
#include "WGridRegular3D2.h"
#include "WValueSet2.h"
#include "WIndexMap.h"

static std::size_t const SLICE_XY = 0;
static std::size_t const SLICE_XZ = 1;
static std::size_t const SLICE_YZ = 2;

// ################################ forward declarations #######################################

// forward declaration
template< typename T >
class WSliceIteratorConst;

/**
 * Implements iteration of xy-, xz- or yz-slices of a regular dataset.
 */
template< typename T >
class WSliceIterator
{
public:

    // the test is a friend
    friend class WSliceIteratorTest;

    // the const version is also a friend
    friend class WSliceIteratorConst< T >;

    // The data access is a friend so it can create iterators using the non-default constructor.
    template< typename GridTT, typename ValueT >
    friend class WDataAccess;

    /**
     * Default constructor, creates an invalid iterator.
     */
    WSliceIterator()
        : m_grid( NULL ),
          m_valueSet( NULL )
    {
    }

    /**
     * Destructor.
     */
    ~WSliceIterator()
    {
    }

    /**
     * Copy construction.
     *
     * \param si The iterator to copy from.
     */
    WSliceIterator( WSliceIterator const& si )  // NOLINT not explicit
        : m_grid( si.m_grid ),
          m_valueSet( si.m_valueSet ),
          m_index( si.m_index ),
          m_incr1( si.m_incr1 ),
          m_incr2( si.m_incr2 ),
          m_d( si.m_d ),
          m_count( si.m_count )
    {
    }

    /**
     * Increment an iterator. If the iterator is valid, it will point to the next element
     * after beeing incremented (or it will be an end iterator if it was the last voxel).
     *
     * Incrementing an end iterator or an invalid iterator yields undefined behavior.
     *
     * \return *this.
     */
    WSliceIterator operator++()
    {
        m_index += m_incr1;
        ++m_count;
        m_index += m_incr2 * ( m_count / m_d );
        m_count = m_count % m_d;
        return *this;
    }

    /**
     * Decrement an iterator. If the iterator is valid, it will point to the previous element
     * after beeing decremented (or it will be an  iterator to the last voxel if it was the end iterator).
     *
     * Decrementing a begin iterator or an invalid iterator yields undefined behavior.
     *
     * \return *this.
     */
    WSliceIterator operator--()
    {
        m_index -= m_incr1;
        --m_count;
        m_index -= m_incr2 * ( m_count / m_d );
        m_count = m_count % m_d;
        return *this;
    }

    /**
     * Increment an iterator. If the iterator is valid, it will point to the next element
     * after beeing incremented (or it will be an end iterator if it was the last voxel).
     *
     * Incrementing an end iterator or an invalid iterator yields undefined behavior.
     *
     * \return The iterator before incrementing.
     */
    WSliceIterator operator++( int )
    {
        WSliceIterator vi = *this;
        m_index += m_incr1;
        ++m_count;
        m_index += m_incr2 * ( m_count / m_d );
        m_count = m_count % m_d;
        return vi;
    }

    /**
     * Decrement an iterator. If the iterator is valid, it will point to the previous element
     * after beeing decremented (or it will be an  iterator to the last voxel if it was the end iterator).
     *
     * Decrementing a begin iterator or an invalid iterator yields undefined behavior.
     *
     * \return The iterator before decrementing.
     */
    WSliceIterator operator--( int )
    {
        WSliceIterator vi = *this;
        m_index -= m_incr1;
        --m_count;
        m_index -= m_incr2 * ( m_count / m_d );
        m_count = m_count % m_d;
        return vi;
    }

    /**
     * Accesses the data for the voxel the iterator is currently pointing to. In debug mode, the iterator will be tested and
     * access via invalid or end iterators will yield a WException. When build for release, there is no such test!
     *
     * \return A reference to the data for the voxel the iterator currently points to.
     */
    // TODO( reichenbach ): add a WInvalidIteratorException and a WNoValidVoxel(name?)Exception
    typename ValueTypeTraits< T >::ReturnType operator*() // const?
    {
#ifdef DEBUG
        if( !m_grid || !m_valueSet )
        {
            throw WException( "Trying element access with invalid iterator." );
        }
        if( m_index >= m_grid->numVoxels() )
        {
            throw WException( "Trying element access with end iterator." );
        }
#endif
        return m_valueSet->operator[]( m_index );
    }

    /**
     * Get the voxel coordinates of the voxel the iterator points to.
     *
     * \return The voxel's coords in the grid.
     */
    typename WGridRegular3D2::VoxelIndex operator() () const
    {
        return WIndexMap< WGridRegular3D2 >::getVoxelCoords( *m_grid, m_index );
    }

    /**
     * Compares iterators. Comparing two invalid iterators yields undefined behavior.
     *
     * \param si The iterator to compare to.
     *
     * \return true, if the iterators point to the same voxel of the same dataset.
     */
    bool operator==( WSliceIterator const& si ) const
    {
        return m_grid == si.m_grid && m_valueSet == si.m_valueSet && m_index == si.m_index;
    }

    /**
     * Compares iterators. Comparing two invalid iterators yields undefined behavior.
     *
     * \param si The iterator to compare to.
     *
     * \return true, if the iterators do not point to the same voxel of the same dataset.
     */
    bool operator!=( WSliceIterator const& si ) const
    {
        return m_grid != si.m_grid || m_valueSet != si.m_valueSet || m_index != si.m_index;
    }

    /**
     * Compares iterators. Comparing two invalid iterators yields undefined behavior.
     *
     * \param si The iterator to compare to.
     *
     * \return true, if the iterators point to the same voxel of the same dataset.
     */
    bool operator==( WSliceIteratorConst< T > const& si ) const
    {
        return m_grid == si.m_grid && m_valueSet == si.m_valueSet && m_index == si.m_index;
    }

    /**
     * Compares iterators. Comparing two invalid iterators yields undefined behavior.
     *
     * \param si The iterator to compare to.
     *
     * \return true, if the iterators do not point to the same voxel of the same dataset.
     */
    bool operator!=( WSliceIteratorConst<  T > const& si ) const
    {
        return m_grid != si.m_grid || m_valueSet != si.m_valueSet || m_index != si.m_index;
    }

    /**
     * Returns a void const* that can be used to test if an iterator is valid, just as you
     * would using an operator bool().
     *
     * \return NULL if the iterator is invalid, something else if it is not.
     */
    operator void const*() const
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

protected:

    /**
     * Constructor that can only be used by the data access object. Used to create iterators pointing to
     * a dataset. This is the only way to construct valid iterators from scratch. (You can still copy iterators.)
     *
     * \param grid The grid of the dataset pointed to by this iterator.
     * \param valueSet The respective valueset.
     * \param idx The relative index of the voxel in the slice that is currently pointed to.
     * \param dir The orientation of the slice ( 0: xy, 1: xz, 2: yz ).
     * \param slice The index of the slice to iterate.
     */
    WSliceIterator( WGridRegular3D2 const* grid, typename ValueTypeTraits< T >::ValueSetType* valueSet, std::size_t idx,
                    std::size_t dir, std::size_t slice )
        : m_grid( grid ),
          m_valueSet( valueSet )
    {
        switch( dir )
        {
        case 0: // XY
            m_index = m_grid->getNbVoxelsX() * m_grid->getNbVoxelsY() * slice
                    + ( idx % m_grid->getNbVoxelsX() ) + ( idx / m_grid->getNbVoxelsX() ) * m_grid->getNbVoxelsX();
            m_d = m_grid->getNbVoxelsX();
            m_incr1 = 1;
            m_incr2 = 0;
            break;
        case 1: // XZ
            m_index = m_grid->getNbVoxelsX() * slice + ( idx % m_grid->getNbVoxelsX() )
                    + ( idx / m_grid->getNbVoxelsX() ) * m_grid->getNbVoxelsX() * m_grid->getNbVoxelsY();
            m_d = m_grid->getNbVoxelsX();
            m_incr1 = 1;
            m_incr2 = m_grid->getNbVoxelsX() * m_grid->getNbVoxelsY() - m_grid->getNbVoxelsX();
            break;
        case 2: // YZ
            m_index = slice + ( idx % m_grid->getNbVoxelsY() ) * m_grid->getNbVoxelsX()
                    + ( idx / m_grid->getNbVoxelsY() ) * m_grid->getNbVoxelsX() * m_grid->getNbVoxelsY();
            m_d = m_grid->getNbVoxelsY();
            m_incr1 = m_grid->getNbVoxelsX();
            m_incr2 = 0;
            break;
        }
        m_count = idx % m_d;
    }

private:

    //! The grid.
    WGridRegular3D2 const* m_grid;

    //! The values.
    typename ValueTypeTraits< T >::ValueSetType* m_valueSet; // the valueset is not const for the non-const iterator

    //! The current voxel index for the whole grid (not only the current slice).
    std::size_t m_index;

    //! The increment of the index per iterator increment.
    std::size_t m_incr1;

    //! The increment of the index every m_d increments of the iterator.
    std::size_t m_incr2;

    //! The number of iterator increments done since we added m_incr2.
    std::size_t m_d;

    //! Counts increments and decrements.
    int m_count;
};

/**
 * Implements iteration of xy-, xz- or yz-slices of a regular const dataset.
 */
template< typename T >
class WSliceIteratorConst
{
public:

    // the test is a friend
    friend class WSliceIteratorTest;

    // the const version is also a friend
    friend class WSliceIterator< T >;

    // The data access is a friend so it can create iterators using the non-default constructor.
    template< typename GridTT, typename ValueT >
    friend class WDataAccessConst;

    /**
     * Default constructor, creates an invalid iterator.
     */
    WSliceIteratorConst()
        : m_grid( NULL ),
          m_valueSet( NULL )
    {
    }

    /**
     * Destructor.
     */
    ~WSliceIteratorConst()
    {
    }

    /**
     * Copy construction.
     *
     * \param si The iterator to copy from.
     */
    WSliceIteratorConst( WSliceIteratorConst const& si )  // NOLINT not explicit
        : m_grid( si.m_grid ),
          m_valueSet( si.m_valueSet ),
          m_index( si.m_index ),
          m_incr1( si.m_incr1 ),
          m_incr2( si.m_incr2 ),
          m_d( si.m_d ),
          m_count( si.m_count )
    {
    }

    /**
     * Copy construction.
     *
     * \param si The iterator to copy from.
     */
    WSliceIteratorConst( WSliceIterator< T > const& si )  // NOLINT not explicit
        : m_grid( si.m_grid ),
          m_valueSet( si.m_valueSet ),
          m_index( si.m_index ),
          m_incr1( si.m_incr1 ),
          m_incr2( si.m_incr2 ),
          m_d( si.m_d ),
          m_count( si.m_count )
    {
    }

    /**
     * Assignment operator for assigning from non-const iterators.
     *
     * \param si The iterator to copy.
     *
     * \return *this.
     */
    WSliceIteratorConst& operator= ( WSliceIterator< T > const& si )
    {
        m_grid = si.m_grid;
        m_valueSet = si.m_valueSet;
        m_index = si.m_index;
        m_incr1 = si.m_incr1;
        m_incr2 = si.m_incr2;
        m_d = si.m_d;
        m_count = si.m_count;
        return *this;
    }

    /**
     * Increment an iterator. If the iterator is valid, it will point to the next element
     * after beeing incremented (or it will be an end iterator if it was the last voxel).
     *
     * Incrementing an end iterator or an invalid iterator yields undefined behavior.
     *
     * \return *this.
     */
    WSliceIteratorConst operator++()
    {
        // add m_incr1 to the index to get the next element in the slice
        // however, every m_d increments, we need to add another m_incr2 to the index
        m_index += m_incr1;
        ++m_count;
        m_index += m_incr2 * ( m_count / m_d );
        m_count = m_count % m_d;
        return *this;
    }

    /**
     * Decrement an iterator. If the iterator is valid, it will point to the previous element
     * after beeing decremented (or it will be an  iterator to the last voxel if it was the end iterator).
     *
     * Decrementing a begin iterator or an invalid iterator yields undefined behavior.
     *
     * \return *this.
     */
    WSliceIteratorConst operator--()
    {
        m_index -= m_incr1;
        --m_count;
        m_index -= m_incr2 * ( m_count / m_d );
        m_count = m_count % m_d;
        return *this;
    }

    /**
     * Increment an iterator. If the iterator is valid, it will point to the next element
     * after beeing incremented (or it will be an end iterator if it was the last voxel).
     *
     * Incrementing an end iterator or an invalid iterator yields undefined behavior.
     *
     * \return The iterator before incrementing.
     */
    WSliceIteratorConst operator++( int )
    {
        WSliceIteratorConst vi = *this;
        m_index += m_incr1;
        ++m_count;
        m_index += m_incr2 * ( m_count / m_d );
        m_count = m_count % m_d;
        return vi;
    }

    /**
     * Decrement an iterator. If the iterator is valid, it will point to the previous element
     * after beeing decremented (or it will be an  iterator to the last voxel if it was the end iterator).
     *
     * Decrementing a begin iterator or an invalid iterator yields undefined behavior.
     *
     * \return The iterator before decrementing.
     */
    WSliceIteratorConst operator--( int )
    {
        WSliceIteratorConst vi = *this;
        m_index -= m_incr1;
        --m_count;
        m_index -= m_incr2 * ( m_count / m_d );
        m_count = m_count % m_d;
        return vi;
    }

    /**
     * Accesses the data for the voxel the iterator is currently pointing to. In debug mode, the iterator will be tested and
     * access via invalid or end iterators will yield a WException. When build for release, there is no such test!
     *
     * \return A const reference to the data for the voxel the iterator currently points to.
     */
    // TODO( reichenbach ): add a WInvalidIteratorException and a WNoValidVoxel(name?)Exception
    typename ValueTypeTraits< T >::ConstReturnType operator*() // const?
    {
#ifdef DEBUG
        if( !m_grid || !m_valueSet )
        {
            throw WException( "Trying element access with invalid iterator." );
        }
        if( m_index >= m_grid->numVoxels() )
        {
            throw WException( "Trying element access with end iterator." );
        }
#endif
        return m_valueSet->operator[]( m_index );
    }

    /**
     * Get the voxel coordinates of the voxel the iterator points to.
     *
     * \return The voxel's coords in the grid.
     */
    typename WGridRegular3D2::VoxelIndex operator() () const
    {
        return WIndexMap< WGridRegular3D2 >::getVoxelCoords( *m_grid, m_index );
    }

    /**
     * Compares iterators. Comparing two invalid iterators yields undefined behavior.
     *
     * \param si The iterator to compare to.
     *
     * \return true, if the iterators point to the same voxel of the same dataset.
     */
    bool operator==( WSliceIterator< T > const& si ) const
    {
        return m_grid == si.m_grid && m_valueSet == si.m_valueSet && m_index == si.m_index;
    }

    /**
     * Compares iterators. Comparing two invalid iterators yields undefined behavior.
     *
     * \param si The iterator to compare to.
     *
     * \return true, if the iterators do not point to the same voxel of the same dataset.
     */
    bool operator!=( WSliceIterator< T > const& si ) const
    {
        return m_grid != si.m_grid || m_valueSet != si.m_valueSet || m_index != si.m_index;
    }

    /**
     * Compares iterators. Comparing two invalid iterators yields undefined behavior.
     *
     * \param si The iterator to compare to.
     *
     * \return true, if the iterators point to the same voxel of the same dataset.
     */
    bool operator==( WSliceIteratorConst const& si ) const
    {
        return m_grid == si.m_grid && m_valueSet == si.m_valueSet && m_index == si.m_index;
    }

    /**
     * Compares iterators. Comparing two invalid iterators yields undefined behavior.
     *
     * \param si The iterator to compare to.
     *
     * \return true, if the iterators do not point to the same voxel of the same dataset.
     */
    bool operator!=( WSliceIteratorConst const& si ) const
    {
        return m_grid != si.m_grid || m_valueSet != si.m_valueSet || m_index != si.m_index;
    }

    /**
     * Returns a void const* that can be used to test if an iterator is valid, just as you
     * would using an operator bool().
     *
     * \return NULL if the iterator is invalid, something else if it is not.
     */
    operator void const*() const
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

protected:

    /**
     * Constructor that can only be used by the data access object. Used to create iterators pointing to
     * a dataset. This is the only way to construct valid iterators from scratch. (You can still copy iterators.)
     *
     * \param grid The grid of the dataset pointed to by this iterator.
     * \param valueSet The respective valueset.
     * \param idx The relative index of the voxel in the slice that is currently pointed to.
     * \param dir The orientation of the slice ( 0: xy, 1: xz, 2: yz ).
     * \param slice The index of the slice to iterate.
     */
    WSliceIteratorConst( WGridRegular3D2 const* grid, typename ValueTypeTraits< T >::ValueSetType const* valueSet, std::size_t idx,
                         std::size_t dir, std::size_t slice )
        : m_grid( grid ),
          m_valueSet( valueSet )
    {
        switch( dir )
        {
        case 0: // XY
            m_index = m_grid->getNbVoxelsX() * m_grid->getNbVoxelsY() * slice
                    + ( idx % m_grid->getNbVoxelsX() ) + ( idx / m_grid->getNbVoxelsX() ) * m_grid->getNbVoxelsX();
            m_d = m_grid->getNbVoxelsX();
            m_incr1 = 1;
            m_incr2 = 0;
            break;
        case 1: // XZ
            m_index = m_grid->getNbVoxelsX() * slice + ( idx % m_grid->getNbVoxelsX() )
                    + ( idx / m_grid->getNbVoxelsX() ) * m_grid->getNbVoxelsX() * m_grid->getNbVoxelsY();
            m_d = m_grid->getNbVoxelsX();
            m_incr1 = 1;
            m_incr2 = m_grid->getNbVoxelsX() * m_grid->getNbVoxelsY() - m_grid->getNbVoxelsX();
            break;
        case 2: // YZ
            m_index = slice + ( idx % m_grid->getNbVoxelsY() ) * m_grid->getNbVoxelsX()
                    + ( idx / m_grid->getNbVoxelsY() ) * m_grid->getNbVoxelsX() * m_grid->getNbVoxelsY();
            m_d = m_grid->getNbVoxelsY();
            m_incr1 = m_grid->getNbVoxelsX();
            m_incr2 = 0;
            break;
        }
        m_count = idx % m_d;
    }

private:

    //! The grid.
    WGridRegular3D2 const* m_grid;

    //! The values.
    typename ValueTypeTraits< T >::ValueSetType const* m_valueSet; // the valueset is not const for the non-const iterator

    //! The current voxel index for the whole grid (not only the current slice).
    std::size_t m_index;

    //! The increment of the index per iterator increment.
    std::size_t m_incr1;

    //! The increment of the index every m_d increments of the iterator.
    std::size_t m_incr2;

    //! The number of iterator increments done since we added m_incr2.
    std::size_t m_d;

    //! Counts increments and decrements.
    int m_count;
};


#endif  // WSLICEITERATOR_H
