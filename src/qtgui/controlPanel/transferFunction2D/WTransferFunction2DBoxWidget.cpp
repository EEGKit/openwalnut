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

#include <QMenu>
#include <QKeyEvent>
#include "QGraphicsSceneContextMenuEvent"
#include "QApplication"
#include "QBrush"
#include "QPainter"
#include "QColorDialog"
#include "QGraphicsSceneMouseEvent"
#include "QGraphicsScene"

#include "WTransferFunction2DBoxWidget.h"

#include "core/common/WLogger.h"

WTransferFunction2DBoxWidget::WTransferFunction2DBoxWidget( WTransferFunction2DGUIWidget *parent )
{
    setFlag( ItemIsMovable );
    setFlag( ItemSendsScenePositionChanges );
    setFlag( ItemIsSelectable );
    setFlag( ItemIsFocusable );

    m_width = 50;
    m_height = 50;
    m_box = QRectF( 0, 0, m_width, m_height );
    m_parent = parent;
    m_color = QColor( 255, 0, 0 , 10 );
    m_resizePoints = NONE;

    // We always start with top right and go clock wise
    m_controlPoints.push_back( new WTransferFunction2DControlPoint( this, boundingRect().topRight(), TOPRIGHT ) );
    m_controlPoints.push_back( new WTransferFunction2DControlPoint( this, boundingRect().bottomRight(), BOTTOMRIGHT ) );
    m_controlPoints.push_back( new WTransferFunction2DControlPoint( this, boundingRect().bottomLeft(), BOTTOMLEFT ) );
    m_controlPoints.push_back( new WTransferFunction2DControlPoint( this, boundingRect().topLeft(), TOPLEFT )  );

    connect( m_controlPoints[0], SIGNAL( resizeHandleChanged( ResizePointsRect, QPointF ) ),
             this, SLOT( setResizeHandle( ResizePointsRect, QPointF ) ) );
    connect( m_controlPoints[1], SIGNAL( resizeHandleChanged( ResizePointsRect, QPointF ) ),
             this, SLOT( setResizeHandle( ResizePointsRect, QPointF ) ) );
    connect( m_controlPoints[2], SIGNAL( resizeHandleChanged( ResizePointsRect, QPointF ) ),
             this, SLOT( setResizeHandle( ResizePointsRect, QPointF ) ) );
    connect( m_controlPoints[3], SIGNAL( resizeHandleChanged( ResizePointsRect, QPointF ) ),
             this, SLOT( setResizeHandle( ResizePointsRect, QPointF ) ) );
}
WTransferFunction2DBoxWidget::~WTransferFunction2DBoxWidget()
{
}

QRectF WTransferFunction2DBoxWidget::boundingRect() const
{
    return m_box;
}

void WTransferFunction2DBoxWidget::setControlPointsToCorner()
{
    prepareGeometryChange();
    m_controlPoints[0]->setPos( m_box.topRight() ); // top right
    m_controlPoints[1]->setPos( m_box.bottomRight() ); // bottom right
    m_controlPoints[2]->setPos( m_box.bottomLeft() ); // bottom left
    m_controlPoints[3]->setPos( m_box.topLeft() ); // top left
    // We also need to update the m_width and m_height property of the box
    m_width = m_box.right() - m_box.left();
    m_height = m_box.top() - m_box.bottom();
    update();
}

QVariant WTransferFunction2DBoxWidget::itemChange( GraphicsItemChange change, const QVariant &value )
{
    if( change == ItemPositionChange && scene() )
    {
        // value is the new position.
        QPointF newPos = value.toPointF();
        QRectF rect = scene()->sceneRect();
        // We create a bounding reactangle with the size of the parent bounds.
        // Because this Rect is always moved when moving the widget,
        // we have to translate it back into (0,0) of the parent bounds
        // by using the inverse of the origin of this widget
        rect.translate( -m_box.topLeft() );
        rect.setRight( rect.right() - m_box.width() - 1 );
        rect.setBottom( rect.bottom() - m_box.height() - 1 );

        if( !rect.contains( newPos ) )
        {
            // Keep the item inside the scene
            newPos.setX( qMin( rect.right(), qMax( newPos.x(), rect.left() ) ) );
            newPos.setY( qMin( rect.bottom(), qMax( newPos.y(), rect.top() ) ) );
            return newPos;
        }
    }
    return QGraphicsItem::itemChange( change, value );
}

