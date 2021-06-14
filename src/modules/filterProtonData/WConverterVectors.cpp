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

#include "WConverterVectors.h"


WConverterVectors::WConverterVectors()
{
    m_vertices = SPFloatVector( new std::vector< float >() );
    m_sizes = SPFloatVector( new std::vector< float >() );
    m_colors = SPFloatVector( new std::vector< float >() );
    m_edeps = SPFloatVector( new std::vector< float >() );
    m_fiberStartIndexes = SPSizeVector( new std::vector< size_t >() );
    m_fiberLengths = SPSizeVector( new std::vector< size_t >() );
    m_verticesReverse = SPSizeVector( new std::vector< size_t >() );
    m_eventIDs = SPSizeVector( new std::vector< size_t >() );
}

void WConverterVectors::clear()
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

SPFloatVector WConverterVectors::getVertices()
{
    return m_vertices;
}

SPFloatVector WConverterVectors::getSizes()
{
    return m_sizes;
}

SPFloatVector WConverterVectors::getColors()
{
    return m_colors;
}

SPFloatVector WConverterVectors::getEdeps()
{
    return m_edeps;
}

SPSizeVector WConverterVectors::getFiberStartIndexes()
{
    return m_fiberStartIndexes;
}

SPSizeVector WConverterVectors::getFiberLengths()
{
    return m_fiberLengths;
}

SPSizeVector WConverterVectors::getVerticesReverse()
{
    return m_verticesReverse;
}

SPSizeVector WConverterVectors::getEventIDs()
{
    return m_eventIDs;
}

