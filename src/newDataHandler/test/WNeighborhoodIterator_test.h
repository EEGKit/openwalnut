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

#ifndef WNEIGHBORHOODITERATOR_TEST_H
#define WNEIGHBORHOODITERATOR_TEST_H

#include <vector>

#include <cxxtest/TestSuite.h>

#include "../WDataAccess.h"
#include "../WDataSet2.h"
#include "../WNeighborhoodIterator.h"
#include "../WGridRegular3D2.h"
#include "../structuralTypes/WScalarStructural.h"

/**
 * A visitor that creates a list of the values of the neighbors of the chosen voxel.
 */
class NeighborListVisitor
{
public:

    /**
     * Constructor.
     *
     * \param voxel The index of the voxel from which we want to get the neighbor indices.
     */
    explicit NeighborListVisitor( int voxel )
        : m_voxelnum( voxel )
    {
    }

    /**
     * The operator called when visiting the dataset. Used for any types but double.
     * Does nothing.
     *
     * \tparam T The type of the data in the provided dataset.
     */
    template< typename T >
    void operator() ( WDataAccess< WGridRegular3D2, T > /* access */ )
    {
    }

    /**
     * The operator called when visiting the dataset, double version. Iterates over a dataset
     * and stores the values of the neighbors of the chosen voxel.
     *
     * \param access The access object for the provided dataset.
     */
    void operator() ( WDataAccess< WGridRegular3D2, double > access )
    {
        typedef WDataAccess< WGridRegular3D2, double >::VoxelIterator VI;
        typedef WDataAccess< WGridRegular3D2, double >::VoxelNeighborIterator NI;
        VI vi = access.voxels().first;

        for( int i = 0; i < m_voxelnum; ++i )
        {
            ++vi;
        }

        NI ni, ne;
        for( tie( ni, ne ) = vi.neighbors( m_nbrHood, *m_strat ); ni != ne; ++ni )
        {
            m_nbrIndices.push_back( *ni );
        }
    }

    /**
     * The operator called when visiting the dataset, const version. Used for any types but double.
     * Does nothing.
     *
     * \tparam T The type of the data in the provided dataset.
     */
    template< typename T >
    void operator() ( WDataAccessConst< WGridRegular3D2, T > /* access */ )
    {
    }

    /**
     * The operator called when visiting the dataset, const and double version. Iterates over a dataset
     * and stores the values of the neighbors of the chosen voxel.
     *
     * \param access The access object for the provided dataset.
     */
    void operator() ( WDataAccessConst< WGridRegular3D2, double > const access )
    {
        typedef WDataAccessConst< WGridRegular3D2, double >::VoxelIterator VI;
        typedef WDataAccessConst< WGridRegular3D2, double >::VoxelNeighborIterator NI;
        VI vi = access.voxels().first;

        for( int i = 0; i < m_voxelnum; ++i )
        {
            ++vi;
        }

        NI ni, ne;
        for( tie( ni, ne ) = vi.neighbors( m_nbrHood, *m_strat ); ni != ne; ++ni )
        {
            m_nbrIndices.push_back( *ni );
        }
    }

    /**
     * Get the indices of neighbors of the chosen voxel.
     *
     * \return A vector containing the indices of the voxels that are neighbors to the first voxel.
     */
    std::vector< int > const& getNeighborIndices() const
    {
        return m_nbrIndices;
    }

    /**
     * Sets the neighborhood for neighbor iteration.
     *
     * \param nbrHood The neighborhood.
     */
    void setNeighborhood( WNeighborhood nbrHood )
    {
        m_nbrHood = nbrHood;
    }

    /**
     * Set a pointer to a strategy.
     *
     * \param strat The strategy to use.
     */
    void setBoundaryStrategy( WBoundaryStrategy< double > const* strat )
    {
        m_strat = strat;
    }

private:

    //! The index of the voxel from which we want to get the neighbor indices.
    int m_voxelnum;

    //! A neighborhood to iterate.
    WNeighborhood m_nbrHood;

    //! The strategy for nonexisting voxels.
    WBoundaryStrategy< double > const* m_strat;

    //! Stores the indices of the neighborvoxels that have been found.
    std::vector< int > m_nbrIndices;
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
 * Test functionality of WNeighborhoodIterator.
 */
class WNeighborhoodIteratorTest : public CxxTest::TestSuite
{
public:

