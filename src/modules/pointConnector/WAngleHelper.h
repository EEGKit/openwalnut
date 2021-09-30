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

#include <unordered_map>
#include <utility>
#include <vector>

#include "WFiberHandler.h"

/**
 * Contains functions to help determine the path with the smallest angle change
 */
namespace WAngleHelper
{
    /**
     * Hash function for the unorder map
     */
    struct HashFn
    {
        /**
         * Hashes a WPosition object.
         * \param pos The position.
         * \return size_t The hash.
         */
        size_t operator()( const WPosition& pos ) const
        {
            return  ( ( std::hash< double >{}( pos.x() )                    // NOLINT
                    ^ ( std::hash< double >{}( pos.y() ) << 1 ) ) >> 1 )    // NOLINT
                    ^ ( std::hash< double >{}( pos.z() ) << 1 );            // NOLINT
        }
    };

    /**
     * An unordered map from WPosition to WPosition.
     */
    typedef std::unordered_map< WPosition, WPosition, HashFn > PositionMap;

    /**
     * An unordered map from WPosition to double.
     */
    typedef std::unordered_map< WPosition, double, HashFn > PositionDoubleMap;

    /**
     * An unordered map from WPosition to a PositionDoubleMap
     */
    typedef std::unordered_map< WPosition, PositionMap, HashFn > PositionLineMap;

    /**
     * The output of the createLine function
     */
    typedef std::pair< WAngleHelper::PositionLineMap, WAngleHelper::PositionLineMap > DJLinePair;

    /**
     * The output of the dijkstra algorithm
     */
    typedef std::pair< PositionMap, PositionDoubleMap > DJOut;

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
