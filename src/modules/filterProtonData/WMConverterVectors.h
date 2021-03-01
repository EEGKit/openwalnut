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

#ifndef WMCONVERTERVECTORS_H
#define WMCONVERTERVECTORS_H

#include <vector>

#include <boost/shared_ptr.hpp>

/**
 * represents a boost::shared_ptr to a vector containing a vector of floats.
 */
typedef boost::shared_ptr< std::vector< float > > SPFloatVector;

/**
 * represents a boost::shared_ptr to a vector containing a vector of size_t.
 */
typedef boost::shared_ptr< std::vector< size_t > > SPSizeVector;

class WMConverterVectors
{
public:
    typedef boost::shared_ptr< WMConverterVectors > SPtr;

    WMConverterVectors();

    void clear();

    SPFloatVector getVertices();

    SPFloatVector getSizes();

    SPFloatVector getColors();

    SPFloatVector getEdeps();

    SPSizeVector getFiberStartIndexes();

    SPSizeVector getFiberLengths();

    SPSizeVector getVerticesReverse();

    SPSizeVector getEventIDs();

private:
    SPFloatVector m_vertices;

    SPFloatVector m_sizes;

    SPFloatVector m_colors;

    SPFloatVector m_edeps;

    SPSizeVector m_fiberStartIndexes;

    SPSizeVector m_fiberLengths;

    SPSizeVector m_verticesReverse;

    SPSizeVector m_eventIDs;
};

#endif  // WMCONVERTERVECTORS_H
