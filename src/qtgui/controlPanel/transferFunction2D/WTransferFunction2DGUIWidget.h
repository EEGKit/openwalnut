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
#include "core/common/WHistogram2D.h"
#include "WTransferFunction2DBoxWidget.h"
#include "WTransferFunction2DBackground.h"

class QGraphicsScene;
class WTransferFunction2DBoxWidget;

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

    /**
     * Is called when the GUI is changed
     * \param tf the new transfer function
     */
    virtual void guiUpdate( const WTransferFunction2D& tf ) = 0;
};

/**
 * A widget that holds a scene to display and modify the transfer function.
 *
 * Currently most data storage is done in this class, which is not a great
 * design. Thus, we should try to split the model and the view a bit more,
 * even though this is not intended by QGraphicsView.
 */
class WTransferFunction2DGUIWidget : public QGraphicsView
{
    Q_OBJECT

public:
    /** type of our base class for easier coding */
    typedef QGraphicsView BaseClass;

    /**
     * Constructor. Create a TF widget with a given parent.
     *
     * \param qparent parent widget
     * \param parent  a class that receives notifications
     */
    WTransferFunction2DGUIWidget(QWidget* qparent = NULL, WTransferFunction2DGuiNotificationClass* parent = NULL );

    /**
     * Destructor.
     */
    virtual ~WTransferFunction2DGUIWidget();

    /**
     * Redraw the widget
     */
    void forceRedraw();

    /**
     * Sets the histogram
     *
     * \param newHistogram is the histogram which is to be set here
     */
    void setHistogram( WHistogram2D newHistogram );
public slots:
    /**
    * Notification that the data changed, i.e., a control point has been moved or a color changed.
    */
    void updateTexture();
private slots:
    /**
    * Adds a box widget to the scene
    */
    void addBoxWidget();
    /**
    * Removes all transfer function widgets from the scene
    */
    void cleanTransferFunction();
protected:
    /**
     * Draws the background.
     *
     * \param painter the painter to use
     * \param rect the QRectF to repaint
     */
    virtual void drawBackground( QPainter *painter, const QRectF &rect );

    /**
     * Updates the transfer function.
     */
    void updateTransferFunction();

    /**
     * Internal helper function to update the QGraphicsPixmapItem that holds a representation
     * of the current color map and displays it as a background of the widget.
     * This function samples the texture into a QPixmap and updates the QGraphicsPixmapItem.
     */
    void setMyBackground();

private:
    /**
     * custom context menu e.g. add widgets, clear TF, ...
     *
     * @param event event fired when right clicked
     */
    void contextMenuEvent( QContextMenuEvent *event );

    /** the class that receives our update notifications */
    WTransferFunction2DGuiNotificationClass *parent;

    /** List of widgets */
    std::vector< WTransferFunction2DBoxWidget* > m_widgets;

    /** our scene */
    QGraphicsScene *scene;

    /** background that displays the histogram */
    WTransferFunction2DBackground *background;
    /** histogram which is displayed on the background */
    WHistogram2D *hist;
    /** set to true after initialization */
    bool initialized;
};

#endif  // WTRANSFERFUNCTION2DWIDGET_H
