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

#ifndef WTYPEINTERPOLATORSTANDARD_H
#define WTYPEINTERPOLATORSTANDARD_H

#include "WValueSet2.h"
#include "WInterpolationTypes.h"
#include "WGridRegular3D2.h"

/**
 * A standard functional for calculating the interpolated value in a cell of a regular 3D-grid
 * from weights and sampled values. Uses simple addition and multiplication.
 *
 * \tparam ValueT The type of the data to be interpolated.
 */
template< typename ValueT >
class WTypeInterpolatorStandard
{
public:

    // the following types must be specified.
    //! The type of the data to be interpolated.
    typedef ValueT ValueType;

    //! The type of the valueset.

    typedef typename ValueTypeTraits< ValueT >::ValueSetType ValueSetType;
    // typedef WValueSet2< ValueType > ValueSetType;

    /**
     * The actual calculation of the interpolated value. This is the version for linear
     * interpolation on the regular 3D-grid.
     *
     * \tparam WeightsArrayT The type of the array that stores the weights.
     * \tparam ValueArrayT The type of the array that stores the samples.
     *
     * \param weights The weights array.
     * \param values The values array.
     *
     * \return The interpolated value.
     */
    template< typename WeightsArrayT, typename ValueArrayT >
    inline static typename ValueArrayT::DataType combine( WInterpolationLinear< WGridRegular3D2 > const& /* not used */,
                                                          WeightsArrayT const& weights, ValueArrayT const& values )
    {
        return weights[ 0 ] * values[ 0 ] + weights[ 1 ] * values[ 1 ]
             + weights[ 2 ] * values[ 2 ] + weights[ 3 ] * values[ 3 ]
             + weights[ 4 ] * values[ 4 ] + weights[ 5 ] * values[ 5 ]
             + weights[ 6 ] * values[ 6 ] + weights[ 7 ] * values[ 7 ];
    }

    // Notes on writing your own interpolation functions (for example for quadratic interpolation):
    //
    // The first parameter should be the interpolation precision functional, for example
    // you could write a WInterpolationQuadratic. This will specify the number of elements in the weights and value arrays.
    // The return type should always be typename ValueArrayT::DataType, so that the interpolation works with the user-type-access
    // (WDataProxy).

    // this class is supposed to use addition and multiplication operators
    // if you intend to provide other methods, for example 2nd-order tensor interpolation in the log-eucledean space,
    // then add another class for them
};

#endif  // WTYPEINTERPOLATORSTANDARD_H
