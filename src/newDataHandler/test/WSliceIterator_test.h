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

#ifndef WSLICEITERATOR_TEST_H
#define WSLICEITERATOR_TEST_H

#include <string>

#include <cxxtest/TestSuite.h>

#include "../WDataAccess.h"
#include "../WDataSet.h"
#include "../WVoxelIterator.h"
#include "../WGridRegular3D2.h"
#include "../WGridRegular3D2Specializations.h"
#include "../structuralTypes/WScalarStructural.h"

/**
 * Enumerates the voxels in a dataset.
 */
class EnumerateVoxels
{
public:

    /**
     * The operator called when visiting the dataset. Enumerates the voxels
     * in the dataset.
     *
     * \tparam T The type of the data in the provided dataset.
     *
     * \param access The access object for the provided dataset.
     */
    template< typename T >
    void operator() ( WDataAccess< WGridRegular3D2, T > access )
    {
        typedef typename WDataAccess< WGridRegular3D2, T >::VoxelIterator VI;
        VI vi, ve;

        int i = 0;
        for( tie( vi, ve ) = access.voxels(); vi != ve; ++vi )
        {
            *vi = i++;
        }
    }
};

/**
 * Tests a full iteration of a xy-slice for both non-const and const datasets.
 */
class TestSliceIterationXY
{
public:

    /**
     * The non-const visitor operation for this visitor. Iterates a
     * full slice and writes the data for all voxels to a stringstream.
     *
     * \tparam T The type of the data stored per voxel.
     * \param access The access object for data access.
     */
    template< typename T >
    void operator() ( WDataAccess< WGridRegular3D2, T > access ) const
    {
        std::string s = "12 13 14 15 16 17 18 19 20 21 22 23 ";
        std::stringstream ss;
        typename WDataAccess< WGridRegular3D2, T >::SliceIterator si, se;

        for( tie( si, se ) = access.slice( 0, 1 ); si != se; ++si )
        {
            ss << *si << " ";
        }
        TS_ASSERT_EQUALS( s, ss.str() );
    }

    /**
     * The const visitor operation for this visitor. Iterates a
     * full slice and writes the data for all voxels to a stringstream.
     *
     * \tparam T The type of the data stored per voxel.
     * \param access The access object for data access.
     */
    template< typename T >
    void operator() ( WDataAccessConst< WGridRegular3D2, T > access ) const
    {
        std::string s = "12 13 14 15 16 17 18 19 20 21 22 23 ";
        std::stringstream ss;
        typename WDataAccessConst< WGridRegular3D2, T >::SliceIterator si, se;

        for( tie( si, se ) = access.slice( 0, 1 ); si != se; ++si )
        {
            ss << *si << " ";
        }
        TS_ASSERT_EQUALS( s, ss.str() );
    }
};

/**
 * Tests a full iteration of a yz-slice for both non-const and const datasets.
 */
class TestSliceIterationYZ
{
public:

    /**
     * The non-const visitor operation for this visitor. Iterates a
     * full slice and writes the data for all voxels to a stringstream.
     *
     * \tparam T The type of the data stored per voxel.
     * \param access The access object for data access.
     */
    template< typename T >
    void operator() ( WDataAccess< WGridRegular3D2, T > access ) const
    {
        std::string s = "2 5 8 11 14 17 20 23 26 29 32 35 ";
        std::stringstream ss;
        typename WDataAccess< WGridRegular3D2, T >::SliceIterator si, se;

        for( tie( si, se ) = access.slice( 2, 2 ); si != se; ++si )
        {
            ss << *si << " ";
        }
        TS_ASSERT_EQUALS( s, ss.str() );
    }

    /**
     * The const visitor operation for this visitor. Iterates a
     * full slice and writes the data for all voxels to a stringstream.
     *
     * \tparam T The type of the data stored per voxel.
     * \param access The access object for data access.
     */
    template< typename T >
    void operator() ( WDataAccessConst< WGridRegular3D2, T > access ) const
    {
        std::string s = "2 5 8 11 14 17 20 23 26 29 32 35 ";
        std::stringstream ss;
        typename WDataAccessConst< WGridRegular3D2, T >::SliceIterator si, se;

        for( tie( si, se ) = access.slice( 2, 2 ); si != se; ++si )
        {
            ss << *si << " ";
        }
        TS_ASSERT_EQUALS( s, ss.str() );
    }
};

/**
 * Tests a full iteration of a xz-slice for both non-const and const datasets.
 */
class TestSliceIterationXZ
{
public:

