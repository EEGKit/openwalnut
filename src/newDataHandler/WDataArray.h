#ifndef WDATAARRAY_H
#define WDATAARRAY_H

#include "../common/WAssert.h"

/**
 * \class WDataArray
 *
 * This class creates an array of s elements of type T.
 *
 * The type T should support the following:
 *
 * - an unsigned integer T::Size, that contains the size of an object of type T in bytes
 */
template< typename T >
class WDataArray
{
public:
    /**
     * Standard constructor.
     *
     * \param s The number of objects that must fit into the array.
     */
    WDataArray( std::size_t s );

    /**
     * Standard destructor.
     */
    ~WDataArray();

    /**
     * Access operator.
     *
     * \param i The position of the requested element.
     *
     * \return The element at position i.
     */
    T const& operator[] ( std::size_t i ) const;

    /**
     * Access operator.
     *
     * \param i The position of the requested element.
     *
     * \return The element at position i.
     */
    T& operator[] ( std::size_t );

private:
    /**
     * Copy constructor.
     *
     * \param v The array to copy.
     */
    WDataArray( WDataArray const& v );

    /**
     * Copy assignment.
     *
     * \param The array to copy.
     *
     * \return *this
     */
    WDataArray& operator = ( WDataArray const& v );

    //! the size of the whole array in bytes
    std::size_t m_sizeInBytes;

    //! the number of elements
    std::size_t m_numElements;

    //! the elements
    T* const m_elements;
};

template< typename T >
WDataArray< T >::WDataArray( std::size_t s )
 : m_sizeInBytes( T::Size * s ),
   m_numElements( s ),
   m_elements( new T[ s ] )
{
}

template< typename T >
WDataArray< T >::~WDataArray()
{
    if( m_elements )
    {
        delete[] m_elements;
    }
}

template< typename T >
T const& WDataArray< T >::operator[] ( std::size_t i ) const
{
#ifdef DEBUG
    WAssert( i < m_numElements, "" );
#endif
    return m_elements[ i ];
}

template< typename T >
T& WDataArray< T >::operator[] ( std::size_t i )
{
#ifdef DEBUG
    WAssert( i < m_numElements, "" );
#endif
    return m_elements[ i ];
}

#endif  // WDATAARRAY_H
