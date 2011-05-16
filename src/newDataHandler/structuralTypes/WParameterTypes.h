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
typedef boost::make_variant_over< WEvenNumbersTypeVector > WEvenNumbersVariant;

/**
 * Use even numbers as allowed orders for symmetric spherical harmonic and tensor types.
 */
typedef WEvenNumbersVariant SymmetricOrderVariant;

#endif  // WPARAMETERTYPES_H

