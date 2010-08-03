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

#ifndef WVALUESETHISTOGRAM_H
#define WVALUESETHISTOGRAM_H

#include <map>
#include <list>
#include <utility>

#include <boost/shared_ptr.hpp>
#include <boost/scoped_array.hpp>
#include <boost/shared_array.hpp>

#include "../WValueSet.h"

/**
 * Used to find the occurrence frequencies of values in a value set. It implements a classical histogram but allows easy modification of bucket
 * sizes without unnecessary recalculation of the whole histogram.
 *
 * \note This histogram is different from from WValueSetHistogram which is a generic histogram class.
 */
class WValueSetHistogram
{
public:
    /**
     * Constructor. Creates the histogram for the specified value set.
     *
     * \param valueSet source of the data for the histogram
     */
    explicit WValueSetHistogram( boost::shared_ptr< WValueSetBase > valueSet );


    /**
     * Constructor. Creates the histogram for the specified value set.
     *
     * \param valueSet source of the data for the histogram
     */
    //explicit WValueSetHistogram( const WValueSetBase& valueSet );

    /**
     * Copy constructor. If another interval size is given the histogram gets matched to it using the initial bucket data.
     *
     * \param histogram another WValueSetHistogram
     * \param intervalSize the size of one bucket in the mapped histogram
     */
    //explicit WValueSetHistogram( const WValueSetHistogram& histogram, double intervalSize = 0.0 );

    /**
     * Destructor.
     */
    ~WValueSetHistogram();

    /**
     * Get the size of the bucket.
     *
     * \param index which buckets size is to be returned, starts with 0 which is the bucket
     * containing the smallest values.
     *
     * \return elements in the bucket.
     */
    size_t operator[]( size_t index );

    /**
     * Get the size of the bucket. Testing if the position is valid.
     *
     * \param index which buckets size is to be returned, starts with 0 which is the bar with
     * the smallest values
     *
     * \return elements in the bucket
     */
    size_t at( size_t index );

    /**
     * Returns the number of buckets in the histogram with the actual mapping.
     *
     * \return number of buckets
     */
    size_t size() const;

    /**
     * Returns the minimum value in the value set.
     *
     * \return minimum
     */
    double getMinimum() const;

    /**
     * Returns the maximum value in the value set.
     *
     * \return maximum
     */
    double getMaximum() const;

    /**
     * Return the size of one bucket.
     *
     * \return the size of a bucket.
     */
    double getBucketSize() const;

protected:
    /**
     * Return the initial buckets.
     *
     * \return m_initialBuckets
     */
    boost::shared_array< size_t > getInitialBuckets() const;

    /**
     * Return the number of initial buckets.
     *
     * \return m_nInitialBuckets
     */
    size_t getNInitialBuckets() const;

    /**
     * Return the size of one initial bucket.
     *
     * \return m_bucketSize
     */
    double getInitialBucketSize() const;

private:
    /**
     * The smallest value in the ValueSet
     */
    double m_minimum;

    /**
     * The biggest value in the ValueSet
     */
    double m_maximum;

    /**
     * Size of one bucket in the initial histogram.
     */
    double m_initialBucketSize;

    /**
     * Pointer to all initial buckets of the histogram.
     */
    boost::shared_array< size_t > m_initialBuckets;

    /**
     * Number of buckets in the initial histogram.
     */
    size_t m_nInitialBuckets;

    /**
     * Pointer to all initial buckets of the histogram.
     */
    boost::shared_array< size_t > m_mappedBuckets;

    /**
     * Tracks the number of a buckets in the mapped histogram.
     */
    size_t m_nMappedBuckets;

    /**
     * Size of one bucket in the mapped histogram.
     */
    double m_mappedBucketSize;

    /**
     * To calculate the new buckets
     *
     * \param intervalSize the size of one bucket
     */
    void calculateMapping( double intervalSize );

    /**
     * increment the value by one, contains the logic to find the element place in the array.
     * Should only be used in the constructor i.e. while iterating over WValueSet.
     *
     * \param value value to increment
     */
    void insert( double value );
};

#endif  // WVALUESETHISTOGRAM_H

