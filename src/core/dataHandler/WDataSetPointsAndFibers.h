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

#ifndef WDATASETPOINTSANDFIBERS_H
#define WDATASETPOINTSANDFIBERS_H

#include "WDataSet.h"
#include "WDataSetPoints.h"
#include "WDataSetFibers.h"

/**
 * Dataset to store a bunch of points and Fibers.
 */
class WDataSetPointsAndFibers : public WDataSet 
{
public:
    /**
     * Construct WDataSetPointsAndFibers object
     *
     * \param points Bunch of points.
     * \param fibers Bunch of fibers.
     */
    explicit WDataSetPointsAndFibers( WDataSetPoints::SPtr points, WDataSetFibers::SPtr fibers );

    /**
     * The standard constructor.
     */
    WDataSetPointsAndFibers();

    /**
     * Destructs this dataset.
     */
    virtual ~WDataSetPointsAndFibers();

    /**
     * Getter method to receive points
     *
     * \return m_points (Dataset to store a bunch of points.) 
     */
    WDataSetPoints::SPtr getPoints();

    /**
     * Getter method to receive fibers
     *
     * \return m_fibers (dataset to store a bunch of fibers.)
     */
    WDataSetFibers::SPtr getFibers();

private:
    /**
     * Dataset to store a bunch of points.
     */
    WDataSetPoints::SPtr m_points;

    /**
     * Dataset to store a bunch of  Fibers. 
     */
    WDataSetFibers::SPtr m_fibers;

};

#endif  // WDATASETPOINTSANDFIBERS_H