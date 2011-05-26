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

#ifndef WVALUE_H
#define WVALUE_H

#include <algorithm>
#include <cmath>
#include <vector>

#include "../WAssert.h"
#include "../WStringUtils.h"
#include "linearAlgebra/WLinearAlgebra.h"

/**
 * Base class for all higher level values like tensors, vectors, matrices and so on.
 */
template< typename T > class WValue
{
template< typename S > friend class WValue; //!< All WValues are friends of each other.

// We exclude this from doxygen since they are documented already as functions and I don't want to duplicate that documentation
// \cond
template< typename U > friend std::ostream& operator<<( std::ostream& os, const WValue< U > &rhs );
template< typename U > friend std::istream& operator>>( std::istream& in, WValue< U >& rhs );
// \endcond
public:
    /**
     * Create a WValue with the given number of components.
     * The components will be set to zero if T is a type representing numbers.
     * \param nbComponents Number of elements the WValue consists of.
     */
    explicit WValue( size_t nbComponents )
        : m_components( nbComponents )
    {
    }

    /**
     * Create a WValue as copy of the one given as parameter.
     * \param newValue The WValue to be copied.
     */
    WValue( const WValue& newValue )
        : m_components( newValue.m_components )
    {
    }

    /**
     * Create a WValue as copy of the one given as parameter but with another template type.
     * \param newValue The WValue to be copied.
     */
    template< typename S > explicit WValue( const WValue< S >& newValue )
    {
        m_components.resize( newValue.m_components.size() );
        for( size_t i = 0; i < m_components.size(); ++i )
        {
            m_components[i] = newValue.m_components[i];
        }
    }

    /**
     * Create a WValue from the given WVector_2.
     * \param newValues The WVector_2 with the values..
     */
    explicit WValue( const WVector_2& newValues )
        : m_components( static_cast< std::size_t >( newValues.size() ) )
    {
        for ( std::size_t i = 0; i < m_components.size(); ++i )
        {
            m_components[ i ] = static_cast< T >( newValues( i ) );
        }
    }

    /**
     * Get number of components the value consists of.
     */
    size_t size() const
    {
        return m_components.size();
    }

    /**
     * Returns a reference to the i-th component in order
     * to provide access to the component.
     * \param i element id
     */
    T& operator[]( size_t i )
    {
        WAssert( i <  m_components.size(), "Index out of bounds." );
        return m_components[i];
    }

    /**
     * Returns a const reference to the i-th component in order
     * to provide read-only access to the component.
     * \param i element id
     */
    const T& operator[]( size_t i ) const
    {
        WAssert( i <  m_components.size(), "Index out of bounds." );
        return m_components[i];
    }

    /**
     * Compares two WValues and returns true if they contain the same data.
     * \param rhs The right hand side of the comparison
     */
    bool operator==( const WValue& rhs ) const
    {
        return ( m_components == rhs.m_components );
    }

    /**
     * Compares two WValues and returns true if they contain the different data.
     * \param rhs The right hand side of the comparison
     */
    bool operator!=( const WValue& rhs ) const
    {
        return ( m_components != rhs.m_components );
    }

    /**
     * Assigns the contents of its argument to the contents of this WValue.
     * \param rhs The right hand side of the assignment
     */
    WValue& operator=( const WValue& rhs )
    {
        m_components = rhs.m_components;
        return *this;
    }

    /**
     * Adds a the argument component-wise to the components of this WValue
     * \param rhs The right hand side of the assignment
     */
    WValue& operator+=( const WValue& rhs )
    {
        WAssert( m_components.size() == rhs.m_components.size(), "Incompatible sizes of lhs and rhs of operator." );
        for( unsigned int i = 0; i < m_components.size(); ++i )
            m_components[i] += rhs.m_components[i];
        return *this;
    }

    /**
     * Subtracts the argument component-wise from the components of this WValue
     * \param rhs The right hand side of the assignment
     */
    WValue& operator-=( const WValue& rhs )
    {
        WAssert( m_components.size() == rhs.m_components.size(), "Incompatible sizes of lhs and rhs of operator." );
        for( unsigned int i = 0; i < m_components.size(); ++i )
            m_components[i] -= rhs.m_components[i];
        return *this;
    }

    /**
     * Scales each component of this WValue with the given scalar argument
     * \param rhs The right hand side of the assignment
     */
    WValue& operator*=( double rhs )
    {
        for( unsigned int i = 0; i < m_components.size(); ++i )
            m_components[i] *= rhs;
        return *this;
    }

    /**
     * Scales each component of this WValue with the corresponding
     * component of the given argument WValue
     * \param rhs The right hand side of the assignment
     */
    WValue& operator*=( const WValue& rhs )
    {
        WAssert( m_components.size() == rhs.m_components.size(), "Incompatible sizes of lhs and rhs of operator." );
        for( unsigned int i = 0; i < m_components.size(); ++i )
            m_components[i] *= rhs.m_components[i];
        return *this;
    }

    /**
     * Scales each component of this WValue with the given scalar argument
     * \param rhs The right hand side of the assignment
     */
    WValue& operator/=( const double rhs )
    {
        for( unsigned int i = 0; i < m_components.size(); ++i )
            m_components[i] /= rhs;
        return *this;
    }


    /**
     * Component-wise addition.
     * \param summand2 The right hand side of the summation
     */
    const WValue operator+( const WValue& summand2 ) const
    {
        WAssert( m_components.size() == summand2.m_components.size(), "Incompatible sizes of summands." );
        WValue result( *this );
        result += summand2;
        return result;
    }

    /**
     * Component-wise subtraction.
     * \param subtrahend The right hand side of the subtraction
     */
    const WValue operator-( const WValue& subtrahend ) const
    {
        WAssert( m_components.size() == subtrahend.m_components.size(), "Incompatible sizes of subtrahend and minuend." );
        WValue result( *this );
        result -= subtrahend;
        return result;
    }

    /**
     * Component-wise multiplication.
     * \param factor2 The right hand side of the product
     */
    const WValue operator*( const WValue& factor2 ) const
    {
        WAssert( m_components.size() == factor2.m_components.size(), "Incompatible sizes of factors." );
        WValue result( *this );
        result *= factor2;
        return result;
    }

    /**
     * Square root of sum of squares of elements.
     * This function return double instead of T
     * because norm includes a square root and thus
     * its computation automatically results in a
     * floating point number.
     */
    double norm() const
    {
        return sqrt( this->normSquare() );
    }

    /**
     * Sum of squares of elements.
     * This function returns double instead of T
     * because normSquare includes many squares and thus
     * might return large values that might not fit into
     * T's range of values. Double prevents an overflow.
     * Additionally this is consistent with norm().
     */
    double normSquare() const
    {
        double normSquare = 0.0;

        for( unsigned int i = 0; i < m_components.size(); ++i )
        {
            normSquare += m_components[i] * m_components[i];
        }

        return normSquare;
    }

    /**
     * Make the norm of this WValue be 1 by dividing by WValue::norm()
     */
    void normalize()
    {
        double currentNorm = norm();
        for( unsigned int i = 0; i < m_components.size(); ++i )
        {
            WAssert( currentNorm > 0.0, "Norm is non-positive!" );
            m_components[i] /= currentNorm;
        }
    }

    /**
     * Return a normalized version of the current WValue without modifying it.
     */
    WValue normalized() const
    {
        WValue result = *this;
        result.normalize();
        return result;
    }

    /**
     * Returns the mean value of all values stored in this WValue.
     */
    T mean() const
    {
        WAssert( !m_components.empty(), "WValue has no entries." );
        T sum = 0;
        for ( typename std::vector< T >::const_iterator it = m_components.begin(); it != m_components.end(); it++  )
        {
            sum += ( *it );
        }
        return ( sum / static_cast< T >( m_components.size() ) );
    }

    /**
     * Returns the median of all values stored in this WValue.
     */
    T median() const
    {
        WAssert( !m_components.empty(), "WValue has no entries. " );
        std::vector< T > components( m_components );
        std::sort( components.begin(), components.end() );
        return components[ components.size() / 2 ];
    }

    /**
     * Changes the number of scalars held by this WValue.
     * \param size The number of scalars stored in the WValue.
     */
    void resize( size_t size )
    {
        m_components.resize( size );
    }

    /**
     * Returns this WValue as WVector_2.
     */
    WVector_2 toWVector()
    {
        WVector_2 result( m_components.size() );
        for ( size_t i = 0; i < m_components.size(); ++i )
        {
            result( i ) = static_cast<double>( m_components[ i ] );
        }
        return result;
    }

protected:
private:
    /**
     * The components the value is composed of. This contains the actual data
     */
    std::vector< T > m_components;
};

