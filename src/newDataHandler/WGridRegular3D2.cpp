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


std::size_t WGridRegular3D2::numVoxels()
{
    return m_coordsX * m_coordsY * m_coordsZ;
}

std::size_t WGridRegular3D2::getVoxelIndex( std::size_t x, std::size_t y, std::size_t z )
{
    return x + y * m_coordsX + z * m_coordsX * m_coordsY;
}

boost::array< std::size_t, 3 > WGridRegular3D2::getVoxelCoords( std::size_t index )
{
#ifdef _DEBUG
    WAssert( index < numVoxels(), "The index was too large for this grid." );
#endif

    boost::array< std::size_t, 3 > res;

    std::size_t xy = m_coordsX * m_coordsY;

    res[ 2 ] = index / xy;
    res[ 1 ] = index % xy;
    res[ 0 ] = res[ 1 ] / m_coordsX;
    res[ 1 ] =res[ 1 ] % m_coordsX;

    return res;
}

std::size_t WGridRegular3D2::getNbVoxelsX()
{
    return m_coordsX;
}

std::size_t WGridRegular3D2::getNbVoxelsY()
{
    return m_coordsY;
}

std::size_t WGridRegular3D2::getNbVoxelsZ()
{
    return m_coordsZ;
}

boost::array< std::size_t, 3 > WGridRegular3D2::getNbVoxelsXYZ()
{
    boost::array< std::size_t, 3 > res;
    res[ 0 ] = m_coordsX;
    res[ 1 ] = m_coordsY;
    res[ 2 ] = m_coordsZ;
    return res;
}
