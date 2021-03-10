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

#include "WDataSet.h"
#include "WDataSetPoints.h"

/**
 * Dataset to store a bunch of points and Fibers.
 */
class WDataSetPointsAndEventID : public WDataSet 
{
    /**
     * represents a boost::shared_ptr to a vector containing a vector of floats.
     */
    typedef boost::shared_ptr< std::vector< int > > SPIntVector;

public:
    /**
     * Construct WDataSetPointsAndEventID object.
     *
     * \param points Bunch of points.
     * \param eventIDs Bunch of eventIDs.
     */
    explicit WDataSetPointsAndEventID( WDataSetPoints::SPtr points,  SPIntVector eventIDs );

    /**
     * The standard constructor.
     */
    WDataSetPointsAndEventID();

    /**
     * Destructs this dataset.
     */
    virtual ~WDataSetPointsAndEventID();

    /**
     * Getter method to receive points.
     *
     * \return m_points  a dataset with given points.
     */
    WDataSetPoints::SPtr getPoints();

    /**
     * Getter method to receive fibers.
     *
     * \return m_eventIDs a dataset with given EventIDs.
     */
    SPIntVector getEventIDs();

private:
    /**
     * Dataset to store a bunch of points.
     */
    WDataSetPoints::SPtr m_points;

    /**
     * Dataset to store a bunch of  Fibers. 
     */
    SPIntVector m_eventIDs;

};

#endif  // WDATASETPOINTSANDEVENTID_H