/**
 * Multiplies a WValue with a scalar
 * \param lhs left hand side of product
 * \param rhs right hand side of product
 */
template< typename T > inline const WValue< T > operator*( const WValue< T >& lhs, double rhs )
{
    WValue< T > result( lhs );
    result *= rhs;
    return result;
}

/**
 * This functions only exists to make scalar multiplication commutative
 * \param lhs left hand side of product
 * \param rhs right hand side of product
 */
template< typename T > inline const WValue< T > operator*( double lhs, const WValue< T >& rhs )
{
    WValue< T > result( rhs );
    result *= lhs;
    return result;
}

/**
 * Divides a WValue by a scalar
 * \param lhs left hand side of product
 * \param rhs right hand side of product
 */
template< typename T > inline const WValue< T > operator/( const WValue< T >& lhs, double rhs )
{
    WValue< T > result( lhs );
    result /= rhs;
    return result;
}

/**
 * Writes a meaningful representation of that object to the given stream.
 *
 * \param os The operator will write to this stream.
 * \param rhs This will be written to the stream.
 *
 * \return the output stream
 */
template< typename U > inline std::ostream& operator<<( std::ostream& os, const WValue< U > &rhs )
{
    return string_utils::operator<<( os, rhs.m_components );
}

/**
 * Write an input stream into a WValue.
 *
 * \param in the input stream
 * \param rhs the value to where to write the stream
 *
 * \return the input stream
 */
template< typename U > inline std::istream& operator>>( std::istream& in, WValue< U >& rhs )
{
    return string_utils::operator>>( in, rhs.m_components );
}

#endif  // WVALUE_H
