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

bool WTransferFunction2D::operator==( const WTransferFunction2D &rhs ) const
{
    if( m_histogram == rhs.m_histogram )
    {
        //wlog::debug("WTransferFunction2D") << "== true";
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
void WTransferFunction2D::setHistogram( const WHistogram2D &histogram )
{
    //wlog::debug("WTransferFunction2D") << "setHistogram";
    if( histogram != m_histogram )
        m_histogram = histogram;
}
std::ostream& operator << ( std::ostream& out, const WTransferFunction2D& tf )
{
    out << "";
    return out;
}
