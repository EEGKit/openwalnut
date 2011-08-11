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

#ifndef WVOXELITERATOR_H
#define WVOXELITERATOR_H

#include <string> // for std::size_t
#include <utility>

#include "WNeighborhood.h"

// ################################ forward declarations #######################################

// a forward declaration for the type used for neighbor voxel iteration
template< typename T >
class WNeighborhoodIterator;

// a forward declaration for the type used for neighbor voxel iteration (const version)
template< typename T >
class WNeighborhoodIteratorConst;

// a forward declaration for the boundary specific functionality
template< typename T >
class WBoundaryStrategy;

// a forward declaration needed for a friend declaration below
template< typename GridT, typename ValueT >
class WDataAccess;

// a forward declaration needed for a friend declaration below
template< typename GridT, typename ValueT >
class WDataAccessConst;

// one for the valueset
template< typename ValueT >
class WValueSet;

// and the index map
template< typename GridT >
class WIndexMap;

// a forward declaration for the test
class WVoxelIteratorTest;

// for the slice iter test
class WSliceIteratorTest;

// forward the const version of the voxel iterator, declared in this header
template< typename GridT, typename ValueT >
class WVoxelIteratorConst;

// TODO( reichenbach ): remove the neighborhood iterator construction from this class and move it to
//                      a specialization of the data access

/**
 * A class for iterating voxels of a dataset. Provides operations for incrementing, decrementing, data access and so on,
 * similar to the std containers' iterators.
 *
 * The default constructor creates invalid iterators. An invalid iterator is an iterator that does not point to a dataset.
 * Operations on these yield undefined behavior most of the time.
 *
 * A valid iterator does NOT guarantee you can access data, as it may be an end iterator too. So if you want to test if an iterator
 * can be used to access data, use this:
 *
 * if( vi && vi != ve ) { ... }
 *
 * (Where 'vi' is your iterator and 've' a corresponding end iterator of the dataset.)
 *
 * Things to be careful about:
 *
 * PLEASE READ THIS!
 *
 * The iterators itself are NOT thread-safe.
 *
 * Also, be sure to keep a shared_ptr to your dataset while working with iterators. If the last shared_ptr to the data gets destroyed
 * (say you didn't copy the input data shared_ptr from your input connector and it gets updated while you do your stuff with the iterators),
 * there will be dangling pointers and a segfault is imminent!
 *
 * \param GridT The type of the grid.
 * \param T The type of the data in the valueset.
 */
template< typename GridT, typename T >
class WVoxelIterator
{
public:

    // the test is a friend
    friend class WVoxelIteratorTest;

    // another test that needs the constructor
    friend class WSliceIteratorTest;

    // the const version is also a friend
    friend class WVoxelIteratorConst< GridT, T >;

    // The data access is a friend so it can create iterators using the non-default constructor.
    template< typename GridTT, typename ValueT >
    friend class WDataAccess;

    //! A typedef for the grid's type.
    typedef GridT GridType;

    /**
     * Default constructor, creates an invalid iterator.
     */
    WVoxelIterator()
        : m_grid( NULL ),
          m_valueSet( NULL ),
          m_index( 0 )
    {
    }

    /**
     * Destructor.
     */
    ~WVoxelIterator()
    {
        // do not(!!!) delete m_grid
        // do not(!!!) delete m_valueSet
    }

