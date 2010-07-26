#ifndef WVECTOR_H
#define WVECTOR_H

#include <boost/array.hpp>

//#######################################################
//#### ###### #####   ####       ##       #####    ######
//####  ##### #### ### ###### ##### ######### ##### #####
//#### # #### #### ### ###### ##### ######## ############
//#### ## ### ### ##### ##### #####       ### ###########
//#### ### ## ### ##### ##### ##### ##########    #######
//#### #### # #### ### ###### ##### ############## ######
//#### #####  #### ### ###### ##### ######### ##### #####
//#### ###### #####   ####### #####       ####     ######
//#######################################################
//
// - how much overhead is initialization on construction?
//     - ( currently data is initialized to T() in the default constructor )
//
//
//

/**
 * \class WVector
 *
 *
 */
template< std::size_t s, typename T = double >
class WVector
{
public:
    //! unnamed enum that defines some template meta data
    enum
    {
        NumElements = s; //! the number of elements
        ElementSize = sizeof( T ); //! the size of a single element, in bytes
        Size = NumElements * ElementSize; //! the size of the whole array, in bytes
    };

    //! the data type
    typedef T DataType;

    /**
     * Default constructor.
     */
    WVector();

    /**
     * Default destructor.
     */
    ~WVector();

    /**
     *
     */
    T const& operator[] ( std::size_t i ) const;

    /**
     *
     */
    T& operator[] ( std::size_t i );

private:
    //! the data
    // this should store the elements in the WVector object
    // instead of storing only a pointer pointing to some
    // place else
    boost::array< T, s > m_data;

    // note: try to keep the memory usage as low as possible,
    // this is also one of the reasons for making the number of
    // elements a template parameter
};

template< std::size_t s, typename T >
WVector< s, T >::WVector()
 : m_data()
{
    // pre-initialize?
    for( std::size_t k = 0; k < NumElements; ++k )
    {
        m_data[ k ] = T();
    }
}

template< std::size_t s, typename T >
WVector< s, T >::~WVector()
{
}

template< std::size_t s, typename T >
T const& WVector< s, T >::operator[] ( std::size_t i ) const
{
#ifdef DEBUG
    WAssert( i < NumElements );
#endif
    return m_data[ i ];
}

template< std::size_t s, typename T >
T& WVector< s, T >::operator[] ( std::size_t i )
{
    return const_cast< T >( static_cast< WVector const& >( *this ) [ i ] );
}

#endif  // WVECTOR_H
