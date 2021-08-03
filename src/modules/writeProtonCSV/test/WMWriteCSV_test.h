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
#include <memory>
#include <string>
#include <vector>

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
    typedef std::shared_ptr< std::vector< float > > SPFloatVector;

    /**
     * A shared_ptr for a size_t vector
     */
    typedef std::shared_ptr< std::vector< size_t > > SPSizeVector;

    /**
     * test for converting a fiber to a tuple of vectors and ids
     */
    void testgetListOfInternalVertex()
    {
        WMWriteCSV writerCSV;
        std::tuple < WDataSetFibers::SPtr, std::vector< std::tuple < float, float, float, size_t > > >
                                                                                          samples = createSampleFibers();
        WDataSetFibers::SPtr fibers = std::get< 0 >( samples );
        std::vector< std::tuple < float, float, float, size_t > > referenceList = std::get< 1 >( samples );
        std::list< std::tuple < float, float, float, size_t > > testList = writerCSV.getListOfInternalVertex( fibers );

        TS_ASSERT_EQUALS( referenceList.size(), testList.size() );

        size_t referenceListCounter = 0;
        for( auto element = testList.begin(); element != testList.end(); element++  )
        {
            std::tuple < float, float, float, size_t > refTuple = referenceList.at( referenceListCounter++ );

            osg::Vec3 refVector( std::get< 0 >( refTuple ), std::get< 1 >( refTuple ), std::get< 2 >( refTuple ) );
            size_t refIndex = std::get< 3 >( refTuple );

            osg::Vec3 testVector( std::get< 0 >( *element ), std::get< 1 >( *element ), std::get< 2 >( *element ) );
            size_t testIndex = std::get< 3 >( *element );

            TS_ASSERT_EQUALS( refVector.x(), testVector.x() );
            TS_ASSERT_EQUALS( refVector.y(), testVector.y() );
            TS_ASSERT_EQUALS( refVector.z(), testVector.z() );
            TS_ASSERT_EQUALS( refIndex, testIndex );
        }
    }

    /**
     * test the checks whether the source domain contains the specified float
     */
    void testContains()
    {
        WMWriteCSV writerCSV;

        std::list< std::tuple < std::string, float , bool > > refList;

        refList.push_back( std::tuple < std::string, float, bool >(
                "2212,1,0,4.20922e-09,4.20922e-09,0.0199097,0.0250083,725.314,-1.86984,4.55793,225.244,-1.86984,"
                "4.55793,0.0125,-0.00614681,0.0255574,0.999654,0,0,0,0,0,0,0,0,0,0,0,1,-0.154322,0.156973,-500,0,"
                "1,0,277.4,0,[0;1;0;3;-1;-1;-1;-1;-1;-1],Transportation,NULL,NULL",
                -1.86984,
                true
        ) );

        refList.push_back( std::tuple < std::string, float, bool >(
                "2212,1,0,4.20922e-09,4.20922e-09,0.0199097,0.0250083,725.314,-1.86984,4.55793,225.244,-1.86984,"
                "4.55793,0.0125,-0.00614681,0.0255574,0.999654,0,0,0,0,0,0,0,0,0,0,0,1,-0.154322,0.156973,-500,0,"
                "1,0,277.4,0,[0;1;0;3;-1;-1;-1;-1;-1;-1],Transportation,NULL,NULL",
                -123.123,
                false
        ) );

        refList.push_back( std::tuple < std::string, float, bool >(
                "2212,1,0,4.96962e-09,4.96962e-09,0.0184161,0.0250029,844.443,-9.20755,2.35148,344.394,-9.20755,"
                "2.35148,0.0125,-0.0047637,0.0141168,0.999889,1,2,0,0,0,0,0,0,0,0,0,1,-3.1341,0.319804,-500,0,32,"
                "0,461.8,0,[0;2;2;3;-1;-1;-1;-1;-1;-1],Transportation,NULL,NULL",
                0.999889,
                true
        ) );

        refList.push_back( std::tuple < std::string, float, bool >(
                "2212,1,0,4.96962e-09,4.96962e-09,0.0184161,0.0250029,844.443,-9.20755,2.35148,344.394,-9.20755,"
                "2.35148,0.0125,-0.0047637,0.0141168,0.999889,1,2,0,0,0,0,0,0,0,0,0,1,-3.1341,0.319804,-500,0,32,"
                "0,461.8,0,[0;2;2;3;-1;-1;-1;-1;-1;-1],Transportation,NULL,NULL",
                123.123,
                false
        ) );

        for( auto element : refList )
        {
            std::string sourceString = std::get<0 >( element );
            float num = std::get< 1 >( element );
            bool result = std::get< 2 >( element );

            TS_ASSERT_EQUALS( writerCSV.contains( sourceString, num ), result );
        }
    }

    /**
     * test goes through the existing EvenIDs and returns the next one.
     */
    void testCreateStartCounter()
    {
        WMWriteCSV writerCSV;

        std::list< std::tuple < float, float, float, size_t > > listOfInternalVertex;

        size_t refCounter = 0;
        for( refCounter = 0; refCounter < 100; refCounter++ )
        {
            listOfInternalVertex.push_back( std::tuple < float, float, float, size_t >( 0.0, 0.0, 0.0, refCounter ) );
        }

        TS_ASSERT_EQUALS( writerCSV.createStartCounter( listOfInternalVertex ), refCounter );

        listOfInternalVertex.clear();

        listOfInternalVertex.push_back( std::tuple < float, float, float, size_t >( 0.0, 0.0, 0.0, 13 ) );
        listOfInternalVertex.push_back( std::tuple < float, float, float, size_t >( 0.0, 0.0, 0.0, 214 ) );
        listOfInternalVertex.push_back( std::tuple < float, float, float, size_t >( 0.0, 0.0, 0.0, 44 ) );
        listOfInternalVertex.push_back( std::tuple < float, float, float, size_t >( 0.0, 0.0, 0.0, 23 ) );
        listOfInternalVertex.push_back( std::tuple < float, float, float, size_t >( 0.0, 0.0, 0.0, 64 ) );
        listOfInternalVertex.push_back( std::tuple < float, float, float, size_t >( 0.0, 0.0, 0.0, 223 ) );
        listOfInternalVertex.push_back( std::tuple < float, float, float, size_t >( 0.0, 0.0, 0.0, 132 ) );
        listOfInternalVertex.push_back( std::tuple < float, float, float, size_t >( 0.0, 0.0, 0.0, 322 ) );

        TS_ASSERT_EQUALS( writerCSV.createStartCounter( listOfInternalVertex ), 323 );
    }

