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

#include "QBrush"
#include "QGraphicsItem"
#include "QPainter"

WTransferFunction2DQuadTool::WTransferFunction2DQuadTool( QGraphicsItem *parent )
{
    pressed = false;
    setZValue( 10 );
    setFlag( ItemIsMovable );
    setFlag( QGraphicsItem::ItemSendsScenePositionChanges );

    m_parent = parent;
}
WTransferFunction2DQuadTool::~WTransferFunction2DQuadTool()
{
}

QRectF WTransferFunction2DQuadTool::boundingRect() const
{
    return QRectF( 0, 0, 50, 50 );
}

QVariant WTransferFunction2DQuadTool::itemChange( GraphicsItemChange change, const QVariant &value )
{
    if( change == ItemPositionChange && scene() )
    {
        // value is the new position.
        QPointF newPos = value.toPointF();
        QRectF rect( 0, 0, m_parent->boundingRect().width() - this->boundingRect().width(),
                         m_parent->boundingRect().height() - this->boundingRect().height() );
        wlog::debug( "QUADParent" ) << "Height: " << - m_parent->boundingRect().height() << " Width: " << - m_parent->boundingRect().width();

        wlog::debug( "QUAD" ) << "Height: " << - this->boundingRect().height() << " Width: " << - this->boundingRect().width();
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
    QBrush brush( Qt::red );

    if( pressed )
    {
        brush.setColor( Qt::green );
    }
    else
    {
        brush.setColor( Qt::cyan );
    }
    painter->fillRect( rect, brush );
    painter->drawRect( rect );
}

void WTransferFunction2DQuadTool::mousePressEvent( QGraphicsSceneMouseEvent *event )
{
    pressed = true;
    update();
    QGraphicsItem::mousePressEvent( event );
}

void WTransferFunction2DQuadTool::mouseReleaseEvent( QGraphicsSceneMouseEvent *event )
{
    pressed = false;
    update();
    QGraphicsItem::mouseReleaseEvent( event );
}

