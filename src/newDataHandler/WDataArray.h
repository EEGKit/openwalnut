#ifndef WDATAARRAY_H
#define WDATAARRAY_H

#include "../common/WAssert.h"

/**
 * \class WDataArray
 *
 * This class creates an array of exactly s elements of type t.
 * The size of the array is a template parameter, so there is no need
 * to store the actual size for every instance of WDataArray.
 *
 * \note This might lead to some bloat.
 *
 * After enough memory for s elements is allocated, they are initialized inside the
 * preallocated memory block. The memory belongs to the WDataArray object,
 * and is deallocated on destruction of the WDataArray object. All references
 * and pointers to the elements of the array thus become invalid. Keep
 * this in mind when using the WDataArray class. Be especially careful when
 * writing multithreaded applications. WDataArray is not threadsafe!
 *
 * \note This class goes hand in hand with \see WValue.
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
     * Standard constructor. Try to allocate enough memory for s
     * objects of type T and initialize the objects.
     *
     * \param s The number of objects that must fit into the array.
     */
    WDataArray( std::size_t s );

    /**
     * Standard destructor.
     */
    ~WDataArray();

    /**
     *
     */
    T const& operator[] ( std::size_t i ) const;

    /**
     *
     */
    T& operator[] ( std::size_t );

private:
    /**
     * Copy constructor.
     *
     * \param v
     */
    WDataArray( WDataArray const& v );

    /**
     * Copy assignment.
     *
     * \param
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
    if( m_data )
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
    return const_cast< T >( static_cast< WDataArray const& >( *this ) [ i ] );
}

#endif  // WDATAARRAY_H
