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

#include "WTransferFunction2DQuadTool.h"
#include "core/common/WLogger.h"

#include "QApplication"
#include "QBrush"
#include "QGraphicsItem"
#include "QPainter"
#include "QColorDialog"

WTransferFunction2DQuadTool::WTransferFunction2DQuadTool( QGraphicsItem *parent )
{
    m_pressed = false;
    setZValue( 2 );
    setFlag( ItemIsMovable );
    setFlag( ItemSendsScenePositionChanges );
    setFlag( ItemIsSelectable );
    setFlag( ItemIsFocusable );
    setAcceptHoverEvents( true );
    setHandlesChildEvents( false );

    m_width = 50;
    m_height = 50;
    m_parent = parent;
    m_color = QColor( 255, 0, 0 , 128 );
}
WTransferFunction2DQuadTool::~WTransferFunction2DQuadTool()
{
}

QRectF WTransferFunction2DQuadTool::boundingRect() const
{
    return QRectF( 0, 0, m_width, m_height );
}

QVariant WTransferFunction2DQuadTool::itemChange( GraphicsItemChange change, const QVariant &value )
{
    if( change == ItemPositionChange && scene() )
    {
        // value is the new position.
        QPointF newPos = value.toPointF();
        QRectF rect( 0, 0, m_parent->boundingRect().width() - this->boundingRect().width(),
                         m_parent->boundingRect().height() - this->boundingRect().height() );

        if( !rect.contains( newPos ) )
        {
            // Keep the item inside the scene rect.
            newPos.setX( qMin( rect.right(), qMax( newPos.x(), rect.left() ) ) );
            newPos.setY( qMin( rect.bottom(), qMax( newPos.y(), rect.top() ) ) );
            return newPos;
        }
    }

    return QGraphicsItem::itemChange( change, value );
}

void WTransferFunction2DQuadTool::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    QRectF rect = boundingRect();
    QBrush brush( m_color );

    painter->fillRect( rect, brush );
    painter->drawRect( rect );
}

void WTransferFunction2DQuadTool::mousePressEvent( QGraphicsSceneMouseEvent *event )
{
    double mousePosX = event->pos().x();
    double mousePosY = event->pos().y();

    QRectF bRect = boundingRect();
    if( mousePosX >= bRect.right() - 10 && mousePosX <= bRect.right() + 10 )
    {
        m_pressed = true;
        m_resizing = true;
        m_resizePoints = RIGHT;
        m_lastMousePosX = event->pos().x();
        wlog::debug( "QUADTOOL" ) << "right bound clicked.";
    }
    else if( mousePosY >= bRect.bottom() - 10 && mousePosY <= bRect.bottom() + 10 )
    {
        m_resizing = true;
        m_resizePoints = BOTTOM;
        m_lastMousePosY = mousePosY;
    }
    update();
    QGraphicsItem::mousePressEvent( event );
}
void WTransferFunction2DQuadTool::mouseMoveEvent( QGraphicsSceneMouseEvent *event )
{
    double mousePosX = event->pos().x();
    double mousePosY = event->pos().y();

    if( m_resizing )
    {
        prepareGeometryChange();
        switch( m_resizePoints )
        {
            case RIGHT:
                // Do not allow resizing in negative direction
                if( mousePosX > boundingRect().left() + 20
                    && mousePosX <= m_parent->boundingRect().right() && mousePosX >= m_parent->boundingRect().left() ) // bounds of our texture
                {
                    double deltaMouseX = event->pos().x() - m_lastMousePosX;
                    m_width += deltaMouseX;
                    m_lastMousePosX = mousePosX;
                }
                break;
            case BOTTOM:
                // Do not allow resizing in negative direction
                if( mousePosY > boundingRect().top() + 20
                    && mousePosY <= m_parent->boundingRect().bottom() && mousePosY >= m_parent->boundingRect().top() ) // bounds of our texture
                {
                    double deltaMouseY = mousePosY - m_lastMousePosY;
                    m_height += deltaMouseY;
                    m_lastMousePosY = mousePosY;
                }
                break;
            default:
                break;
        }
        update();
    }
    else
    {
        /*
         * We only want to call the baseclass implementation of the mouse move event
         * when we are not resizing the item. The base class implements the functionality
         * that the item is movable and we do not want to move the item when resizing
         */
        update();
        QGraphicsItem::mouseMoveEvent( event );
    }
}
void WTransferFunction2DQuadTool::mouseReleaseEvent( QGraphicsSceneMouseEvent *event )
{
    m_pressed = false;
    m_resizing = false;
    m_lastMousePosX = 0;
    update();
    QGraphicsItem::mouseReleaseEvent( event );
}


void WTransferFunction2DQuadTool::mouseDoubleClickEvent( QGraphicsSceneMouseEvent *event )
{
    event->accept();
    wlog::debug( "SelectionPoint" ) << " Klappt. ";
    QGraphicsObject::mouseDoubleClickEvent( event );
    update();
    showColorPicker();
}

void WTransferFunction2DQuadTool::colorSelected( const QColor &newcolor )
{
    m_color = newcolor;
    if( m_parent )
    {
        m_parent->update();
    }
}

void WTransferFunction2DQuadTool::showColorPicker()
{
    QColorDialog *dialog = new QColorDialog( );
    dialog->setCurrentColor( m_color );
    dialog->setOption( QColorDialog::NoButtons );
    dialog->setOption( QColorDialog::ShowAlphaChannel );
    connect( dialog, SIGNAL( currentColorChanged( const QColor& ) ), this, SLOT( colorSelected( const QColor& ) ) );
    dialog->open();
}
