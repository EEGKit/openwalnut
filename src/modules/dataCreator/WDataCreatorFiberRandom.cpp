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

#include <random>

#include "WDataCreatorFiberRandom.h"

WDataCreatorFiberRandom::WDataCreatorFiberRandom():
    WObjectNDIP< WDataSetFibersCreatorInterface >( "Random", "Create random fibers." )
{
}

WDataCreatorFiberRandom::~WDataCreatorFiberRandom()
{
    // cleanup
}

void WDataCreatorFiberRandom::operator()( int seed,
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
                                        WDataSetFibers::ColorArray colors )
{
    std::uniform_real_distribution< double > unif( 0.0, 1.0 );
    std::default_random_engine re;
    re.seed( seed );

    for( size_t fidx = 0; fidx < numFibers; ++fidx )
    {
        double x1 = unif( re ) * size.x();
        double y1 = unif( re ) * size.y();
        double z1 = unif( re ) * size.z();
        double x2 = unif( re ) * size.x();
        double y2 = unif( re ) * size.y();
        double z2 = unif( re ) * size.z();

        WVector3d v1( x1, y1, z1 );
        WVector3d v2( x2, y2, z2 );

        WVector3d ray = ( v2 - v1 ) / static_cast< double >( numVertsPerFiber );
        // ^- Build new ray between start and end so that all vertices can fit in between

        fibIdx->push_back( fidx * numVertsPerFiber );
        lengths->push_back( numVertsPerFiber );

        for( size_t vidx = 0; vidx < numVertsPerFiber; ++vidx )
        {
            WVector3d vec = ray * static_cast< double >( vidx );

            vertices->push_back( origin.x() + v1.x() + vec.x() );
            vertices->push_back( origin.y() + v1.y() + vec.y() );
            vertices->push_back( origin.z() + v1.z() + vec.z() );

            colors->push_back( color.x() );
            colors->push_back( color.y() );
            colors->push_back( color.z() );
            fibIdxVertexMap->push_back( fidx );
        }

        ++( *progress );
    }
}
