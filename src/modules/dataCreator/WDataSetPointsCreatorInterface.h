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

#ifndef WDATASETPOINTSCREATORINTERFACE_H
#define WDATASETPOINTSCREATORINTERFACE_H

#include <core/common/WProgress.h>
#include <core/common/math/linearAlgebra/WMatrixFixed.h>
#include <core/dataHandler/WDataSetPoints.h>

/**
 * Define the interface which is injected into an \ref WObjectNDIP. Remember that \ref WObjectNDIP is a template class deriving from its
 * template type. This way we can inject methods into the base class. It avoids derivation from \ref WObjectNDIP.
 *
 * This class is especially useful for all dataset types that are WDataSetPoints types.
 */
class WDataSetPointsCreatorInterface
{
public:
    /**
     * Create the dataset. This needs to be implemented by all the creators you write.
     *
     * \param seed the seed for the random values.
     * \param progress progress indicator
     * \param color color of all points
     * \param numPoints number of points
     * \param origin origin of the bbox
     * \param size size of the bounding box
     * \param vertices the vertex array. Fill this.
     * \param colors the color array. Fill this.
     */
    virtual void operator()( int seed,
                             WProgress::SPtr progress,
                             const WColor& color,
                             size_t numPoints,
                             const WPosition& origin,
                             const WPosition& size,
                             WDataSetPoints::VertexArray vertices,
                             WDataSetPoints::ColorArray colors ) = 0;

    /**
     * Destructor
     */
    virtual ~WDataSetPointsCreatorInterface();
};

#endif  // WDATASETPOINTSCREATORINTERFACE_H

