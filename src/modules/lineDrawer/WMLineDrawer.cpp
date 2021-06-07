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
#include <vector>

#include "WMLineDrawer.h"

W_LOADABLE_MODULE( WMLineDrawer )

WMLineDrawer::WMLineDrawer()
{
    m_lines = boost::shared_ptr< std::vector< WLine > >( new std::vector< WLine >() );
    startNewLine();
}

WMLineDrawer::~WMLineDrawer()
{
}

const std::string WMLineDrawer::getName() const
{
    return "Line Drawer";
}

const std::string WMLineDrawer::getDescription() const
{
    return "Allows user to draw lines.";
}

boost::shared_ptr< WModule > WMLineDrawer::factory() const
{
    return boost::shared_ptr< WModule >( new WMLineDrawer() );
}

const char** WMLineDrawer::getXPMIcon() const
{
    return NULL;
}

void WMLineDrawer::moduleMain()
{
    osg::ref_ptr< osgViewer::View > viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getView();
    osg::ref_ptr< WDrawHandler > drawHandler = new WDrawHandler( this );
    viewer->addEventHandler( drawHandler.get() );

    ready();

    updateLines();

    debugLog() << "READY...";

    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();
        if( m_shutdownFlag() )
        {
            break;
        }
    }

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_overlay );
}

void WMLineDrawer::conenctors()
{
    WModule::connectors();
}

void WMLineDrawer::properties()
{
    WModule::properties();
}

void WMLineDrawer::updateLines()
{
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_overlay );

    m_overlay = new WOverlay();

    osg::ref_ptr< osg::Geode > lines = new osg::Geode();
    osg::ref_ptr< osg::Geometry > geometry = new osg::Geometry();
    osg::ref_ptr< osg::Vec3Array > vertices = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array() );
    osg::ref_ptr< osg::Vec4Array > colors = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array() );

    size_t start = 0;
    for( size_t i = 0; i < m_lines->size(); i++ )
    {
        WLine line = m_lines->at( i );

        for( size_t j = 0; j < line.size(); j++ )
        {
            WPosition pos = line.at( j );
            vertices->push_back( osg::Vec3( pos.x(), pos.y(), pos.z() ) );
            colors->push_back( osg::Vec4( 0.39, 0.58, 0.93, 1 ) );
        }
        geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, start, line.size() ) );
        start += line.size();
    }

    geometry->setVertexArray( vertices );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

    lines->addDrawable( geometry );
    lines->getOrCreateStateSet()->setAttributeAndModes( new osg::LineWidth( 10.0 ), osg::StateAttribute::ON );

    osg::ref_ptr< osg::MatrixTransform > matrix = new osg::MatrixTransform();
    matrix->setMatrix( osg::Matrix::identity() );
    matrix->setReferenceFrame( osg::Transform::ABSOLUTE_RF );

    matrix->addChild( lines );
    m_overlay->addChild( matrix );

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_overlay );
}

void WMLineDrawer::startNewLine()
{
    m_lines->push_back( WLine() );
}

void WMLineDrawer::addPoint( float x, float y )
{
    WLine& line = m_lines->back();
    line.push_back( WPosition( x, y, 0 ) );
}
