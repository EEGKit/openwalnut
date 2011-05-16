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

#ifndef WSTRUCTURALTYPES_TEST_H
#define WSTRUCTURALTYPES_TEST_H

#include <cxxtest/TestSuite.h>

#include "../WStructuralTypes.h"
#include "../WParameterTypes.h"

/**
 * A simple structural type for testing WStructuralTypeResolution and WStructuralTypeStore.
 */
class TestStructuralType
{
public:
    /**
     * The vector storing the compile-time parameters of the real-type.
     */
    typedef boost::mpl::vector< IntegralVariant > ParameterVector;  // NOLINT - No, this is not a std::vector

    /**
     * Template to convert the structural type to the real-type with a given list of compile-time parameters as mpl::vector.
     *
     * \tparam RealParameterVector the vector of the real compile-time types.
     */
    template< typename RealParameterVector >
    struct ToRealType
    {
        /**
         * The real-type. Directly uses the given types to construct the real type.
         */
        typedef typename boost::mpl::at< RealParameterVector, boost::mpl::size_t< 0 > >::type Type;
    };
};

/**
 * Simple functor class to test WStructuralTypeResolution. It therefore provides several operator() to check that the correct one is called.
 */
class TestFunctor
{
public:
    /**
     * Constructor.
     */
    TestFunctor():
        m_returnValue( 0 )
    {
    }

    /**
     * Operator called by resolving mechanism.
     * Const Version. Should never be called!
     *
     * \tparam T the value-type stored in the instance ow TestStructuralType.
     * \param x sample parameter.
     */
    template< typename T >
    void operator()( T /* x */ ) const
    {
        m_returnValue = 1;
    }

    /**
     * Operator called by resolving mechanism.
     * Un-typed version.
     *
     * \tparam T the value-type stored in the instance ow TestStructuralType.
     * \param x sample parameter.
     */
    template< typename T >
    void operator()( T /* x */ )
    {
        m_returnValue = 2;
    }

    /**
     * Operator called by resolving mechanism. Double version.
     *
     * \tparam T the value-type stored in the instance ow TestStructuralType.
     * \param x sample parameter.
     */
    void operator()( double /* x */ )
    {
        m_returnValue = 3;
    }

    /**
     * Operator called by resolving mechanism. uint8_t Version.
     *
     * \tparam T the value-type stored in the instance ow TestStructuralType.
     * \param x sample parameter.
     */
    void operator()( uint8_t /* x */ )
    {
        m_returnValue = 4;
    }

    /**
     * Simply returns the value set in the operators.
     *
     * \return the value of the typed operators.
     */
    int getReturnValue() const
    {
        return m_returnValue;
    }

private:
    /**
     * Simple value to check which operator was called.
     */
    int m_returnValue;
};

/**
 * Test important functionality of WStructuralTypeResolution class
 */
class WStructuralTypeResolutionTest : public CxxTest::TestSuite
{
public:

    /**
     * Test resolution of different types.
     */
    void testResolution()
    {
        // abbreviations
        typedef WStructuralTypeStore< TestStructuralType::ParameterVector > PS;
        typedef WStructuralTypeResolution< TestStructuralType, TestFunctor > PR;

        // Functor
        TestFunctor fUint8;
        TestFunctor fDouble;
        TestFunctor fOther;
        TestFunctor fConstOther;

        // Create a structural type instance and initialize its variants
        PS psUint8;
        psUint8.m_variant = uint8_t();
        PS psDouble;
        psDouble.m_variant = double();
        PS psOther;
        psOther.m_variant = float();

        // Do resolution
        PR prUint8( psUint8, fUint8 );
        prUint8.resolv();
        TS_ASSERT( fUint8.getReturnValue() == 4 );

        PR prDouble( psDouble, fDouble );
        prDouble.resolv();
        TS_ASSERT( fDouble.getReturnValue() == 3 );

        PR prOther( psOther, fOther );
        prOther.resolv();
        TS_ASSERT( fOther.getReturnValue() == 2 );

        // Const PR must not call the const operator!
        const PR prConstOther( psOther, fConstOther );
        prConstOther.resolv();
        TS_ASSERT( fConstOther.getReturnValue() != 1 );
    }
};

#endif  // WSTRUCTURALTYPES_TEST_H

