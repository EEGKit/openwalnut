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

#ifndef WCSVCONVERTER_TEST_H
#define WCSVCONVERTER_TEST_H

#include <vector>
#include <string>
#include <cxxtest/TestSuite.h>

#include "core/kernel/WModuleFactory.h"
#include "../WCsvConverter.h"
#include "../../transferFunctionColorBar/WMTransferFunctionColorBar.h"

/**
 * Test class of WCsvConverter class
 */

class WCsvConverterTest : public CxxTest::TestSuite
{
public:
    /**
     * Test the constructors of WCsvConverter, so no exception is thrown, when input parameters are correct
     */
    static void testConstructorsNoThrow()
    {
        auto emptyStringVector = new std::vector< std::string >( 1, "" );

        WDataSetCSV::ContentSPtr emptyHeader( new WDataSetCSV::Content() );
        emptyHeader->push_back( *emptyStringVector );

        WDataSetCSV::ContentSPtr emptyData( new WDataSetCSV::Content() );
        emptyHeader->push_back( *emptyStringVector );

        WModule::SPtr emptyColorBar = WModule::SPtr( new WMTransferFunctionColorBar() );

        WProtonData::SPtr tmpProtonData = WProtonData::SPtr( new WProtonData( emptyHeader, emptyData ) );
        boost::shared_ptr< WPropertyStatus > tmpPropertyStatus( new WPropertyStatus() );

        TS_ASSERT_THROWS_NOTHING( WCsvConverter( tmpProtonData, tmpPropertyStatus ) );
        TS_ASSERT_THROWS_NOTHING( WCsvConverter( tmpProtonData, tmpPropertyStatus,  emptyColorBar ) );
    }

    /**
     * Test the constructor of WCsvConverter, so no exception is thrown, when input parameters are correct
     */
    static void testConstructorWithoutColorThrowNullptrParameter()
    {
        /*
        auto emptyStringVector = new std::vector< std::string >( 1, "" );

        WDataSetCSV::ContentSPtr emptyHeader( new WDataSetCSV::Content() );
        emptyHeader->push_back( *emptyStringVector );

        WDataSetCSV::ContentSPtr emptyData( new WDataSetCSV::Content() );
        emptyHeader->push_back( *emptyStringVector );

        WProtonData::SPtr tmpProtonData = WProtonData::SPtr( new WProtonData( emptyHeader, emptyData ) );
        boost::shared_ptr< WPropertyStatus > tmpPropertyStatus( new WPropertyStatus() );

        TS_ASSERT_THROWS_NOTHING( WCsvConverter( tmpProtonData, tmpPropertyStatus ) );
        */

        /*
        WDataSetCSV::ContentSPtr emptyHeader( new WDataSetCSV::Content() );
        emptyHeader->push_back( *emptyStringVector );

        WDataSetCSV::ContentSPtr emptyData( new WDataSetCSV::Content() );
        emptyHeader->push_back( *emptyStringVector );

        tmpProtonData = WProtonData::SPtr( new WProtonData( emptyHeader, emptyData ) );
        tmpPropertyStatus = nullptr;
        TS_ASSERT_THROWS( WCsvConverter( tmpProtonData, tmpPropertyStatus ), WException &e );
        */
    }
};

#endif  // WCSVCONVERTER_TEST_H
