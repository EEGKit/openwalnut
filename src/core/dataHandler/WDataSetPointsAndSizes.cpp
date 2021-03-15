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

#include <string>

#include "WDataSetPointsAndSizes.h"


boost::shared_ptr< WPrototyped > WDataSetPointsAndSizes::m_prototype = boost::shared_ptr< WPrototyped >();

WDataSetPointsAndSizes::WDataSetPointsAndSizes( WDataSetPointsAndSizes::VertexArray vertices,
                                                WDataSetPointsAndSizes::ColorArray colors,
                                                WDataSetPointsAndSizes::SizeArray sizes,
                                                WBoundingBox boundingBox ):
    WDataSetPoints( vertices, colors, boundingBox ),
    m_sizes( sizes )
{
    WAssert( sizes->size() == vertices->size() / 3 , "Number of floats in size array must be 1 per vertex" );
}

WDataSetPointsAndSizes::WDataSetPointsAndSizes( WDataSetPointsAndSizes::VertexArray vertices,
                                                WDataSetPointsAndSizes::ColorArray colors,
                                                WDataSetPointsAndSizes::SizeArray sizes ):
    WDataSetPoints( vertices, colors ),
    m_sizes( sizes )
{
    WAssert( sizes->size() == vertices->size() / 3 , "Number of floats in size array must be 1 per vertex" );
}

WDataSetPointsAndSizes::WDataSetPointsAndSizes()
{
}

WDataSetPointsAndSizes::~WDataSetPointsAndSizes()
{
}

const std::string WDataSetPointsAndSizes::getName() const
{
    return "WDataSetPointsAndSizes";
}

const std::string WDataSetPointsAndSizes::getDescription() const
{
    return "Dataset which contains points including their size.";
}

boost::shared_ptr< WPrototyped > WDataSetPointsAndSizes::getPrototype()
{
    if( !m_prototype )
    {
        m_prototype = boost::shared_ptr< WPrototyped >( new WDataSetPointsAndSizes() );
    }

    return m_prototype;
}

WDataSetPointsAndSizes::SizeArray WDataSetPointsAndSizes::getSizes() const
{
    return m_sizes;
}

float WDataSetPointsAndSizes::getSize( const size_t pointIdx ) const
{
    if( !isValidPointIdx( pointIdx ) )
    {
        throw WOutOfBounds( "The specified index is invalid." );
    }

    return m_sizes->operator[]( pointIdx );
}
