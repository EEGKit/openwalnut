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

#ifndef WVALUESETHISTOGRAM_TEST_H
#define WVALUESETHISTOGRAM_TEST_H

#include <vector>

#include <cxxtest/TestSuite.h>

#include "../../WValueSet.h"

#include "../WValueSetHistogram.h"

/**
 * Test WValueSetHistogram
 **/
class WValueSetHistogramTest : public CxxTest::TestSuite
{
    public:
        /**
         * Test instantiation
         */
        void testInstantiation( void )
        {
            // create some test data
            double a[2] = { 0.0, 3.1415 };
            const std::vector< double > v( a, a + sizeof( a ) / sizeof( double ) );
            WValueSet< double >* valueSet = new WValueSet< double >( 0, 1, v, W_DT_DOUBLE );

            // test both constructors with default bucket size
            TS_ASSERT_THROWS_NOTHING( WValueSetHistogram hist( *valueSet ) );
            boost::shared_ptr< WValueSet< double > > vsPtr( valueSet );
            TS_ASSERT_THROWS_NOTHING( WValueSetHistogram hist( vsPtr ) );
        }

        /**
         * Test operator[]
         */
        void testOperator( void )
        {
            // create some test data
            double a[4] = { 0.0, 1.0, 1.0, 4.0 };
            const std::vector< double > v( a, a + sizeof( a ) / sizeof( double ) );
            WValueSet< double >* valueSet = new WValueSet< double >( 0, 1, v, W_DT_DOUBLE );

            // create the histogram
            WValueSetHistogram hist( *valueSet, 5 );
            TS_ASSERT_EQUALS( hist[0], 1 ); // 0.0
            TS_ASSERT_EQUALS( hist[1], 2 ); // 1.0
            TS_ASSERT_EQUALS( hist[2], 0 ); // 2.0
            TS_ASSERT_EQUALS( hist[3], 0 ); // 3.0
            TS_ASSERT_EQUALS( hist[4], 1 ); // 4.0
        }

        /**
         * Test at()
         */
        void testAt( void )
        {
            // create some test data
            double a[4] = { 0.0, 1.0, 1.0, 4.0 };
            const std::vector< double > v( a, a + sizeof( a ) / sizeof( double ) );
            WValueSet< double >* valueSet = new WValueSet< double >( 0, 1, v, W_DT_DOUBLE );

            // create histogram
            WValueSetHistogram hist( *valueSet, 5 );

            // test access with valid indices
            TS_ASSERT_EQUALS( hist.at( 0 ), 1 ); // 0.0
            TS_ASSERT_EQUALS( hist.at( 1 ), 2 ); // 1.0
            TS_ASSERT_EQUALS( hist.at( 2 ), 0 ); // 2.0
            TS_ASSERT_EQUALS( hist.at( 3 ), 0 ); // 3.0
            TS_ASSERT_EQUALS( hist.at( 4 ), 1 ); // 4.0

            // test access with invalid index
            TS_ASSERT_THROWS_ANYTHING( hist.at( 5 ) );
        }

        /**
         * Test getMinimum(), getMaximum()
         */
        void testMinMax( void )
        {
            // create some test data
            double a[4] = { 0.0, -5.0, 1.0, 2.0 };
            const std::vector< double > v( a, a + sizeof( a ) / sizeof( double ) );
            WValueSet< double >* valueSet = new WValueSet< double >( 0, 1, v, W_DT_DOUBLE );

            // create histogram
            WValueSetHistogram hist( *valueSet );

            TS_ASSERT_EQUALS( hist.getMinimum(), -5.0 );
            TS_ASSERT_EQUALS( hist.getMaximum(), 2.0 );
        }

        /**
         * Test size(), and getBucketSize()
         */
        void testSizes( void )
        {
            // create some test data
            double a[5] = { 0.0, 4.0, 1.0, 2.0, 1.0 };
            const std::vector< double > v( a, a + sizeof( a ) / sizeof( double ) );
            WValueSet< double >* valueSet = new WValueSet< double >( 0, 1, v, W_DT_DOUBLE );

            // create histogram
            WValueSetHistogram hist( *valueSet, 5 );

            TS_ASSERT_EQUALS( hist.size(), 5 ); // 0.0, 1.0, 2.0, 3.0 and 4.0
            TS_ASSERT_EQUALS( hist.getBucketSize(), 1.0 ); // 0.0, 1.0, 2.0, 3.0 and 4.0
        }

        /**
         * Test copy construction.
         **/
        void testCopyWithoutIntervalChanges( void )
        {
            // create some test data
            double a[5] = { 0.0, 4.0, 1.0, 2.0, 1.0 };
            const std::vector< double > v( a, a + sizeof( a ) / sizeof( double ) );
            WValueSet< double >* valueSet = new WValueSet< double >( 0, 1, v, W_DT_DOUBLE );

            // create histogram
            WValueSetHistogram hist( *valueSet, 5 );
            WValueSetHistogram hist2 = hist;    // copy constructor
            WValueSetHistogram hist3( *valueSet, 10 );

            // is everything the same?
            TS_ASSERT_EQUALS( hist.m_minimum, hist2.m_minimum );
            TS_ASSERT_EQUALS( hist.m_maximum, hist2.m_maximum );
            TS_ASSERT_EQUALS( hist.m_initialBucketSize, hist2.m_initialBucketSize );
            TS_ASSERT_EQUALS( hist.m_initialBuckets.get(), hist2.m_initialBuckets.get() ); // initial buckets must be the same as it is a shared array
            TS_ASSERT_EQUALS( hist.m_nInitialBuckets, hist2.m_nInitialBuckets );
            TS_ASSERT_EQUALS( hist.m_mappedBuckets.get(), hist2.m_mappedBuckets.get() );
            TS_ASSERT_EQUALS( hist.m_nMappedBuckets, hist2.m_nMappedBuckets );
            TS_ASSERT_EQUALS( hist.m_mappedBucketSize, hist2.m_mappedBucketSize );

            // test copy assignment
            hist = hist3;
            TS_ASSERT_EQUALS( hist.m_minimum, hist3.m_minimum );
            TS_ASSERT_EQUALS( hist.m_maximum, hist3.m_maximum );
            TS_ASSERT_EQUALS( hist.m_initialBucketSize, hist3.m_initialBucketSize );
            TS_ASSERT_EQUALS( hist.m_initialBuckets.get(), hist3.m_initialBuckets.get() ); // initial buckets must be the same as it is a shared array
            TS_ASSERT_EQUALS( hist.m_nInitialBuckets, hist3.m_nInitialBuckets );
            TS_ASSERT_EQUALS( hist.m_mappedBuckets, hist3.m_mappedBuckets );
            TS_ASSERT_EQUALS( hist.m_nMappedBuckets, hist3.m_nMappedBuckets );
            TS_ASSERT_EQUALS( hist.m_mappedBucketSize, hist3.m_mappedBucketSize );
        }

        /**
         * Test copy construction with changed interval sizes.
         **/
        void testCopyWithIntervalChanges( void )
        {
        }
};

#endif  // WVALUESETHISTOGRAM_TEST_H
