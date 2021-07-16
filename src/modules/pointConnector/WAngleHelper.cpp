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
#include <limits>
#include <map>
#include <vector>

#include "WAngleHelper.h"

static int compareWPosition( WPosition a, WPosition b )
{
    return a.z() < b.z();
}

static double calcAngle( WPosition a, WPosition b )
{
    double num = a.x() * b.x() + a.y() * b.y() + a.z() * b.z();
    double denom = ( a.x() * a.x() + a.y() * a.y() + a.z() * a.z() ) *
                   ( b.x() * b.x() + b.y() * b.y() + b.z() * b.z() );
    denom = sqrt( denom );
    double angle = acos( num / denom ) * 180.0 / M_PI;

    return angle;
}

static std::vector< WAngleHelper::DJLine > createLines( std::vector< WPosition > positions )
{
    std::vector< WAngleHelper::DJLine > lines;

    if( positions.empty() )
    {
        return lines;
    }

    std::sort( positions.begin(), positions.end(), compareWPosition );
    std::vector< WPosition > oldPoints;
    std::vector< WPosition > currentPoints;
    double currentZ = positions.at( 0 ).z();

    for( size_t idx = 0; idx < positions.size(); idx++ )
    {
        WPosition point = positions.at( idx );
        if( point.z() != currentZ )
        {
            currentZ = point.z();
            oldPoints.clear();
            oldPoints.insert( oldPoints.end(), currentPoints.begin(), currentPoints.end() );
            currentPoints.clear();
        }
        for( size_t j = 0; j < oldPoints.size(); j++ )
        {
            WPosition p = oldPoints.at( j );
            lines.push_back( std::make_tuple( p, point, calcAngle( p, point ) ) );
        }
        currentPoints.push_back( point );
    }

    return lines;
}

static WAngleHelper::DJOut dijkstra( std::vector< WAngleHelper::DJLine > lines, std::vector< WPosition > points, WPosition start )
{
    std::vector< WPosition > Q;
    std::map< WPosition, double > dist;
    std::map< WPosition, WPosition > prev;

    for( size_t idx = 0; idx < points.size(); idx++ )
    {
        WPosition p = points.at( idx );
        dist[p] = std::numeric_limits< double >::infinity();
        Q.push_back( p );
    }
    dist[start] = 0;

    while( Q.size() > 0 )
    {
        std::sort( Q.begin(), Q.end(),
            [&dist]( WPosition a, WPosition b )
            {
                return dist[a] < dist[b];
            }
        );
        WPosition u = Q.at( 0 );
        Q.erase( Q.begin() );

        if( dist[u] == std::numeric_limits< double >::infinity() )
        {
            break;
        }

        for( size_t idx = 0; idx < lines.size(); idx++ )
        {
            WAngleHelper::DJLine l = lines.at( idx );
            if( std::get< 0 >( l ) != u )
            {
                continue;
            }

            WPosition v = std::get< 1 >( l );

            auto old = std::find_if( lines.begin(), lines.end(),
                [&prev, u]( WAngleHelper::DJLine line )
                {
                    return std::get< 0 >( line ) == prev[u] && std::get< 1 >( line ) == u;
                }
            );

            double alt = 0;
            if( old != lines.end() )
            {
                alt = dist[u] + abs( std::get< 2 >( l ) - std::get< 2 >( *old ) );
            }
            if( alt < dist[v] )
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
    std::sort( positions.begin(), positions.end(), compareWPosition );

    std::vector< WAngleHelper::DJLine > lines = createLines( positions );

    std::map< WPosition, WPosition > prevs;
    WPosition ePoint;
    double eDis = std::numeric_limits< double >::infinity();

    double firstZ = positions.at( 0 ).z();
    double lastZ = positions.at( positions.size() - 1 ).z();
    auto points = positions.begin();
    while( points->z() == firstZ )
    {
        DJOut res = dijkstra( lines, positions, *points );
        double endDis = std::numeric_limits< double >::infinity();
        WPosition endPoint;

        auto end = positions.end();
        end--;
        while( end->z() == lastZ )
        {
            if( res.second[ *end ] < endDis )
            {
                endDis = res.second[ *end ];
                endPoint = *end;
            }
            end--;
        }

        if( endDis < eDis )
        {
            prevs = res.first;
            ePoint = endPoint;
            eDis = endDis;
        }

        points++;
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
    std::sort( positions.begin(), positions.end(), compareWPosition );

    if( fiber.empty() )
    {
        return findSmoothestPath( positions );
    }

    std::vector< WPosition > out;
    auto posIter = positions.begin();
    auto fibIter = fiber.begin();
    osg::Vec3* oldFib = nullptr;

    // Selection before fiber
    if( posIter->z() < fibIter->z() )
    {
        std::vector< WPosition > pos;
        while( posIter < positions.end() && posIter->z() < fibIter->z() )
        {
            pos.push_back( *posIter );
            posIter++;
        }
        pos.push_back( *fibIter );
        pos = findSmoothestPath( pos );
        out.insert( out.end(), pos.begin(), pos.end() );
    }

    // Selection between fiber
    while( fibIter < fiber.end() && posIter < positions.end() )
    {
        if( fibIter->z() == posIter->z() )
        {
            posIter++;
        }
        else if( fibIter->z() > posIter->z() )
        {
            std::vector< WPosition > pos;
            if( oldFib != nullptr )
            {
                pos.push_back( WPosition( oldFib->x(), oldFib->y(), oldFib->z() ) );
            }
            while( posIter < positions.end() && posIter->z() < fibIter->z() )
            {
                pos.push_back( *posIter );
                posIter++;
            }
            pos.push_back( WPosition( fibIter->x(), fibIter->y(), fibIter->z() ) );
            pos = findSmoothestPath( pos );
            out.insert( out.end(), pos.begin(), pos.end() );
        }
        else
        {
            oldFib = &( *fibIter );
            fibIter++;
        }
    }

    // Selection after fiber
    if( posIter < positions.end() )
    {
        std::vector< WPosition > pos;
        if( oldFib != nullptr )
        {
            pos.push_back( WPosition( oldFib->x(), oldFib->y(), oldFib->z() ) );
        }
        while( posIter != positions.end() )
        {
            pos.push_back( *posIter );
            posIter++;
        }
        pos = findSmoothestPath( pos );
        out.insert( out.end(), pos.begin(), pos.end() );
    }

    return out;
}
