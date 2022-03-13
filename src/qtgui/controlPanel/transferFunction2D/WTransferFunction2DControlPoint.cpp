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


#include "QPainter"
#include "QtGui"

#include "core/common/WLogger.h"
#include "WTransferFunction2DControlPoint.h"
#include "WTransferFunction2DQuadTool.h"
#include "WTransferFunction2DScene.h"

WTransferFunction2DControlPoint::WTransferFunction2DControlPoint( BaseClass *parent, QPointF position, ResizePointsRect handleLocation )
{
    m_parent = parent;
    m_position = position;
    m_radius = 6.0;
    m_handleLocation = handleLocation;

    setFlag( QGraphicsItem::ItemSendsGeometryChanges );
    setFlag( QGraphicsItem::ItemIsSelectable );
    setFlag( ItemIsMovable );
    setPos( position );
    setParentItem( parent );
    setAcceptHoverEvents( true );
}

WTransferFunction2DControlPoint::~WTransferFunction2DControlPoint()
{
}

QRectF WTransferFunction2DControlPoint::boundingRect() const
{
    const double padding( 2 );
    const double diameter( m_radius * 2 );
    return QRectF( -m_radius - padding, -m_radius - padding, diameter + padding, m_radius + padding );
}

void WTransferFunction2DControlPoint::paint( QPainter *painter, const QStyleOptionGraphicsItem*, QWidget* )
{
    QRadialGradient gradient( 0.0, 0.0, m_radius );

    gradient.setColorAt( 0, QColor( Qt::gray ).lighter( 120 ) );
    gradient.setColorAt( 1, QColor( Qt::darkGray ).lighter( 120 ) );
    painter->setPen( QPen( Qt::red, 0 ) );

    painter->setBrush( gradient );

    const double diameter( m_radius*2 );
    painter->drawEllipse( -m_radius, -m_radius, diameter, diameter );
}

void WTransferFunction2DControlPoint::setPosition( QPointF position )
{
    prepareGeometryChange();
    setPos( position );
    update();
}

QVariant WTransferFunction2DControlPoint::itemChange( GraphicsItemChange change, const QVariant &value )
{
     if( change == ItemPositionChange && scene() )
     {
        // value is the new position.
        QPointF newPos = value.toPointF();
        QRectF rect = scene()->sceneRect();
        rect.translate( mapFromScene( pos() ) );
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

void WTransferFunction2DControlPoint::mousePressEvent( QGraphicsSceneMouseEvent *event )
{
    if( m_handleLocation != NONE )
    {
        m_pressed = true;
    }
    update();
    BaseClass::mousePressEvent( event );
}
void WTransferFunction2DControlPoint::mouseMoveEvent( QGraphicsSceneMouseEvent *event )
{
    switch( m_handleLocation )
    {
        case TOPRIGHT:
            emit( resizeHandleChanged( TOPRIGHT, this->pos() ) );
            break;
        case BOTTOMRIGHT:
            emit( resizeHandleChanged( BOTTOMRIGHT, this->pos() ) );
            //wlog::debug( "CP") << pos().x() << " " << pos().y();
            break;
        case BOTTOMLEFT:
            emit( resizeHandleChanged( BOTTOMLEFT, this->pos() ) );
            break;
        case TOPLEFT:
            emit( resizeHandleChanged( TOPLEFT, this->pos() ) );
            break;
        default:
            break;
    }
    update();
    m_parent->update();
    BaseClass::mouseMoveEvent( event );
}

void WTransferFunction2DControlPoint::mouseReleaseEvent( QGraphicsSceneMouseEvent *event )
{
    m_pressed = false;
    update();
    BaseClass::mouseReleaseEvent( event );
}

void WTransferFunction2DControlPoint::hoverEnterEvent( QGraphicsSceneHoverEvent *event )
{
    switch( m_handleLocation )
    {
        case TOPRIGHT:
        case BOTTOMLEFT:
            QGraphicsItem::setCursor( QCursor( Qt::SizeBDiagCursor ) );
            break;
        case BOTTOMRIGHT:
        case TOPLEFT:
            QGraphicsItem::setCursor( QCursor( Qt::SizeFDiagCursor ) );
            break;
        default:
            break;
    }
    update();
    QGraphicsItem::hoverEnterEvent( event );
}

void WTransferFunction2DControlPoint::hoverLeaveEvent( QGraphicsSceneHoverEvent *event )
{
    QGraphicsItem::setCursor( QCursor( Qt::ArrowCursor ) );
    emit( resizeHandleChanged( NONE, QPointF( 0, 0 ) ) );
    update();
    QGraphicsItem::hoverLeaveEvent( event );
}

