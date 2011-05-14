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

#include <cxxtest/TestSuite.h>
#include <boost/function.hpp>

#include "../WDataSetVisitor.h"

#include <boost/mpl/at.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/size_t.hpp>
#include <boost/mpl/size.hpp>
#include <boost/variant.hpp>

class A
{
public:
    typedef boost::mpl::vector< IntegralVariant > ParameterVector;

    template< typename RealParameterVector >
    struct ToRealType
    {
        typedef typename boost::mpl::at< RealParameterVector, boost::mpl::size_t< 0 > >::type Type;
    };
};

class functor
{
public:
    template< typename T >
    void operator()( T /* x */ ) const
    {
        returnValue = 1;
    }

    template< typename T >
    void operator()( T /* x */ )
    {
        returnValue = 2;
    }

    void operator()( IntegralVariant /* x */ )
    {
        returnValue = 3;
    }

    void operator()( double /* x */ )
    {
        returnValue = 4;
    }

    void operator()( uint8_t /* x */ )
    {
        returnValue = 5;
    }

    int returnValue;
};

/**
 * Test important functionality of WDataHandler class
 */
class WDataSetVisitorTest : public CxxTest::TestSuite
{
public:
    /**
     * Instatiation should throw nothing.
     */
    void testInstantiation()
    {
        functor f;
        f.returnValue = 0;
        ParameterStore< A::ParameterVector > ps;
        ps.m_variant = uint8_t( 1 );

        typedef ParameterResolution< A, functor > PR;
        PR pr( ps, f );
        pr.resolv();
        TS_ASSERT( f.returnValue == 5 );
    }
};

#endif  // WDATASETVISITOR_TEST_H

