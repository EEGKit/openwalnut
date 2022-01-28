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

/**
 * A class that stores a 1D transfer function which consists
 * of a linear interpolation of alpha and color values.
 */
class WTransferFunction2D // NOLINT problem with macro
{
public:
    /**
     * Default constructor of a meaningless transfer function
     */
    WTransferFunction2D() : m_isominDataSet0( 0. ), m_isomaxDataSet0( 0. ),
                            m_isominDataSet1( 0. ), m_isomaxDataSet1( 0. ),
                            m_opacityScale( 1.0 )
    {
    }

    /**
     * Deep copy constructor.
     *
     * \param rhs the value to histogram
     */
    WTransferFunction2D( const WTransferFunction2D &rhs )
            : m_isominDataSet0( rhs.m_isominDataSet0 ), m_isomaxDataSet0( rhs.m_isomaxDataSet0 )
            , m_isominDataSet1( rhs.m_isominDataSet1 ), m_isomaxDataSet1( rhs.m_isomaxDataSet1 )
            , m_opacityScale( 1.0 )
    {
    }

    /**
     * Deep copy operator
     *
     * \param rhs the value to copy
     * \returns reference to current object
     * \returns reference to current object
     */
    WTransferFunction2D& operator=( const WTransferFunction2D &rhs )
    {
        this->m_isominDataSet0 = rhs.m_isominDataSet0;
        this->m_isomaxDataSet0 = rhs.m_isomaxDataSet0;
        this->m_isominDataSet1 = rhs.m_isominDataSet1;
        this->m_isomaxDataSet1 = rhs.m_isomaxDataSet1;
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

private:

    /**
     * The smallest used iso value of the first data set.
     */
    double m_isominDataSet0;

    /**
     * The largest used iso value of the first data set.
     */
    double m_isomaxDataSet0;

    /**
     * The smallest used iso value of the second data set.
     */
    double m_isominDataSet1;

    /**
     * The largest used iso value of the seocond data set.
     */
    double m_isomaxDataSet1;

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
