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

#include <boost/math/special_functions/fpclassify.hpp> // isnan, isinf

/**
 * Project wide limits for different quantities.
 */
namespace wlimits
{
    extern const double MAX_DOUBLE; //!< Maximum double value

    extern const float MAX_FLOAT; //!< Maximum float value

    extern const size_t MAX_SIZE_T; //!< Maximum size value

    extern const int32_t MAX_INT32_T; //!< Maximum int32_t value

    extern const double MIN_DOUBLE; //!< Positive minimum double value

    extern const float MIN_FLOAT; //!< Positive minimum float value

    extern const size_t MIN_SIZE_T; //!< Lowest/Minimum size value (equivalent to LOWEST_SIZE_T)

    extern const int32_t MIN_INT32_T; //!< Lowest/Minimum int32_t value (equivalent to LOWEST_INT32_T)

    extern const double LOWEST_DOUBLE; //!< Minimum double value

    extern const float LOWEST_FLOAT; //!< Minimum float value

    extern const size_t LOWEST_SIZE_T; //!< Lowest/Minimum size value (equivalent to MIN_SIZE_T)

    extern const int32_t LOWEST_INT32_T; //!< Lowest/Minimum int32_t value (equivalent to MIN_INT32_T)

    /**
     * Smallest double such: 1.0 + DBL_EPS == 1.0 is still true.
     */
    extern const double DBL_EPS;

    /**
     * Smallest float such: 1.0 + FLT_EPS == 1.0 is still true.
     */
    extern const float FLT_EPS;

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