    /**
     * The non-const visitor operation for this visitor. Iterates a
     * full slice and writes the data for all voxels to a stringstream.
     *
     * \tparam T The type of the data stored per voxel.
     * \param access The access object for data access.
     */
    template< typename T >
    void operator() ( WDataAccess< WGridRegular3D2, T > access ) const
    {
        std::string s = "3 4 5 15 16 17 27 28 29 ";
        std::stringstream ss;
        typename WDataAccess< WGridRegular3D2, T >::SliceIterator si, se;

        for( tie( si, se ) = access.slice( 1, 1 ); si != se; ++si )
        {
            ss << *si << " ";
        }
        TS_ASSERT_EQUALS( s, ss.str() );
    }

    /**
     * The const visitor operation for this visitor. Iterates a
     * full slice and writes the data for all voxels to a stringstream.
     *
     * \tparam T The type of the data stored per voxel.
     * \param access The access object for data access.
     */
    template< typename T >
    void operator() ( WDataAccessConst< WGridRegular3D2, T > access ) const
    {
        std::string s = "3 4 5 15 16 17 27 28 29 ";
        std::stringstream ss;
        typename WDataAccessConst< WGridRegular3D2, T >::SliceIterator si, se;

        for( tie( si, se ) = access.slice( 1, 1 ); si != se; ++si )
        {
            ss << *si << " ";
        }
        TS_ASSERT_EQUALS( s, ss.str() );
    }
};

/**
 * Test functionality of WSliceIterator.
 */
class WSliceIteratorTest : public CxxTest::TestSuite
{
public:

    // the test dataset looks like this:
    //
    //  ^  9 10 11
    //  |  6  7  8
    //  |  3  4  5     (z = 0)
    //  y  0  1  2
    //     x ---->
    //
    //    21 22 23
    //    18 19 20
    //    15 16 17     (z = 1)
    //    12 13 14
    //
    //    33 34 35
    //    30 31 32
    //    27 28 29     (z = 2)
    //    24 25 26
    //

    /**
     * Test construction.
     */
    void testConstruction()
    {
        typedef WSliceIterator< double > SI;

        // create an invalid iterator
        TS_ASSERT_THROWS_NOTHING( SI() );

        SI si;

        // it should be invalid
        TS_ASSERT( !si );

        // now create a valid iterator
        WGridRegular3D2 g( 3, 3, 3 );
        WValueSet< double > vs( 27 );

        TS_ASSERT_THROWS_NOTHING( SI( &g, &vs, 0, 0, 0 ) );

        SI so( &g, &vs, 0, 0, 0 );

        TS_ASSERT( so );
    }

    /**
     * Test const iter construction.
     */
    void testConstructionConst()
    {
        typedef WSliceIteratorConst< double > SI;

        // create an invalid iterator
        TS_ASSERT_THROWS_NOTHING( SI() );

        SI si;

        // it should be invalid
        TS_ASSERT( !si );

        // now create a valid iterator
        WGridRegular3D2 g( 3, 3, 3 );
        WValueSet< double > vs( 27 );

        TS_ASSERT_THROWS_NOTHING( SI( &g, &vs, 0, 0, 0 ) );

        SI so( &g, &vs, 0, 0, 0 );

        TS_ASSERT( so );
    }

    /**
     * Check if the index calculation works correctly on construction.
     */
    void testConstructionXY()
    {
        WGridRegular3D2 g( 3, 4, 3 );
        WValueSet< int > vs( 36 );

        {
            WSliceIterator< int > si( &g, &vs, 2, 0, 0 );
            TS_ASSERT_EQUALS( si.m_index, 2 );
        }
        {
            WSliceIterator< int > si( &g, &vs, 5, 0, 0 );
            TS_ASSERT_EQUALS( si.m_index, 5 );
        }
        {
            WSliceIterator< int > si( &g, &vs, 6, 0, 0 );
            TS_ASSERT_EQUALS( si.m_index, 6 );
        }
        {
            WSliceIterator< int > si( &g, &vs, 12, 0, 0 );
            TS_ASSERT_EQUALS( si.m_index, 12 );
        }
        {
            WSliceIterator< int > si( &g, &vs, 0, 0, 1 );
            TS_ASSERT_EQUALS( si.m_index, 12 );
        }
        {
            WSliceIterator< int > si( &g, &vs, 8, 0, 1 );
            TS_ASSERT_EQUALS( si.m_index, 20 );
        }
        {
            WSliceIterator< int > si( &g, &vs, 3, 0, 2 );
            TS_ASSERT_EQUALS( si.m_index, 27 );
        }
    }

