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

#include "core/common/math/WMath.h"

#include "WDataCreatorFiberTorus.h"

WDataCreatorFiberTorus::WDataCreatorFiberTorus():
    WObjectNDIP< WDataSetFibersCreatorInterface >( "Torus", "Create fibers in a torus shape." )
{
    m_innerRadius = m_properties->addProperty( "Inner radius", "The inner radius of the torus.", 0.1 );
    m_innerRadius->setMin( 0.01 );
    m_innerRadius->setMax( 0.99 );

    m_generateInner = m_properties->addProperty( "Generate inner fibers", "Whether inner fibers should be generated.", true );


    m_rotateX = m_properties->addProperty( "X rotation", "The rotation of the torus around the x axis.", 0.0 );
    m_rotateX->setMin( 0 );
    m_rotateX->setMax( pi() * 2 );

    m_rotateY = m_properties->addProperty( "Y rotation", "The rotation of the torus around the y axis.", 0.0 );
    m_rotateY->setMin( 0 );
    m_rotateY->setMax( pi() * 2 );

    m_rotateZ = m_properties->addProperty( "Z rotation", "The rotation of the torus around the z axis.", 0.0 );
    m_rotateZ->setMin( 0 );
    m_rotateZ->setMax( pi() * 2 );
}

WDataCreatorFiberTorus::~WDataCreatorFiberTorus()
{
    // cleanup
}

void WDataCreatorFiberTorus::operator()( int seed,
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
    double innerRadius = m_innerRadius->get() / 2.0;

    std::uniform_real_distribution< double > unifRadius( 0.01, innerRadius );
    std::uniform_real_distribution< double > unifAngle( 0.0, pi() * 2.0 );
    std::default_random_engine re;
    re.seed( seed );

    WMatrix4d transform(
        osg::Matrixd::rotate(
            m_rotateX->get(), osg::Vec3d( 1, 0, 0 ), m_rotateY->get(), osg::Vec3d( 0, 1, 0 ), m_rotateZ->get(), osg::Vec3d( 0, 0, 1 ) ) *
        osg::Matrixd::scale( size.x() / 2.0, size.y() / 2.0, size.z() / 2.0 ) *
        osg::Matrixd::translate( origin.x(), origin.y(), origin.z() )
    );

    for( size_t fidx = 0; fidx < numFibers; ++fidx )
    {
        double irad = m_generateInner->get() ? unifRadius( re ) : innerRadius;
        double iangle = unifAngle( re );

        double angleParam = pi() * 2.0 / numVertsPerFiber;

        fibIdx->push_back( fidx * numVertsPerFiber );
        lengths->push_back( numVertsPerFiber );

        double radCache = ( ( 1.0 - innerRadius ) + irad * cos( iangle ) );

        for( size_t vidx = 0; vidx < numVertsPerFiber; ++vidx )
        {
            double x = radCache * cos( angleParam * vidx );
            double y = radCache * sin( angleParam * vidx );
            double z = ( irad * sin( iangle ) );

            WVector4d vec( x, y, z, 1 );
            vec = transform * vec;

            vertices->push_back( vec.x() );
            vertices->push_back( vec.y() );
            vertices->push_back( vec.z() );

            colors->push_back( color.x() );
            colors->push_back( color.y() );
            colors->push_back( color.z() );
            fibIdxVertexMap->push_back( fidx );
        }

        ++( *progress );
    }
}
