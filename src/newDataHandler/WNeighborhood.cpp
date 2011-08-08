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

#include "WNeighborhood.h"

WNeighborhood::WNeighborhood()
{
}

WNeighborhood::~WNeighborhood()
{
}

void WNeighborhood::add( int offX, int offY, int offZ )
{
    m_offsets.push_back( offX );
    m_offsets.push_back( offY );
    m_offsets.push_back( offZ );
}

boost::array< int, 3 > WNeighborhood::get( std::size_t num ) const
{
    boost::array< int, 3 > res = { { m_offsets[ 3 * num + 0 ], m_offsets[ 3 * num + 1 ], m_offsets[ 3 * num + 2 ] } };  // NOLINT braces
    return res;
}

std::size_t WNeighborhood::size() const
{
    return m_offsets.size() / 3;
}

WNeighborhood makeMoore( int distance )
{
    WNeighborhood nbh;
    if( distance > 0 )
    {
        for( int z = -distance; z <= distance; ++z )
        {
            for( int y = -distance; y <= distance; ++y )
            {
                for( int x = -distance; x <= distance; ++x )
                {
                    if( x != 0 || y != 0 || z != 0 )
                    {
                        nbh.add( x, y, z );
                    }
                }
            }
        }
    }
    return nbh;
}

WNeighborhood makeVonNeumann( int distance )
{
    WNeighborhood nbh;
    if( distance > 0 )
    {
        for( int z = -distance; z <= distance; ++z )
        {
            for( int y = -distance; y <= distance; ++y )
            {
                for( int x = -distance; x <= distance; ++x )
                {
                    int a = std::abs( x ) + std::abs( y ) + std::abs( z );
                    if( 0 < a && a <= distance )
                    {
                        nbh.add( x, y, z );
                    }
                }
            }
        }
    }
    return nbh;
}
