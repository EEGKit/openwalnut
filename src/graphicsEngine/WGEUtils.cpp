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

#include <vector>

#include <osg/Array>

#include "../common/math/WPosition.h"
#include "WGEUtils.h"

osg::ref_ptr< osg::Vec3Array > wge::osgVec3Array( const std::vector< wmath::WPosition >& posArray )
{
    osg::ref_ptr< osg::Vec3Array > result = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    result->reserve( posArray.size() );
    std::vector< wmath::WPosition >::const_iterator cit;
    for( cit = posArray.begin(); cit != posArray.end(); ++cit )
    {
        result->push_back( wge::osgVec3( *cit ) );
    }
    return result;
}

osg::Vec3 wge::unprojectFromScreen( const osg::Vec3 screen, osg::ref_ptr< osg::Camera > camera )
{
    return screen * osg::Matrix::inverse( camera->getViewMatrix() * camera->getProjectionMatrix() * camera->getViewport()->computeWindowMatrix() );
}

WColor wge::createColorFromIndex( int index )
{
    float r = 0.0;
    float g = 0.0;
    float b = 0.0;
    float mult = 1.0;

    if ( index == 0 )
    {
        return WColor( 0.0, 0.0, 0.0 );
    }

    if ( ( index & 1 ) == 1 )
    {
        b = 1.0;
    }
    if ( ( index & 2 ) == 2 )
    {
        g = 1.0;
    }
    if ( ( index & 4 ) == 4 )
    {
        r = 1.0;
    }
    if ( ( index & 8 ) == 8 )
    {
        mult -= 0.15;
        if ( r < 1.0 && g < 1.0 && b < 1.0 )
        {
            r = 1.0;
            g = 1.0;
        }
    }
    if ( ( index & 16 ) == 16 )
    {
        mult -= 0.15;
        if ( r < 1.0 && g < 1.0 && b < 1.0 )
        {
            b = 1.0;
            g = 1.0;
        }
    }
    if ( ( index & 32 ) == 32 )
    {
        mult -= 0.15;
        if ( r < 1.0 && g < 1.0 && b < 1.0 )
        {
            r = 1.0;
            b = 1.0;
        }
    }
    if ( ( index & 64 ) == 64 )
    {
        mult -= 0.15;
        if ( r < 1.0 && g < 1.0 && b < 1.0 )
        {
            g = 1.0;
        }
    }
    if ( ( index & 128 ) == 128 )
    {
        mult -= 0.15;
        if ( r < 1.0 && g < 1.0 && b < 1.0 )
        {
            r = 1.0;
        }
    }
    r *= mult;
    g *= mult;
    b *= mult;

    return WColor( r, g, b );
}

WColor wge::createColorFromHSV( int h, float s, float v )
{
    h = h % 360;

    int hi = h / 60;
    float f =  ( static_cast<float>( h ) / 60.0 ) - hi;

    float p = v * ( 1.0 - s );
    float q = v * ( 1.0 - s * f );
    float t = v * ( 1.0 - s * ( 1.0 - f ) );

    switch ( hi )
    {
        case 0:
            return WColor( v, t, p, 1.0 );
        case 1:
            return WColor( q, v, p, 1.0 );
        case 2:
            return WColor( p, v, t, 1.0 );
        case 3:
            return WColor( p, q, v, 1.0 );
        case 4:
            return WColor( t, p, v, 1.0 );
        case 5:
            return WColor( v, p, q, 1.0 );
        case 6:
            return WColor( v, t, p, 1.0 );
        default:
            return WColor( v, t, p, 1.0 );
    }
}

WColor wge::getNthHSVColor( int n, int parts )
{
    if ( parts > 360 )
    {
        parts = 360;
    }
    int frac = 360 / parts;
    return createColorFromHSV( n * frac );
}