    /**
     * Check if the index calculation works correctly on construction.
     */
    void testConstructionXZ()
    {
        WGridRegular3D2 g( 3, 4, 3 );
        WValueSet< int > vs( 36 );

        {
            WSliceIterator< int > si( &g, &vs, 2, 1, 1 );
            TS_ASSERT_EQUALS( si.m_index, 5 );
        }
        {
            WSliceIterator< int > si( &g, &vs, 5, 1, 1 );
            TS_ASSERT_EQUALS( si.m_index, 17 );
        }
        {
            WSliceIterator< int > si( &g, &vs, 6, 1, 1 );
            TS_ASSERT_EQUALS( si.m_index, 27 );
        }
        {
            WSliceIterator< int > si( &g, &vs, 12, 1, 1 );
            TS_ASSERT_EQUALS( si.m_index, 51 );
        }
        {
            WSliceIterator< int > si( &g, &vs, 0, 1, 0 );
            TS_ASSERT_EQUALS( si.m_index, 0 );
        }
        {
            WSliceIterator< int > si( &g, &vs, 6, 1, 0 );
            TS_ASSERT_EQUALS( si.m_index, 24 );
        }
        {
            WSliceIterator< int > si( &g, &vs, 8, 1, 2 );
            TS_ASSERT_EQUALS( si.m_index, 32 );
        }
        {
            WSliceIterator< int > si( &g, &vs, 8, 1, 3 );
            TS_ASSERT_EQUALS( si.m_index, 35 );
        }
    }

    /**
     * Check if the index calculation works correctly on construction.
     */
    void testConstructionYZ()
    {
        WGridRegular3D2 g( 3, 4, 3 );
        WValueSet< int > vs( 36 );

        {
            WSliceIterator< int > si( &g, &vs, 2, 2, 1 );
            TS_ASSERT_EQUALS( si.m_index, 7 );
        }
        {
            WSliceIterator< int > si( &g, &vs, 5, 2, 1 );
            TS_ASSERT_EQUALS( si.m_index, 16 );
        }
        {
            WSliceIterator< int > si( &g, &vs, 6, 2, 1 );
            TS_ASSERT_EQUALS( si.m_index, 19 );
        }
        {
            WSliceIterator< int > si( &g, &vs, 12, 2, 1 );
            TS_ASSERT_EQUALS( si.m_index, 37 );
        }
        {
            WSliceIterator< int > si( &g, &vs, 0, 2, 0 );
            TS_ASSERT_EQUALS( si.m_index, 0 );
        }
        {
            WSliceIterator< int > si( &g, &vs, 6, 2, 0 );
            TS_ASSERT_EQUALS( si.m_index, 18 );
        }
        {
            WSliceIterator< int > si( &g, &vs, 8, 2, 2 );
            TS_ASSERT_EQUALS( si.m_index, 26 );
        }
        {
            WSliceIterator< int > si( &g, &vs, 1, 2, 2 );
            TS_ASSERT_EQUALS( si.m_index, 5 );
        }
    }

    /**
     * Test incementing and decrementing iterators.
     */
    void testIncDecr()
    {
        WGridRegular3D2 g( 3, 3, 3 );
        WValueSet< double > vs( 27 );

        WSliceIterator< double > si( &g, &vs, 0, 0, 0 );

        WSliceIterator< double > begin = si;
        WSliceIterator< double > end( &g, &vs, 9, 0, 0 );

        for( int i = 0; i < 9; ++i )
        {
            ++si;
            TS_ASSERT( si != begin );
            TS_ASSERT( !( si == begin ) );
        }

        TS_ASSERT( si == end );

        for( int i = 0; i < 9; ++i )
        {
            --si;
        }

        TS_ASSERT( si == begin );

        ++si;
        ++begin;

        TS_ASSERT( si == begin );
    }

    /**
     * Test incementing and decrementing const iterators.
     */
    void testIncDecrConst()
    {
        WGridRegular3D2 g( 3, 3, 3 );
        WValueSet< double > vs( 27 );

        WSliceIteratorConst< double > si( &g, &vs, 0, 0, 0 );

        WSliceIteratorConst< double > begin = si;
        WSliceIteratorConst< double > end( &g, &vs, 9, 0, 0 );

        for( int i = 0; i < 9; ++i )
        {
            ++si;
            TS_ASSERT( si != begin );
            TS_ASSERT( !( si == begin ) );
        }

        TS_ASSERT( si == end );

        for( int i = 0; i < 9; ++i )
        {
            --si;
        }

        TS_ASSERT( si == begin );

        ++si;
        ++begin;

        TS_ASSERT( si == begin );
    }

