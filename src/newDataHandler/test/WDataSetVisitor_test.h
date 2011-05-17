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

#ifndef WDATASETVISITOR_TEST_H
#define WDATASETVISITOR_TEST_H

#include <boost/shared_ptr.hpp>

#include <cxxtest/TestSuite.h>

#include "../structuralTypes/WParameterTypes.h"
#include "../WDataSetVisitor.h"
#include "../WValueMapper.h"

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
    static const size_t TheSize = Size;
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
        s.next().getVariant() = boost::mpl::size_t< SampleT::TheSize >();
        return s;
    }

    /**
     * Tests validity of the given type.
     *
     * \tparam SampleT sample type to check
     *
     * \return true if valid.
     */
    template< typename SampleT >
    static bool ValidateType( const SampleT& /* sample */ )
    {
        // only accept these integral types
        BOOST_MPL_ASSERT( ( boost::mpl::contains< WIntegralTypesVector, typename SampleT::IntegralType > ) );
        BOOST_MPL_ASSERT( ( boost::mpl::contains< WNaturalNumbersTypeVector, boost::mpl::size_t< SampleT::TheSize > > ) );

        // we should do this statically using:
        // BOOST_STATIC_ASSERT( SampleT::TheSize != 0 );
        // but we want to test reaction later to invalid samples, so do at runtime:
        return ( SampleT::TheSize != 0 );
    }
};

/**
 * Simple test grid.
 */
class TestGrid
{
public:
    /**
     * Abbreviation for shared ptr
     */
    typedef boost::shared_ptr< TestGrid > SPtr;

    /**
     * Abbreviation for shared ptr
     */
    typedef boost::shared_ptr< const TestGrid > ConstSPtr;

    /**
     * Return number of voxels in grid
     *
     * \return number of voxels
     */
    size_t size() const
    {
        return 8;   // simple 2x2x2 grid
    }
};

/**
 * Simple functor class to test WDataSetVisitor. It therefore provides several operator() to check that the correct one is called.
 */
class TestVisitor: public WDataSetVisitor< TestGrid, TestStructuralType, TestVisitor >
{
public:
    /**
     * Constructor.
     */
    TestVisitor():
        WDataSetVisitor(),
        m_returnValue( 0 )
    {
    }

    /**
     * Operator called by resolving mechanism.
     *
     * \tparam T the real type.
     */
    template< typename T >
    void operator()( T /* x */ )
    {
        m_returnValue = 1;
    }

    /**
     * Operator called by resolving mechanism.
     *
     * \tparam T the real type.
     */
    void operator()( TestRealType< double, 3 > /* x */ )
    {
        m_returnValue = 2;
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
 * Test important functionality of WDataSetVisitor class
 */
class WDataSetVisitorTest : public CxxTest::TestSuite
{
public:

    /**
     * Test resolution of different types.
     */
    void testVisit()
    {
        // we need a value mapper instance
        typedef WValueMapper< TestVisitor::GridType, TestVisitor::StructuralType > ValueMapper;
        TestVisitor::GridType::SPtr grid = TestVisitor::GridType::SPtr( new TestVisitor::GridType() );
        ValueMapper::SPtr vm = ValueMapper::SPtr( new ValueMapper( grid, TestRealType< double, 3 >() ) );

        // create our visitor
        TestVisitor* visitor = new TestVisitor();
        vm->applyVisitor( visitor );

        TS_ASSERT( visitor->getReturnValue() == 2 )
    }
};

#endif  // WDATASETVISITOR_TEST_H

