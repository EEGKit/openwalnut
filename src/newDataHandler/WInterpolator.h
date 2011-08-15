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

#ifndef WINTERPOLATOR_H
#define WINTERPOLATOR_H

#include <vector>

#include <boost/array.hpp>
#include <boost/mpl/size_t.hpp>

// forward declarations
template< typename GridT, typename ValueT >
class WDataAccess;

template< typename GridT, typename ValueT >
class WDataAccessConst;

template< typename T >
class WDataProxy;

template< typename T >
class WDataProxyConst;

/**
 * Defines a type for a list of weights to use during interpolation.
 *
 * \tparam size The size of the list.
 */
template< int size >
class WInterpolationWeightList
{
public:

    //! The actual list type.
    typedef boost::array< double, size > type;
    // using a boost::array here, because it will be allocated on the stack
    // no operator new!
};

/**
 * Empty Specialization for size 0.
 */
template<>
class WInterpolationWeightList< -1 >
{
};

/**
 * A preliminary specialization for -1.
 * This might be used for dynamic sizes later on.
 */
template<>
class WInterpolationWeightList< 0 >
{
};

// ################################# Value Array ###################################

/**
 * Implements an array for data references/pointers. Used to store where the values that
 * will be used for interpolation are.
 *
 * \tparam size The number of values that are to be referenced.
 * \tparam valueT The value type.
 */
template< int size, typename ValueT >
class WInterpolationDataReferenceArray
{
public:

    //! The type of the actual data.
    typedef ValueT DataType;

    /**
     * Set the i'th value.
     *
     * \param index The index of the value.
     * \param value The value, may NOT be a temporary.
     */
    void set( std::size_t index, ValueT const& value )
    {
        m_array[ index ] = &value;
    }

    /**
     * Get the i'th value.
     *
     * \param index The index.
     * \return The value.
     */
    ValueT operator[] ( std::size_t index ) const
    {
        return *m_array[ index ];
    }

private:

    //! The array pointing to the values.
    boost::array< ValueT const*, size > m_array;
};

/**
 * Specialization for data proxies.
 *
 * \tparam size The number of values that are to be referenced.
 * \tparam T The data type.
 */
template< int size, typename T >
class WInterpolationDataReferenceArray< size, WDataProxy< T > >
{
public:

    //! The type of the actual data.
    typedef T DataType;

    /**
     * Set the i'th value.
     *
     * \param index The index of the value.
     * \param value The value, may NOT be a temporary.
     */
    void set( std::size_t index, WDataProxyConst< T > const& value )
    {
        m_array[ index ] = value;
    }

    /**
     * Get the i'th value.
     *
     * \param index The index.
     * \return The value.
     */
    T operator[] ( std::size_t index ) const
    {
        return static_cast< T >( m_array[ index ] );
    }

private:

    //! The array pointing to the values.
    // the data proxies are kinda pointers themselves
    boost::array< WDataProxyConst< T >, size > m_array;
};

/**
 * Specialization for -1.
 */
template< typename ValueT >
class WInterpolationDataReferenceArray< -1, ValueT >
{
};

/**
 * Specialization for 0.
 */
template< typename ValueT >
class WInterpolationDataReferenceArray< 0, ValueT >
{
};

// ##################################### interpolator ########################################

/**
 * The actual interpolator object. Assembles the final interpolation operation from
 * the functors.
 *
 * \tparam InterpolationPrecisionT
 * \tparam TypeInterpolatorT
 */
template< typename InterpolationPrecisionT, typename TypeInterpolatorT >
class WInterpolator
{
public:

    // the next 2 are allowed to create interpolator objects
    //! WDataAccess is a friend.
    template< typename, typename > friend class WDataAccess;

    //! WDataAccessConst is a friend.
    template< typename, typename > friend class WDataAccessConst;

    //! The type of the array used to store weights.
    typedef typename WInterpolationWeightList< InterpolationPrecisionT::numValues >::type WeightsArray;

    //! The type of the array used to store references/pointers to the data.
    typedef WInterpolationDataReferenceArray< InterpolationPrecisionT::numValues, typename TypeInterpolatorT::ValueType > DataRefArray;

    //! The type of the interpolation result.
    typedef typename DataRefArray::DataType ResultType;

    /**
     * The interpolation function.
     *
     * \param coords The coordinates of the value to be interpolated.
     * \param success A bool flag that indicates failure/success.
     *
     * \return The interpolated value.
     */
    ResultType interpolate( typename InterpolationPrecisionT::CoordinateType const& coords, bool* success ) const
    {
        WeightsArray weights;
        DataRefArray values;

        InterpolationPrecisionT::getWeightsAndValues( m_grid, m_valueSet, coords, success, weights, values );
        if( *success )
        {
            return TypeInterpolatorT::combine( InterpolationPrecisionT(), weights, values );
        }
        else
        {
            return ResultType();
        }
    }

private:

    /**
     * Constructor.
     *
     * \param grid The grid.
     * \param valueSet The valueset.
     */
    WInterpolator( typename InterpolationPrecisionT::GridType::ConstSPtr const& grid,
                   typename TypeInterpolatorT::ValueSetType::ConstSPtr const& valueSet )
        : m_grid( grid ),
          m_valueSet( valueSet )
    {
    }

    //! The grid.
    typename InterpolationPrecisionT::GridType::ConstSPtr m_grid;

    //! The valueset.
    typename TypeInterpolatorT::ValueSetType::ConstSPtr m_valueSet;
};

#endif  // WINTERPOLATOR_H
