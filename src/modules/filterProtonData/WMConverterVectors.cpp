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

#include <vector>

#include "WMConverterVectors.h"

WMConverterVectors::WMConverterVectors()
{
    m_vertices = SPFloatVector( new std::vector< float >() );
    m_sizes = SPFloatVector( new std::vector< float >() );
    m_colors = SPFloatVector( new std::vector< float >() );
    m_edeps = SPFloatVector( new std::vector< float >() );
    m_fiberStartIndexes = SPSizeVector( new std::vector< size_t >() );
    m_fiberLengths = SPSizeVector( new std::vector< size_t >() );
    m_verticesReverse = SPSizeVector( new std::vector< size_t >() );
    m_eventIDs = SPSizeVector( new std::vector< size_t >() );
    m_selectedEventIDs = SPIntVector( new std::vector< int >() );
}

void WMConverterVectors::clear()
{
    m_vertices->clear();
    m_sizes->clear();
    m_colors->clear();
    m_edeps->clear();
    m_fiberStartIndexes->clear();
    m_fiberLengths->clear();
    m_verticesReverse->clear();
    m_eventIDs->clear();
}

SPFloatVector WMConverterVectors::getVertices()
{
    return m_vertices;
}

SPFloatVector WMConverterVectors::getSizes()
{
    return m_sizes;
}

SPFloatVector WMConverterVectors::getColors()
{
    return m_colors;
}

SPFloatVector WMConverterVectors::getEdeps()
{
    return m_edeps;
}

SPSizeVector WMConverterVectors::getFiberStartIndexes()
{
    return m_fiberStartIndexes;
}

SPSizeVector WMConverterVectors::getFiberLengths()
{
    return m_fiberLengths;
}

SPSizeVector WMConverterVectors::getVerticesReverse()
{
    return m_verticesReverse;
}

SPSizeVector WMConverterVectors::getEventIDs()
{
    return m_eventIDs;
}

SPIntVector WMConverterVectors::getSelectedEventIDs()
{
    return m_selectedEventIDs;
}

