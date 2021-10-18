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

#include <vector>

#include "WOnscreenSelection.h"

WOnscreenSelection::WOnscreenSelection():
    m_projection( new osg::Projection() ),
    m_selectionType( WSelectionType::BRUSH ),
    m_selectionHandler( new WOnscreenSelectionHandler( this ) ),
    m_isSelecting( false ),
    m_shader( new WGEShader( "WOnscreenSelection" ) ),
    m_thickness( 25.0f ),
    m_clickType( false )
{
    WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getView()->addEventHandler( m_selectionHandler.get() );
}

WOnscreenSelection::~WOnscreenSelection()
{
    WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getView()->removeEventHandler( m_selectionHandler.get() );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_projection );
}

void WOnscreenSelection::start( float x, float y )
{
    if( m_isSelecting )
    {
        return;
    }

    clear();

    m_isSelecting = true;
    m_moved = false;
    WPosition pos( x, y, 0 );

    m_line.push_back( pos );
    if( m_selectionType == WSelectionType::BOX )
    {
        m_line.push_back( pos );
    }
    updateDisplay();

    if( m_onstart )
    {
        m_onstart( m_selectionType, x, y );
    }
}

void WOnscreenSelection::end( float x, float y )
{
    if( !m_isSelecting )
    {
        return;
    }
    m_isSelecting = false;

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

    if( m_onend )
    {
        m_onend( m_selectionType, x, y );
    }
}

void WOnscreenSelection::move( float x, float y )
{
    if( !m_isSelecting )
    {
        return;
    }

    m_moved = true;

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

    if( m_onmove )
    {
        m_onmove( m_selectionType, x, y );
    }
}

enum WOnscreenSelection::WSelectionType WOnscreenSelection::getSelectionType()
{
    return m_selectionType;
}

void WOnscreenSelection::setSelectionType( enum WOnscreenSelection::WSelectionType selectionType )
{
    m_selectionType = selectionType;
}

void WOnscreenSelection::updateDisplay()
{
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_projection );

    if( m_line.empty() )
    {
        return;
    }

    m_projection = new osg::Projection();
    m_projection->setMatrix( osg::Matrix::ortho2D( 0, 1.0, 0, 1.0 ) );
    m_projection->getOrCreateStateSet()->setRenderBinDetails( 15, "RenderBin" );
    m_projection->getOrCreateStateSet()->setDataVariance( osg::Object::DYNAMIC );
    m_projection->getOrCreateStateSet()->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );
    m_projection->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    m_projection->getOrCreateStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    m_projection->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE );

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

    float thickness = m_selectionType == WSelectionType::BRUSH ? m_thickness : 5;

    osg::StateSet* state = lines->getOrCreateStateSet();
    osg::Camera* camera = WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getCamera();
    state->addUniform( new osg::Uniform( "u_viewport", osg::Vec2( camera->getViewport()->width(), camera->getViewport()->height() ) ) );
    state->addUniform( new osg::Uniform( "u_thickness", thickness ) );
    state->addUniform( new osg::Uniform( "u_clickType", m_clickType ) );

    osg::ref_ptr< osg::MatrixTransform > matrix = new osg::MatrixTransform();
    matrix->setMatrix( osg::Matrix::identity() );
    matrix->setReferenceFrame( osg::Transform::ABSOLUTE_RF );

    m_shader->apply( lines );
    matrix->addChild( lines );
    m_projection->addChild( matrix );

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_projection );
}

void WOnscreenSelection::setOnstart( CallbackType onstart )
{
    m_onstart = onstart;
}

void WOnscreenSelection::setOnend( CallbackType onend )
{
    m_onend = onend;
}

void WOnscreenSelection::setOnmove( CallbackType onmove )
{
    m_onmove = onmove;
}

bool WOnscreenSelection::isSelecting()
{
    return m_isSelecting;
}

void WOnscreenSelection::clear()
{
    m_line.clear();
    updateDisplay();
}

bool WOnscreenSelection::isSelected( float x, float y, float z )
{
    if( m_line.empty() )
    {
        return false;
    }

    // Calculate normalized screen coordinates for 3D point.
    osg::Camera* camera = WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getCamera();
    osg::Matrix viewMatrix = camera->getViewMatrix();
    osg::Matrix projectionMatrix = camera->getProjectionMatrix();
    osg::Matrix cameraMatrix = viewMatrix * projectionMatrix;
    osg::Vec3 point = osg::Vec3( x, y, z ) * cameraMatrix;
    float xPos = ( point.x()  + 1.0 ) / 2.0;
    float yPos = ( point.y()  + 1.0 ) / 2.0;

    if( xPos < 0 || xPos > 1 || yPos < 0 || yPos > 1 )
    {
        return false;
    }

    switch( m_selectionType )
    {
        case WSelectionType::BRUSH:
            return brushCheck( xPos, yPos );
        case WSelectionType::LINELOOP:
            return lineloopCheck( xPos, yPos );
        case WSelectionType::BOX:
            return boxCheck( xPos, yPos );
        default:
            return false;
    }
}

