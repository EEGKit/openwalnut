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

#ifndef WPARAMETERTYPES_H
#define WPARAMETERTYPES_H

#include <stdint.h>

#include <boost/variant.hpp>
#include <boost/mpl/size_t.hpp>
#include <boost/mpl/vector.hpp>

/**
 * The possible integral types that are allowed in our structural types.
 */
typedef boost::mpl::vector< uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, uint64_t, int64_t, float, double > WIntegralTypesVector;

/**
 * Natural numbers as MPL types.
 */
typedef boost::mpl::vector< boost::mpl::size_t< 0 >, boost::mpl::size_t< 1 >, boost::mpl::size_t< 2 >,
                            boost::mpl::size_t< 3 >, boost::mpl::size_t< 4 >, boost::mpl::size_t< 5 >,
                            boost::mpl::size_t< 6 >, boost::mpl::size_t< 7 >, boost::mpl::size_t< 8 >,
                            boost::mpl::size_t< 9 > > WNaturalNumbersTypeVector;

/**
 * Natural numbers as MPL types without 0.
 */
typedef boost::mpl::vector<                          boost::mpl::size_t< 1 >, boost::mpl::size_t< 2 >,
                            boost::mpl::size_t< 3 >, boost::mpl::size_t< 4 >, boost::mpl::size_t< 5 >,
                            boost::mpl::size_t< 6 >, boost::mpl::size_t< 7 >, boost::mpl::size_t< 8 >,
                            boost::mpl::size_t< 9 > > WNaturalNumbersNoZeroTypeVector;

/**
 * Even natural numbers.
 */
typedef boost::mpl::vector< boost::mpl::size_t< 0 >, boost::mpl::size_t< 2 >, boost::mpl::size_t< 4 >,
                            boost::mpl::size_t< 6 >, boost::mpl::size_t< 8 >, boost::mpl::size_t< 10 >,
                            boost::mpl::size_t< 12 >, boost::mpl::size_t< 14 >, boost::mpl::size_t< 16 > > WEvenNumbersTypeVector;


/**
 * Construct the variant from above vector for allowed integral types.
 */
typedef boost::make_variant_over< WIntegralTypesVector >::type WIntegralVariant;

/**
 * Natural numbers as MPL types.
 */
typedef boost::make_variant_over< WNaturalNumbersTypeVector >::type WNaturalNumbersVariant;

/**
 * Natural numbers without 0.
 */
typedef boost::make_variant_over< WNaturalNumbersNoZeroTypeVector >::type WNaturalNumbersNoZeroVariant;

/**
 * Even numbers as MPL types.
 * This is needed as orders for tensors.
 */
typedef boost::make_variant_over< WEvenNumbersTypeVector >::type WEvenNumbersVariant;

/**
 * Use even numbers as allowed orders for symmetric spherical harmonic and tensor types.
 */
typedef WEvenNumbersVariant SymmetricOrderVariant;

/**
 * A traits template for variants that provides functions and types used for runtime
 * variant initialization. Needed to construct variants from runtime parameters, such as
 * values loaded from a file.
 *
 * \tparam VariantT The type of the variant.
 */
template< typename VariantT >
struct VariantRuntimeValue;

/**
 * A runtime specific types and functions for WIntegralVariant.
 */
template<>
struct VariantRuntimeValue< WIntegralVariant >
{
    //! The type of the runtime value to init the variant from.
    typedef std::size_t ValueType;

    /**
     * Construct an initialized variant from the runtime value. This uses
     * the datatype codes also used by nifti headers.
     *
     * \param v The runtime value.
     * \return The initialized variant.
     */
    static WIntegralVariant variantFromValue( ValueType const& v )
    {
        WIntegralVariant var;
        switch( v )
        {
        case 2: // W_DT_UNSIGNED_CHAR
            var = uint8_t();
            break;
        case 256: // W_DT_INT8
            var = int8_t();
            break;
        case 512: // W_DT_UINT16
            var = uint16_t();
            break;
        case 4: // W_DT_SIGNED_SHORT
            var = int16_t();
            break;
        case 768: // W_DT_UINT32
            var = uint32_t();
            break;
        case 8: // W_DT_SIGNED_INT
            var = int();
            break;
        case 1280: // W_DT_UINT64
            var = uint64_t();
            break;
        case 1024: // W_DT_INT64
            var = int64_t();
            break;
        case 16: // W_DT_FLOAT
            var = float();
            break;
        case 64: // W_DT_DOUBLE
            var = double();
            break;
        }
        return var;
    }
};