    /**
     * Copy construction.
     *
     * \param vi The iterator to copy from.
     */
    WVoxelIterator( WVoxelIterator const& vi ) // NOLINT not explicit ( std::make_pair won't work )
        : m_grid( vi.m_grid ),
          m_valueSet( vi.m_valueSet ),
          m_index( vi.m_index )
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
    WVoxelIterator operator++()
    {
        ++m_index;
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
    WVoxelIterator operator--()
    {
        --m_index;
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
    WVoxelIterator operator++( int )
    {
        WVoxelIterator vi = *this;
        ++m_index;
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
    WVoxelIterator operator--( int )
    {
        WVoxelIterator vi = *this;
        --m_index;
        return vi;
    }

    /**
     * Accesses the data for the voxel the iterator is currently pointing to. In debug mode, the iterator will be tested and
     * access via invalid or end iterators will yield a WException. When build for release, there is no such test!
     *
     * \return A reference to the data for the voxel the iterator currently points to.
     */
    // TODO( reichenbach ): add a WInvalidIteratorException and a WNoValidVoxel(name?)Exception
    typename WValueSet< T >::ValueReturnType operator*() // const?
    {
#ifdef _DEBUG
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
    typename GridType::VoxelIndex operator() () const
    {
        // the index will be tested by getVoxelCoords
        return WIndexMap< GridType >::getVoxelCoords( *m_grid, m_index );
    }

    /**
     * Compares iterators. Comparing two invalid iterators yields undefined behavior.
     *
     * \param vi The iterator to compare to.
     *
     * \return true, if the iterators point to the same voxel of the same dataset.
     */
    bool operator==( WVoxelIterator const& vi ) const
    {
        return m_grid == vi.m_grid && m_valueSet == vi.m_valueSet && m_index == vi.m_index;
    }

    /**
     * Compares iterators. Comparing two invalid iterators yields undefined behavior.
     *
     * \param vi The iterator to compare to.
     *
     * \return true, if the iterators do not point to the same voxel of the same dataset.
     */
    bool operator!=( WVoxelIterator const& vi ) const
    {
        return m_grid != vi.m_grid || m_valueSet != vi.m_valueSet || m_index != vi.m_index;
    }

    /**
     * Compares iterators. Comparing two invalid iterators yields undefined behavior.
     *
     * \param vi The iterator to compare to.
     *
     * \return true, if the iterators point to the same voxel of the same dataset.
     */
    bool operator==( WVoxelIteratorConst< GridT, T > const& vi ) const
    {
        return m_grid == vi.m_grid && m_valueSet == vi.m_valueSet && m_index == vi.m_index;
    }

    /**
     * Compares iterators. Comparing two invalid iterators yields undefined behavior.
     *
     * \param vi The iterator to compare to.
     *
     * \return true, if the iterators do not point to the same voxel of the same dataset.
     */
    bool operator!=( WVoxelIteratorConst< GridT, T > const& vi ) const
    {
        return m_grid != vi.m_grid || m_valueSet != vi.m_valueSet || m_index != vi.m_index;
    }

    /**
     * Returns a void const* that can be used for tests like
     * ' if( it ) { doSth() } ' ('it' beeing a WVoxelIterator). Use this to test
     * if the iterator is valid, i.e. it points to a dataset. You still need to test against the end iterator
     * before accessing data.
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

    /**
     * Get a range of neighbor iterators that can be used to iterate all voxels that are in
     * a specified neighborhood of the voxel that this iterator currently points to. Changes made to the voxel iterator after
     * construction of a neighborhood iterator range do not affect that range.
     *
     * \param nbh A neighborhood that defines the offsets of the voxels that will be neighbors to a currently selected voxel.
     * \param bs A strategy to use for out-of-grid voxels.
     *
     * \return A pair of begin and end iterators for the neighbors of the voxel currently pointed to by this iterator.
     */
    std::pair< WNeighborhoodIterator< T >, WNeighborhoodIterator< T > > neighbors( WNeighborhood const& nbh, WBoundaryStrategy< T > const& bs )
    {
        return std::make_pair( WNeighborhoodIterator< T >( m_grid, m_valueSet, nbh, bs, this->operator() (), 0 ),
                               WNeighborhoodIterator< T >( m_grid, m_valueSet, nbh, bs, this->operator() (), nbh.size() ) );
    }

protected:

    /**
     * Constructor that can only be used by the data access object. Used to create iterators pointing to
     * a dataset. This is the only way to construct valid iterators from scratch. (You can still copy iterators.)
     *
     * \param grid The grid of the dataset pointed to by this iterator.
     * \param valueSet The respective valueset.
     * \param idx The index of the voxel that is currently pointed to.
     */
    WVoxelIterator( GridT const* grid, WValueSet< T >* valueSet, std::size_t idx )
        : m_grid( grid ),
          m_valueSet( valueSet ),
          m_index( idx )
    {
    }

private:

    //! The grid.
    GridT const* m_grid;

    //! The values.
    WValueSet< T >* m_valueSet; // the valueset is not const for the non-const iterator

    //! The current voxel.
    std::size_t m_index;
};

// ############################################# const version ##################################################

/**
 * This is the const version of the voxel iterator class.
 */
template< typename GridT, typename T >
class WVoxelIteratorConst
{
public:

    // the test is a friend
    friend class WVoxelIteratorTest;

    // the non-const version is also a friend
    friend class WVoxelIterator< GridT, T >;

    //! A typedef for the grid's type.
    typedef GridT GridType;

    // The data access is a friend so it can create iterators using the non-default constructor.
    template< typename GridTT, typename ValueT >
    friend class WDataAccess;

    // The const data access is a friend so it can create iterators using the non-default constructor.
    template< typename GridTT, typename ValueT >
    friend class WDataAccessConst;

    /**
     * Default constructor, create an invalid iterator.
     */
    WVoxelIteratorConst()
        : m_grid( NULL ),
          m_valueSet( NULL ),
          m_index( 0 )
    {
    }

    /**
     * Destructor.
     */
    ~WVoxelIteratorConst()
    {
        // do not(!!!) delete m_grid
        // do not(!!!) delete m_valueSet
    }

    /**
     * Copy construction.
     *
     * \param vi The iterator to copy from.
     */
    WVoxelIteratorConst( WVoxelIteratorConst const& vi )  // NOLINT not explicit ( std::make_pair won't work )
        : m_grid( vi.m_grid ),
          m_valueSet( vi.m_valueSet ),
          m_index( vi.m_index )
    {
    }

    /**
     * Copy construction from non-const iterator.
     *
     * \param vi The iterator to copy from.
     */
    WVoxelIteratorConst( WVoxelIterator< GridT, T > const& vi )  // NOLINT not explicit ( std::make_pair won't work )
        : m_grid( vi.m_grid ),
          m_valueSet( vi.m_valueSet ),
          m_index( vi.m_index )
    {
    }

    /**
     * A special copy operator that allows copying from a non-const iterator.
     *
     * \param vi The iterator to copy from.
     *
     * \return *this.
     */
    WVoxelIteratorConst& operator=( WVoxelIterator< GridT, T > const& vi )
    {
        m_grid = vi.m_grid;
        m_valueSet = vi.m_valueSet;
        m_index = vi.m_index;
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
    WVoxelIteratorConst operator++()
    {
        ++m_index;
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
    WVoxelIteratorConst operator--()
    {
        --m_index;
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
    WVoxelIteratorConst operator++( int )
    {
        WVoxelIteratorConst vi = *this;
        ++m_index;
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
    WVoxelIteratorConst operator--( int )
    {
        WVoxelIteratorConst vi = *this;
        --m_index;
        return vi;
    }

    /**
     * Accesses the data for the voxel the iterator is currently pointing to. In debug mode, the iterator will be tested and
     * access via invalid or end iterators will yield a WException. When build for release, there is no such test!
     *
     * \return A reference to the data for the voxel the iterator currently points to.
     */
    // TODO( reichenbach ): add a WInvalidIteratorException and a WNoValidVoxel(name?)Exception
    typename WValueSet< T >::ValueReturnTypeConst operator*() const
    {
#ifdef _DEBUG
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
    typename GridType::VoxelIndex operator() () const
    {
        // the index will be tested by getVoxelCoords
        return WIndexMap< GridType >::getVoxelCoords( *m_grid, m_index );
    }

    /**
     * Compares iterators. Comparing two invalid iterators yields undefined behavior.
     *
     * \param vi The iterator to compare to.
     *
     * \return true, if the iterators point to the same voxel of the same dataset.
     */
    bool operator==( WVoxelIteratorConst const& vi ) const
    {
        return m_grid == vi.m_grid && m_valueSet == vi.m_valueSet && m_index == vi.m_index;
    }

    /**
     * Compares iterators. Comparing two invalid iterators yields undefined behavior.
     *
     * \param vi The iterator to compare to.
     *
     * \return true, if the iterators do not point to the same voxel of the same dataset.
     */
    bool operator!=( WVoxelIteratorConst const& vi ) const
    {
        return m_grid != vi.m_grid || m_valueSet != vi.m_valueSet || m_index != vi.m_index;
    }

    /**
     * Compares iterators. Comparing two invalid iterators yields undefined behavior.
     *
     * \param vi The iterator to compare to.
     *
     * \return true, if the iterators point to the same voxel of the same dataset.
     */
    bool operator==( WVoxelIterator< GridT, T > const& vi ) const
    {
        return m_grid == vi.m_grid && m_valueSet == vi.m_valueSet && m_index == vi.m_index;
    }

    /**
     * Compares iterators. Comparing two invalid iterators yields undefined behavior.
     *
     * \param vi The iterator to compare to.
     *
     * \return true, if the iterators do not point to the same voxel of the same dataset.
     */
    bool operator!=( WVoxelIterator< GridT, T > const& vi ) const
    {
        return m_grid != vi.m_grid || m_valueSet != vi.m_valueSet || m_index != vi.m_index;
    }

    /**
     * Returns a void const* that can be used for tests like
     * ' if( it ) { doSth() } ' ('it' beeing a WVoxelIterator). Use this to test
     * if the iterator is valid, i.e. it points to a dataset. You still need to test against the end iterator
     * before accessing data.
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

    /**
     * Get a range of neighbor iterators that can be used to iterate all voxels that are in
     * a specified neighborhood of the voxel that this iterator currently points to. Changes made to the voxel iterator after
     * construction of a neighborhood iterator range do not affect that range.
     *
     * \param nbh A neighborhood that defines the offsets of the voxels that will be neighbors to a currently selected voxel.
     * \param bs A strategy to use for out-of-grid voxels.
     *
     * \return A pair of begin and end iterators for the neighbors of the voxel currently pointed to by this iterator.
     */
    std::pair< WNeighborhoodIteratorConst< T >, WNeighborhoodIteratorConst< T > >
        neighbors( WNeighborhood const& nbh, WBoundaryStrategy< T > const& bs )
    {
        return std::make_pair( WNeighborhoodIteratorConst< T >( m_grid, m_valueSet, nbh, bs, this->operator() (), 0 ),
                               WNeighborhoodIteratorConst< T >( m_grid, m_valueSet, nbh, bs, this->operator() (), nbh.size() ) );
    }

protected:

    /**
     * Constructor that can only be used by the data access object. Used to create iterators pointing to
     * a dataset. This is the only way to construct valid iterators from scratch. (You can still copy iterators.)
     *
     * \param grid The grid of the dataset pointed to by this iterator.
     * \param valueSet The respective valueset.
     * \param idx The index of the voxel that is currently pointed to.
     */
    WVoxelIteratorConst( GridT const* grid, WValueSet< T > const* valueSet, std::size_t idx )
        : m_grid( grid ),
          m_valueSet( valueSet ),
          m_index( idx )
    {
    }

private:

    //! The grid.
    GridT const* m_grid;

    //! The values.
    WValueSet< T > const* m_valueSet;

    //! The current voxel.
    std::size_t m_index;
};

#endif  // WVOXELITERATOR_H