private:
    /**
     * helpermethod that generates a fiber example
     * \return std::tuple < WDataSetFibers::SPtr, std::vector< std::tuple < osg::Vec3, size_t > > > The fibers
     */
    std::tuple < WDataSetFibers::SPtr, std::vector< std::tuple < float, float, float, size_t > > > createSampleFibers()
    {
        std::vector< std::tuple < float, float, float, size_t > > referenceList;

        SPFloatVector vertices = SPFloatVector( new std::vector< float >() );
        SPSizeVector fiberStartIndexes = SPSizeVector( new std::vector< size_t >() );
        SPSizeVector fiberLengths = SPSizeVector( new std::vector< size_t >() );
        SPSizeVector verticesReverse = SPSizeVector( new std::vector< size_t >() );
        SPSizeVector eventIDs = SPSizeVector( new std::vector< size_t >() );

        size_t eventIDCounter = 0;
        size_t counter = 0;

        //create vertices
        for( size_t vertexCounter = 0; vertexCounter < 10; vertexCounter++ )
        {
            vertices->push_back( vertexCounter );
            vertices->push_back( vertexCounter );
            vertices->push_back( vertexCounter );

            if( counter == 2 )
            {
                counter = 0;
                eventIDCounter++;
            }

            eventIDs->push_back( eventIDCounter );

            std::tuple < float, float, float, size_t  > tupleTemp( vertexCounter, vertexCounter, vertexCounter, eventIDCounter );

            referenceList.push_back( tupleTemp );
            counter++;
        }

        size_t fiberLength = 0;
        size_t fiberStartIndex = 0;
        size_t reversePos = 0;
        size_t currentEventID = eventIDs->at( 0 );

        fiberStartIndexes->push_back( fiberStartIndex );

        //calculate fibers
        for( size_t eID : *eventIDs )
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

        WDataSetFibers::SPtr fibers = std::shared_ptr< WDataSetFibers >( new WDataSetFibers(
                vertices,
                fiberStartIndexes,
                fiberLengths,
                verticesReverse
        ) );

        std::tuple < WDataSetFibers::SPtr, std::vector< std::tuple < float, float, float, size_t  > > >
                                                                                          returnTuple( fibers, referenceList );
        return returnTuple;
    }
};

#endif  // WMWRITECSV_TEST_H
