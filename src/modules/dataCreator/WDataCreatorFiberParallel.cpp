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

#include "WDataCreatorFiberParallel.h"

WDataCreatorFiberParallel::WDataCreatorFiberParallel():
    WObjectNDIP< WDataSetFibersCreatorInterface >( "Parallel", "Create parallel fibers." )
{
    m_direction = m_properties->addProperty( "Direction", "The direction of the parallel fibers", WPosition( 1.0, 0.0, 0.0 ) );
}

WDataCreatorFiberParallel::~WDataCreatorFiberParallel()
{
    // cleanup
}

void WDataCreatorFiberParallel::operator()( int seed,
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
    WAssert( length2( m_direction->get() ) != 0, "The direction should not be (0, 0, 0)!" );

    std::uniform_real_distribution< double > unif( 0.0, 1.0 );
    std::default_random_engine re;
    re.seed( seed );

    WVector3d dir = m_direction->get();

    for( size_t fidx = 0; fidx < numFibers; ++fidx )
    {
        double x = unif( re ) * size.x();
        double y = unif( re ) * size.y();
        double z = unif( re ) * size.z();

        // parameter of ray to calculate the intersection points
        double tmin, tmax, tymin, tymax, tzmin, tzmax;
        tmin  = ( 0.0 * size.x() - x ) / dir.x();
        tmax  = ( 1.0 * size.x() - x ) / dir.x();
        tymin = ( 0.0 * size.y() - y ) / dir.y();
        tymax = ( 1.0 * size.y() - y ) / dir.y();
        tzmin = ( 0.0 * size.z() - z ) / dir.z();
        tzmax = ( 1.0 * size.z() - z ) / dir.z();
        tmin = fmax( fmax( tmin, tymin ), tzmin );
        tmax = fmin( fmin( tmax, tymax ), tzmax );

        WVector3d pos( x, y, z );                           // start position of the ray
        WVector3d min = pos + ( dir * tmin );               // first intersection
        WVector3d max = pos + ( dir * tmax );               // second intersection
        WVector3d ray = ( max - min ) / static_cast< double >( numVertsPerFiber );
        // ^- Build new ray between start and end so that all vertices can fit in between

        fibIdx->push_back( fidx * numVertsPerFiber );
        lengths->push_back( numVertsPerFiber );

        for( size_t vidx = 0; vidx < numVertsPerFiber; ++vidx )
        {
            WVector3d vec = ray * static_cast< double >( vidx );

            vertices->push_back( origin.x() + min.x() + vec.x() );
            vertices->push_back( origin.y() + min.y() + vec.y() );
            vertices->push_back( origin.z() + min.z() + vec.z() );

            colors->push_back( color.x() );
            colors->push_back( color.y() );
            colors->push_back( color.z() );
            fibIdxVertexMap->push_back( fidx );
        }

        ++( *progress );
    }
}
