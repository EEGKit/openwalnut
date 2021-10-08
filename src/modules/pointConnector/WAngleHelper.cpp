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

#define _USE_MATH_DEFINES

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <unordered_map>
#include <queue>
#include <thread>
#include <vector>

#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>
#include <boost/lockfree/queue.hpp>

#include "WAngleHelper.h"


/**
 * Data for the thread handling
 */
struct SaptData
{
    /**
     * Creates the sapt data
     * \param prevs The unordered_map of previous points.
     * \param endPoint The endpoint.
     * \param distance The distance to the endpoint.
     */
    SaptData( WAngleHelper::PositionMap prevs, WPosition endPoint, double distance ):
        prevs( prevs ), endPoint( endPoint ), distance( distance )
    {
    }

    WAngleHelper::PositionMap prevs; //!< The unordered_map of previous points.
    WPosition endPoint; //!< The endpoint.
    double distance; //!< The distance to the endpoint.
};

/**
 * A queue with data that is threadsafe
 */
typedef boost::lockfree::queue< SaptData*, boost::lockfree::fixed_sized< false > > lfr_queue;


static int compareWPosition( WPosition a, WPosition b )
{
    return a.z() < b.z();
}

double WAngleHelper::calculateAngle( WPosition a, WPosition b )
{
    double num = a.x() * b.x() + a.y() * b.y() + a.z() * b.z();
    double denom = ( a.x() * a.x() + a.y() * a.y() + a.z() * a.z() ) *
                   ( b.x() * b.x() + b.y() * b.y() + b.z() * b.z() );
    denom = sqrt( denom );
    double angle = acos( num / denom ) * 180.0 / M_PI;

    return angle;
}

static WAngleHelper::DJLinePair createLines( std::vector< WPosition > positions )
{
    WAngleHelper::PositionLineMap prevMap;  // A map containing all the backwards connections of a point
    WAngleHelper::PositionLineMap postMap;  // A map containing all the forward connections of a point

    if( positions.empty() )
    {
        return std::make_pair( prevMap, postMap );
    }

    std::sort( positions.begin(), positions.end(), compareWPosition );
    std::vector< WPosition > oldPoints;         // holds the points that are on the layer one before the current one
    std::vector< WPosition > currentPoints;     // holds all the points on the current layer (becomes oldPoints afterwards)
    double currentZ = positions.at( 0 ).z();

    // WPosition vert( 0.0, 0.0, 1.0 );            // A vertical position used for angle calculation

    for( size_t idx = 0; idx < positions.size(); idx++ )
    {
        WPosition point = positions.at( idx );
        if( point.z() - currentZ > 4.0 )     // A new layer is found
        {
            currentZ = point.z();
            oldPoints.clear();
            oldPoints.insert( oldPoints.end(), currentPoints.begin(), currentPoints.end() );
            currentPoints.clear();
        }

        WAngleHelper::PositionMap prevM;  // All the backwards connections
        for( size_t j = 0; j < oldPoints.size(); j++ )
        {
            WPosition p = oldPoints.at( j );
            // double angle = WAngleHelper::calculateAngle( p - vert, point - p );

            double angleX = -atan2( point.x() - p.x(), point.y() - p.y() ) / M_PI * 180.0 + 180.0;
            double angleY = asin( ( point.z() - p.z() ) / distance( p, point ) ) / M_PI * 180.0;
            WPosition angle( angleX, angleY, 0.0 );

            prevM[p] = angle;               // create a backwards connection
            postMap[p][point] = angle;      // create a forward connection
        }
        prevMap[point] = prevM;
        postMap[point] = WAngleHelper::PositionMap();
        currentPoints.push_back( point );
    }

    return std::make_pair( prevMap, postMap );
}