    /**
     * Test self-assignment of slice iterators.
     */
    void testSelfAssignment()
    {
        WGridRegular3D2 g( 3, 3, 3 );
        WValueSet< double > vs( 27 );

        WSliceIteratorConst< double > si( &g, &vs, 0, 0, 0 );

        TS_ASSERT_THROWS_NOTHING( si = si );

        WSliceIteratorConst< double > sic( &g, &vs, 0, 0, 0 );

        TS_ASSERT_THROWS_NOTHING( sic = sic );
    }

    /**
     * Test copying and comparing of const and non-const iterators.
     */
    void testCopyAndCompare()
    {
        WGridRegular3D2 g( 3, 3, 3 );
        WValueSet< double > vs( 27 );

        WSliceIterator< double > si( &g, &vs, 0, 0, 0 );

        // copy constructor
        WSliceIteratorConst< double > sic( si );

        TS_ASSERT( sic );
        TS_ASSERT( sic == si );

        WSliceIteratorConst< double > sid;
        sid = si++;

        TS_ASSERT( sid );
        TS_ASSERT( sid == sic );

        sid++;
        TS_ASSERT( sid == si );

        TS_ASSERT( sid != sic );
    }

    /**
     * Test iteration ranges that only include a part of a slice.
     */
    void testPartialIteration()
    {
        WGridRegular3D2 g( 3, 4, 3 );
        WValueSet< int > vs( 36 );

        WVoxelIterator< WGridRegular3D2, int > vi( &g, &vs, 0 );
        WVoxelIterator< WGridRegular3D2, int > ve( &g, &vs, 36 );

        // enumerate voxels
        int i = 0;
        for( ; vi != ve; ++vi )
        {
            *vi = i++;
        }

        {
            WSliceIterator< int > si( &g, &vs, 2, 0, 1 );
            WSliceIterator< int > se( &g, &vs, 9, 0, 1 );

            std::stringstream ss;
            for( ; si != se; ++si )
            {
                ss << *si << " ";
            }

            std::string res = "14 15 16 17 18 19 20 ";

            TS_ASSERT_EQUALS( ss.str(), res );
        }

        {
            WSliceIterator< int > si( &g, &vs, 3, 1, 1 );
            WSliceIterator< int > se( &g, &vs, 9, 1, 1 );

            std::stringstream ss;
            for( ; si != se; ++si )
            {
                ss << *si << " ";
            }

            std::string res = "15 16 17 27 28 29 ";

            TS_ASSERT_EQUALS( ss.str(), res );
        }

        {
            WSliceIterator< int > si( &g, &vs, 1, 2, 2 );
            WSliceIterator< int > se( &g, &vs, 8, 2, 2 );

            std::stringstream ss;
            for( ; si != se; ++si )
            {
                ss << *si << " ";
            }

            std::string res = "5 8 11 14 17 20 23 ";

            TS_ASSERT_EQUALS( ss.str(), res );
        }
    }

    /**
     * Test iteration of full slices.
     */
    void testCompleteIteration()
    {
        typedef WStillNeedsAName< WGridRegular3D2, WScalarStructural > ValueMapper;
        boost::shared_ptr< WGridRegular3D2 > g( new WGridRegular3D2( 3, 4, 3 ) );

        ValueMapper::SPtr vm = ValueMapper::SPtr( new ValueMapper( g, double() ) );
        ValueMapper::ConstSPtr vmc = vm;

        // enumerate all voxels
        {
            EnumerateVoxels* visitor = new EnumerateVoxels();
            vm->applyVisitor( visitor );
            delete visitor;
        }

        // XY
        {
            TestSliceIterationXY* visitor = new TestSliceIterationXY();
            vm->applyVisitor( visitor );
            vmc->applyVisitor( visitor );
            delete visitor;
        }

        // XZ
        {
            TestSliceIterationXZ* visitor = new TestSliceIterationXZ();
            vm->applyVisitor( visitor );
            vmc->applyVisitor( visitor );
            delete visitor;
        }

        // YZ
        {
            TestSliceIterationYZ* visitor = new TestSliceIterationYZ();
            vm->applyVisitor( visitor );
            vmc->applyVisitor( visitor );
            delete visitor;
        }
    }
};

#endif  // WSLICEITERATOR_TEST_H
