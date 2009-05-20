//---------------------------------------------------------------------------
//
// Project: OpenWalnut
//
// Copyright 2009 SomeCopyrightowner
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

#ifndef WVALUESET_TEST_H
#define WVALUESET_TEST_H

#include <cxxtest/TestSuite.h>

#include "../WValueSet.h"

/**
 * UnitTests the WValueSet class
 */
class WValueSetTest : public CxxTest::TestSuite
{
public:
    /**
     * An instantiation should never throw an exception
     */
    void testInstantiation( void )
    {
        TS_ASSERT_THROWS_NOTHING( WValueSet valueSet() );
    }

    /**
     * The number of values retrieved is the same used internally
     */
    void testGetNumberOfValues( void )
    {
        WValueSet valueSet;
        TS_ASSERT_EQUALS( valueSet.getNumberOfValues(), 0 );
        valueSet.m_numberOfValues = 42;
        TS_ASSERT_EQUALS( valueSet.getNumberOfValues(), 42 );
    }
};

#endif  // WVALUESET_TEST_H
