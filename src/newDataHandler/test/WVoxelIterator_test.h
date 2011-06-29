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

#ifndef WVOXELITERATOR_TEST_H
#define WVOXELITERATOR_TEST_H

#include <cxxtest/TestSuite.h>

#include "../WDataAccess.h"
#include "../WDataSet.h"
#include "../WVoxelIterator.h"
#include "../WGridRegular3D2.h"
#include "../structuralTypes/WScalarStructural.h"

/**
 * Used to count the number of voxels in an iterator range.
 */
class IterationRangeTestVisitor
{
public:

    /**
     * The operator called when visiting the dataset. Iterates over a dataset
     * and stores the number of voxels.
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
            ++i;
        }

        m_count = i;
    }

    /**
     * Get the number of voxels found in the iteration.
     *
     * \return The number of voxels.
     */
    int getCount()
    {
        return m_count;
    }

private:

    //! Stores the number of voxels found.
    int m_count;
};

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
 * This is used to get the data of given voxels.
 */
class DataTestVisitor
{
public:

    /**
     * The operator called when visiting the dataset. Stores the value of the
     * voxel at position m_index in m_value.
     *
     * \tparam T The type of the data in the provided dataset.
     *
     * \param access The access object for the provided dataset.
     */
    template< typename T >
    void operator() ( WDataAccess< WGridRegular3D2, T > access )
    {
        m_value = access.getAt( m_index );
    }

    /**
     * Sets the voxel coords.
     *
     * \param x The coords on the x-axis.
     * \param y The coords on the y-axis.
     * \param z The coords on the z-axis.
     */
    void setVoxel( std::size_t x, std::size_t y, std::size_t z )
    {
        m_index[ 0 ] = x;
        m_index[ 1 ] = y;
        m_index[ 2 ] = z;
    }

    /**
     * Get the value read from the voxel.
     *
     * \return The voxel's value.
     */
    double getValue()
    {
        return m_value;
    }

private:
    //! the value read from the voxel
    double m_value;

    //! the index of the voxel
    WGridRegular3D2::VoxelIndex m_index;
};

/**
 * Test functionality of WVoxelIterator.
 */
class WVoxelIteratorTest : public CxxTest::TestSuite
{
public:

    /**
     * Test construction.
     */
    void testConstruction()
    {
        typedef WVoxelIterator< WGridRegular3D2, double > VI;

        // create an invalid iterator
        TS_ASSERT_THROWS_NOTHING( VI() );

        WVoxelIterator< WGridRegular3D2, double > vi;

        // it should be invalid
        TS_ASSERT( !vi );

        // now create a valid iterator
        WGridRegular3D2 g( 3, 3, 3 );
        WValueSet< double > vs( 27 );

        TS_ASSERT_THROWS_NOTHING( VI( &g, &vs, 0 ) );

        WVoxelIterator< WGridRegular3D2, double > vo( &g, &vs, 0 );

        TS_ASSERT( vo );
    }

    /**
     * Test incementing and decrementing iterators.
     */
    void testIncDecr()
    {
        WGridRegular3D2 g( 3, 3, 3 );
        WValueSet< double > vs( 27 );

        WVoxelIterator< WGridRegular3D2, double > vi( &g, &vs, 0 );

        WVoxelIterator< WGridRegular3D2, double > begin = vi;
        WVoxelIterator< WGridRegular3D2, double > end( &g, &vs, 27 );

        for( int i = 0; i < 27; ++i )
        {
            ++vi;
            TS_ASSERT( vi != begin );
            TS_ASSERT( !( vi == begin ) );
        }

        TS_ASSERT( vi == end );

        for( int i = 0; i < 27; ++i )
        {
            --vi;
        }

        TS_ASSERT( vi == begin );

        ++vi;
        ++begin;

        TS_ASSERT( vi == begin );
    }

    /**
     * Test self-assignment of voxel iterators.
     */
    void testSelfAssignment()
    {
        WGridRegular3D2 g( 3, 3, 3 );
        WValueSet< double > vs( 27 );

        WVoxelIterator< WGridRegular3D2, double > vi( &g, &vs, 0 );

        TS_ASSERT_THROWS_NOTHING( vi = vi );
    }

    /**
     * Test if iteration aborts correctly.
     */
    void testIterationCount()
    {
        typedef WStillNeedsAName< WGridRegular3D2, WScalarStructural > ValueMapper;
        boost::shared_ptr< WGridRegular3D2 > g( new WGridRegular3D2( 3, 3, 3 ) );

        ValueMapper::SPtr vm = ValueMapper::SPtr( new ValueMapper( g, double() ) );

        IterationRangeTestVisitor* visitor = new IterationRangeTestVisitor();
        vm->applyVisitor( visitor );

        // a 3x3x3 grid has 27 voxels
        TS_ASSERT_EQUALS( visitor->getCount(), 27 );

        delete visitor;
    }

    /**
     * Test access to voxel data.
     */
    void testAccess()
    {
        typedef WStillNeedsAName< WGridRegular3D2, WScalarStructural > ValueMapper;
        boost::shared_ptr< WGridRegular3D2 > g( new WGridRegular3D2( 3, 3, 3 ) );

        ValueMapper::SPtr vm = ValueMapper::SPtr( new ValueMapper( g, double() ) );

        // enumerate all voxels
        {
            EnumerateVoxels* visitor = new EnumerateVoxels();
            vm->applyVisitor( visitor );
            delete visitor;
        }

        // now check different voxels
        DataTestVisitor dtv;

        dtv.setVoxel( 0, 0, 0 );
        vm->applyVisitor( &dtv );
        TS_ASSERT_EQUALS( dtv.getValue(), 0 );

        dtv.setVoxel( 1, 2, 0 );
        vm->applyVisitor( &dtv );
        TS_ASSERT_EQUALS( dtv.getValue(), 7 );

        dtv.setVoxel( 2, 1, 2 );
        vm->applyVisitor( &dtv );
        TS_ASSERT_EQUALS( dtv.getValue(), 23 );
    }
};

#endif  // WVOXELITERATOR_TEST_H
