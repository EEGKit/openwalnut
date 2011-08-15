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

#include "../common/WAssert.h"

#include "WGridRegular3D2.h"

WGridRegular3D2::WGridRegular3D2( std::size_t nx, std::size_t ny, std::size_t nz )
    : m_coordsX( nx ),
      m_coordsY( ny ),
      m_coordsZ( nz )
{
}

WGridRegular3D2::~WGridRegular3D2()
{
}

std::size_t WGridRegular3D2::numVoxels() const
{
    return m_coordsX * m_coordsY * m_coordsZ;
}

std::size_t WGridRegular3D2::size() const
{
    return numVoxels();
}

std::size_t WGridRegular3D2::getNbVoxelsX() const
{
    return m_coordsX;
}

std::size_t WGridRegular3D2::getNbVoxelsY() const
{
    return m_coordsY;
}

std::size_t WGridRegular3D2::getNbVoxelsZ() const
{
    return m_coordsZ;
}

boost::array< std::size_t, 3 > WGridRegular3D2::getNbVoxelsXYZ() const
{
    boost::array< std::size_t, 3 > res;
    res[ 0 ] = m_coordsX;
    res[ 1 ] = m_coordsY;
    res[ 2 ] = m_coordsZ;
    return res;
}

std::size_t WGridRegular3D2::sliceSize( std::size_t dir ) const
{
    switch( dir )
    {
    case 0:
        return m_coordsX * m_coordsY;
    case 1:
        return m_coordsX * m_coordsZ;
    case 2:
        return m_coordsY * m_coordsZ;
    }

    return 0;
}

WGridRegular3D2::CoordinateType WGridRegular3D2::transformToGridSpace( CoordinateType const& coords ) const
{
    // TODO( reichenbach ): implement this
    return coords;
}

bool WGridRegular3D2::isInGrid( CoordinateType const& coords ) const
{
    CoordinateType c = transformToGridSpace( coords );
    return c[ 0 ] >= 0.0 && c[ 1 ] >= 0.0 && c[ 2 ] >= 0.0 && c[ 0 ] <= -1.0 + m_coordsX
             && c[ 1 ] <= -1.0 + m_coordsY && c[ 2 ] <= -1.0 + m_coordsZ;
}

