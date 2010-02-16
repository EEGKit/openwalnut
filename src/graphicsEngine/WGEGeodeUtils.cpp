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

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Vec3>
#include <osg/Array>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>

#include "../math/WPosition.h"
#include "WGEGeodeUtils.h"
#include "WGEUtils.h"

osg::ref_ptr< osg::Geode > wge::generateBoundingBoxGeode( const wmath::WPosition& pos1, const wmath::WPosition& pos2, const WColor& color )
{
    assert( pos1[0] <= pos2[0] && pos1[1] <= pos2[1] && pos1[2] <= pos2[2] && "pos1 doesn't seem to be the frontLowerLeft corner of the BB!" );
    using osg::ref_ptr;
    ref_ptr< osg::Vec3Array > vertices = ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    ref_ptr< osg::Vec4Array > colors = ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    ref_ptr< osg::Geometry > geometry = ref_ptr< osg::Geometry >( new osg::Geometry );

    vertices->push_back( osg::Vec3( pos1[0], pos1[1], pos1[2] ) );
    vertices->push_back( osg::Vec3( pos2[0], pos1[1], pos1[2] ) );
    vertices->push_back( osg::Vec3( pos2[0], pos2[1], pos1[2] ) );
    vertices->push_back( osg::Vec3( pos1[0], pos2[1], pos1[2] ) );
    vertices->push_back( osg::Vec3( pos1[0], pos1[1], pos1[2] ) );
    vertices->push_back( osg::Vec3( pos1[0], pos1[1], pos2[2] ) );
    vertices->push_back( osg::Vec3( pos2[0], pos1[1], pos2[2] ) );
    vertices->push_back( osg::Vec3( pos2[0], pos2[1], pos2[2] ) );
    vertices->push_back( osg::Vec3( pos1[0], pos2[1], pos2[2] ) );
    vertices->push_back( osg::Vec3( pos1[0], pos1[1], pos2[2] ) );

    geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, 0, vertices->size() ) );

    vertices->push_back( osg::Vec3( pos1[0], pos2[1], pos1[2] ) );
    vertices->push_back( osg::Vec3( pos1[0], pos2[1], pos2[2] ) );
    vertices->push_back( osg::Vec3( pos2[0], pos2[1], pos1[2] ) );
    vertices->push_back( osg::Vec3( pos2[0], pos2[1], pos2[2] ) );
    vertices->push_back( osg::Vec3( pos2[0], pos1[1], pos1[2] ) );
    vertices->push_back( osg::Vec3( pos2[0], pos1[1], pos2[2] ) );

    geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINES, vertices->size() - 6, 6 ) );

    geometry->setVertexArray( vertices );
    colors->push_back( wge::osgColor( color ) );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_OVERALL );
    osg::ref_ptr< osg::Geode > geode = osg::ref_ptr< osg::Geode >( new osg::Geode );
    geode->addDrawable( geometry );

    // disable light for this geode as lines can't be lit properly
    osg::StateSet* state = geode->getOrCreateStateSet();
    state->setMode( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );

    return geode;
}

