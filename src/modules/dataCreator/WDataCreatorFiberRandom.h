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

#ifndef WDATACREATORFIBERRANDOM_H
#define WDATACREATORFIBERRANDOM_H

#include "WDataSetFibersCreatorInterface.h"
#include "core/common/WObjectNDIP.h"

/**
 * Create random fibers
 */
class WDataCreatorFiberRandom: public WObjectNDIP< WDataSetFibersCreatorInterface >
{
public:
    /**
     * Abbreviate shared_ptr
     */
    typedef std::shared_ptr< WDataCreatorFiberRandom > SPtr;

    /**
     * Abbreviate const shared_ptr
     */
    typedef std::shared_ptr< const WDataCreatorFiberRandom > ConstSPtr;

    /**
     * Default constructor.
     */
    WDataCreatorFiberRandom();

    /**
     * Destructor.
     */
    virtual ~WDataCreatorFiberRandom();

    /**
     * Create the dataset. This needs to be implemented by all the creators you write.
     *
     * \param seed the seed for the random values.
     * \param progress progress indicator
     * \param color color of all fibers
     * \param numFibers number of fibers
     * \param numVertsPerFiber number of vertices per fiber
     * \param origin origin of the bbox
     * \param size size of the bounding box
     * \param vertices the vertex array. Fill this.
     * \param fibIdx the fiber index array. Fill this.
     * \param lengths the lengths array. Fill this.
     * \param fibIdxVertexMap inverse map. Fill this.
     * \param colors the color array. Fill this.
     */
    virtual void operator()( int seed,
                             WProgress::SPtr progress,
                             const WColor& color,
                             size_t numFibers,
                             size_t numVertsPerFiber,
                             const WPosition& origin,
                             const WPosition& size,
                             WDataSetFibers::VertexArray vertices,
                             WDataSetFibers::IndexArray fibIdx,
                             WDataSetFibers::LengthArray lengths,
                             WDataSetFibers::IndexArray fibIdxVertexMap,
                             WDataSetFibers::ColorArray colors );
protected:
private:
};

#endif  // WDATACREATORFIBERRANDOM_H
