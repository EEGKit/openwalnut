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

#include "WMath.h"
#include "WPlane.h"
#include "WPosition.h"
#include "WVector3D.h"
#include "../WAssert.h"
#include "../WLimits.h"

bool wmath::testIntersectTriangle( const wmath::WPosition& p1, const wmath::WPosition& p2, const wmath::WPosition& p3, const WPlane& p )
{
    const WVector3D& normal = p.getNormal();
    const WPosition& planePoint = p.getPosition();

    double r1 = normal.dotProduct( p1 - planePoint );
    double r2 = normal.dotProduct( p2 - planePoint );
    double r3 = normal.dotProduct( p3 - planePoint );

    // TODO(math): use signum here!
    if( std::abs( ( ( r1 > 0 ) - ( r1 < 0 ) ) + ( ( r2 > 0) - ( r2 < 0 ) ) + ( ( r3 > 0 ) - ( r3 < 0 ) ) ) == 3 )
    {
        return false;
    }
    return true;
}

bool wmath::intersectPlaneSegment( const WPlane& p,
                                   const wmath::WPosition& p1,
                                   const wmath::WPosition& p2,
                                   boost::shared_ptr< wmath::WPosition > pointOfIntersection )
{
    const wmath::WVector3D& normal = p.getNormal().normalized();
    double const d = normal.dotProduct( p.getPosition() );
    WAssert( pointOfIntersection.get(), "Place to store a point of intersection is not ready!" );
    *pointOfIntersection = p.getPosition();   // otherwise it would be undefined

    // at least one point is in plane (maybe the whole segment)
    if( std::abs( normal.dotProduct( p1 - p.getPosition() ) ) <= 2*wlimits::DBL_EPS )
    {
        *pointOfIntersection = p1;
        return true;
    }
    else if( std::abs( normal.dotProduct( p2 - p.getPosition() ) ) <= 2*wlimits::DBL_EPS )
    {
        *pointOfIntersection = p2;
        return true;
    }

    if( std::abs( normal.dotProduct( p1 - p2 ) ) < wlimits::DBL_EPS ) // plane and line are parallel
    {
        return false;
    }

    double const t = ( d - normal.dotProduct( p2 ) ) / ( normal.dotProduct( p1 - p2 ) );

    *pointOfIntersection = p2 + t * ( p1 - p2 );

    if( t >= -wlimits::DBL_EPS && t <= ( 1.0 + wlimits::DBL_EPS ) )
    {
        return true;
    }
    return false;
}

bool wmath::intersectPlaneLineNearCP( const WPlane& p, const wmath::WLine& l, boost::shared_ptr< wmath::WPosition > cutPoint )
{
    bool result = false;
    double minDistance = wlimits::MAX_DOUBLE;
    WAssert( cutPoint.get(), "Place to store a point of intersection is not ready!" );
    *cutPoint = wmath::WPosition( 0, 0, 0 );
    for( size_t i = 1; i < l.size(); ++i ) // test each segment
    {
        boost::shared_ptr< wmath::WPosition > cP( new wmath::WPosition( 0, 0, 0 ) );
        if( intersectPlaneSegment( p, l[i-1], l[i], cP ) )
        {
            result = true;
            double dist = ( *cP - p.getPosition() ).normSquare();
            if( dist < minDistance )
            {
                minDistance = dist;
                *cutPoint = *cP;
            }
        }
    }
    return result;
}
