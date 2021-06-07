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

#include "WSelectionManager.h"

static const osg::Vec4 SEL_COLOR( 0.39, 0.58, 0.93, 1 );

WSelectionManager::WSelectionManager():
    m_overlay( new WOverlay() ),
    m_selectionType( WSelectionType::BRUSH ),
    m_hasStarted( false )
{
    m_selectionHandler = new WSelectionHandler( this );
    WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getView()->addEventHandler( m_selectionHandler );
}

WSelectionManager::~WSelectionManager()
{
    WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getView()->removeEventHandler( m_selectionHandler );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_overlay );
}

void WSelectionManager::start( float x, float y )
{
    if( m_hasStarted )
    {
        return;
    }
    m_hasStarted = true;

    m_line.clear();
    WPosition pos( x, y, 0 );

    m_line.push_back( pos );
    if( m_selectionType == WSelectionType::BOX )
    {
        m_line.push_back( pos );
    }
    updateDisplay();
}

void WSelectionManager::end( float x, float y )
{
    if( !m_hasStarted )
    {
        return;
    }
    m_hasStarted = false;

    WPosition pos( x, y, 0 );
    switch( m_selectionType )
    {
        case WSelectionType::BRUSH:
            m_line.push_back( pos );
            break;
        case WSelectionType::LINELOOP:
            m_line.push_back( pos );
            m_line.push_back( m_line.at( 0 ) );
            break;
        case WSelectionType::BOX:
            m_line.at( 1 ) = pos;
            break;
    }
    updateDisplay();
}

void WSelectionManager::move( float x, float y )
{
    if( !m_hasStarted )
    {
        return;
    }

    WPosition pos( x, y, 0 );
    switch( m_selectionType )
    {
        case WSelectionType::BRUSH:
        case WSelectionType::LINELOOP:
            m_line.push_back( pos );
            break;
        case WSelectionType::BOX:
            m_line.at( 1 ) = pos;
            break;
    }

    updateDisplay();
}

enum WSelectionManager::WSelectionType WSelectionManager::getSelectionType()
{
    return m_selectionType;
}

void WSelectionManager::setSelectionType( enum WSelectionManager::WSelectionType selectionType )
{
    m_selectionType = selectionType;
}

void WSelectionManager::updateDisplay()
{
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_overlay );

    m_overlay = new WOverlay();

    osg::ref_ptr< osg::Geode > lines = new osg::Geode();
    osg::ref_ptr< osg::Geometry > geometry = new osg::Geometry();
    osg::ref_ptr< osg::Vec3Array > vertices = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array() );
    osg::ref_ptr< osg::Vec4Array > colors = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array() );

    if( m_selectionType == WSelectionType::BOX )
    {
        WPosition pos1 = m_line.at( 0 );
        WPosition pos2 = m_line.at( 1 );

        vertices->push_back( osg::Vec3( pos1.x(), pos1.y(), 0 ) );
        vertices->push_back( osg::Vec3( pos2.x(), pos1.y(), 0 ) );
        vertices->push_back( osg::Vec3( pos2.x(), pos2.y(), 0 ) );
        vertices->push_back( osg::Vec3( pos1.x(), pos2.y(), 0 ) );
        vertices->push_back( osg::Vec3( pos1.x(), pos1.y(), 0 ) );

        colors->push_back( SEL_COLOR );
        colors->push_back( SEL_COLOR );
        colors->push_back( SEL_COLOR );
        colors->push_back( SEL_COLOR );
        colors->push_back( SEL_COLOR );
        geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, 0, 5 ) );
    }
    else
    {
        for( size_t i = 0; i < m_line.size(); i++ )
        {
            WPosition pos = m_line.at( i );
            vertices->push_back( osg::Vec3( pos.x(), pos.y(), pos.z() ) );
            colors->push_back( SEL_COLOR );
        }
        geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, 0, m_line.size() ) );
    }

    geometry->setVertexArray( vertices );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

    lines->addDrawable( geometry );

    float width = m_selectionType == WSelectionType::BRUSH ? 25.0 : m_selectionType == WSelectionType::LINELOOP ? 10.0 : 5.0;
    lines->getOrCreateStateSet()->setAttributeAndModes( new osg::LineWidth( width ), osg::StateAttribute::ON );

    osg::ref_ptr< osg::MatrixTransform > matrix = new osg::MatrixTransform();
    matrix->setMatrix( osg::Matrix::identity() );
    matrix->setReferenceFrame( osg::Transform::ABSOLUTE_RF );

    matrix->addChild( lines );
    m_overlay->addChild( matrix );

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_overlay );
}
