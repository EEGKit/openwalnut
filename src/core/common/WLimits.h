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

#ifndef WLIMITS_H
#define WLIMITS_H

#include <stdint.h>

#include <cstddef>
#include <limits>

#include <boost/math/special_functions/fpclassify.hpp> // isnan, isinf

/**
 * Project wide limits for different quantities.
 */
namespace wlimits
{
    inline extern const double MAX_DOUBLE = std::numeric_limits< double >::max(); //!< Maximum double value

    inline extern const float MAX_FLOAT = std::numeric_limits< float >::max(); //!< Maximum float value

    inline extern const size_t MAX_SIZE_T = std::numeric_limits< size_t >::max(); //!< Maximum size value

    inline extern const int32_t MAX_INT32_T = std::numeric_limits< int32_t >::max(); //!< Maximum int32_t value

    inline extern const double MIN_DOUBLE = std::numeric_limits< double >::min(); //!< Positive minimum double value

    inline extern const float MIN_FLOAT = std::numeric_limits< float >::min(); //!< Positive minimum float value

    inline extern const size_t MIN_SIZE_T = std::numeric_limits< size_t >::min(); //!< Lowest/Minimum size value (equivalent to LOWEST_SIZE_T)

    inline extern const int32_t MIN_INT32_T = std::numeric_limits< int32_t >::min(); //!< Lowest/Minimum int32_t value (equivalent to LOWEST_INT32_T)

    inline extern const double LOWEST_DOUBLE = std::numeric_limits< double >::lowest(); //!< Minimum double value

    inline extern const float LOWEST_FLOAT = std::numeric_limits< float >::lowest(); //!< Minimum float value

    inline extern const size_t LOWEST_SIZE_T = std::numeric_limits< size_t >::lowest(); //!< Lowest/Minimum size value (equivalent to MIN_SIZE_T)

    /**
     * Lowest/Minimum int32_t value (equivalent to MIN_INT32_T)
     */
    inline extern const int32_t LOWEST_INT32_T = std::numeric_limits< int32_t >::lowest();

    /**
     * Smallest double such: 1.0 + DBL_EPS == 1.0 is still true.
     */
    inline extern const double DBL_EPS = std::numeric_limits< double >::epsilon();

    /**
     * Smallest float such: 1.0 + FLT_EPS == 1.0 is still true.
     */
    inline extern const float FLT_EPS = std::numeric_limits< float >::epsilon();

    /**
     * Determines if a number is considered as NaN (aka Not a Number) or not.
     *
     * \note The reason for using here a wrapper to cmath's isnan is that it is only included in C99 which is not part of any existing C++ standard yet.
     *
     * \param value The value to be checked
     *
     * \return True if the value is a NaN, false otherwise.
     */
    template< typename T > bool isNaN( T value );

    /**
     * Determines if a number is considered as infinity or not.
     *
     * \note The reason for using here a wrapper to cmath's isinf is that it is only included in C99 which is not part of any existing C++ standard yet.
     *
     * \param value The value to be checked
     *
     * \return True if the value is infinity, false otherwise.
     */
    template< typename T > bool isInf( T value );
}

template< typename T > bool wlimits::isNaN( T value )
{
    return ( boost::math::isnan )( value );
}

template< typename T > bool wlimits::isInf( T value )
{
    return ( boost::math::isinf )( value );
}

#endif  // WLIMITS_H
