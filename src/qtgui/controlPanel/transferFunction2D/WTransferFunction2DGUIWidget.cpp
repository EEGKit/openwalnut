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

#include <QGraphicsSceneMouseEvent>
#include <QMouseEvent>
#include <QPaintEngine>
#include <QMenu>

#include "core/common/WTransferFunction2D.h"
#include "qtgui/controlPanel/transferFunction2D/WTransferFunction2DBackground.h"
#include "qtgui/controlPanel/transferFunction2D/WTransferFunction2DScene.h"
#include "core/common/WHistogram2D.h"
#include "core/common/WLogger.h"
#include "WTransferFunction2DGUIWidget.h"


WTransferFunction2DGUIWidget::WTransferFunction2DGUIWidget( QWidget* qparent, WTransferFunction2DGuiNotificationClass* parent ):
        BaseClass( qparent ),
        parent( parent ),
        background( 0x0 ),
        initialized( false )
{
    // std::cout << "new widget" << std::endl;
    const int xMin( 0 );
    const int yMin( 0 );
    const int xMax( 300 );
    const int yMax( 300 );

    // set up the scene and the parameters that define how we paint things
    setMinimumSize( xMax-xMin+20, yMax - yMin + 30 );
    this->setViewportUpdateMode( QGraphicsView::FullViewportUpdate );

    scene = new WTransferFunction2DScene( this );
    scene->setItemIndexMethod( QGraphicsScene::NoIndex );
    scene->setSceneRect( xMin, yMin, xMax, yMax );
    this->setScene( scene );

    this->setCacheMode( CacheNone );
    this->setRenderHint( QPainter::Antialiasing );

    this->setContextMenuPolicy( Qt::CustomContextMenu );

    connect( this, SIGNAL( customContextMenuRequested( const QPoint & ) ),
            this, SLOT( showContextMenu( const QPoint & ) ) );

    hist = NULL;

    // insert background and histogram items
    scene->addItem( background = new WTransferFunction2DBackground( this ) );

    addBoxWidget();
    initialized = true;
    // initialize the color map (aka. background)
    setMyBackground(); // trigger first paint of transfer function
}

WTransferFunction2DGUIWidget::~WTransferFunction2DGUIWidget()
{
}

void WTransferFunction2DGUIWidget::setMyBackground()
{
    if( background && hist != NULL )
    {
        wlog::debug( "WTransferFunction2DWidget" ) << "::setMyBackground()";
        unsigned char * data = hist->getRawTexture();

        size_t imageWidth = hist->getBucketsX();
        size_t imageHeight = hist->getBucketsY();

        QImage* image = new QImage( data, imageWidth, imageHeight, QImage::Format_RGBA8888 );
        QPixmap pixmap;

#if( QT_VERSION >= 0x040700 )
        pixmap.convertFromImage( *image );
    #else
        // older versions have convertFromImage in Qt3Support
        // to avoid linking to that one, we use the slower version
        // here, which creates a copy, first.
        pixmap = QPixmap::fromImage( image );
    #endif
        background->setMyPixmap( pixmap );
    }
}

void WTransferFunction2DGUIWidget::updateTexture()
{
    wlog::info( "Texture update " );
    updateTransferFunction();
}

void WTransferFunction2DGUIWidget::drawBackground( QPainter *painter, const QRectF &rect )
{
    BaseClass::drawBackground( painter, rect );
}

void WTransferFunction2DGUIWidget::setHistogram( WHistogram2D newHistogram )
{
    hist = &newHistogram;
    this->updateTransferFunction();
    forceRedraw();
}

void WTransferFunction2DGUIWidget::forceRedraw()
{
    if( !initialized )
    {
        return;
    }
    QRectF viewport( scene->sceneRect() );
    scene->invalidate( viewport );
    this->update();
    this->setMyBackground();
}

void WTransferFunction2DGUIWidget::updateTransferFunction()
{
    WTransferFunction2D tf;
    {
        // this part does not trigger qt rendering updates
        // we need to set this or the hist wont show
        if( hist != NULL )
        {
            //tf.setHistogram( *hist ); // get the data back because we need this for comparison
        }
        std::vector< WTransferFunction2DBoxWidget* >::iterator it;
        //this is set to 300 at the moment and is equal to the size of the scene/histogram
        size_t imageWidth = 300;
        size_t imageHeight = 300;

        unsigned char* data = new unsigned char[ imageWidth * imageHeight * 4 ]();

        // Iterate over every widget inside our window and call create the 2D Texture
        for( it = m_widgets.begin(); it != m_widgets.end(); it++ )
        {
            ( *it )->sampleWidgetToImage( data, imageWidth, imageHeight );
        }
        tf.setTexture( data, 300, 300 );
    }
    if( parent )
    {
        parent->guiUpdate( tf );
    }
}

void WTransferFunction2DGUIWidget::addBoxWidget()
{
    WTransferFunction2DBoxWidget* box = new WTransferFunction2DBoxWidget( this );
    scene->addItem( box );
    m_widgets.push_back( box );
}

void WTransferFunction2DGUIWidget::cleanTransferFunction()
{
    std::vector< WTransferFunction2DBoxWidget* >::iterator it;
    for( it = m_widgets.begin(); it != m_widgets.end(); it++ )
    {
        delete( *it );
    }
    m_widgets.clear();
    updateTransferFunction();
}

void WTransferFunction2DGUIWidget::showContextMenu( const QPoint &pos )
{
    QMenu contextMenu( tr( "Context menu" ), this );

    QAction action1( "Add box widget", this );
    QAction action2( "Clean transfer function", this );
    connect( &action1, SIGNAL( triggered() ), this, SLOT( addBoxWidget() ) );
    connect( &action2, SIGNAL( triggered() ), this, SLOT( cleanTransferFunction() ) );

    contextMenu.addAction( &action1 );
    contextMenu.addSeparator();
    contextMenu.addAction( &action2 );

    contextMenu.exec( mapToGlobal( pos ) );
}

void WTransferFunction2DGUIWidget::removeWidget( WTransferFunction2DBoxWidget *widget )
{
    auto it = std::find( m_widgets.begin(), m_widgets.end(), widget );
    m_widgets.erase( it );
    scene->removeItem( widget );
    delete( widget );
    updateTexture();
}
