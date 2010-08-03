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

#ifndef WVALUESETBASE_TEST_H
#define WVALUESETBASE_TEST_H

#include <cxxtest/TestSuite.h>

#include "../WValueSetBase.h"
#include "../WDataHandlerEnums.h"

/**
 * Dummy class for testing the abstract class WValueSetBase
 */
class Dummy : public WValueSetBase
{
friend class WValueSetBaseTest;

public:
    /**
     * Standard constructor of Dummy class.
     */
    Dummy()
        : WValueSetBase( 0, 1, W_DT_INT8 )
    {
    }

    /**
     * Constructor of Dummy class for testing
     * \param dimension tensor dimension
     */
    explicit Dummy( char dimension )
        : WValueSetBase( 0, dimension, W_DT_INT8 )
    {
    }

    /**
     * Destructor.
     */
    virtual ~Dummy()
    {
    }

    /**
     * Get the size.
     *
     * \return The size.
     */
    virtual size_t size() const
    {
        return 255;
    }

    /**
     * Get the raw size.
     *
     * \return The raw size.
     */
    virtual size_t rawSize() const
    {
        return 255;
    }

    /**
     * Get the value.
     *
     * \param i The position of the value.
     * \return The value at position i.
     */
    virtual double getScalarDouble( size_t i ) const
    {
        return 255;
    }
};

/**
 * Testing abstract class via a Dummy class derived esp. for this purpose.
 */
class WValueSetBaseTest : public CxxTest::TestSuite
{
public:
    /**
     *  Checks if the Dummy is instanceable.
     */
    void testInstantiation( void )
    {
        Dummy d;
    }

    /**
     *  Checks if the dimension using the dummy is right
     */
    void testDimension( void )
    {
        Dummy d1;
        TS_ASSERT_EQUALS( d1.dimension(), 1 );
        Dummy d2( 2 );
        TS_ASSERT_EQUALS( d2.dimension(), 2 );
    }
    /**
     *  Checks if the dimension using the dummy is right
     */
    void testDataType( void )
    {
        Dummy d1;
        TS_ASSERT_EQUALS( d1.getDataType(), W_DT_INT8 );
    }
};

#endif  // WVALUESETBASE_TEST_H
