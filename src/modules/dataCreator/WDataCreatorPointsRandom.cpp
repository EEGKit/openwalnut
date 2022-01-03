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

#include "WDataCreatorPointsRandom.h"

WDataCreatorPointsRandom::WDataCreatorPointsRandom():
    WObjectNDIP< WDataSetPointsCreatorInterface >( "Random", "Create random points." )
{
}

WDataCreatorPointsRandom::~WDataCreatorPointsRandom()
{
    // cleanup
}

void WDataCreatorPointsRandom::operator()( int seed,
                             WProgress::SPtr progress,
                             const WColor& color,
                             size_t numPoints,
                             const WPosition& origin,
                             const WPosition& size,
                             WDataSetPoints::VertexArray vertices,
                             WDataSetPoints::ColorArray colors )
{
    std::uniform_real_distribution< double > unif( -0.5, 0.5 );
    std::default_random_engine re;
    re.seed( seed );


    for( size_t pidx = 0; pidx < numPoints; ++pidx )
    {
        double x = unif( re ) * size.x();
        double y = unif( re ) * size.y();
        double z = unif( re ) * size.z();
        vertices->push_back( origin.x() + x );
        vertices->push_back( origin.y() + y );
        vertices->push_back( origin.z() + z );

        colors->push_back( color.x() );
        colors->push_back( color.y() );
        colors->push_back( color.z() );

        ++( *progress );
    }
}
