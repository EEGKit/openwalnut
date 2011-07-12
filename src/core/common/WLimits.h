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

#include <stdint.h> // since <cstdint> is part of c++0x upcoming standard

#include <cstddef>
#include <limits>

#include <boost/math/special_functions/fpclassify.hpp> // isnan, isinf

/**
 * Project wide limits for different quantitities.
 */
namespace wlimits
{
    /**
     * Maximum sample size per axis for image
     * data like MRI, CT, funcMRI and dwMRI.
     */
    static const unsigned int MAX_IMAGE_DIMENSION = 256;

    /**
     * Maximum number of channels for a certain modality.
     */
    static const unsigned int MAX_RECORDING_CHANNELS = 1024;

    /**
     * Maximum number of samples of a recording.
     * (2^32)-1 this is often equal to UINT_MAX
     */
    static const unsigned int MAX_RECORDING_SAMPLES = 4294967295U;

    /**
     * Maximum number of segments of a recording.
     */
    static const unsigned int MAX_RECORDING_SEGMENTS = 128;

    /**
     * Maximum samplimg frequency of a recording.
     */
    static const unsigned int MAX_RECORDING_SAMPLING_FREQUENCY = 20000;

    static const double MAX_DOUBLE = std::numeric_limits< double >::max(); //!< Maximum double value

    static const float MAX_FLOAT = std::numeric_limits< float >::max(); //!< Maximum float value

    static const size_t MAX_SIZE_T = std::numeric_limits< size_t >::max(); //!< Maximum size value

    static const int32_t MAX_INT32_T = std::numeric_limits< int32_t >::max(); //!< Maximum int32_t value

    static const double MIN_DOUBLE = std::numeric_limits< double >::min(); //!< Positive minimum double value

    /**
     * Smallest double such: 1.0 + DBL_EPS == 1.0 is still true.
     */
    static const double DBL_EPS = std::numeric_limits< double >::epsilon();

    /**
     * Smallest float such: 1.0 + FLT_EPS == 1.0 is still true.
     */
    static const float FLT_EPS = std::numeric_limits< float >::epsilon();

    /**
     * Determines if a number is considered as NaN (aka Not a Number) or not.
     *
     * \note The reason for using here a wrapper to cmath's isnan is that it is only included in C99 which is not part of any existing C++ standard yet.
     *
     * \param value The value to be checked
     *
     * \return True if the value is a NaN, false otherwise.
     */
    template< typename T > bool isnan( T value );

    /**
     * Determines if a number is considered as infinity or not.
     *
     * \note The reason for using here a wrapper to cmath's isinf is that it is only included in C99 which is not part of any existing C++ standard yet.
     *
     * \param value The value to be checked
     *
     * \return True if the value is infinity, false otherwise.
     */
    template< typename T > bool isinf( T value );

    //! The maximum texture dimension.
    static std::size_t const MAX_TEXTURE_DIMENSION = 2048;

    //! We support only 8 textures because some known hardware does not support more texture coordinates.
    static std::size_t const MAX_NUMBER_OF_TEXTURES = 8;
}

template< typename T > bool wlimits::isnan( T value )
{
    return boost::math::isnan( value );
}

template< typename T > bool wlimits::isinf( T value )
{
    return boost::math::isinf( value );
}

#endif  // WLIMITS_H