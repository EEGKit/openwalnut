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

#ifndef WTRANSFERFUNCTION2DBOXWIDGET_H
#define WTRANSFERFUNCTION2DBOXWIDGET_H

#include <vector>

#include "QGraphicsItem"

#include "WTransferFunction2DGUIWidget.h"
#include "WTransferFunction2DControlPoint.h"
#include "WTransferFunction2DEnums.h"
class WTransferFunction2DGUIWidget;
/**
 * This class defines a resizable box and is used as a
 * manipulation widget for the 2D Transfer Function.
 * With this widget you select how the data is depicted
 * later, through setting a color and opacity.
 *
 */
class WTransferFunction2DBoxWidget : public QGraphicsObject
{
    Q_OBJECT
public:
    /**
     * Constructor
     *
     * @param parent of the box
     * @param width of the box
     * @param height of the box
     * @param color of the box
     */
    explicit WTransferFunction2DBoxWidget( WTransferFunction2DGUIWidget *parent, double width, double height, QColor color );

    /**
     * Destructor
     */
    ~WTransferFunction2DBoxWidget();

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

    /**
     * Get the color of the box.
     *
     * \return  the color of the box
     */
    QColor getColor()
    {
        return m_color;
    }

    /**
     * Get the width of the box.
     *
     * \return  the width of the box
     */
    double getWidth()
    {
        return m_width;
    }

    /**
     * Get the height of the box.
     *
     * \return  the height of the box
     */
    double getHeight()
    {
        return m_height;
    }

    /**
     * Set the color of the box.
     *
     * \param color the new color
     */
    void setColor( QColor color );

public slots:
    /**
     * Called by the color dialog every time the user changes the color.
     *
     * \param color the new color
     */
    void colorSelected( const QColor& color );

    /**
     * Sets the resize handle to a new position
     *
     * \param handle handle location e.g. TOPLEFT
     * \param position position where it was moved to
     */
    void setResizeHandle( ResizePointsRect handle, QPointF position );

protected:
    /**
     * Fires event, when mouse button is pressed
     *
     * @param event
     */
    void mousePressEvent( QGraphicsSceneMouseEvent *event ) override;

    /**
     * Fires event, when mouse button is moved
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
      * Fires event, when mouse button is double clicked
      *
      * @param event
      */
    void mouseDoubleClickEvent( QGraphicsSceneMouseEvent *event ) override;

    /**
      * Fires event, when a key is pressed
      *
      * @param event
      */
    void keyPressEvent( QKeyEvent *event ) override;

    /**
     *  Handles events, when state of this item changes
     *
     *  from QtDocs:
     *  @param change is the parameter of the item that is changing
     *  @param value is the new value
     *
     *  @return a Qt datatype
     */
    QVariant itemChange( GraphicsItemChange change, const QVariant &value ) override;

private:
    /**
     * Show the color picker dialog.
     **/
    void showColorPicker();

    /**
     * Sets all control points in their desired corner of the rect
     * */
    void setControlPointsToCorner();

    ResizePointsRect m_resizePoints; /*!< Flag of current selected resize point */
    WTransferFunction2DGUIWidget* m_parent;  /*!< The parent object */
    QColor m_color;  /*!< The color of the box */
    double m_width; /*!< The width of the box */
    double m_height; /*!< The height of the box */
    std::vector< WTransferFunction2DControlPoint* > m_controlPoints; /*!< The different control handles in the corners */
    QRectF m_box; /*!< The box which is painted */
};
#endif  // WTRANSFERFUNCTION2DBOXWIDGET_H
