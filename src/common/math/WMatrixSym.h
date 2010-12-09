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

#ifndef WMATRIXSYM_H
#define WMATRIXSYM_H

#include <algorithm>
#include <cassert>
#include <vector>

#include "../exceptions/WOutOfBounds.h"

/**
 * Symmetric square matrix, storing only the elements of the triangular matrix without the main
 * diagonal. So in case of a NxN matrix there are only (N^2-N)/2 elements stored.
 *
 * \note There exists also a WWriter and WReader for storing/reading the matrix in VTK file format.
 */
template< typename T >
class WMatrixSymImpl
{
friend class WMatrixSymTest;
public:
    /**
     * Type of stored elements.
     */
    typedef T value_type;

    /**
     * Generates new symmetric matrix.
     *
     * \param n number of rows and cols
     */
    explicit WMatrixSymImpl( size_t n );

    /**
     * Default constructor leaving all empty.
     */
    WMatrixSymImpl();

    /**
     * Element acces operator as if the elements where stored as a normal matrix.
     *
     * \warning Acessing elements of the main diagonal is forbidden!
     *
     * \param i The i'th row
     * \param j The j'th column
     *
     * \return reference to the (i,j) element of the table
     */
    T& operator()( size_t i, size_t j ) throw( WOutOfBounds );

    /**
     * Returns the number of elements stored inside this matrix.
     */
    size_t numElements() const;

    /**
     * Returns the number of rows and cols.
     */
    size_t size() const;

    /**
     * Returns the elements stored inside of this container.
     *
     * \return Read-only reference to the elements stored inside this container.
     */
    const std::vector< T >& getData() const;

    /**
     * Resets the internal data to the given vector of elements.
     *
     * \param data new data in row major arrangement
     */
    void setData( const std::vector< T > &data ) throw( WOutOfBounds );

private:
    /**
     * Internal data structure to store the elements. The order is row major.
     */
    std::vector< T > m_data;

    /**
     * Number of rows and cols.
     */
    size_t m_n;
};

template< typename T >
inline WMatrixSymImpl< T >::WMatrixSymImpl( size_t n )
    : m_data( ( n * ( n - 1 ) ) / 2, 0.0 ),
      m_n( n )
{
}

template< typename T >
inline WMatrixSymImpl< T >::WMatrixSymImpl()
    : m_n( 0 )
{
}

template< typename T >
inline T& WMatrixSymImpl< T >::operator()( size_t i, size_t j ) throw( WOutOfBounds )
{
    if( i == j || i >= m_n || j >= m_n )
    {
        std::stringstream ss;
        ss << "Invalid Element Access ( " << i << ", " << j << " ). No diagonal elements or indices bigger than " << m_n << " are allowed.";
        throw WOutOfBounds( ss.str() );
    }
    if( i > j )
    {
        std::swap( i, j );
    }
    return m_data[( i * m_n + j - ( i + 1 ) * ( i + 2 ) / 2 )];
}

template< typename T >
inline size_t WMatrixSymImpl< T >::numElements() const
{
    return m_data.size();
}

template< typename T >
inline size_t WMatrixSymImpl< T >::size() const
{
    return m_n;
}

template< typename T >
inline const typename std::vector< T >& WMatrixSymImpl< T >::getData() const
{
    return m_data;
}

template< typename T >
inline void WMatrixSymImpl< T >::setData( const std::vector< T > &data ) throw( WOutOfBounds )
{
    if( m_n * ( m_n - 1 ) / 2 != data.size() )
    {
        std::stringstream ss;
        ss << "Data vector length: " << data.size() << " doesn't fit to number of rows and cols: " << m_n;
        throw WOutOfBounds( ss.str() );
    }
    m_data = std::vector< T >( data ); // copy content
}

typedef WMatrixSymImpl< double > WMatrixSym;

#endif  // WMATRIXSYM_H
