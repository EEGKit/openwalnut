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

#ifndef WDATASETPOINTSANDSIZES_TEST_H
#define WDATASETPOINTSANDSIZES_TEST_H

#include <vector>

#include <boost/shared_ptr.hpp>
#include <cxxtest/TestSuite.h>
#include "../WDataSetPointsAndSizes.h"


/**
 * Test constructor of WDataSetPointsAndSizes class
 */

class WDataSetPointsAndSizesTest : public CxxTest::TestSuite
{
public:
    /**
     * Test instantiation assert of WDatasetPointAndSizes class
     */
    void testInstantiationAssert( void )
    {
        WDataSetPointsAndSizes::VertexArray vertex = boost::shared_ptr< std::vector< float > >( new std::vector< float >() );
        WDataSetPointsAndSizes::ColorArray color = boost::shared_ptr< std::vector< float > >( new std::vector< float >() );
        WDataSetPointsAndSizes::SizeArray sizes = boost::shared_ptr< std::vector< float > >( new std::vector< float >() );

        vertex->push_back( 1.0 );
        vertex->push_back( 2.0 );
        vertex->push_back( 3.0 );

        color->push_back( 0.2 );
        color->push_back( 0.11 );
        color->push_back( 0.7 );

        TS_ASSERT_THROWS_ANYTHING( WDataSetPointsAndSizes test( vertex, color, sizes ) );
    }

    /**
     * Test instantiation of WDataSetPointsAndSizes class
     */
    void testInstantiation( void )
    {
        WDataSetPointsAndSizes::VertexArray vertex = boost::shared_ptr< std::vector< float > >( new std::vector< float >() );
        WDataSetPointsAndSizes::ColorArray color = boost::shared_ptr< std::vector< float > >( new std::vector< float >() );
        WDataSetPointsAndSizes::SizeArray sizes = boost::shared_ptr< std::vector< float > >( new std::vector< float >() );

        vertex->push_back( 1.0 );
        vertex->push_back( 2.0 );
        vertex->push_back( 3.0 );

        color->push_back( 0.2 );
        color->push_back( 0.11 );
        color->push_back( 0.7 );

        sizes->push_back( 1.2 );

        TS_ASSERT_THROWS_NOTHING( WDataSetPointsAndSizes test( vertex, color, sizes ) );
    }

    /**
     * Test function getSize() of WDatatSetPointsAndSizes class
     */
    void testGetSize( void )
    {
        WDataSetPointsAndSizes::VertexArray vertex = boost::shared_ptr< std::vector< float > >( new std::vector< float >() );
        WDataSetPointsAndSizes::ColorArray color = boost::shared_ptr< std::vector< float > >( new std::vector< float >() );
        WDataSetPointsAndSizes::SizeArray sizes = boost::shared_ptr< std::vector< float > >( new std::vector< float >() );

        vertex->push_back( 1.0 );
        vertex->push_back( 2.0 );
        vertex->push_back( 3.0 );

        color->push_back( 0.2 );
        color->push_back( 0.11 );
        color->push_back( 0.7 );

        sizes->push_back( 1.2 );

        WDataSetPointsAndSizes testset( vertex, color, sizes );

        TS_ASSERT_THROWS_ANYTHING( testset.getSize( 1 ) );
    }
};

#endif  // WDATASETPOINTSANDSIZES_TEST_H
