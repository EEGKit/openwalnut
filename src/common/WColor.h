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

#ifndef WCOLOR_H
#define WCOLOR_H

#include <cassert>
#include <istream>
#include <ostream>
#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>

/**
 * Represents a RGBA Color
 */
class WColor
{
public:

    /**
     * Standard way of constructing colors, alpha is optional, and black is default
     * \param red red value between [0,1]
     * \param green green value between [0,1]
     * \param blue blue value between [0,1]
     * \param alpha opacity value between [0,1]
     */
    WColor( float red = 0.0, float green = 0.0, float blue = 0.0, float alpha = 1.0 );

    /**
     * Sets the green channel for this color
     * \param green green value between [0,1]
     */
    void setGreen( float green );

    /**
     * Sets the blue channel for this color
     * \param blue blue value between [0,1]
     */
    void setBlue( float blue );

    /**
     * Sets the red channel for this color
     * \param red red value between [0,1]
     */
    void setRed( float red );

    /**
     * Sets the alpha channel for this color
     * \param alpha opacity value between [0,1]
     */
    void setAlpha( float alpha );

    /**
     * \return red channel for this color
     */
    float getRed() const;

    /**
     * \return green channel for this color
     */
    float getGreen() const;

    /**
     * \return blue channel for this color
     */
    float getBlue() const;

    /**
     * \return alpha channel for this color
     */
    float getAlpha() const;

    /**
     * Reset this color via a given HSV color.
     * \param h hue
     * \param s saturation
     * \param v value
     */
    void setHSV( double h, double s, double v );

    /**
     * Reset all channels at once.
     * \param r red value between [0,1]
     * \param g green value between [0,1]
     * \param b blue value between [0,1]
     */
    void setRGB( double r, double g, double b );

protected:
private:
    float m_red; //!< Red channel
    float m_green; //!< Green channel
    float m_blue; //!< Blue channel
    float m_alpha; //!< Alpha channel
};

/**
 * Write a color in string represensation to the given output stream.
 */
std::ostream& operator<<( std::ostream& out, const WColor& c );

/**
 * Write a color in string represensation to the given input stream.
 */
std::istream& operator>>( std::istream& in, WColor& c );

#endif  // WCOLOR_H