void WTransferFunction2DBoxWidget::paint( QPainter *painter, const QStyleOptionGraphicsItem*, QWidget* )
{
    QBrush brush( m_color );

    painter->fillRect( m_box, brush );
    painter->drawRect( m_box );
}

void WTransferFunction2DBoxWidget::sampleWidgetToImage( unsigned char * array, size_t imageWidth, size_t imageHeight )
{
    size_t xMin, xMax, yMin, yMax;
    xMin = pos().x();
    yMin = pos().y();
    xMax = pos().x() + m_box.width();
    yMax = pos().y() + m_box.height();

    for( size_t x = xMin; x < xMax; ++x )
    {
        for( size_t y = yMin; y < yMax; ++y )
        {
            array[4 * imageWidth * x + 4 * y + 0] = static_cast< unsigned char >( m_color.red() );
            array[4 * imageWidth * x + 4 * y + 1] = static_cast< unsigned char >( m_color.green() );
            array[4 * imageWidth * x + 4 * y + 2] = static_cast< unsigned char >( m_color.blue() );
            array[4 * imageWidth * x + 4 * y + 3] = static_cast< unsigned char >( m_color.alpha() );
        }
    }
}

void WTransferFunction2DBoxWidget::setResizeHandle( ResizePointsRect handle, QPointF position )
{
    m_resizePoints = handle;
    prepareGeometryChange();
    switch( m_resizePoints )
    {
        case TOPRIGHT:
            m_box.setTopRight( position );
            setControlPointsToCorner();
            break;
        case BOTTOMRIGHT:
            m_box.setBottomRight( position );
            setControlPointsToCorner();
            break;
        case BOTTOMLEFT:
            m_box.setBottomLeft( position );
            setControlPointsToCorner();
            break;
        case TOPLEFT:
            m_box.setTopLeft( position );
            setControlPointsToCorner();
            break;
        default:
            break;
    }
    update();
    m_parent->updateTransferFunction();
}

void WTransferFunction2DBoxWidget::mousePressEvent( QGraphicsSceneMouseEvent *event )
{
    update();
    m_parent->updateTransferFunction();
    QGraphicsItem::mousePressEvent( event );
}

void WTransferFunction2DBoxWidget::mouseMoveEvent( QGraphicsSceneMouseEvent *event )
{
    update();
    m_parent->updateTransferFunction();
    QGraphicsItem::mouseMoveEvent( event );
}

void WTransferFunction2DBoxWidget::mouseReleaseEvent( QGraphicsSceneMouseEvent *event )
{
    update();
    m_parent->updateTransferFunction();
    QGraphicsItem::mouseReleaseEvent( event );
}

void WTransferFunction2DBoxWidget::mouseDoubleClickEvent( QGraphicsSceneMouseEvent *event )
{
    event->accept();
    QGraphicsObject::mouseDoubleClickEvent( event );
    update();
    showColorPicker();
}

void WTransferFunction2DBoxWidget::colorSelected( const QColor &newcolor )
{
    m_color = newcolor;
    if( m_parent )
    {
        m_parent->updateTransferFunction();
    }
}

void WTransferFunction2DBoxWidget::showColorPicker()
{
    // Create a color dialog to create an RGBA quadrupel
    QColorDialog *dialog = new QColorDialog( );
    dialog->setCurrentColor( m_color );
    dialog->setOption( QColorDialog::NoButtons );
    dialog->setOption( QColorDialog::ShowAlphaChannel );
    connect( dialog, SIGNAL( currentColorChanged( const QColor& ) ), this, SLOT( colorSelected( const QColor& ) ) );
    dialog->open();
}

void WTransferFunction2DBoxWidget::keyPressEvent( QKeyEvent *event )
{
    if( event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete )
    {
        m_parent->removeWidget( this );
    }
}
