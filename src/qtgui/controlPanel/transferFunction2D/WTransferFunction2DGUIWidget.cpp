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
        background( nullptr ),
        hist( nullptr ),
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

    // insert background and histogram items
    scene->addItem( background = new WTransferFunction2DBackground( this ) );
    initialized = true;
    // initialize the histogram (aka. background)
    setMyBackground(); // trigger first paint of transfer function
}

WTransferFunction2DGUIWidget::~WTransferFunction2DGUIWidget()
{
}

void WTransferFunction2DGUIWidget::setMyBackground()
{
    if( background && hist != nullptr )
    {
        unsigned char * data = hist->getRawTexture();

        size_t imageWidth = hist->getBucketsX();
        size_t imageHeight = hist->getBucketsY();

        QImage image( data, imageWidth, imageHeight, QImage::Format::Format_RGBA8888 );
        QPixmap pixmap;

#if( QT_VERSION >= 0x040700 )
        pixmap.convertFromImage( image );
    #else
        // older versions have convertFromImage in Qt3Support
        // to avoid linking to that one, we use the slower version
        // here, which creates a copy, first.
        pixmap = QPixmap::fromImage( image );
    #endif
        background->setMyPixmap( pixmap );
    }
}


void WTransferFunction2DGUIWidget::drawBackground( QPainter *painter, const QRectF &rect )
{
    BaseClass::drawBackground( painter, rect );
}

void WTransferFunction2DGUIWidget::setHistogram( const std::shared_ptr< WHistogram2D >& newHistogram )
{
    hist = newHistogram;
    dataChanged();
}

void WTransferFunction2DGUIWidget::dataChanged()
{
    if( !initialized )
    {
        return;
    }
    this->updateTransferFunction();
    this->setMyBackground();
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
}

namespace
{
    WColor toWColor( const QColor& q )
    {
        return WColor( q.redF(), q.greenF(), q.blueF(), q.alphaF() );
    }
}

void WTransferFunction2DGUIWidget::updateTransferFunction()
{
    WTransferFunction2D tf;
    {
        if( hist != nullptr )
        {
            tf.setHistogram( hist ); // get the data back because we need this for comparison
        }

        // Serialite the current TF into the 2D TF object in normalized space
        for( auto const &w : m_widgets )
        {
            double isoX = ( w->mapToScene( w->boundingRect().topLeft() ).x() / 300 );
            double isoY = ( w->mapToScene( w->boundingRect().topLeft() ).y() / 300 );
            double width = ( w->getWidth() / 300 );
            double height = ( w->getHeight() / 300 );
            QColor col( w->getColor().red(), w->getColor().green(), w->getColor().blue(), w->getColor().alpha() );
            tf.addBoxWidget( isoX, isoY, width, height, toWColor( col ) );
        }
    }
    if( parent )
    {
        parent->guiUpdate( tf );
    }
}

void WTransferFunction2DGUIWidget::insertBoxWidgetNormalized( const QPointF &pos = QPointF( 0., 0. ), const double width = .167,
                                                              const double height = .167,
                                                              const QColor *const color = new QColor( 255., 0., 0., 10. ) )
{
    // Scene width and height are 300.
    insertBoxWidget( QPointF( pos.x()*300., pos.y()*300. ), width*300., height*300., color );
}

void WTransferFunction2DGUIWidget::insertBoxWidget( const QPointF &pos, const double width, const double height,
                                                    const QColor *const color )
{
    WTransferFunction2DBoxWidget *box( new WTransferFunction2DBoxWidget( this, width, height, *color ) );
    box->setPos( pos );
    scene->addItem( box );
    m_widgets.push_back( box );
    this->update();
    dataChanged();
}


void WTransferFunction2DGUIWidget::addBoxWidget()
{
    insertBoxWidgetNormalized();
}

void WTransferFunction2DGUIWidget::cleanTransferFunction()
{
    std::vector< WTransferFunction2DBoxWidget* >::iterator it;
    for( it = m_widgets.begin(); it != m_widgets.end(); it++ )
    {
        scene->removeItem( *it );
        delete( *it );
    }
    m_widgets.clear();
    dataChanged();
}

void WTransferFunction2DGUIWidget::showContextMenu( const QPoint &pos )
{
    QMenu contextMenu( tr( "Context menu" ), this );

    QAction action1( "Add box widget", this );
    QAction action2( "Clean transfer function", this );
    connect( &action1, SIGNAL( triggered() ), this,
             SLOT( addBoxWidget() ) );
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
    this->dataChanged();
}
