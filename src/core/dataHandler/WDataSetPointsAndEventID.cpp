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

#include "WDataSetPointsAndEventID.h"

boost::shared_ptr< WPrototyped > WDataSetPointsAndEventID::m_prototype = boost::shared_ptr< WPrototyped >();

WDataSetPointsAndEventID::WDataSetPointsAndEventID( WDataSetPointsAndEventID::VertexArray vertices,
                                                WDataSetPointsAndEventID::ColorArray colors,
                                                WDataSetPointsAndEventID::EventIDArray eventIDs,
                                                WBoundingBox boundingBox ):
    WDataSetPoints( vertices, colors, boundingBox ),
    m_eventIDs( eventIDs )
{
    WAssert( eventIDs->size() == vertices->size() / 3 , "Number of ints in eventID array must be 1 per vertex" );
}

WDataSetPointsAndEventID::WDataSetPointsAndEventID( WDataSetPointsAndEventID::VertexArray vertices,
                                                WDataSetPointsAndEventID::ColorArray colors,
                                                WDataSetPointsAndEventID::EventIDArray eventIDs ):
    WDataSetPoints( vertices, colors ),
    m_eventIDs( eventIDs )
{
    WAssert( eventIDs->size() == vertices->size() / 3 , "Number of ints in eventID array must be 1 per vertex" );
}

WDataSetPointsAndEventID::WDataSetPointsAndEventID()
{
}

WDataSetPointsAndEventID::~WDataSetPointsAndEventID()
{
}

const std::string WDataSetPointsAndEventID::getName() const
{
    return "WDataSetPointsAndEventID";
}

const std::string WDataSetPointsAndEventID::getDescription() const
{
    return "Dataset which contains points including their eventID.";
}

boost::shared_ptr< WPrototyped > WDataSetPointsAndEventID::getPrototype()
{
    if( !m_prototype )
    {
        m_prototype = boost::shared_ptr< WPrototyped >( new WDataSetPointsAndEventID() );
    }

    return m_prototype;
}

WDataSetPointsAndEventID::EventIDArray WDataSetPointsAndEventID::getEventIDs() const
{
    return m_eventIDs;
}

int WDataSetPointsAndEventID::getEventID( const int pointIdx ) const
{
    if( !isValidPointIdx( pointIdx ) )
    {
        throw WOutOfBounds( "The specified index is invalid." );
    }

    return m_eventIDs->operator[]( pointIdx );
}