    /**
     * Test construction.
     */
    void testConstruction()
    {
        typedef WNeighborhoodIterator< double > NI;

        // create an invalid iterator
        TS_ASSERT_THROWS_NOTHING( NI() );

        NI ni;

        // it should be invalid
        TS_ASSERT( !ni );

        // now create a valid iterator
        WGridRegular3D2 g( 3, 3, 3 );
        WValueSet2< double > vs( 27 );
        WNeighborhood nbh = makeMoore( 1 );
        boost::array< std::size_t, 3 > coords = { { 0, 0, 0 } };  // NOLINT braces
        WBoundaryStrategyDefault< double > bs( -1 );

        TS_ASSERT_THROWS_NOTHING( NI( &g, &vs, nbh, bs, coords, 0 ) );

        NI no( &g, &vs, nbh, bs, coords, 0 );

        TS_ASSERT( no );
    }

    /**
     * Test const construction.
     */
    void testConstructionConst()
    {
        typedef WNeighborhoodIteratorConst< double > NI;

        // create an invalid iterator
        TS_ASSERT_THROWS_NOTHING( NI() );

        NI ni;

        // it should be invalid
        TS_ASSERT( !ni );

        // now create a valid iterator
        WGridRegular3D2 g( 3, 3, 3 );
        WValueSet2< double > vs( 27 );
        WNeighborhood nbh = makeMoore( 1 );
        boost::array< std::size_t, 3 > coords = { { 0, 0, 0 } };  // NOLINT braces
        WBoundaryStrategyDefault< double > bs( -1 );

        TS_ASSERT_THROWS_NOTHING( NI( &g, &vs, nbh, bs, coords, 0 ) );

        NI no( &g, &vs, nbh, bs, coords, 0 );

        TS_ASSERT( no );
    }

    /**
     * Test incementing and decrementing iterators.
     */
    void testIncDecr()
    {
        WGridRegular3D2 g( 3, 3, 3 );
        WValueSet2< double > vs( 27 );
        WNeighborhood nbh = makeMoore( 1 );
        boost::array< std::size_t, 3 > coords = { { 0, 0, 0 } };  // NOLINT braces
        WBoundaryStrategyClamp< double > bs;

        WNeighborhoodIterator< double > ni( &g, &vs, nbh, bs, coords, 0 );

        WNeighborhoodIterator< double > begin = ni;
        WNeighborhoodIterator< double > end( &g, &vs, nbh, bs, coords, 26 );

        for( int i = 0; i < 26; ++i )
        {
            ++ni;
            TS_ASSERT( ni != begin );
            TS_ASSERT( !( ni == begin ) );
        }

        TS_ASSERT( ni == end );

        for( int i = 0; i < 26; ++i )
        {
            --ni;
        }

        TS_ASSERT( ni == begin );

        ++ni;
        ++begin;

        TS_ASSERT( ni == begin );
    }

    /**
     * Test incementing and decrementing const iterators.
     */
    void testIncDecrConst()
    {
        WGridRegular3D2 g( 3, 3, 3 );
        WValueSet2< double > vs( 27 );
        WNeighborhood nbh = makeMoore( 1 );
        boost::array< std::size_t, 3 > coords = { { 0, 0, 0 } };  // NOLINT braces
        WBoundaryStrategyClamp< double > bs;

        WNeighborhoodIteratorConst< double > ni( &g, &vs, nbh, bs, coords, 0 );

        WNeighborhoodIteratorConst< double > begin = ni;
        WNeighborhoodIteratorConst< double > end( &g, &vs, nbh, bs, coords, 26 );

        for( int i = 0; i < 26; ++i )
        {
            ++ni;
            TS_ASSERT( ni != begin );
            TS_ASSERT( !( ni == begin ) );
        }

        TS_ASSERT( ni == end );

        for( int i = 0; i < 26; ++i )
        {
            --ni;
        }

        TS_ASSERT( ni == begin );

        ++ni;
        ++begin;

        TS_ASSERT( ni == begin );
    }

    /**
     * Test self-assignment of neighborhood iterators.
     */
    void testSelfAssignment()
    {
        WGridRegular3D2 g( 3, 3, 3 );
        WValueSet2< double > vs( 27 );
        WNeighborhood nbh = makeMoore( 1 );
        boost::array< std::size_t, 3 > coords = { { 0, 0, 0 } };  // NOLINT braces
        WBoundaryStrategyDefault< double > bs;

        // TODO( reichenbach ): this test could be a little more sophisticated

        WNeighborhoodIterator< double > ni( &g, &vs, nbh, bs, coords, 0 );

        TS_ASSERT_THROWS_NOTHING( ni = ni );

        WNeighborhoodIteratorConst< double > nic( &g, &vs, nbh, bs, coords, 0 );

        TS_ASSERT_THROWS_NOTHING( nic = nic );
    }

