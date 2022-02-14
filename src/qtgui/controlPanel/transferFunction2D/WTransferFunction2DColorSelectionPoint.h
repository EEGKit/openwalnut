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

#ifndef WTRANSFERFUNCTION2DCOLORSELECTIONPOINT_H
#define WTRANSFERFUNCTION2DCOLORSELECTIONPOINT_H

#include "QGraphicsItem"
#include "QGraphicsSceneMouseEvent"

/**
 * This class defines the color control point to select the color and opacity at a
 * specific point in the selection widget.
 *
 */
class WTransferFunction2DColorSelectionPoint : public QGraphicsObject
{
    Q_OBJECT
public:
    /**
     * Constructor
     *
     * @param parent the parent object/widget
     * @param position the coordinate where the color selection point should be placed within the parent
     */
    explicit WTransferFunction2DColorSelectionPoint( QGraphicsItem *parent, QPointF position );

    /**
     * Destructor
     */
    ~WTransferFunction2DColorSelectionPoint();

    /**
     * Defines the bounding rectangle of this item and returns it
     *
     * @return
     */
    QRectF boundingRect() const;

    /**
     * Method for painting which must be overriden by this subclass
     *
     * @param painter
     * @param option
     * @param widget
     */
    void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr );

    /**
     * Returns the last color picked in the color picker dialog
     *
     * @return color
     */
    QColor getColor()
    {
        return color;
    }

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
    void mouseDoubleClickEvent( QGraphicsSceneMouseEvent *event );

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

private:
    /**
     * Show the color picker dialog.
     **/
    void showColorPicker();

    bool pressed; /*!< Flag to check if mouse button is pressed */

    QGraphicsItem* m_parent;  /*!< The parent object */

    double m_radius; /*!< The parent object */

    QColor color; /*!< The parent object */
};
#endif  // WTRANSFERFUNCTION2DCOLORSELECTIONPOINT_H
