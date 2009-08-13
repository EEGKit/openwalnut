//---------------------------------------------------------------------------
//
// Project: OpenWalnut
//
// Copyright 2009 SomeCopyrightowner
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

#include <iostream>

#include <QtGui/QKeyEvent>

#include "WQtGLWidget.h"

WQtGLWidget::WQtGLWidget( QWidget* parent )
    : QWidget( parent ),
      m_recommendedSize()
{
    m_recommendedSize.setWidth( 200 );
    m_recommendedSize.setHeight( 200 );

    // initialize OpenGL context and OpenSceneGraph

#if defined(__APPLE__)
    // Extract a WindowPtr from the HIViewRef that QWidget::winId() returns.
    // Without this change, the peer tries to call GetWindowPort on the HIViewRef
    // which returns 0 and we only render white.
    wdata = boost::shared_ptr<WindowData>(
            new WindowData( HIViewGetWindow( static_cast<HIViewRef>winId() ) )
    );
#else  // all others
    wdata = boost::shared_ptr<WindowData>( new WindowData( winId() ) );
#endif

    // create viewer
    m_Viewer = boost::shared_ptr<WGEViewer>( new WGEViewer(  wdata, x(), y(), width(), height() ) );

    // timer
    m_Timer.start( 10 );
    connect( &m_Timer, SIGNAL( timeout() ), this, SLOT( repaint() ) );

    // required
    setAttribute( Qt::WA_PaintOnScreen );
    setAttribute( Qt::WA_NoSystemBackground );
    setFocusPolicy( Qt::ClickFocus );
}

WQtGLWidget::~WQtGLWidget()
{
    m_Timer.stop();
}

QSize WQtGLWidget::sizeHint() const
{
    return m_recommendedSize;
}

void WQtGLWidget::paintEvent( QPaintEvent* event )
{
    m_Viewer->paint();
}

#ifndef WIN32
void WQtGLWidget::destroyEvent( bool destroyWindow, bool destroySubWindows )
{
    // forward events
    m_Viewer->close();
}


void WQtGLWidget::closeEvent( QCloseEvent* event )
{
    event->accept();

    // forward events
    m_Viewer->close();
}


void WQtGLWidget::resizeEvent( QResizeEvent* event )
{
    m_Viewer->resize( event->size().width(), event->size().height() );
}

int WQtGLWidget::translateButton( QMouseEvent* event )
{
    switch( event->button() )
    {
        case( Qt::LeftButton ):
            return 1;
        case( Qt::MidButton ):
            return 2;
        case( Qt::RightButton ):
            return 3;
        case( Qt::NoButton ):
            return 0;
        default:
            return 0;
    }
}

void WQtGLWidget::keyPressEvent( QKeyEvent* event )
{
    m_Viewer->keyEvent( WGEViewer::KEYPRESS, *event->text().toAscii().data() );
}

void WQtGLWidget::keyReleaseEvent( QKeyEvent* event )
{
    m_Viewer->keyEvent( WGEViewer::KEYRELEASE, *event->text().toAscii().data() );
}


void WQtGLWidget::mousePressEvent( QMouseEvent* event )
{
    m_Viewer->mouseEvent( WGEViewer::MOUSEPRESS, event->x(), event->y(), translateButton( event ) );
}

void WQtGLWidget::mouseDoubleClickEvent( QMouseEvent* event )
{
    m_Viewer->mouseEvent( WGEViewer::MOUSEDOUBLECLICK, event->x(), event->y(), translateButton( event ) );
}

void WQtGLWidget::mouseReleaseEvent( QMouseEvent* event )
{
    m_Viewer->mouseEvent( WGEViewer::MOUSERELEASE, event->x(), event->y(), translateButton( event ) );
}

void WQtGLWidget::mouseMoveEvent( QMouseEvent* event )
{
    m_Viewer->mouseEvent( WGEViewer::MOUSEMOVE, event->x(), event->y(), 0 );
}
#endif

