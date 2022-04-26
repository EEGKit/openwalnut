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

#include <cmath>
#include <iostream>
#include <vector>

#include "WAssert.h"
#include "WTransferFunction2D.h"
#include "WLogger.h"
void WTransferFunction2D::sample2DTransferFunction( unsigned char*array, int width, int height ) const
{
    if( m_widget.size() < 1 ) return;

    WColor color;
    // Iterate over widgets and write the color in the "texture" (array) in normalized space
    for( auto const &w : m_widget )
    {
        color = w.color;
        int xMin = static_cast< int >( w.isoX * width );
        int xMax = static_cast< int >( ( w.isoX + w.width ) * width );
        int yMin = static_cast< int >( w.isoY * height );
        int yMax = static_cast< int >( ( w.isoY + w.height ) * height );
        for( int x = xMin; x < xMax; ++x )
        {
            for( int y = yMin; y < yMax; ++y )
            {
                for( int j = 0; j < 3; ++j )
                {
                    array[ 4 * width * x + 4 * y + j ] = color[ j ] * 255.;
                }
                array[ 4 * width * x + 4 * y + 3 ] = color[ 3 ] * 255. * m_opacityScale;
            }
        }
    }
}

bool WTransferFunction2D::operator==( const WTransferFunction2D &rhs ) const
{
    if( m_histogram != rhs.m_histogram )
    {
        return false;
    }
    if( m_widget.size() != rhs.m_widget.size() )
    {
        return false;
    }

    std::vector< BoxEntry >::const_iterator it1 = m_widget.begin();
    std::vector< BoxEntry >::const_iterator it2 = rhs.m_widget.begin();
    for( ;
            it1 != m_widget.end();
            ++it1, ++it2 )
    {
        if( !( *it1 == *it2 ) )
        {
            return false;
        }
    }
    return true;
}

bool WTransferFunction2D::operator!=( const WTransferFunction2D &rhs ) const
{
    return !( ( *this ) == rhs );
}

void WTransferFunction2D::setHistogram( const std::shared_ptr< WHistogram2D >& histogram )
{
    if( m_histogram != histogram )
    {
        m_histogram = histogram;
    }
}

void WTransferFunction2D::addBoxWidget( double isoX, double isoY, double width, double height, const WColor &color )
{
    m_widget.emplace_back( BoxEntry( isoX, isoY, width, height, color ) );
}

std::ostream& operator << ( std::ostream& out, const WTransferFunction2D& tf )
{
    size_t numBoxWidgets = tf.numBoxWidgets();
    for( size_t i = 0; i < numBoxWidgets; ++i )
    {
        double isoX = tf.getIsovalueX( i );
        double isoY = tf.getIsovalueY( i );
        double width = tf.getWidth( i );
        double height = tf.getHeight( i );
        WColor c = tf.getColor( i );
        out << "c:" << isoX << ":" << isoY << ":" << width << ":" << height << ":"
            << c[ 0 ] << ":" << c[ 1 ] << ":" << c[ 2 ] << ":" << c[ 3 ] << ";";
    }
    return out;
}
