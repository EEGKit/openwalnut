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

#ifndef WSCALARSTRUCTURAL_H
#define WSCALARSTRUCTURAL_H

#include <boost/static_assert.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/contains.hpp>

#include "WStructuralTypes.h"
#include "WParameterTypes.h"

/**
 * Structural type, representing a single scalar value. The only parameter of a scalar type is its integral type. For details, have a look at
 * \ref WStructuralTypeExample.
 *
 * \see WStructuralTypeExample
 */
class WScalarStructural
{
public:
    /**
     * A type-vector containing all the needed parameters for this structural type. In this case, only the integral type variant
     */
    typedef boost::mpl::vector< WIntegralVariant > ParameterVector;   // NOLINT - no this is not a std::vector

    /**
     * Converts the structural type to the real type by using the resolved types vector.
     *
     * \tparam RealParameterVector resolved types. Defines the real type
     */
    template< typename RealParameterVector >
    struct ToRealType
    {
        /**
         * This typedef reconstructs the real type.
         */
        typedef typename mpl::at< RealParameterVector, mpl::size_t< 0 > >::type Type;
    };

    /**
     * This function is the inverse of ToRealType. It creates an structural type sample instance describing the real type's compile time parameters.
     *
     * \tparam SampleT the sample of real type to convert to a structural type sample.
     *
     * \return the sample.
     */
    template< typename SampleT >
    static WStructuralTypeStore< ParameterVector > FromRealType( const SampleT& /* sample */ )
    {
        // First: create the StructuralTypeStore
        WStructuralTypeStore< ParameterVector > s;

        // get the variant for the integral type. This is the first element in the ParameterVector
        s.getVariant() = SampleT();
        return s;
    }

    /**
     * This function checks for validity of a given sample type.
     *
     * \tparam SampleT the type to check
     *
     * \return true if valid
     */
    template< typename SampleT >
    static bool ValidateType( const SampleT& /* sample */ )
    {
        // only accept these integral types
        BOOST_MPL_ASSERT( ( boost::mpl::contains< WIntegralTypesVector, SampleT > ) );
        return true;
    }
};

#endif  // WSCALARSTRUCTURAL_H

