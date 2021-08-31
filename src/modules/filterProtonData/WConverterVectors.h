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

#ifndef WCONVERTERVECTORS_H
#define WCONVERTERVECTORS_H

#include <memory>
#include <vector>



/**
 * represents a std::shared_ptr to a vector containing a vector of floats.
 */
typedef std::shared_ptr< std::vector< float > > SPFloatVector;

/**
 * represents a std::shared_ptr to a vector containing a vector of size_t.
 */
typedef std::shared_ptr< std::vector< size_t > > SPSizeVector;

/**
 * Holds the vectors that store the data for the points and fibers.
 */
class WConverterVectors
{
public:
    /**
     * A shared_ptr to this class.
     */
    typedef std::shared_ptr< WConverterVectors > SPtr;

    /**
     * Initializes the vectors as empty vectors.
     */
    WConverterVectors();

    /**
     * Clears all vectors.
     */
    void clear();

    /**
     * Gets the vertices as shared_ptr to a float vector.
     * \return SPFloatVector The vertices.
     */
    SPFloatVector getVertices();

    /**
     * Gets the sizes as shared_ptr to a float vector.
     * \return SPFloatVector The sizes.
     */
    SPFloatVector getSizes();

    /**
     * Gets the colors as shared_ptr to a float vector.
     * \return SPFloatVector The colors.
     */
    SPFloatVector getColors();

    /**
     * Gets the edeps as shared_ptr to a float vector.
     * \return SPFloatVector The edeps.
     */
    SPFloatVector getEdeps();

    /**
     * Gets the fiber start indexes as shared_ptr to a size_t vector.
     * \return SPSizeVector The fiber start indexes.
     */
    SPSizeVector getFiberStartIndexes();

    /**
     * Gets the fiber lengths as shared_ptr to a size_t vector.
     * \return SPSizeVector The fiber lengths.
     */
    SPSizeVector getFiberLengths();

    /**
     * Gets the reverse vertices as shared_ptr to a size_t vector.
     * \return SPSizeVector The reverse vertices.
     */
    SPSizeVector getVerticesReverse();

    /**
     * Gets the event IDs as shared_ptr to a size_t vector.
     * \return SPSizeVector The event IDs.
     */
    SPSizeVector getEventIDs();

private:
    SPFloatVector m_vertices;   //!< The vertices.

    SPFloatVector m_sizes;  //!< The sizes.

    SPFloatVector m_colors; //!< The colors.

    SPFloatVector m_edeps;  //!< The edeps.

    SPSizeVector m_fiberStartIndexes;   //!< The fiber start indexes.

    SPSizeVector m_fiberLengths;    //!< The fiber lengths.

    SPSizeVector m_verticesReverse; //!< The reverse vertices.

    SPSizeVector m_eventIDs;    //!< the event IDs.
};

#endif  // WCONVERTERVECTORS_H
