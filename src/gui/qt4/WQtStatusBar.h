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

#ifndef WQTSTATUSBAR_H
#define WQTSTATUSBAR_H

#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QStandardItemModel>
#include <QtGui/QStatusBar>
#include <QtGui/QTableView>

#include "events/WLogEvent.h"
#include "guiElements/WQtStatusIcon.h"

/**
 * The status bar at the bottom of the main window, displays messages and
 * with the help of a colored circle ifany errors or warnings occure during the
 * use of OpenWalnut
 **/
class WQtStatusBar : public QStatusBar
{
    Q_OBJECT

    friend class WMainWindow;

    public:
        /**
         * constructor
         **/
        explicit WQtStatusBar( QWidget* parent = 0 );

        /**
         * destructor
         **/
        ~WQtStatusBar();

        /**
         * Closes the table view again. Does nothing if view is not opened.
         */
        void closeView();

    protected:
        /**
         * reimplementing QStatusBar::event()
         **/
        virtual bool event( QEvent* event );

        /**
         * reimplementing QStatusBar::mousePressEvent()
         **/
        void mousePressEvent( QMouseEvent* event );

    private:
        /**
         * pointer to the "trafic light" element in the bottom left corner of the status bar.
         **/
        WQtStatusIcon *m_statusIcon;

        /**
         * pointer to the label in the status bar, displaying the the newest
         * and the severest message at that point in time.
         **/
        QLabel *m_label;

        /**
         * the standard item model holding the event data.
         **/
        QStandardItemModel *m_model;

        /**
         * table view that displays the model.
         **/
        QTableView *m_view;

        /**
         * used to filter the content of the model
         **/
        QSortFilterProxyModel *m_filter;

        /**
         * contains log and combo box to decide what is displayed
         **/
        QWidget *m_logWidget;

        /**
         * creates a new entry in the QStandardItemModel, represented by one row in the QTableView.
         *
         * \param entry the entry from whom the new row is created
         **/
        void createLogEntry( const WLogEntry& entry );

    private slots:
        /**
         * gets called when the log level is changed in the log widget (through the combo box)
         *
         * \param logLevel the new log level
         **/
        void logLevelChanged( int logLevel );
};

#endif  // WQTSTATUSBAR_H