    /**
     * Test copying and comparing of const and non-const iterators.
     */
    void testCopyAndCompare()
    {
        WGridRegular3D2 g( 3, 3, 3 );
        WValueSet2< double > vs( 27 );
        WNeighborhood nbh = makeMoore( 1 );
        boost::array< std::size_t, 3 > coords = { { 0, 0, 0 } };  // NOLINT braces
        WBoundaryStrategyClamp< double > bs;

        WNeighborhoodIterator< double > ni( &g, &vs, nbh, bs, coords, 0 );

        // copy constructor
        WNeighborhoodIteratorConst< double > nic( ni );

        TS_ASSERT( nic );
        TS_ASSERT( nic == ni );

        WNeighborhoodIteratorConst< double > nid;
        nid = ni++;

        TS_ASSERT( nid );
        TS_ASSERT( nid == nic );

        nid++;
        TS_ASSERT( nid == ni );

        TS_ASSERT( nid != nic );
    }

    /**
     * Check if we can get the correct values of the neighbors of a voxel when using the default value boundary strategy
     * with non-const iterators.
     */
    void testIterationDefault()
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

        // get the indices
        {
            // these are the expected indices
            int ref[] = { 1, 3, 4, 9, 10, 12, 13 };  // NOLINT braces

            NeighborListVisitor* visitor = new NeighborListVisitor( 0 );

            WNeighborhood nh = makeMoore( 1 );
            WBoundaryStrategy< double >* bs = new WBoundaryStrategyDefault< double >( -1 );

            visitor->setNeighborhood( nh );
            visitor->setBoundaryStrategy( bs );

            // this will call the non-const double operator of the visitor
            vm->applyVisitor( visitor );

            std::vector< int > const& indices = visitor->getNeighborIndices();

            TS_ASSERT_EQUALS( indices.size(), 7 );
            TS_ASSERT_SAME_DATA( &indices[ 0 ], ref, sizeof( int ) * 7 );

            delete bs;
            delete visitor;
        }

        {
            // these are the expected indices
            int ref[] = { 1, 2, 4, 5, 7, 8, 10, 11, 13, 16, 17, 19, 20, 22, 23, 25, 26 };  // NOLINT braces

            NeighborListVisitor* visitor = new NeighborListVisitor( 14 );

            WNeighborhood nh = makeMoore( 1 );
            WBoundaryStrategy< double >* bs = new WBoundaryStrategyDefault< double >( -1 );

            visitor->setNeighborhood( nh );
            visitor->setBoundaryStrategy( bs );

            // this will call the non-const double operator of the visitor
            vm->applyVisitor( visitor );

            std::vector< int > const& indices = visitor->getNeighborIndices();

            TS_ASSERT_EQUALS( indices.size(), 17 );
            TS_ASSERT_SAME_DATA( &indices[ 0 ], ref, sizeof( int ) * 17 );

            delete bs;
            delete visitor;
        }
    }

    /**
     * Check if we can get the correct values of the neighbors of a voxel when using the clamping boundary strategy
     * with non-const iterators.
     */
    void testIterationClamping()
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

