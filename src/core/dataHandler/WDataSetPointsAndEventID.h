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

#ifndef WDATASETPOINTSANDEVENTID_H
#define WDATASETPOINTSANDEVENTID_H

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "WDataSetPoints.h"

/**
 * Dataset to store a bunch of points with their sizes but without order or topology.
 */
class WDataSetPointsAndEventID : public WDataSetPoints
{
public:
    /**
     * Pointer to dataset.
     */
    typedef boost::shared_ptr< WDataSetPointsAndEventID > SPtr;

    /**
     * Pointer to const dataset.
     */
    typedef boost::shared_ptr< const WDataSetPointsAndEventID > ConstSPtr;

    /**
     * Sizes for each vertix in VertexArray
     */
    typedef boost::shared_ptr< std::vector< int > > EventIDArray;

    /**
     * Constructs a new set of points. If no color is specified, white is used for all points.
     *
     * \note the number of floats in vertices must be a multiple of 3
     * \note the number of floats in colors (if not NULL) must be vertices->size() / 3  times one of 1,3, or 4
     * \note the number of floats in sizes must be vertices->size() / 3
     *
     * \param vertices the vertices of the points, stored in x1,y1,z1,x2,y2,z2, ..., xn,yn,zn scheme
     * \param colors the colors of each vertex. Can be NULL. Stored as R1,G1,B1,A1, ... Rn,Gn,Bn,An
     * \param eventIDs the eventID of each vertex.
     * \param boundingBox The bounding box of the points (first minimum, second maximum).
     */
    WDataSetPointsAndEventID( VertexArray vertices, ColorArray colors,
                            EventIDArray eventIDs, WBoundingBox boundingBox );

    /**
     * Constructs a new set of points. The bounding box is calculated during construction. If no color is specified, white is used for all
     * points.
     *
     * \note the number of floats in vertices must be a multiple of 3
     * \note the number of floats in colors (if not NULL) must be vertices->size() / 3  times one of 1,3, or 4
     * \note the number of floats in sizes must be vertices->size() / 3
     *
     * \param vertices the vertices of the points, stored in x1,y1,z1,x2,y2,z2, ..., xn,yn,zn scheme
     * \param eventIDs the eventID of each vertex.
     * \param colors the colors of each vertex. Can be NULL. Stored as R1,[G1,B1,[A1,]] ... Rn,[Gn,Bn,[An]]
     */
    WDataSetPointsAndEventID( VertexArray vertices, ColorArray colors,
                            EventIDArray eventIDs );

    /**
     * Constructs a new set of points. The constructed instance is empty.
     */
    WDataSetPointsAndEventID();

    /**
     * Destructor.
     */
    ~WDataSetPointsAndEventID();

    /**
     * Gets the name of this prototype.
     *
     * \return the name.
     */
    const std::string getName() const;

    /**
     * Gets the description for this prototype.
     *
     * \return the description
     */
    const std::string getDescription() const;

    /**
     * Returns a prototype instantiated with the true type of the deriving class.
     *
     * \return the prototype.
     */
    static boost::shared_ptr< WPrototyped > getPrototype();

    /**
     * Getter for the point eventIDs
     * \return The eventIDs
     */
    EventIDArray getEventIDs() const;

    /**
     * The eventID of a given point.
     *
     * \throw WOutOfBounds if invalid index is used.
     * \param pointIdx the point index.
     *
     * \return the eventID
     */
    int getEventID( const int pointIdx ) const;

protected:
    /**
     * The prototype as singleton.
     */
    static boost::shared_ptr< WPrototyped > m_prototype;

private:
    /**
     * An array of the eventID per vertex.
     */
    EventIDArray m_eventIDs;
};

#endif  // WDATASETPOINTSANDEVENTID_H