osg::ref_ptr< osg::Geometry > wge::createUnitCube( const WColor& color )
{
    // create the unit cube manually as the ShapeDrawable and osg::Box does not provide 3D texture coordinates
    osg::ref_ptr< osg::Geometry > cube = new osg::Geometry();
    osg::ref_ptr< osg::Vec3Array > vertices = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    osg::ref_ptr< osg::Vec3Array > normals = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    osg::ref_ptr< osg::Vec4Array > colors = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array );

    // front face
    vertices->push_back( osg::Vec3( 0.0, 0.0, 0.0 ) );
    vertices->push_back( osg::Vec3( 1.0, 0.0, 0.0 ) );
    vertices->push_back( osg::Vec3( 1.0, 1.0, 0.0 ) );
    vertices->push_back( osg::Vec3( 0.0, 1.0, 0.0 ) );
    normals->push_back( osg::Vec3( 0.0, 0.0, -1.0 ) );

    // back face
    vertices->push_back( osg::Vec3( 0.0, 0.0, 1.0 ) );
    vertices->push_back( osg::Vec3( 1.0, 0.0, 1.0 ) );
    vertices->push_back( osg::Vec3( 1.0, 1.0, 1.0 ) );
    vertices->push_back( osg::Vec3( 0.0, 1.0, 1.0 ) );
    normals->push_back( osg::Vec3( 0.0, 0.0, 1.0 ) );

    // left
    vertices->push_back( osg::Vec3( 0.0, 0.0, 0.0 ) );
    vertices->push_back( osg::Vec3( 0.0, 1.0, 0.0 ) );
    vertices->push_back( osg::Vec3( 0.0, 1.0, 1.0 ) );
    vertices->push_back( osg::Vec3( 0.0, 0.0, 1.0 ) );
    normals->push_back( osg::Vec3( -1.0, 0.0, 0.0 ) );

    // right
    vertices->push_back( osg::Vec3( 1.0, 0.0, 0.0 ) );
    vertices->push_back( osg::Vec3( 1.0, 1.0, 0.0 ) );
    vertices->push_back( osg::Vec3( 1.0, 1.0, 1.0 ) );
    vertices->push_back( osg::Vec3( 1.0, 0.0, 1.0 ) );
    normals->push_back( osg::Vec3( 1.0, 0.0, 0.0 ) );

    // bottom
    vertices->push_back( osg::Vec3( 0.0, 0.0, 0.0 ) );
    vertices->push_back( osg::Vec3( 1.0, 0.0, 0.0 ) );
    vertices->push_back( osg::Vec3( 1.0, 0.0, 1.0 ) );
    vertices->push_back( osg::Vec3( 0.0, 0.0, 1.0 ) );
    normals->push_back( osg::Vec3( 0.0, -1.0, 0.0 ) );

    // top
    vertices->push_back( osg::Vec3( 0.0, 1.0, 0.0 ) );
    vertices->push_back( osg::Vec3( 1.0, 1.0, 0.0 ) );
    vertices->push_back( osg::Vec3( 1.0, 1.0, 1.0 ) );
    vertices->push_back( osg::Vec3( 0.0, 1.0, 1.0 ) );
    normals->push_back( osg::Vec3( 0.0, 1.0, 0.0 ) );

    // set it up and set arrays
    cube->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, vertices->size() ) );
    cube->setVertexArray( vertices );

    // set 3D texture coordinates here.
    cube->setTexCoordArray( 0, vertices );

    // set normals
    cube->setNormalArray( normals );
    cube->setNormalBinding( osg::Geometry::BIND_PER_PRIMITIVE );

    // finally, the colors
    colors->push_back( wge::osgColor( color ) );
    cube->setColorArray( colors );
    cube->setColorBinding( osg::Geometry::BIND_OVERALL );

    return cube;
}

osg::ref_ptr< osg::Node > wge::generateSolidBoundingBoxNode( const wmath::WPosition& pos1, const wmath::WPosition& pos2, const WColor& color,
        bool threeDTexCoords )
{
    assert( pos1[0] <= pos2[0] && pos1[1] <= pos2[1] && pos1[2] <= pos2[2] && "pos1 doesn't seem to be the frontLowerLeft corner of the BB!" );

    // create a uni cube
    osg::ref_ptr< osg::Geode > cube = new osg::Geode();
    if ( threeDTexCoords )
    {
        cube->addDrawable( createUnitCube( color ) );
    }
    else
    {
        osg::ref_ptr< osg::ShapeDrawable > cubeDrawable = new osg::ShapeDrawable( new osg::Box( osg::Vec3( 0.5, 0.5, 0.5 ), 1.0 ) );
        cubeDrawable->setColor( wge::osgColor( color ) );
        cube->addDrawable( cubeDrawable );
    }

    // transform the cube to match the bbox
    osg::Matrixd transformM;
    osg::Matrixd scaleM;
    transformM.makeTranslate( wge::osgVec3( pos1 ) );
    scaleM.makeScale( wge::osgVec3( pos2 - pos1 ) );

    // apply transformation to bbox
    osg::ref_ptr< osg::MatrixTransform > transform = new osg::MatrixTransform();
    transform->setMatrix( transformM * scaleM );
    transform->addChild( cube );

    // we do not need light
    osg::StateSet* state = cube->getOrCreateStateSet();
    state->setMode( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );

    return transform;
}

