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

#ifndef WSCALARBASEIMPL_H
#define WSCALARBASEIMPL_H

#include <stdint.h>

template < typename T >
class WScalarImplTyped;

typedef WScalarImplTyped< double > WScalarImplDouble;
typedef WScalarImplTyped< int32_t > WScalarImplInt32;

class WScalarImpl
{
public:

    WScalarImpl(){};

    virtual ~WScalarImpl(){};

    // needed for == and !=
    virtual bool compare( const WScalarImpl& rhs ) const = 0;
    virtual bool compareTyped( const WScalarImplDouble& rhs ) const = 0;
    virtual bool compareTyped( const WScalarImplInt32& rhs ) const = 0;

protected:

private:
};

template < typename T1, typename T2 >
struct WScalarTypeAdapter
{
};

template <>
struct WScalarTypeAdapter< double, double >
{
    typedef double TARGET_TYPE;
};

template <>
struct WScalarTypeAdapter< int32_t, double >
{
    typedef double TARGET_TYPE;
};

template <>
struct WScalarTypeAdapter< double, int32_t >
{
    typedef double TARGET_TYPE;
};

template <>
struct WScalarTypeAdapter< int32_t, int32_t >
{
    typedef int32_t TARGET_TYPE;
};

template < typename T >
class WScalarImplTyped: public WScalarImpl
{
public:

    explicit WScalarImplTyped( T val ):
        WScalarImpl(),
        m_value( val ){};

    virtual ~WScalarImplTyped(){};

    virtual bool compare( const WScalarImpl& rhs ) const
    {
        // calls the correct compare method as the type of *this is known and the most specific
        return rhs.compareTyped( *this );
    };

    virtual bool compareTyped( const WScalarImplTyped< double >& rhs ) const
    {
        // the common target type for the operation.
        typedef typename WScalarTypeAdapter< T, double >::TARGET_TYPE C;

        return ( static_cast< C >( m_value ) == static_cast< C >( rhs.m_value ) );
    };

    virtual bool compareTyped( const WScalarImplTyped< int32_t >& rhs ) const
    {
        // the common target type for the operation.
        typedef typename WScalarTypeAdapter< T, int32_t >::TARGET_TYPE C;

        return ( static_cast< C >( m_value ) == static_cast< C >( rhs.m_value ) );
    };

    T m_value;

protected:

private:
};

#endif  // WSCALARBASEIMPL_H

