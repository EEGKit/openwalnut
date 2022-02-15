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

#ifndef WTRANSFERFUNCTION2DQUADTOOL_H
#define WTRANSFERFUNCTION2DQUADTOOL_H

#include "QGraphicsItem"
#include "WTransferFunction2DColorSelectionPoint.h"

/**
    * Points where we can perform a resizing
    * TODO(@Kai): Add more points
    * */
enum ResizePoints
{
    RIGHT,
    BOTTOM
};

/**
 * This class defines a resizable box and is used as a
 * manipulation widget for the 2D Transfer Function.
 * With this widget you select how the data is depicated
 * later, through setting the color and opacity.
 *
 */
class WTransferFunction2DQuadTool : public QGraphicsItem
{
public:
    /**
     * Constructor
     *
     * @param parent
     */
    explicit WTransferFunction2DQuadTool( QGraphicsItem *parent );

    /**
     * Destructor
     */
    ~WTransferFunction2DQuadTool();

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

protected:
    /**
     * Fires event, when mouse button is pressed
     *
     * @param event
     */
    void mousePressEvent( QGraphicsSceneMouseEvent *event ) override;

    /**
     * Fires event, when mouse button is released
     *
     * @param event
     */
    void mouseMoveEvent( QGraphicsSceneMouseEvent *event ) override;

    /**
     * Fires event, when mouse button is released
     *
     * @param event
     */
    void mouseReleaseEvent( QGraphicsSceneMouseEvent *event ) override;

   // void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

   // void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;

   // void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    /**
     *  Handles events, when state of this item changes
     *
     *  from QtDocs:
     *  @param change is the parameter of the item that is changing
     *  @param value is the new value
     *
     *  @return
     */
    QVariant itemChange( GraphicsItemChange change, const QVariant &value ) override;

private:
    bool m_pressed; /*!< Flag to check if mouse button is pressed */

    bool m_resizing; /*!< Flag to check if mouse button is pressed */

    ResizePoints m_resizePoints; /*!< Flag to check if mouse button is pressed */

    QGraphicsItem* m_parent;  /*!< The parent object */

    WTransferFunction2DColorSelectionPoint* cpoint; /*!< Point to select a color */

    double m_lastMousePosX;
    double m_lastMousePosY;
    double m_width;
    double m_height;


};

#endif  // WTRANSFERFUNCTION2DQUADTOOL_H
