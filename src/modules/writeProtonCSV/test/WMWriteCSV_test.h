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

#include <iostream>
#include <list>
#include <vector>

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
     * A shared_ptr for a float vector
     */
    typedef boost::shared_ptr< std::vector< float > > SPFloatVector;

    /**
     * A shared_ptr for a size_t vector
     */
    typedef boost::shared_ptr< std::vector< size_t > > SPSizeVector;

    /**
     * A shared_ptr for an int vector
     */
    typedef boost::shared_ptr< std::vector< int > > SPIntVector;

    /**
     * test for converting a fiber to a tuple of vectors and ids
     */
    void testgetListOfInternalVertex()
    {
        WMWriteCSV writerCSV;
        std::tuple < WDataSetFibers::SPtr, std::vector< std::tuple < osg::Vec3, int > > > samples = createSampleFibers();
        WDataSetFibers::SPtr fibers = std::get< 0 >( samples );
        std::vector< std::tuple < osg::Vec3, int > > referenceList = std::get< 1 >( samples );
        std::list< std::tuple < osg::Vec3, int > > testList = writerCSV.getListOfInternalVertex( fibers );

        TS_ASSERT_EQUALS( referenceList.size(), testList.size() );

        size_t referenceListCounter = 0;
        for( auto element = testList.begin(); element != testList.end(); element++  )
        {
            std::tuple < osg::Vec3, int > refTuple = referenceList.at( referenceListCounter++ );

            osg::Vec3 refVector = std::get< 0 >( refTuple );
            int refIndex = std::get< 1 >( refTuple );

            osg::Vec3 testVector = std::get< 0 >( *element );
            int testIndex = std::get< 1 >( *element );

            TS_ASSERT_EQUALS( refVector.x(), testVector.x() );
            TS_ASSERT_EQUALS( refVector.y(), testVector.y() );
            TS_ASSERT_EQUALS( refVector.z(), testVector.z() );
            TS_ASSERT_EQUALS( refIndex, testIndex );
        }
    }
private:
    /**
     * helpermethod that generates a fiber example
     * \return std::tuple < WDataSetFibers::SPtr, std::vector< std::tuple < osg::Vec3, int > > > The fibers
     */
    std::tuple < WDataSetFibers::SPtr, std::vector< std::tuple < osg::Vec3, int > > > createSampleFibers()
    {
        std::vector< std::tuple < osg::Vec3, int > > referenceList;

        SPFloatVector vertices = SPFloatVector( new std::vector< float >() );
        SPSizeVector fiberStartIndexes = SPSizeVector( new std::vector< size_t >() );
        SPSizeVector fiberLengths = SPSizeVector( new std::vector< size_t >() );
        SPSizeVector verticesReverse = SPSizeVector( new std::vector< size_t >() );
        SPIntVector eventIDs = SPIntVector( new std::vector< int >() );

        size_t eventIDCounter = 0;
        size_t counter = 0;

        //create vertices
        for( size_t vertexCounter = 0; vertexCounter < 10; vertexCounter++ )
        {
            osg::Vec3 vectorTemp( vertexCounter, vertexCounter, vertexCounter );

            vertices->push_back( vertexCounter );
            vertices->push_back( vertexCounter );
            vertices->push_back( vertexCounter );

            if( counter == 2 )
            {
                counter = 0;
                eventIDCounter++;
            }

            eventIDs->push_back( eventIDCounter );

            std::tuple < osg::Vec3, int > tupleTemp( vectorTemp, eventIDCounter );

            referenceList.push_back( tupleTemp );
            counter++;
        }

        int fiberLength = 0;
        int fiberStartIndex = 0;
        int reversePos = 0;
        size_t currentEventID = eventIDs->at( 0 );

        fiberStartIndexes->push_back( fiberStartIndex );

        //calculate fibers
        for( int eID : *eventIDs )
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
        fiberStartIndexes->push_back( fiberLength );

        WDataSetFibers::SPtr fibers = boost::shared_ptr< WDataSetFibers >( new WDataSetFibers(
                vertices,
                fiberStartIndexes,
                fiberLengths,
                verticesReverse
        ) );

        std::tuple < WDataSetFibers::SPtr, std::vector< std::tuple < osg::Vec3, int > > > returnTuple( fibers, referenceList );
        return returnTuple;
    }
};

#endif  // WMWRITECSV_TEST_H
