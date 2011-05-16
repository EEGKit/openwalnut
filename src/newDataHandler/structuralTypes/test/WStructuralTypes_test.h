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
 * A simple type for testing which gets mapped by TestStructuralType.
 *
 * \tparam IntegralT an integral type
 * \tparam Size A size
 */
template< typename IntegralT, size_t Size >
class TestRealType
{
public:
    /**
     * The integral type.
     */
    typedef IntegralT IntegralType;

    /**
     * The size of the thing.
     */
    typedef boost::mpl::size_t< Size > TheSize;
};

/**
 * A simple structural type for testing WStructuralTypeResolution and WStructuralTypeStore.
 */
class TestStructuralType
{
public:
    /**
     * The vector storing the compile-time parameters of the real-type.
     */
    typedef boost::mpl::vector< WIntegralVariant, WNaturalNumbersVariant > ParameterVector;  // NOLINT - No, this is not a std::vector

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
        typedef TestRealType< typename boost::mpl::at< RealParameterVector, boost::mpl::size_t< 0 > >::type,
                                       boost::mpl::at< RealParameterVector, boost::mpl::size_t< 1 > >::type::value > Type;
    };

    /**
     * Converts the given sample to real type.
     *
     * \tparam SampleT type of the sample
     *
     * \return the structural type sample
     */
    template< typename SampleT >
    static WStructuralTypeStore< ParameterVector > FromRealType( const SampleT& /* sample */ )
    {
        WStructuralTypeStore< ParameterVector > s;
        s.getVariant() = typename SampleT::IntegralType();
        s.next().getVariant() = typename SampleT::TheSize();
        return s;
    }
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
     */
    void operator()( TestRealType< double, 5 > /* x */ )
    {
        m_returnValue = 3;
    }

    /**
     * Operator called by resolving mechanism. uint8_t Version.
     *
     * \tparam T the value-type stored in the instance ow TestStructuralType.
     */
    void operator()( TestRealType< uint8_t, 4 > /* x */ )
    {
        m_returnValue = 4;
    }

    /**
     * Operator called by resolving mechanism. Only typed for the size element.
     *
     * \tparam T the value-type stored in the instance ow TestStructuralType.
     */
    template< typename T >
    void operator()( TestRealType< T, 2 > /* x */ )
    {
        m_returnValue = 5;
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
        TestFunctor fDouble_5;
        TestFunctor fOther_2;
        TestFunctor fConstOther;

        // Create a structural type instance and initialize its variants
        PS psUint8 = TestStructuralType::FromRealType( TestRealType< uint8_t, 4 >() );


        PS psDouble_5;
        psDouble_5.getVariant() = double();
        psDouble_5.next().getVariant() = boost::mpl::size_t< 5 >();

        PS psOther_2;
        psOther_2.getVariant() = float();
        psOther_2.next().getVariant() = boost::mpl::size_t< 2 >();

        // Do resolution
        PR prUint8( psUint8, fUint8 );
        prUint8.resolve();
        TS_ASSERT( fUint8.getReturnValue() == 4 );

        PR prDouble_5( psDouble_5, fDouble_5 );
        prDouble_5.resolve();
        TS_ASSERT( fDouble_5.getReturnValue() == 3 );

        PR prOther_2( psOther_2, fOther_2 );
        prOther_2.resolve();
        TS_ASSERT( fOther_2.getReturnValue() == 5 );

        // a const resolver needs to call the non-const operator
        const PR prConstOther_2( psOther_2, fConstOther );
        prConstOther_2.resolve();
        TS_ASSERT( fConstOther.getReturnValue() != 1 );
    }
};

#endif  // WSTRUCTURALTYPES_TEST_H

