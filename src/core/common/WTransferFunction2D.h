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

#ifndef WTRANSFERFUNCTION2D_H
#define WTRANSFERFUNCTION2D_H

#include <iosfwd>
#include <vector>

#include "WColor.h"
#include "WHistogram2D.h"
#include "core/dataHandler/WValueSetBase.h"

/**
 * A class that stores a 2D transfer function texture and scales it to
 * the desired size given by the user. Also holds a reference to the 2D Histogram
 */
class WTransferFunction2D // NOLINT problem with macro
{
public:
    /**
     * Default constructor of a meaningless transfer function
     */
    WTransferFunction2D() : m_opacityScale( 1.0 ),
                            m_histogram( nullptr ),
                            tfTexture( nullptr )
    {
    }

    /**
     * Deep copy constructor.
     *
     * \param rhs the value to histogram
     */
    WTransferFunction2D( const WTransferFunction2D &rhs )
            : m_opacityScale( 1.0 ),
              m_histogram( rhs.m_histogram ),
              tfTexture( rhs.tfTexture )
    {
    }

    /**
     * Deep copy operator
     *
     * \param rhs the value to copy
     * \returns reference to current object
     */
    WTransferFunction2D& operator=( const WTransferFunction2D &rhs )
    {
        if( m_histogram && m_opacityScale && tfTexture )
        {
            this->m_opacityScale = rhs.m_opacityScale;
            this->m_histogram = rhs.m_histogram;
            this->tfTexture = rhs.tfTexture;
            return ( *this );
        }
    }

    /**
     * Check equivalence of two transfer functions.
     *
     * \returns true if this object contains exactly the same
     * data as rhs
     * \param rhs object to compare with
     */
    bool operator==( const WTransferFunction2D &rhs ) const;

    /**
     * Check equivalence of two transfer functions.
     *
     * \returns negated result of operator==
     * \param rhs the value to compare with
     */
    bool operator!=( const WTransferFunction2D &rhs ) const;

    /**
     * Set the histogram going along with the transfer function
     *
     * \param histogram the histogram
     */
    void setHistogram( std::shared_ptr< WHistogram2D> histogram );

    /**
     * Set the transfer function texture
     *
     * \param array the texture
     * \param imageHeight width of the
     * \param imageWidth the histogram
     */
    void setTexture( unsigned char* array, int imageWidth, int imageHeight );

    /**
     * Set the scaling factor for the opacity.
     * \param factor scaling factor of opacity values
     */
    void setOpacityScale( double factor )
    {
        m_opacityScale = factor;
    }

    /**
     * Get the histogram going along with the transfer function
     *
     * \returns the stored histogram
     */
    std::shared_ptr< WHistogram2D > getHistogram( )
    {
        return m_histogram;
    }

    /**
     * sample/render the transfer function linearly between min and max in an RGBA texture.
     * \param array pointer to an allocated data structure
     * \param width is the number of RGBA samples.
     * \param height is the number of RGBA samples.
     * \post array contains the sampled data
     * \pre array is allocated and has space for width * height elements
     */
    void sample2DTransferFunction( unsigned char*array, int width, int height ) const;

private:
    /**
     * Holds a reference to transfer function texture
     */
    osg::ref_ptr< osg::Image > tfTexture;

    /**
     * Holds a reference to the histogram to be rendered
     */
    std::shared_ptr< WHistogram2D > m_histogram;

    /**
     * Factor by which the output opacity is scaled
     * to allow for easier specification of very small
     * opacity values.
     */
    double m_opacityScale;


    friend std::ostream& operator << ( std::ostream& out, const WTransferFunction2D& tf );
};

/**
 * Default output operator. Currently stores values the same way as it is done in the properties.
 * This code should only be used for debugging and you should not rely on the interface.
 *
 * \param tf The transfer function to output
 * \param out The stream to which we write
 *
 * \returns reference to out
 */
std::ostream& operator << ( std::ostream& out, const WTransferFunction2D& tf );

#endif  // WTRANSFERFUNCTION2D_H
