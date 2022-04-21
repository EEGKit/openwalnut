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

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

#include "WAssert.h"
#include "WTransferFunction2D.h"
#include "WLogger.h"
void WTransferFunction2D::sample2DTransferFunction( unsigned char*array, int width, int height ) const
{
    tfTexture->scaleImage( width, height, 1 );
    tfTexture->flipHorizontal();
    unsigned char *data = tfTexture->data();
    // Copy the texture to the WMTransferFunction2D Module
    for( int x = 0; x < width; x++ )
    {
        for( int y = 0; y < height; y++ )
        {
            array[ 4 * width * x + 4 * y + 0 ] = data[ 4 * width * x + 4 * y + 0 ]; // R
            array[ 4 * width * x + 4 * y + 1 ] = data[ 4 * width * x + 4 * y + 1 ]; // G
            array[ 4 * width * x + 4 * y + 2 ] = data[ 4 * width * x + 4 * y + 2 ]; // B
            array[ 4 * width * x + 4 * y + 3 ] = data[ 4 * width * x + 4 * y + 3 ] * m_opacityScale; // A
        }
    }
}

bool WTransferFunction2D::operator==( const WTransferFunction2D &rhs ) const
{
    if( m_histogram == rhs.m_histogram && tfTexture == rhs.tfTexture )
    {
        //wlog::debug("WTransferFunction2D") << "Texture or histogram changed";
        return true;
    }
    //wlog::debug("WTransferFunction2D") << "== false";
    return false;
}

bool WTransferFunction2D::operator!=( const WTransferFunction2D &rhs ) const
{
    //wlog::debug("WTransferFunction2D") << "op != :" << (!( ( *this ) == rhs ));
    return !( ( *this ) == rhs );
}
void WTransferFunction2D::setHistogram( std::shared_ptr< WHistogram2D > histogram )
{
    //wlog::debug("WTransferFunction2D") << "setHistogram";
    if( m_histogram != histogram )
    {
        m_histogram = histogram;
    }
}
void WTransferFunction2D::setTexture( unsigned char* array, int imageWidth, int imageHeight )
{
    osg::ref_ptr< osg::Image > newTexture = new osg::Image();
    newTexture->setImage( imageWidth, imageHeight, 1, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, array, osg::Image::USE_NEW_DELETE );
    tfTexture = newTexture;
}
