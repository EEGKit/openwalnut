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
 * and the manipulation widgets.
 */
class WTransferFunction2D // NOLINT problem with macro
{
public:
    /**
     * Default constructor of a meaningless 2D transfer function
     */
    WTransferFunction2D() : m_opacityScale( 1.0 ),
                            m_histogram( nullptr )
    {
    }

    /**
     * Deep copy constructor.
     *
     * \param rhs the value to histogram
     */
    WTransferFunction2D( const WTransferFunction2D &rhs )
            : m_opacityScale( rhs.m_opacityScale ),
              m_histogram( rhs.m_histogram ),
              m_widget( rhs.m_widget )
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
            this->m_opacityScale = rhs.m_opacityScale;
            setHistogram( rhs.m_histogram );
            this->m_widget = rhs.m_widget;
            return ( *this );
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
     * \param histogram the 2D histogram
     */
    void setHistogram( const std::shared_ptr< WHistogram2D>& histogram );

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
     * Get the number of widgets
     *
     * \returns the number of box widgets
     */
    size_t numBoxWidgets() const
    {
        return m_widget.size();
    }

    /**
     * The isovalue of the color at a given index.
     *
     * \param i the index widget in the list.
     * \returns the posX of the widget at index i
     */
    double getIsovalueX( size_t i ) const
    {
        return m_widget.at( i ).isoX;
    }

    /**
     * The isovalue of the color at a given index.
     *
     * \param i the index widget in the list.
     * \returns the posY of the widget at index i
     */
    double getIsovalueY( size_t i ) const
    {
        return m_widget.at( i ).isoY;
    }

    /**
     * The width of the box widget at a given index.
     *
     * \param i the index widget in the list.
     * \returns the width of the widget at index i
     */
    double getWidth( size_t i ) const
    {
        return m_widget.at( i ).width;
    }

    /**
     * The height of the box widget at a given index.
     *
     * \param i the index widget in the list.
     * \returns the height of the widget at index i
     */
    double getHeight( size_t i ) const
    {
        return m_widget.at( i ).height;
    }

    /**
     * The color of the i-th boxwidget in the list.
     *
     * \param i the index widget in the list.
     * \returns the color of the widget at index i
     */
    WColor getColor( size_t i ) const
    {
        return m_widget.at( i ).color;
    }

    /**
     * Insert a new box widget
     *
     * \param isoX the new iso value
     * \param isoY the new iso value
     * \param width the new iso value
     * \param height the new iso value
     * \param color the new color at the given iso value
     */
    void addBoxWidget( double isoX, double isoY, double width, double height, const WColor& color );

    /**
     * sample/render the transfer function into the given container as a set of RGBA quadruples.
     * \param array pointer to an allocated data structure
     * \param width is the number of RGBA samples in X direction.
     * \param height is the number of RGBA samples in Y direction.
     * \post array contains the sampled data
     * \pre array is allocated and has space for width * height elements
     */
    void sample2DTransferFunction( unsigned char*array, int width, int height ) const;

private:
    /**
     * Prototype for a storage element
     */
    struct Entry
    {
        /**
         * Default constructor
         * \param isoX pos x
         * \param isoY pos y
         */
        explicit Entry( double isoX, double isoY ) : isoX( isoX ), isoY( isoY )
        {
        }

        /**
         * comparison by isovalue
         * \param rhs entry to compare t
         * \returns true if this->iso <=  rhs.iso
         */
        bool operator <= ( const Entry &rhs ) const
        {
            return isoX <= rhs.isoX && isoY <= rhs.isoY;
        }

        /** the isovalue x aka. the position x */
        double isoX;
        /** the isovalue y aka. the position y */
        double isoY;
    };

    /**
     * Entry of a box widget with its position, width, height and color
     */
    struct BoxEntry : public Entry
    {
        /** default constructor
         * \param isoX the x pos
         * \param isoY the y pos
         * \param width the width of the box
         * \param height the height of the box
         * \param color the color of the box
         */
        BoxEntry( double isoX, double isoY, double width, double height, WColor color ) : Entry( isoX, isoY ),
                                                                                          width( width ), height( height ), color( color )
        {
        }

        /**
         * comparison operator to check for changes
         * \param rhs BoxWidget to compare to
         * \returns true if rhs equals this
         */
        bool operator==( const BoxEntry& rhs ) const
        {
            return isoX == rhs.isoX && isoY == rhs.isoY && width == rhs.width
                   && height == rhs.height && color == rhs.color;
        }

        /** the width of the box */
        double width;
        /** the height of the box */
        double height;
        /** the color of the box */
        WColor color;
    };

    /**
     * Factor by which the output opacity is scaled
     * to allow for easier specification of very small
     * opacity values.
     */
    double m_opacityScale;

    /**
     * Holds a reference to the histogram to be rendered
     */
    std::shared_ptr< WHistogram2D > m_histogram;

    /**
    * Holds a list of box widgets used in the 2D TF
    */
    std::vector< BoxEntry > m_widget;

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
