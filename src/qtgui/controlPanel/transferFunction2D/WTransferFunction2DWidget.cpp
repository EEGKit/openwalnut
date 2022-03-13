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

#include "core/common/WTransferFunction2D.h"
#include "qtgui/controlPanel/transferFunction2D/WTransferFunction2DBackground.h"
#include "qtgui/controlPanel/transferFunction2D/WTransferFunction2DScene.h"
#include "core/common/WHistogram2D.h"
#include "core/common/WLogger.h"
#include "WTransferFunction2DWidget.h"


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

    hist = NULL;

    // insert background and histogram items
    scene->addItem( background = new WTransferFunction2DBackground( this ) );

    WTransferFunction2DQuadTool* square = new WTransferFunction2DQuadTool( this );
    scene->addItem( square );
    m_widgets.push_back( square );

    WTransferFunction2DQuadTool* square2 = new WTransferFunction2DQuadTool( this );
    scene->addItem( square2 );
    m_widgets.push_back( square2 );
    initialized = true;
    // initialize the color map (aka. background)
    setMyBackground(); // trigger first paint of transfer function
}

WTransferFunction2DWidget::~WTransferFunction2DWidget()
{
}

void WTransferFunction2DWidget::setMyBackground()
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

void WTransferFunction2DWidget::updateTexture()
{
    wlog::info( "Texture update " );
    updateTransferFunction();
}

void WTransferFunction2DWidget::drawBackground( QPainter *painter, const QRectF &rect )
{
    BaseClass::drawBackground( painter, rect );

    // paint the border
    // painter->setBrush(  Qt::NoBrush );
    // painter->drawRect( rect );
}

void WTransferFunction2DWidget::setHistogram( WHistogram2D newHistogram )
{
    hist = &newHistogram;
    this->updateTransferFunction();
    forceRedraw();
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
    this->setMyBackground();
}

void WTransferFunction2DWidget::updateTransferFunction()
{
    WTransferFunction2D tf;
    {
        // this part does not trigger qt rendering updates
        // we need to set this or the hist wont show
        if( hist != NULL )
        {
            //tf.setHistogram( *hist ); // get the data back because we need this for comparison
        }
        std::vector< WTransferFunction2DQuadTool* >::iterator it;
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
