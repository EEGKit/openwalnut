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

#include <string>

#include <osg/LineWidth>
#include <osg/LightModel>

#include "WROIBox.h"
#include "../kernel/WKernel.h"

size_t WROIBox::maxBoxId = 0;

void buildFacesFromPoints( osg::DrawElementsUInt* surfaceElements )
{
    surfaceElements->push_back( 0 );
    surfaceElements->push_back( 2 );
    surfaceElements->push_back( 3 );
    surfaceElements->push_back( 1 );

    surfaceElements->push_back( 2 );
    surfaceElements->push_back( 6 );
    surfaceElements->push_back( 7 );
    surfaceElements->push_back( 3 );

    surfaceElements->push_back( 6 );
    surfaceElements->push_back( 4 );
    surfaceElements->push_back( 5 );
    surfaceElements->push_back( 7 );

    surfaceElements->push_back( 4 );
    surfaceElements->push_back( 0 );
    surfaceElements->push_back( 1 );
    surfaceElements->push_back( 5 );

    surfaceElements->push_back( 1 );
    surfaceElements->push_back( 3 );
    surfaceElements->push_back( 7 );
    surfaceElements->push_back( 5 );

    surfaceElements->push_back( 0 );
    surfaceElements->push_back( 4 );
    surfaceElements->push_back( 6 );
    surfaceElements->push_back( 2 );
}

void buildLinesFromPoints( osg::DrawElementsUInt* surfaceElements )
{
    surfaceElements->push_back( 0 );
    surfaceElements->push_back( 2 );
    surfaceElements->push_back( 2 );
    surfaceElements->push_back( 3 );
    surfaceElements->push_back( 3 );
    surfaceElements->push_back( 1 );
    surfaceElements->push_back( 1 );
    surfaceElements->push_back( 0 );

    surfaceElements->push_back( 6 );
    surfaceElements->push_back( 4 );
    surfaceElements->push_back( 4 );
    surfaceElements->push_back( 5 );
    surfaceElements->push_back( 5 );
    surfaceElements->push_back( 7 );
    surfaceElements->push_back( 7 );
    surfaceElements->push_back( 6 );

    surfaceElements->push_back( 2 );
    surfaceElements->push_back( 6 );
    surfaceElements->push_back( 7 );
    surfaceElements->push_back( 3 );


    surfaceElements->push_back( 4 );
    surfaceElements->push_back( 0 );
    surfaceElements->push_back( 1 );
    surfaceElements->push_back( 5 );
}

void setVertices( osg::Vec3Array* vertices, wmath::WPosition minPos, wmath::WPosition maxPos )
{
    vertices->push_back( osg::Vec3( minPos[0], minPos[1], minPos[2] ) );
    vertices->push_back( osg::Vec3( minPos[0], minPos[1], maxPos[2] ) );
    vertices->push_back( osg::Vec3( minPos[0], maxPos[1], minPos[2] ) );
    vertices->push_back( osg::Vec3( minPos[0], maxPos[1], maxPos[2] ) );
    vertices->push_back( osg::Vec3( maxPos[0], minPos[1], minPos[2] ) );
    vertices->push_back( osg::Vec3( maxPos[0], minPos[1], maxPos[2] ) );
    vertices->push_back( osg::Vec3( maxPos[0], maxPos[1], minPos[2] ) );
    vertices->push_back( osg::Vec3( maxPos[0], maxPos[1], maxPos[2] ) );
}

WROIBox::WROIBox( wmath::WPosition minPos, wmath::WPosition maxPos ) :
    WROI(),
    boxId( maxBoxId++ )
{
    m_minPos = minPos;
    m_maxPos = maxPos;

    // connect updateGFX with picking
    WKernel* kernel = WKernel::getRunningKernel();
    assert( kernel );
    boost::shared_ptr< WGEViewer > viewer = kernel->getGraphicsEngine()->getViewerByName( "main" );
    assert( viewer );
    m_pickHandler = viewer->getPickHandler();
    m_pickHandler->getPickSignal()->connect( boost::bind( &WROIBox::updateGFX, this, _1 ) );

    osg::Geometry* surfaceGeometry = new osg::Geometry();
    m_geode = new osg::Geode;

    std::stringstream ss;
    ss <<  "ROIBox" << boxId;

    m_geode->setName( ss.str() );

    osg::Vec3Array* vertices = new osg::Vec3Array;
    setVertices( vertices, minPos, maxPos );
    surfaceGeometry->setVertexArray( vertices );

    osg::DrawElementsUInt* surfaceElements;
    surfaceElements = new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 0 );
    buildFacesFromPoints( surfaceElements );

    osg::DrawElementsUInt* lineElements;
    lineElements = new osg::DrawElementsUInt( osg::PrimitiveSet::LINES, 0 );
    buildLinesFromPoints( lineElements );

    surfaceGeometry->addPrimitiveSet( surfaceElements );
    surfaceGeometry->addPrimitiveSet( lineElements );
    m_geode->addDrawable( surfaceGeometry );
    osg::StateSet* state = m_geode->getOrCreateStateSet();
    state->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

    osg::LineWidth* linewidth = new osg::LineWidth();
    linewidth->setWidth( 2.f );
    state->setAttributeAndModes( linewidth, osg::StateAttribute::ON );

    // ------------------------------------------------
    // colors
    osg::Vec4Array* colors = new osg::Vec4Array;

    colors->push_back( osg::Vec4( .0f, .0f, 1.f, 0.5f ) );
    surfaceGeometry->setColorArray( colors );
    surfaceGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    osg::ref_ptr<osg::LightModel> lightModel = new osg::LightModel();
    lightModel->setTwoSided( true );
    state->setAttributeAndModes( lightModel.get(), osg::StateAttribute::ON );
    state->setMode( GL_BLEND, osg::StateAttribute::ON  );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->addChild( m_geode );
}

wmath::WPosition WROIBox::getMinPos() const
{
    return m_minPos;
}

wmath::WPosition WROIBox::getMaxPos() const
{
    return m_maxPos;
}


void WROIBox::updateGFX( std::string text )
{
    boost::shared_lock<boost::shared_mutex> slock;
    slock = boost::shared_lock<boost::shared_mutex>( m_updateLock );

    std::stringstream ss;
    ss <<  "\"ROIBox" << boxId << "\"";
    if( text.find( "Object ") != std::string::npos
        && text.find( ss.str() ) != std::string::npos )
    {
        wmath::WPosition newPos( m_pickHandler->getHitPosition() );
        if( m_isPicked )
        {
            wmath::WVector3D moveVec = m_pickedPosition - newPos;
            osg::Vec3Array* vertices = new osg::Vec3Array;
            m_minPos -= moveVec;
            m_maxPos -= moveVec;
            setVertices( vertices, m_minPos, m_maxPos );
            ((osg::Geometry*)(m_geode->getDrawable( 0 )))->setVertexArray( vertices );
        }
        else
        {
            osg::Vec4Array* colors = new osg::Vec4Array;
            colors->push_back( osg::Vec4( 1.f, .0f, .0f, 0.5f ) );
            ((osg::Geometry*)(m_geode->getDrawable( 0 )))->setColorArray( colors );
        }
        m_pickedPosition = newPos;
        m_isPicked = true;
    }
    if( m_isPicked && text.find( "unpick" ) != std::string::npos )
    {
        osg::Vec4Array* colors = new osg::Vec4Array;
        colors->push_back( osg::Vec4( 0.f, 0.f, 1.f, 0.5f ) );
        ((osg::Geometry*)(m_geode->getDrawable( 0 )))->setColorArray( colors );
        m_isPicked = false;
    }
    slock.unlock();
}
