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

#ifndef WQTNETWORKEDITOR_H
#define WQTNETWORKEDITOR_H

#include <time.h>
#include <string>
#include <list>

#include <boost/shared_ptr.hpp>

#include <QtGui/QWidget>
#include <QtGui/QDockWidget>
#include <QtGui/QVBoxLayout>

#include "../../../kernel/modules/data/WMData.h"
#include "../WQtCombinerToolbar.h"
#include "layout/WNetworkLayout.h"
#include "WQtNetworkItem.h"
#include "WQtNetworkScene.h"

// forward declaration
class WMainWindow;
class WNetworkLayout;

/**
 * Container widget to hold the WQtNetworkScene
 */
class WQtNetworkEditor : public QDockWidget
{
    Q_OBJECT
public:

    /**
     * default constructor
     *
     * \param parent The widget that manages this widget
     */
    explicit WQtNetworkEditor( WMainWindow* parent = 0 );

    /**
     * destructor.
     */
    virtual ~WQtNetworkEditor();

    /**
     * Create a new WQtNetworkItem for a given WModule
     * \param module a new WModule
     */
    void addModule(  boost::shared_ptr< WModule > module );

    /**
     * Simple search the WQtNetworkItem that belongs to the WModule
     *
     * \param module a WModule
     * \return WQtNetworkItem belongs to the WModule
     */
    WQtNetworkItem* findItemByModule( boost::shared_ptr< WModule > module );

    /**
     * Connect SIGNALS with SLOTS
     */
    void connectSlots();

    /**
     * This Method is everytime called when a new Module is inserted or an
     * WQtNetworkItem is moved manually. A Timer is set for calculating the new
     * forces.
     */
    void itemMoved();

    /**
     * Deletes all items in the scene who are selected.
     **/
    void deleteSelectedItems();

protected:

    /**
     * Reference to the main window of the application.
     */
    WMainWindow* m_mainWindow;

    /**
     * Everytime a module is associated, ready, connected, disconnected, removed or
     * deleted the kernels emits a signal and here we look how to behave.
     *
     * \param event the event that should be handled here.
     */
    virtual bool event( QEvent* event );

    /**
     * This event is called every 'timerID' in ms. If the WQtNetworkItems don't
     * move as result of the calculated forces the timer won't be reseted.
     *
     * \param event the timer event
     */
    void timerEvent( QTimerEvent* event );

private:

    WQtNetworkScene* m_scene; //!< QGraphicsScene

    //QGraphicsView* m_view; //!< QGraphicsView

    WNetworkLayout *m_layout; //!< the object that handels the layout

    //QWidget* m_panel; //!< panel

    //QVBoxLayout* m_layout; //!< layout

    QList< WQtNetworkItem* > m_items; //!< a list of the WQtNetworkItems in the WQtNetworkScene

    /**
     * Action which uses a compatibles list (submenu) to connect a selected item with other existing modules.
     */
    QAction* m_connectWithModuleAction;

    /**
     * Action which uses a compatibles list (submenu) to connect a selected item with other prototypes.
     */
    QAction* m_connectWithPrototypeAction;

    /**
     * Action which disconnects a connector from the module.
     */
    QAction* m_disconnectAction;

    /**
     * Is needed for random position of the WQtNetworkItems.
     */
    time_t m_time;

    /**
     * Certain time span in ms in which timerEvent gets called.
     */
    int timerId;

private slots:

    /**
     * Determines possible Connections and the propertytab.
     */
    void selectItem();
};

#endif  // WQTNETWORKEDITOR_H
