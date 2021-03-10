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

#ifndef WMCSVCONVERTER_TEST_H
#define WMCSVCONVERTER_TEST_H

#include <vector>
#include <string>
#include <cxxtest/TestSuite.h>

#include "../WMCsvConverter.h"

/**
 * Test class of WMCsvConverter class
 */

class WMCsvConverterTest : public CxxTest::TestSuite
{
public:
    /**
     * Test the constructor of WMCsvConverter, so no exception is thrown, when input parameters are correct
     */
    void testConstructor()
    {
    }
};

#endif  // WMCSVCONVERTER_TEST_H