static WAngleHelper::DJOut dijkstra( WAngleHelper::DJLinePair lines, std::vector< WPosition > points, WPosition start )
{
    std::vector< WPosition > Q;             // All positions
    WAngleHelper::PositionDoubleMap dist;   // A map from position to the lowest distance it would need to get there
    WAngleHelper::PositionMap prev;         // A map from position to it's previous position

    for( size_t idx = 0; idx < points.size(); idx++ )
    {
        WPosition p = points.at( idx );
        dist[p] = std::numeric_limits< double >::infinity();
        Q.push_back( p );
    }
    dist[start] = 0;

    while( Q.size() > 0 )
    {
        // find position with minimum distance
        auto min = std::min_element( Q.begin(), Q.end(),
            [dist]( WPosition a, WPosition b )
            {
                return dist.at( a ) < dist.at( b );
            }
        );
        WPosition u = *min;
        Q.erase( min );

        if( dist[u] == std::numeric_limits< double >::infinity() )
        {
            break;
        }

        WAngleHelper::PositionMap prevM = lines.first[u];     // forward connections of that position
        WAngleHelper::PositionMap postM = lines.second[u];    // backward connections of that positions

        for( auto it = postM.begin(); it != postM.end(); it++ )     // loop all forward connections
        {
            WPosition v = it->first;
            auto pr = prev.find( u );
            auto old = pr == prev.end() ? prevM.end() : prevM.find( pr->second );   // find the backward connections associated with the
                                                                                    // previous position that has the lowest total distance

            double alt = 0; // The 'alternative' total angle (could become the new one)
            if( old != prevM.end() )
            {
                WPosition angles = it->second - old->second;
                alt = dist[u] + abs( angles.x() ) + abs( angles.y() );    // add difference from old to current to the total distance
            }
            if( alt < dist[v] ) // If alternative angle is lower than current total => override
            {
                dist[v] = alt;
                prev[v] = u;
            }
        }
    }

    return std::make_pair( prev, dist );
}

std::vector< WPosition > WAngleHelper::findSmoothestPath( std::vector< WPosition > positions )
{
    // create the lines
    WAngleHelper::DJLinePair lines = createLines( positions );

    // a queue for all the data that the threads pump out
    std::shared_ptr< lfr_queue > data = std::shared_ptr< lfr_queue >( new lfr_queue( 0 ) );

    double firstZ = positions.at( 0 ).z();
    double lastZ = positions.at( positions.size() - 1 ).z();
    auto points = positions.begin();

    // Since we use the dijkstra from each point on the first layer we can parallelize it
    boost::asio::thread_pool pool( std::thread::hardware_concurrency() );
    while( points->z() == firstZ )
    {
        WPosition start = *points;
        boost::asio::post( pool, [lines, positions, start, lastZ, data]()
        {
            DJOut res = dijkstra( lines, positions, start );
            double endDis = std::numeric_limits< double >::infinity();
            WPosition endPoint;

            auto end = positions.end();
            end--;
            while( end->z() == lastZ )              // Check the paths from the current first layer point to all last layer points
            {                                       // And find the one with the lowest total angle
                if( res.second[ *end ] < endDis )
                {
                    endDis = res.second[ *end ];
                    endPoint = *end;
                }
                end--;
            }

            data->push( new SaptData( res.first, endPoint, endDis ) );
        } );
        points++;
    }

    // wait for all threads
    pool.join();

    WAngleHelper::PositionMap prevs;
    WPosition ePoint;
    double eDis = std::numeric_limits< double >::infinity();

    // Analyze the data of all threads and find the path with the lowest angle deviation among them
    SaptData* d;
    while( data->pop( d ) )
    {
        if( d->distance < eDis )
        {
            prevs = d->prevs;
            ePoint = d->endPoint;
            eDis = d->distance;
        }
        delete d;
    }

    std::vector< WPosition > S;
    WPosition u = ePoint;
    while( true )
    {
        S.insert( S.begin(), u );
        auto it = prevs.find( u );
        if( it == prevs.end() )
        {
            break;
        }
        u = it->second;
    }

    return S;
}

std::vector< WPosition > WAngleHelper::findSmoothestPath( std::vector< WPosition > positions, WFiberHandler::PCFiber fiber )
{
    if( positions.empty() )
    {
        return std::vector< WPosition >();
    }
    std::sort( positions.begin(), positions.end(), compareWPosition );

    if( fiber.empty() )
    {
        return findSmoothestPath( positions );
    }

    std::vector< WPosition > pos;
    auto posIter = positions.begin();
    auto fibIter = fiber.begin();

    // intertwine positions and fiber.
    while( posIter < positions.end() )
    {
        if( fibIter >= fiber.end() || posIter->z() < fibIter->z() )
        {
            pos.push_back( *posIter );
            posIter++;
        }
        else if( posIter->z() == fibIter->z() )
        {
            posIter++;
        }
        else
        {
            pos.push_back( *fibIter );
            fibIter++;
        }
    }

    while( fibIter < fiber.end() )
    {
        pos.push_back( *fibIter );
        fibIter++;
    }

    if( pos.empty() )
    {
        return std::vector< WPosition >();
    }

    return findSmoothestPath( pos );
}
