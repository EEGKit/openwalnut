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

#ifndef WTRANSFERFUNCTION2DWIDGET_H
#define WTRANSFERFUNCTION2DWIDGET_H

#include <vector>

#include "QGraphicsView"


class QGraphicsScene;
class WTransferFunction2DBackground;


/**
 * The class managing the widget that wants to receive updates whenever
 * a change occurrs. This is only here to provide an interface and a better
 * separation between gui code and the transfer function code
 */
struct WTransferFunction2DGuiNotificationClass
{
    /**
     * default destructor
     */
    virtual ~WTransferFunction2DGuiNotificationClass()
    {
    }
};

/**
 * A widget that holds a scene to display and modify the transfer function.
 *
 * Currently most data storage is done in this class, which is not a great
 * design. Thus, we should try to split the model and the view a bit more,
 * even though this is not intended by QGraphicsView.
 */
class WTransferFunction2DWidget : public QGraphicsView
{
    Q_OBJECT

public:
    /** type of our base class for easier coding */
    typedef QGraphicsView BaseClass;

    /**
     * Constructor. Create a TF widget with a given parent.
     *
     * \param qparent parent widgeet
     * \param parent  a class that receives notifications
     */
    WTransferFunction2DWidget( QWidget* qparent = NULL, WTransferFunction2DGuiNotificationClass* parent = NULL );

    /**
     * Destructor.
     */
    virtual ~WTransferFunction2DWidget();

    /**
     * Redraw the widget
     */
    void forceRedraw();

public slots:

    /**
    * Notification that the data changed, i.e., a control point has been moved or a color changed.
    */
    //void dataChanged();

protected:
    /**
     * Draws the background.
     *
     * \param painter the painter to use
     * \param rect the QRectF to repaint
     */
    virtual void drawBackground( QPainter *painter, const QRectF &rect );

    /**
     * Interactions implemented so far:
     * right click: insert new object (Ctrl+left click or two-finger tap on OSX)
     * left click on objects: (handled by individual objects) move control points and select
     *     object
     * double click on objects: (handled by individual objects) open parameter dialog, e.g.,
     *     to change the color
     * "Delete" or "Backspace": delete selected itom
     * \param event the event to handle
     */
    //virtual void keyPressEvent( QKeyEvent *event );

    /**
     * For a documentation of the implemented actions confer the keyPressEvent documentation.
     * \param event the event to handle
     */
    //virtual void mousePressEvent( QMouseEvent *event );

    /**
     * Internal helper function to update the QGraphicsPixmapItem that holds a representation
     * of the current color map and displays it as a background of the widget.
     * This function samples the texture into a QPixmap and updates the QGraphicsPixmapItem.
     */
    void setMyBackground();

private:
    /** the class that receives our update notifications */
    WTransferFunction2DGuiNotificationClass *parent;

    /** our scene */
    QGraphicsScene *scene;

    WTransferFunction2DBackground *background; //! background that displays the color map

    bool initialized; //< set to true after initialization
};

#endif  // WTRANSFERFUNCTION2DWIDGET_H
