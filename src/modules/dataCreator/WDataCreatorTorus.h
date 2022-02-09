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

#ifndef WDATACREATORTORUS_H
#define WDATACREATORTORUS_H

#include <random>

#include "core/common/math/WMath.h"
#include "core/common/WLimits.h"
#include "core/common/WObjectNDIP.h"

#include "WDataSetFibersCreatorInterface.h"
#include "WDataSetPointsCreatorInterface.h"

/**
 * Create a torus
 */
template< class T >
class WDataCreatorTorus: public WObjectNDIP< T >
{
public:
    /**
     * Abbreviate shared_ptr
     */
    typedef std::shared_ptr< WDataCreatorTorus< T > > SPtr;

    /**
     * Abbreviate const shared_ptr
     */
    typedef std::shared_ptr< const WDataCreatorTorus< T > > ConstSPtr;

    /**
     * Default constructor.
     */
    WDataCreatorTorus();

    /**
     * Destructor.
     */
    virtual ~WDataCreatorTorus();

    /**
     * Create the fiber dataset. This needs to be implemented by all the creators you write.
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

    /**
     * Create the point dataset. This needs to be implemented by all the creators you write.
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
                             WDataSetPoints::ColorArray colors );
protected:
private:
    /**
     * Calculates the transformation matrix
     * 
     * \param origin origin of the bbox
     * \param size size of the bounding box
     * \return WMatrix4d The transformation matrix
     */
    WMatrix4d calculateTransform( const WPosition& origin, const WPosition& size );

    /**
     * Calculates a vertex on a torus.
     * 
     * \param torusRadius The radius of the torus ( to the midpoint of the ring)
     * \param ringRadius The radius of the ring
     * \param torusAngle The radius around the torus
     * \param ringAngle The radius around the ring
     * \return WVector4d The vector for the vertex
     */
    WVector4d calculateTorusVertex( double torusRadius, double ringRadius, double torusAngle, double ringAngle );

    /**
     * The inner radius of the torus.
     */
    WPropDouble m_innerRadius;

    /**
     * Property whether inner fibers should be created.
     */
    WPropBool m_generateInner;

    /**
     * The rotation angle for the x axis.
     */
    WPropDouble m_rotateX;

    /**
     * The rotation angle for the x axis.
     */
    WPropDouble m_rotateY;

    /**
     * The rotation angle for the x axis.
     */
    WPropDouble m_rotateZ;
};


typedef WDataCreatorTorus< WDataSetFibersCreatorInterface > WDataCreatorFibersTorus;
typedef WDataCreatorTorus< WDataSetPointsCreatorInterface > WDataCreatorPointsTorus;


template< class T >
WDataCreatorTorus< T >::WDataCreatorTorus():
    WObjectNDIP< T >( "Torus", "Create data in a torus shape." )
{
    m_innerRadius = WObjectNDIP< T >::m_properties->addProperty( "Inner radius", "The inner radius of the torus.", 0.1 );
    m_innerRadius->setMin( 0.01 );
    m_innerRadius->setMax( 0.99 );

    m_generateInner = WObjectNDIP< T >::m_properties->addProperty( "Generate inner data", "Whether inner data should be generated.", true );


    m_rotateX = WObjectNDIP< T >::m_properties->addProperty( "X rotation", "The rotation of the torus around the x axis.", 0.0 );
    m_rotateX->setMin( 0 );
    m_rotateX->setMax( pi() * 2 );

    m_rotateY = WObjectNDIP< T >::m_properties->addProperty( "Y rotation", "The rotation of the torus around the y axis.", 0.0 );
    m_rotateY->setMin( 0 );
    m_rotateY->setMax( pi() * 2 );

    m_rotateZ = WObjectNDIP< T >::m_properties->addProperty( "Z rotation", "The rotation of the torus around the z axis.", 0.0 );
    m_rotateZ->setMin( 0 );
    m_rotateZ->setMax( pi() * 2 );
}

template< class T >
WDataCreatorTorus< T >::~WDataCreatorTorus()
{
    // cleanup
}

template< class T >
WMatrix4d WDataCreatorTorus< T >::calculateTransform( const WPosition& /*origin*/, const WPosition& size )
{
    return WMatrix4d(
        osg::Matrixd::scale( size.x() / 2.0, size.y() / 2.0, size.z() / 2.0 ) *
        osg::Matrixd::rotate(
            m_rotateX->get(), osg::Vec3d( 1, 0, 0 ), m_rotateY->get(), osg::Vec3d( 0, 1, 0 ), m_rotateZ->get(), osg::Vec3d( 0, 0, 1 ) )
    );
}

