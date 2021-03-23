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
#include "../WMFilterProtonData.h"
#include "../../transferFunctionColorBar/WMTransferFunctionColorBar.h"
#include "../../data/io/WReaderCSV.h"

/**
 * Test class of WCsvConverter class
 */

class WCsvConverterTest : public CxxTest::TestSuite
{
public:
    /**
     * Test the constructors of WCsvConverter, so no exception is thrown, when input parameters are correct
     */
    static void testConstructorThrowNothing()
    {
        WReaderCSV csvReader( W_FIXTURE_PATH + "../data/CSVs/valid.csv" );

        boost::shared_ptr< WDataSetCSV > csvDataSet = csvReader.read();

        WProtonData protonData( csvDataSet->getHeader(), csvDataSet->getData() );

        WModule::SPtr tmpColorBar( new WMTransferFunctionColorBar() );

        boost::shared_ptr< WPropertyStatus > tmpPropertyStatus( new WPropertyStatus() );
        WColumnPropertyHandler tmpColumnPropertyHandler( NULL, NULL, NULL );
        tmpPropertyStatus->setColumnPropertyHandler(
                boost::make_shared< WColumnPropertyHandler>( tmpColumnPropertyHandler )
        );

        WEventIDLimitationPropertyHandler tmpEventIDLimitationPropertyHandler( NULL, NULL, NULL );
        tmpPropertyStatus->setEventIDLimitationPropertyHandler(
                boost::make_shared< WEventIDLimitationPropertyHandler>( tmpEventIDLimitationPropertyHandler )
        );

        WFilterPropertyHandler tmpFilterPropertyHandler( NULL, NULL, NULL );
        tmpPropertyStatus->setFilterPropertyHandler(
                boost::make_shared< WFilterPropertyHandler>( tmpFilterPropertyHandler )
        );

        WVisualizationPropertyHandler tmpVisualizationPropertyHandler( NULL, NULL, NULL );
        tmpPropertyStatus->setVisualizationPropertyHandler(
                boost::make_shared< WVisualizationPropertyHandler>( tmpVisualizationPropertyHandler )
        );

        TS_ASSERT_THROWS_NOTHING( WCsvConverter( boost::make_shared< WProtonData >( protonData ),
                                                 tmpPropertyStatus,
                                                 tmpColorBar )
        );
    }
};

#endif  // WCSVCONVERTER_TEST_H
