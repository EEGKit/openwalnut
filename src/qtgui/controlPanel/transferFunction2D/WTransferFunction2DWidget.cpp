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

#include "core/common/WTransferFunction.h"
#include "qtgui/controlPanel/transferFunction2D/WTransferFunction2DBackground.h"
#include "qtgui/controlPanel/transferFunction2D/WTransferFunction2DScene.h"
#include "WTransferFunction2DWidget.h"
#include "core/common/WHistogram2D.h"
#include "core/common/WLogger.h"

WTransferFunction2DWidget::WTransferFunction2DWidget( QWidget* qparent, WTransferFunction2DGuiNotificationClass* parent ):
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
    //this->setTransformationAnchor( AnchorUnderMouse );
    //this->setResizeAnchor( AnchorViewCenter );

    // insert background and histogram items
    scene->addItem( background = new WTransferFunction2DBackground( this ) );

    initialized = true;
    // initialize the color map (aka. background)
    setMyBackground(); // trigger first paint of transfer function
}

WTransferFunction2DWidget::~WTransferFunction2DWidget()
{
    // // loop for debuging only
    // WTransferFunctionPoint *current( first );
    // while ( current )
    // {
    //     current = current->getRight();
    // }
    // hopefully, the QScene will delete all its items.
}

void WTransferFunction2DWidget::setMyBackground()
{
    if( background )
    {
        WHistogram2D* hist = new WHistogram2D( 0.0, 1.0, 0.0, 1.0, 10, 10 );
        hist->insert( 0.5, 0.4 );
        hist->insert( 0.1, 0.4 );
        hist->insert( 0.1, 0.7 );
        hist->insert( 0.2, 0.5 );
        hist->insert( 0.2, 0.5 );
        hist->insert( 0.2, 0.5 );
        hist->insert( 0.2, 0.5 );
        hist->insert( 0.2, 0.5 );
        hist->insert( 0.2, 0.5 );
        hist->insert( 0.2, 0.5 );
        hist->insert( 0.2, 0.5 );
        hist->insert( 0.2, 0.5 );
        hist->insert( 0.2, 0.5 );
        hist->insert( 0.2, 0.5 );
        hist->insert( 0.2, 0.5 );
        hist->insert( 0.2, 0.5 );
        hist->insert( 0.4, 0.1 );
        hist->insert( 0.4, 0.1 );
        hist->insert( 0.4, 0.4 );
        hist->insert( 0.4, 0.4 );
        hist->insert( 0.4, 0.4 );
        hist->insert( 0.4, 0.4 );
        hist->insert( 0.4, 0.4 );
        hist->insert( 0.4, 0.7 );
        hist->insert( 0.7, 0.1 );
        hist->insert( 0.7, 0.4 );
        hist->insert( 0.7, 0.4 );
        hist->insert( 0.7, 0.4 );
        hist->insert( 0.7, 0.4 );
        hist->insert( 0.7, 0.4 );
        hist->insert( 0.7, 0.4 );
        hist->insert( 0.7, 0.4 );
        hist->insert( 0.7, 0.4 );
        hist->insert( 0.7, 0.4 );
        hist->insert( 0.7, 0.4 );
        hist->insert( 0.7, 0.7 );
        hist->insert( 0.7, 0.7 );

        unsigned char * data = hist->getRawTexture();

        if( data == NULL )
        {
            wlog::debug( "2DTRANS" ) << "Data is null";
            return;
        }

        int imageWidth = hist->getBucketsX();
        int imageHeight = hist->getBucketsY();

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

void WTransferFunction2DWidget::drawBackground( QPainter *painter, const QRectF &rect )
{
    BaseClass::drawBackground( painter, rect );

    // paint the border
    // painter->setBrush(  Qt::NoBrush );
    // painter->drawRect( rect );
}

void WTransferFunction2DWidget::forceRedraw()
{
    if( !initialized )
    {
        return;
    }
    QRectF viewport( scene->sceneRect() );
    scene->invalidate( viewport );
    this->update();
}