template< class T >
WVector4d WDataCreatorTorus< T >::calculateTorusVertex( double torusRadius, double ringRadius, double torusAngle, double ringAngle )
{
    double radCache = ( torusRadius + ringRadius * cos( ringAngle ) );
    double x = radCache * cos( torusAngle );
    double y = radCache * sin( torusAngle );
    double z = ( ringRadius * sin( ringAngle ) );

    return WVector4d( x, y, z, 1.0 );
}


template< class T >
void WDataCreatorTorus< T >::operator()( int seed,
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

    WMatrix4d transform = calculateTransform( origin, size );

    float lowX, lowY, lowZ;
    lowX = lowY = lowZ = wlimits::MAX_FLOAT;

    for( size_t fidx = 0; fidx < numFibers; ++fidx )
    {
        double irad = m_generateInner->get() ? unifRadius( re ) : innerRadius;
        double iangle = unifAngle( re );

        double angleParam = pi() * 2.0 / numVertsPerFiber;

        fibIdx->push_back( fidx * numVertsPerFiber );
        lengths->push_back( numVertsPerFiber );

        for( size_t vidx = 0; vidx < numVertsPerFiber; ++vidx )
        {
            WVector4d vec = calculateTorusVertex( 1.0 - innerRadius, irad, angleParam * vidx, iangle );
            vec = transform * vec;  // for whatever reason the translation inside the matrix does not work, so we do it manually
            vec = WVector4d( vec.x() + origin.x() + size.x() / 2.0,
                             vec.y() + origin.y() + size.y() / 2.0,
                             vec.z() + origin.z() + size.z() / 2.0, 1.0 );

            lowX = fmin( lowX, vec.x() );
            lowY = fmin( lowY, vec.y() );
            lowZ = fmin( lowZ, vec.z() );

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

    for( size_t vidx = 0; vidx < vertices->size(); vidx += 3 )
    {
        vertices->at( vidx + 0 ) -= lowX;
        vertices->at( vidx + 1 ) -= lowY;
        vertices->at( vidx + 2 ) -= lowZ;
    }
}

template< class T >
void WDataCreatorTorus< T >::operator()( int seed,
                             WProgress::SPtr progress,
                             const WColor& color,
                             size_t numPoints,
                             const WPosition& origin,
                             const WPosition& size,
                             WDataSetPoints::VertexArray vertices,
                             WDataSetPoints::ColorArray colors )
{
    double innerRadius = m_innerRadius->get() / 2.0;

    std::uniform_real_distribution< double > unifRadius( 0.01, innerRadius );
    std::uniform_real_distribution< double > unifAngle( 0.0, pi() * 2.0 );
    std::default_random_engine re;
    re.seed( seed );

    WMatrix4d transform = calculateTransform( origin, size );

    float lowX, lowY, lowZ;
    lowX = lowY = lowZ = wlimits::MAX_FLOAT;

    for( size_t pidx = 0; pidx < numPoints; ++pidx )
    {
        double irad = m_generateInner->get() ? unifRadius( re ) : innerRadius;
        double iangle = unifAngle( re );
        double iangle2 = unifAngle( re );

        WVector4d vec = calculateTorusVertex( 1.0 - innerRadius, irad, iangle2, iangle );
        vec = transform * vec;
        vec = WVector4d( vec.x() + origin.x() + size.x() / 2.0,
                         vec.y() + origin.y() + size.y() / 2.0,
                         vec.z() + origin.z() + size.z() / 2.0, 1.0 );

        lowX = fmin( lowX, vec.x() );
        lowY = fmin( lowY, vec.y() );
        lowZ = fmin( lowZ, vec.z() );

        vertices->push_back( vec.x() );
        vertices->push_back( vec.y() );
        vertices->push_back( vec.z() );

        colors->push_back( color.x() );
        colors->push_back( color.y() );
        colors->push_back( color.z() );

        ++( *progress );
    }

    for( size_t vidx = 0; vidx < vertices->size(); vidx += 3 )
    {
        vertices->at( vidx + 0 ) -= lowX;
        vertices->at( vidx + 1 ) -= lowY;
        vertices->at( vidx + 2 ) -= lowZ;
    }
}

#endif  // WDATACREATORTORUS_H
