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
#include <iostream>

#include <osg/ShapeDrawable>
#include <osg/Geode>

#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/UFOManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/View>

#include <osgDB/ReadFile>

#include "exceptions/WGEInitFailed.h"
#include "WGE2DManipulator.h"
#include "WGEZoomTrackballManipulator.h"
#include "WPickHandler.h"
#include "../kernel/WKernel.h"

#include "WGEViewer.h"

WGEViewer::WGEViewer( std::string name, osg::ref_ptr<WindowData> wdata, int x, int y,
    int width, int height, WGECamera::ProjectionMode projectionMode )
    : WGEGraphicsWindow( wdata, x, y, width, height ),
      boost::enable_shared_from_this< WGEViewer >(),
      m_name( name )
{
    try
    {
        m_View = osg::ref_ptr<osgViewer::View>( new osgViewer::View );

        m_View->setCamera( new WGECamera( width, height, projectionMode ) );
        m_View->getCamera()->setGraphicsContext( m_GraphicsContext );

        // camera manipulator
        switch( projectionMode )
        {
            case( WGECamera::ORTHOGRAPHIC ):
            case( WGECamera::PERSPECTIVE ):
                m_View->setCameraManipulator( new WGEZoomTrackballManipulator() );
                break;
            case( WGECamera::TWO_D ):
                m_View->setCameraManipulator( new WGE2DManipulator() );
                break;
            default:
                throw WGEInitFailed( "Unknown projection mode" );
        }

        // add the stats handler
        m_View->addEventHandler( new osgViewer::StatsHandler );

        m_View->setLightingMode( osg::View::HEADLIGHT ); // this is the default anyway

        m_pickHandler = new WPickHandler();
        m_View->addEventHandler( m_pickHandler );
    }
    catch( ... )
    {
        throw WGEInitFailed( "Initialization of WGEViewer failed" );
    }
}

WGEViewer::~WGEViewer()
{
    // cleanup
    close();
}

osg::ref_ptr<osgViewer::View> WGEViewer::getView()
{
    return m_View;
}

void WGEViewer::setCameraManipulator( osg::ref_ptr<osgGA::MatrixManipulator> manipulator )
{
    m_View->setCameraManipulator( manipulator );
    // redraw request?? no since it redraws permanently and uses the new settings
}

osg::ref_ptr<osgGA::MatrixManipulator> WGEViewer::getCameraManipulator()
{
    return m_View->getCameraManipulator();
}

void WGEViewer::setCamera( osg::ref_ptr<osg::Camera> camera )
{
    m_View->setCamera( camera );
    // redraw request?? No since it redraws permanently and uses the new settings
}

osg::ref_ptr<osg::Camera> WGEViewer::getCamera()
{
    return m_View->getCamera();
}

void WGEViewer::setScene( osg::ref_ptr<osg::Node> node )
{
    m_View->setSceneData( node );
}

osg::ref_ptr<osg::Node> WGEViewer::getNode()
{
    return m_View->getSceneData();
}

void WGEViewer::setBgColor( WColor bgColor )
{
    m_View->getCamera()->setClearColor( osg::Vec4( bgColor.getRed(), bgColor.getGreen(), bgColor.getBlue(), 1. ) );
}

void WGEViewer::resize( int width, int height )
{
    WGEGraphicsWindow::resize( width, height );

    // also update the camera
    m_View->getCamera()->setViewport( 0, 0, width, height );
    WGECamera* camera = dynamic_cast< WGECamera* >( m_View->getCamera() );
    if( camera )
    {
        camera->resize();
    }
}

void WGEViewer::close()
{
    // forward close event
    WGEGraphicsWindow::close();
}

std::string WGEViewer::getName() const
{
    return m_name;
}

osg::ref_ptr< WPickHandler > WGEViewer::getPickHandler()
{
    return m_pickHandler;
}

void WGEViewer::reset()
{
    m_View->home();
}

