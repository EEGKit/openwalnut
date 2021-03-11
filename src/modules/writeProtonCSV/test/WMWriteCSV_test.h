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

#ifndef WMWRITECSV_TEST_H
#define WMWRITECSV_TEST_H

#include <boost/shared_ptr.hpp>

#include <cxxtest/TestSuite.h>

#include "../WMWriteCSV.h"

/**
 * test class of the writeCSV class
 */
class WMWriteCSVTest : public CxxTest::TestSuite
{
public:
    /**
     * 
     */
    void testgetListOfInternalVertex()
    {
        WMWriteCSV writer();

        typedef boost::shared_ptr< std::vector< float > > SPFloatVector;
        typedef boost::shared_ptr< std::vector< size_t > > SPSizeVector;
        typedef boost::shared_ptr< std::vector< int > > SPIntVector;

        std::vector< std::tuple < osg::Vec3, int > > referenceList

        SPFloatVector vertices; 
        SPSizeVector fiberStartIndexes;   
        SPSizeVector fiberLengths;   
        SPSizeVector verticesReverse; 
        SPIntVector eventIDs;

        size_t eventIDCounter = 0;

        //create vertices
        for(sizt_t vertexCounter = 0; vertexCounter < 10; vertexCounter++)
        {
            osg::Vec3 vectorTemp(vertexCounter, vertexCounter, vertexCounter);

            vertices->push_back( vertexCounter );
            vertices->push_back( vertexCounter );
            vertices->push_back( vertexCounter );

            if (eventIDCounter % 3 == 0 )
            {
                eventIDCounter++;
            }

            eventIDs->push_back( eventIDCounter );
            referenceList->push_back(vectorTemp, eventIDCounter);
        }

        int fiberLength = 0;
        int fiberStartIndex = 0;
        int reversePos = 0;
        size_t currentEventID = eventIDs->at( 0 );

        fiberStartIndexes->push_back( fiberStartIndex );

        //calculate fibers 
        for( size_t eID : eventIDs )
        {
            if( currentEventID != eID )
            {
                fiberStartIndexes->push_back( fiberStartIndex );
                fiberLengths->push_back( fiberLength );

                currentEventID = eID;
                fiberLength = 0;

                reversePos++;
            }
            fiberLength++;
            fiberStartIndex++;
            verticesReverse->push_back( reversePos );
        }
        fiberStartIndex->push_back( fiberLength );



        WDataSetFibers::SPtr fibers = new WDataSetFibers(
                    vertices,
                    fiberStartIndexes,
                    fiberLengths,
                    verticesReverse
            );

        std::list< std::tuple < osg::Vec3, int > > testList = writer.getListOfInternalVertex( fibers );

        TS_ASSERT_EQUALS( referenceList.size, testList.size );

        size_t referenceListCounter = 0
        for( auto element = testList.begin(); element != testList.end(); element++  )
        {
            std::tuple < osg::Vec3, int > refTuple = referenceList.at( referenceListCounter++ );

            osg::Vec3 refVector = std::get< 0 >( *refTuple );
            int refIndex = std::get< 1 >( *refTuple );

            osg::Vec3 testVector = std::get< 0 >( *element );
            int testIndex = std::get< 1 >( *element );

            TS_ASSERT_EQUALS( refVector.x(), testVector.x() );
            TS_ASSERT_EQUALS( refVector.y(), testVector.y() );
            TS_ASSERT_EQUALS( refVector.z(), testVector.z() );
            TS_ASSERT_EQUALS( refIndex, testIndex );
        }
    }
};

#endif  // WMWRITECSV_TEST_H
