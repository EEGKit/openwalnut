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

#ifndef WANGLEHELPER_H
#define WANGLEHELPER_H

#include <map>
#include <utility>
#include <vector>

#include "WFiberHandler.h"

/**
 * Contains functions to help determine the path with the smallest angle change
 */
namespace WAngleHelper
{
    /**
     * The lines that are checked by the dijkstra algorithm
     */
    typedef std::tuple< WPosition, WPosition, double > DJLine;

    /**
     * The output of the dijkstra algorithm
     */
    typedef std::pair< std::map< WPosition, WPosition >, std::map< WPosition, double > > DJOut;

    /**
     * Determines the path with the smallest angle change
     * \param positions The positions to build the path upon
     * \return std::vector< WPosition > All the positions on the smallest path
     */
    std::vector< WPosition > findSmoothestPath( std::vector< WPosition > positions );

    /**
     * Determines the path with the smallest angle change based on the given fiber
     * \param positions The positions to build the path upon
     * \param fiber The fiber used to adjust the path
     * \return std::vector< WPosition > All the positions on the smallest path
     */
    std::vector< WPosition > findSmoothestPath( std::vector< WPosition > positions, WFiberHandler::PCFiber fiber );

    /**
     * Calculates the angle between two positions.
     * \param a The first position.
     * \param b The second position.
     * \return double The angle.
     */
    double calculateAngle( WPosition a, WPosition b );
}

#endif  // WANGLEHELPER_H
