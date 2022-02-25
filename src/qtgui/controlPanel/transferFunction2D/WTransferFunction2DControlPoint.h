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

#ifndef WTRANSFERFUNCTION2DCONTROLPOINT_H
#define WTRANSFERFUNCTION2DCONTROLPOINT_H

#include "QGraphicsItem"
#include "WTransferFunction2DEnums.h"

class WTransferFunction2DControlPoint : public QGraphicsObject {
    Q_OBJECT
public:
    typedef QGraphicsObject BaseClass;

    /**
     * Constructor
     *
     * @param parent the parent object/widget
     * @param position the coordinate where the color selection point should be placed within the parent
     */
    explicit WTransferFunction2DControlPoint( BaseClass *parent, QPointF position, ResizePointsRect handleLocation );

    /**
     * Destructor
     */
    ~WTransferFunction2DControlPoint();

    /**
     * Defines the bounding rectangle of this item and returns it
     *
     * @return
     */
    QRectF boundingRect() const override;
    /**
     * Method for painting which must be overriden by this subclass
     *
     * @param painter
     * @param option
     * @param widget
     */
    void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr ) override;

    void setPosition( QPointF position );

protected:
    /**
     *  Handles events, when state of this item changes
     *
     *  from QtDocs:
     *  @param change is the parameter of the item that is changing
     *  @param value is the new value
     *
     *  @return
     */
    QVariant itemChange( GraphicsItemChange change, const QVariant &value );

    /**
     * Fires event, when mouse button is pressed
     *
     * @param event
     */
    void mouseMoveEvent( QGraphicsSceneMouseEvent *event );

    /**
     * Fires event, when mouse button is pressed
     *
     * @param event
     */
    void mouseReleaseEvent( QGraphicsSceneMouseEvent *event );

    /**
     * Fires event, when mouse button is pressed
     *
     * @param event
     */
    void mousePressEvent( QGraphicsSceneMouseEvent *event );
    void hoverEnterEvent( QGraphicsSceneHoverEvent *event ) override;
    void hoverLeaveEvent( QGraphicsSceneHoverEvent *event ) override;

signals:
    void resizeHandleChanged( ResizePointsRect location, QPointF position );

private:
    BaseClass *m_parent;
    QPointF m_position;
    double m_radius;
    ResizePointsRect m_handleLocation;
    bool m_pressed;

};


#endif // WTRANSFERFUNCTION2DCONTROLPOINT_H
