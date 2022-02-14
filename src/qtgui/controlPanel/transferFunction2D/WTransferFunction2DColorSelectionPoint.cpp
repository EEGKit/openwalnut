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

#include "WTransferFunction2DColorSelectionPoint.h"

#include "QBrush"
#include "QGraphicsItem"
#include "QPainter"
#include "QColorDialog"
#include "core/common/WLogger.h"


WTransferFunction2DColorSelectionPoint::WTransferFunction2DColorSelectionPoint( QGraphicsItem *parent, QPointF position )
    : QGraphicsObject(), m_radius( 6.0 ), pressed( false ), m_parent( parent )
{
    setFlag( QGraphicsItem::ItemSendsGeometryChanges );
    setFlag( QGraphicsItem::ItemIsSelectable );
    setPos( position );
    setParentItem( parent );
}

WTransferFunction2DColorSelectionPoint::~WTransferFunction2DColorSelectionPoint()
{
}

QRectF WTransferFunction2DColorSelectionPoint::boundingRect() const
{
    const double padding( 2 );
    const double diameter( m_radius * 2 );
    return QRectF( -m_radius - padding, -m_radius - padding, diameter + padding, diameter + padding );
}

QVariant WTransferFunction2DColorSelectionPoint::itemChange( GraphicsItemChange change, const QVariant &value )
{
    return QGraphicsItem::itemChange( change, value );
}

void WTransferFunction2DColorSelectionPoint::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    QRadialGradient gradient( 0.0, 0.0, m_radius );

    gradient.setColorAt( 0, QColor( Qt::yellow ).lighter( 120 ) );
    gradient.setColorAt( 1, QColor( Qt::darkYellow ).lighter( 120 ) );
    painter->setPen( QPen( Qt::red, 0 ) );


    painter->setBrush( gradient );

    const double diameter( m_radius*2 );
    painter->drawEllipse( -m_radius, -m_radius, diameter, diameter );
}

void WTransferFunction2DColorSelectionPoint::mouseDoubleClickEvent( QGraphicsSceneMouseEvent *event )
{
    event->accept();
    wlog::debug( "SelectionPoint" ) << " Klappt. ";
    QGraphicsObject::mouseDoubleClickEvent( event );
    update();
    showColorPicker();
}

void WTransferFunction2DColorSelectionPoint::colorSelected( const QColor &newcolor )
{
    color = newcolor;
    //color.setAlpha( 255 ); //< force opacity

    if( m_parent )
    {
        m_parent->update();
    }
}

void WTransferFunction2DColorSelectionPoint::showColorPicker()
{
    QColorDialog *dialog = new QColorDialog( );
    dialog->setCurrentColor( color );
    dialog->setOption( QColorDialog::NoButtons );
    dialog->setOption( QColorDialog::ShowAlphaChannel );
    connect( dialog, SIGNAL( currentColorChanged( const QColor& ) ), this, SLOT( colorSelected( const QColor& ) ) );
    dialog->open();
}