/**
 * A runtime specific types and functions for WNaturalNumbersVariant.
 */
template<>
struct VariantRuntimeValue< WNaturalNumbersVariant >
{
    //! The type of the runtime value to init the variant from.
    typedef std::size_t ValueType;

    /**
     * Construct an initialized variant from the runtime value.
     *
     * \param v The runtime value.
     * \return The initialized variant.
     */
    static WNaturalNumbersVariant variantFromValue( ValueType const& v )
    {
        WNaturalNumbersVariant var;
        switch( v )
        {
        case 0:
            var = boost::mpl::size_t< 0 >();
            break;
        case 1:
            var = boost::mpl::size_t< 1 >();
            break;
        case 2:
            var = boost::mpl::size_t< 2 >();
            break;
        case 3:
            var = boost::mpl::size_t< 3 >();
            break;
        case 4:
            var = boost::mpl::size_t< 4 >();
            break;
        case 5:
            var = boost::mpl::size_t< 5 >();
            break;
        case 6:
            var = boost::mpl::size_t< 6 >();
            break;
        case 7:
            var = boost::mpl::size_t< 7 >();
            break;
        case 8:
            var = boost::mpl::size_t< 8 >();
            break;
        case 9:
            var = boost::mpl::size_t< 9 >();
            break;
        }
        return var;
    }
};

/**
 * A runtime specific types and functions for WNaturalNumbersNoZeroVariant.
 */
template<>
struct VariantRuntimeValue< WNaturalNumbersNoZeroVariant >
{
    //! The type of the runtime value to init the variant from.
    typedef std::size_t ValueType;

    /**
     * Construct an initialized variant from the runtime value.
     *
     * \param v The runtime value.
     * \return The initialized variant.
     */
    static WNaturalNumbersNoZeroVariant variantFromValue( ValueType const& v )
    {
        WNaturalNumbersNoZeroVariant var;
        switch( v )
        {
        case 1:
            var = boost::mpl::size_t< 1 >();
            break;
        case 2:
            var = boost::mpl::size_t< 2 >();
            break;
        case 3:
            var = boost::mpl::size_t< 3 >();
            break;
        case 4:
            var = boost::mpl::size_t< 4 >();
            break;
        case 5:
            var = boost::mpl::size_t< 5 >();
            break;
        case 6:
            var = boost::mpl::size_t< 6 >();
            break;
        case 7:
            var = boost::mpl::size_t< 7 >();
            break;
        case 8:
            var = boost::mpl::size_t< 8 >();
            break;
        case 9:
            var = boost::mpl::size_t< 9 >();
            break;
        }
        return var;
    }
};

/**
 * A runtime specific types and functions for WEvenNumbersVariant.
 */
template<>
struct VariantRuntimeValue< WEvenNumbersVariant >
{
    //! The type of the runtime value to init the variant from.
    typedef std::size_t ValueType;

    /**
     * Construct an initialized variant from the runtime value.
     *
     * \param v The runtime value.
     * \return The initialized variant.
     */
    static WEvenNumbersVariant variantFromValue( ValueType const& v )
    {
        WEvenNumbersVariant var;
        switch( v )
        {
        case 0:
            var = boost::mpl::size_t< 0 >();
            break;
        case 2:
            var = boost::mpl::size_t< 2 >();
            break;
        case 4:
            var = boost::mpl::size_t< 4 >();
            break;
        case 6:
            var = boost::mpl::size_t< 6 >();
            break;
        case 8:
            var = boost::mpl::size_t< 8 >();
            break;
        case 10:
            var = boost::mpl::size_t< 10 >();
            break;
        case 12:
            var = boost::mpl::size_t< 12 >();
            break;
        case 14:
            var = boost::mpl::size_t< 14 >();
            break;
        case 16:
            var = boost::mpl::size_t< 16 >();
            break;
        }
        return var;
    }
};

#endif  // WPARAMETERTYPES_H