std::vector< WPosition > WOnscreenSelection::isSelected( std::vector< WPosition> positions )
{
    std::vector< WPosition > out;
    for( size_t i = 0; i < positions.size(); i++ )
    {
        WPosition pos = positions.at( i );
        if( isSelected( pos.x(), pos.y(), pos.z() ) )
        {
            out.push_back( pos );
        }
    }

    return out;
}


bool WOnscreenSelection::boxCheck( float x, float y )
{
    WPosition pos1 = m_line.at( 0 );
    WPosition pos2 = m_line.at( 1 );

    float maxX = fmax( pos1.x(), pos2.x() );
    float maxY = fmax( pos1.y(), pos2.y() );
    float minX = fmin( pos1.x(), pos2.x() );
    float minY = fmin( pos1.y(), pos2.y() );

    return x >= minX && x <= maxX && y >= minY && y <= maxY;
}

bool WOnscreenSelection::brushCheck( float x, float y )
{
    osg::Camera* camera = WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getCamera();
    float thick2 = ( m_thickness * 0.5 ) * ( m_thickness * 0.5 ); // * 0.5 because we need distance to center of the brush
    float width = camera->getViewport()->width();
    float height = camera->getViewport()->height();

    x = x * width;
    y = y * height;

    for( size_t i = 0; i < m_line.size() - 1; i++ )
    {
        WPosition pos1 = m_line.at( i );
        WPosition pos2 = m_line.at( i + 1 );

        // denormalize
        pos1 = WPosition( pos1.x() * width, pos1.y() * height, 0.0 );
        pos2 = WPosition( pos2.x() * width, pos2.y() * height, 0.0 );

        // calculate distance to linesegment
        float segLen2 = pow( pos1.x() - pos2.x(), 2.0 ) + pow( pos1.y() - pos2.y(), 2.0 );
        if( segLen2 == 0.0 )
        {
            continue;
        }

        float segParam = ( ( x - pos1.x() ) * ( pos2.x() - pos1.x() ) + ( y - pos1.y() ) * ( pos2.y() - pos1.y() ) ) / segLen2;
        segParam = fmax( 0.0, fmin( 1.0, segParam ) );

        float xPos = pos1.x() + segParam * ( pos2.x() - pos1.x() );
        float yPos = pos1.y() + segParam * ( pos2.y() - pos1.y() );

        float dis2 = pow( x - xPos, 2.0 ) + pow( y - yPos, 2.0 );

        if( dis2 <= thick2 )
        {
            return true;
        }
    }
    return false;
}

bool WOnscreenSelection::lineloopCheck( float x, float y )
{
    int t = -1;
    for( size_t i = 0; i < m_line.size(); i++ )
    {
        WPosition pos1 = m_line.at( i );
        WPosition pos2 = i == m_line.size() - 1 ? m_line.at( 0 ) : m_line.at( i + 1 );

        t = t * crossingNumberProduct( x, y, pos1, pos2 );
        if( t == 0 )
        {
            break;
        }
    }
    return t >= 0;
}

int WOnscreenSelection::crossingNumberProduct( float x, float y, WPosition b, WPosition c )
{
    if( y == b.y() && y == c.y() )
    {
        if( ( b.x() <= x && x <= c.y() ) || ( c.x() <= x && x <= b.x() ) )
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    if( y == b.y() && x == b.x() )
    {
        return 0;
    }

    if( b.y() > c.y() )
    {
        WPosition help = b;
        b = c;
        c = help;
    }
    if( y <= b.y() || y > c.y() )
    {
        return 1;
    }

    float delta = ( b.x() - x ) * ( c.y() - y ) - ( b.y() - y ) * ( c.x() - x );
    return delta > 0 ? -1 : delta < 0 ? 1 : 0;
}

void WOnscreenSelection::setClickType( bool clickType )
{
    m_clickType = clickType;
}

bool WOnscreenSelection::getClickType()
{
    return m_clickType;
}

bool WOnscreenSelection::hasMoved()
{
    return m_moved;
}