        // get the indices
        {
            // these are the expected indices
            int ref[] = {  0,  0,  1,  0,  0,  1,  3,  3,  4,
                           0,  0,  1,  0,      1,  3,  3,  4,
                           9,  9, 10,  9,  9, 10, 12, 12, 13 };  // NOLINT braces

            NeighborListVisitor* visitor = new NeighborListVisitor( 0 );

            WNeighborhood nh = makeMoore( 1 );
            WBoundaryStrategy< double >* bs = new WBoundaryStrategyClamp< double >();

            visitor->setNeighborhood( nh );
            visitor->setBoundaryStrategy( bs );

            vm->applyVisitor( visitor );

            std::vector< int > const& indices = visitor->getNeighborIndices();

            TS_ASSERT_EQUALS( indices.size(), 26 );
            TS_ASSERT_SAME_DATA( &indices[ 0 ], ref, sizeof( int ) * 26 );

            delete bs;
            delete visitor;
        }
    }

    /**
     * Check if we can get the correct values of the neighbors of a voxel when using the wrapping boundary strategy
     * with non-const iterators.
     */
    void testIterationWrap()
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

        // get the indices
        {
            // these are the expected indices
            int ref[] = { 18, 6, 2, 1, 3, 9 };  // NOLINT braces

            NeighborListVisitor* visitor = new NeighborListVisitor( 0 );

            WNeighborhood nh = makeVonNeumann( 1 );
            WBoundaryStrategy< double >* bs = new WBoundaryStrategyWrap< double >();

            visitor->setNeighborhood( nh );
            visitor->setBoundaryStrategy( bs );

            vm->applyVisitor( visitor );

            std::vector< int > const& indices = visitor->getNeighborIndices();

            TS_ASSERT_EQUALS( indices.size(), 6 );
            TS_ASSERT_SAME_DATA( &indices[ 0 ], ref, sizeof( int ) * 6 );

            delete bs;
            delete visitor;
        }
    }

    /**
     * Check if we can get the correct values of the neighbors of a voxel when using the default value boundary strategy
     * with const iterators.
     */
    void testIterationDefaultConst()
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

        // get the indices
        {
            // these are the expected indices
            int ref[] = { -1, -1, -1, -1, -1, -1, -1, -1, -1,
                          -1, -1, -1, -1,      1, -1,  3,  4,
                          -1, -1, -1, -1,  9, 10, -1, 12, 13 };  // NOLINT braces

            NeighborListVisitor* visitor = new NeighborListVisitor( 0 );

            WNeighborhood nh = makeMoore( 1 );
            WBoundaryStrategy< double >* bs = new WBoundaryStrategyDefault< double >( -1 );

            visitor->setNeighborhood( nh );
            visitor->setBoundaryStrategy( bs );

            // this calls the const double version of the operator of the visitor
            boost::shared_ptr< ValueMapper const > cvm( vm );
            cvm->applyVisitor( visitor );

            std::vector< int > const& indices = visitor->getNeighborIndices();

            TS_ASSERT_EQUALS( indices.size(), 26 );
            TS_ASSERT_SAME_DATA( &indices[ 0 ], ref, sizeof( int ) * 26 );

            delete bs;
            delete visitor;
        }
    }

    /**
     * Check if we can get the correct values of the neighbors of a voxel when using the clamping boundary strategy
     * with const iterators.
     */
    void testIterationClampingConst()
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

        // get the indices
        {
            // these are the expected indices
            int ref[] = {  0,  0,  1,  0,  0,  1,  3,  3,  4,
                           0,  0,  1,  0,      1,  3,  3,  4,
                           9,  9, 10,  9,  9, 10, 12, 12, 13 };  // NOLINT braces

            NeighborListVisitor* visitor = new NeighborListVisitor( 0 );

            WNeighborhood nh = makeMoore( 1 );
            WBoundaryStrategy< double >* bs = new WBoundaryStrategyClamp< double >();

            visitor->setNeighborhood( nh );
            visitor->setBoundaryStrategy( bs );

            boost::shared_ptr< ValueMapper const > cvm( vm );
            cvm->applyVisitor( visitor );

            std::vector< int > const& indices = visitor->getNeighborIndices();

            TS_ASSERT_EQUALS( indices.size(), 26 );
            TS_ASSERT_SAME_DATA( &indices[ 0 ], ref, sizeof( int ) * 26 );

            delete bs;
            delete visitor;
        }
    }

    /**
     * Check if we can get the correct values of the neighbors of a voxel when using the wrapping boundary strategy
     * with const iterators.
     */
    void testIterationWrapConst()
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

        // get the indices
        {
            // these are the expected indices
            int ref[] = { 18, 6, 2, 1, 3, 9 };  // NOLINT braces

            NeighborListVisitor* visitor = new NeighborListVisitor( 0 );

            WNeighborhood nh = makeVonNeumann( 1 );
            WBoundaryStrategy< double >* bs = new WBoundaryStrategyWrap< double >();

            visitor->setNeighborhood( nh );
            visitor->setBoundaryStrategy( bs );

            boost::shared_ptr< ValueMapper const > cvm( vm );
            cvm->applyVisitor( visitor );

            std::vector< int > const& indices = visitor->getNeighborIndices();

            TS_ASSERT_EQUALS( indices.size(), 6 );
            TS_ASSERT_SAME_DATA( &indices[ 0 ], ref, sizeof( int ) * 6 );

            delete bs;
            delete visitor;
        }
    }
};

#endif  // WNEIGHBORHOODITERATOR_TEST_H
