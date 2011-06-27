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

#ifndef WDATAACCESS_TEST_H
#define WDATAACCESS_TEST_H

#include <cxxtest/TestSuite.h>

#include "../WDataAccess.h"
#include "../WDataSet.h"
#include "../WGridRegular3D2.h"
#include "../structuralTypes/WScalarStructural.h"

/**
 * A visitor that test random access to dataset voxels.
 */
class TestVisitorSetValue
{
public:

    /**
     * Operator called by resolving mechanism.
     *
     * \tparam T the real type.
     */
    template< typename T >
    void operator()( WDataAccess< WGridRegular3D2, T > access )
    {
        access.getAt( 1, 1, 1 ) = static_cast< T >( 1.0 );
    }
};

/**
 * A visitor that reads a value from a data voxel.
 */
class TestVisitorGetValue
{
public:

    /**
     * Constructor.
     */
    TestVisitorGetValue()
        : m_result()
    {
    }

    /**
     * Operator called by resolving mechanism.
     *
     * \tparam T the real type.
     */
    template< typename T >
    void operator()( WDataAccess< WGridRegular3D2, T > access )
    {
        m_result = static_cast< double >( access.getAt( 1, 1, 1 ) );
    }

    /**
     * Get the resulting value.
     *
     * \return The value read from the voxel.
     */
    double getResult() const
    {
        return m_result;
    }

private:

    //! The value read from the voxel.
    double m_result;
};

/**
 * Test functionality of WDataAccess.
 */
class WDataAccessTest : public CxxTest::TestSuite
{
public:

    /**
     * Test access to voxel data.
     */
    void testRandomAccess()
    {
        typedef WStillNeedsAName< WGridRegular3D2, WScalarStructural > ValueMapper;
        boost::shared_ptr< WGridRegular3D2 > g( new WGridRegular3D2( 3, 3, 3 ) );

        ValueMapper::SPtr vm = ValueMapper::SPtr( new ValueMapper( g, double() ) );

        TestVisitorSetValue* visitor = new TestVisitorSetValue();
        vm->applyVisitor( visitor );

        TestVisitorGetValue* visitor2 = new TestVisitorGetValue();
        vm->applyVisitor( visitor2 );

        TS_ASSERT_EQUALS( visitor2->getResult(), 1.0 );

        delete visitor;
        delete visitor2;
    }
};

#endif  // WDATAACCESS_TEST_H
