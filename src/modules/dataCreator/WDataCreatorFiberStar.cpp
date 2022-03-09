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

#include "WDataCreatorFiberStar.h"

WDataCreatorFiberStar::WDataCreatorFiberStar():
    WObjectNDIP< WDataSetFibersCreatorInterface >( "Star", "Create a star of fibers." )
{
}

WDataCreatorFiberStar::~WDataCreatorFiberStar()
{
    // cleanup
}

void WDataCreatorFiberStar::operator()( int seed,
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
    std::uniform_real_distribution< double > unif( -0.5, 0.5 );
    std::default_random_engine re;
    re.seed( seed );

    for( size_t fidx = 0; fidx < numFibers; ++fidx )
    {
        WVector3d dir( unif( re ), unif( re ), unif( re ) );
        dir = normalize( dir );

        WVector3d ray( dir.x() * ( size.x() / 2.0 ), dir.y() * ( size.y() / 2.0 ), dir.z() * ( size.z() / 2.0 ) );
        ray /= numVertsPerFiber;

        fibIdx->push_back( fidx * numVertsPerFiber );
        lengths->push_back( numVertsPerFiber );

        for( size_t vidx = 0; vidx < numVertsPerFiber; ++vidx )
        {
            WVector3d vec = ray * static_cast< double >( vidx );

            vertices->push_back( origin.x() + vec.x() + size.x() / 2.0 );
            vertices->push_back( origin.y() + vec.y() + size.y() / 2.0 );
            vertices->push_back( origin.z() + vec.z() + size.z() / 2.0 );

            colors->push_back( color.x() );
            colors->push_back( color.y() );
            colors->push_back( color.z() );
            fibIdxVertexMap->push_back( fidx );
        }

        ++( *progress );
    }
}
