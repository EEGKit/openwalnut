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
class WTransferFunction2DQuadTool : public QGraphicsObject
{
    Q_OBJECT
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

public slots:
    /**
     * Called by the color dialog every time the user changes the color.
     *
     * \param color the new color
     */
    void colorSelected( const QColor& color );

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

    /**
      * Fires event, when mouse button is pressed
      *
      * @param event
      */
    void mouseDoubleClickEvent( QGraphicsSceneMouseEvent *event );

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
    /**
     * Show the color picker dialog.
     **/
    void showColorPicker();

    bool m_pressed; /*!< Flag to check if mouse button is pressed */

    bool m_resizing; /*!< Flag to check if mouse button is pressed */

    ResizePoints m_resizePoints; /*!< Flag to check if mouse button is pressed */

    QGraphicsItem* m_parent;  /*!< The parent object */

    QColor m_color;  /*!< The color of the box */
    double m_lastMousePosX; /*!< Variable to store the last mouse position in x direction */
    double m_lastMousePosY; /*!< Variable to store the last mouse position in y direction */
    double m_width; /*!< The width of the box */
    double m_height; /*!< The height of the box */
};
#endif  // WTRANSFERFUNCTION2DQUADTOOL_H
