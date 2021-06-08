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

WSelectionManager::WSelectionManager():
    m_overlay( new WOverlay() ),
    m_selectionType( WSelectionType::BRUSH ),
    m_hasStarted( false ),
    m_shader( new WGEShader( "../modules/lineDrawer/shaders/WSelectionManager" ) ) // TODO(eschbach): Remove path when resourcing it to the core
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

    if( m_selectionType == WSelectionType::BOX )
    {
        WPosition pos1 = m_line.at( 0 );
        WPosition pos2 = m_line.at( 1 );

        vertices->push_back( osg::Vec3( pos1.x(), pos2.y(), 0 ) );

        vertices->push_back( osg::Vec3( pos1.x(), pos1.y(), 0 ) );
        vertices->push_back( osg::Vec3( pos2.x(), pos1.y(), 0 ) );
        vertices->push_back( osg::Vec3( pos2.x(), pos2.y(), 0 ) );
        vertices->push_back( osg::Vec3( pos1.x(), pos2.y(), 0 ) );
        vertices->push_back( osg::Vec3( pos1.x(), pos1.y(), 0 ) );

        vertices->push_back( osg::Vec3( pos2.x(), pos1.y(), 0 ) );

        geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP_ADJACENCY, 0, 7 ) );
    }
    else
    {
        size_t add = 2;
        WPosition first = m_line.front();
        WPosition last = m_line.back();

        // first element
        if( m_selectionType == WSelectionType::LINELOOP )
        {
            vertices->push_back( osg::Vec3( last.x(), last.y(), last.z() ) );
        }
        else
        {
            vertices->push_back( osg::Vec3( first.x(), first.y(), first.z() ) );
        }

        // all elements
        for( size_t i = 0; i < m_line.size(); i++ )
        {
            WPosition pos = m_line.at( i );
            vertices->push_back( osg::Vec3( pos.x(), pos.y(), pos.z() ) );
        }

        // last element and close line loop
        if( m_selectionType == WSelectionType::LINELOOP && m_line.size() > 1 )
        {
            vertices->push_back( osg::Vec3( first.x(), first.y(), first.z() ) );
            vertices->push_back( osg::Vec3( m_line.at( 1 ).x(), m_line.at( 1 ).y(), m_line.at( 1 ).z() ) );
            add++;
        }
        else
        {
            vertices->push_back( osg::Vec3( last.x(), last.y(), last.z() ) );
        }

        geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP_ADJACENCY, 0, m_line.size() + add ) );
    }

    geometry->setVertexArray( vertices );

    lines->addDrawable( geometry );

    float thickness = m_selectionType == WSelectionType::BRUSH ? 25 : m_selectionType == WSelectionType::LINELOOP ? 5 : 10;

    osg::StateSet* state = lines->getOrCreateStateSet();
    osg::Camera* camera = WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getCamera();
    state->addUniform( new osg::Uniform( "u_viewport", osg::Vec2( camera->getViewport()->width(), camera->getViewport()->height() ) ) );
    state->addUniform( new osg::Uniform( "u_thickness", thickness ) );

    osg::ref_ptr< osg::MatrixTransform > matrix = new osg::MatrixTransform();
    matrix->setMatrix( osg::Matrix::identity() );
    matrix->setReferenceFrame( osg::Transform::ABSOLUTE_RF );

    m_shader->apply( lines );
    matrix->addChild( lines );
    m_overlay->addChild( matrix );

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_overlay );
}
