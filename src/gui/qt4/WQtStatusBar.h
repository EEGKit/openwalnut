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
 * The status bar at the bottom of the main window, displays messages severity.
 * Different colors are used to indicate what type of message occured during the
 * use of OpenWalnut.
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
         * Pointer to the "trafic light" element in the bottom left corner of the status bar.
         * Indicator for the 'severest' event that occured.
         **/
        WQtStatusIcon *m_statusIcon;

        /**
         * Pointer to the label in the status bar, displaying the severest message and its origin.
         **/
        QLabel *m_label;

        /**
         * The standard item model holding the event data log.
         * Used to display the event log table.
         **/
        QStandardItemModel *m_model;

        /**
         * Table view that displays the model.
         **/
        QTableView *m_view;

        /**
         * Used to filter the content of the model by severity of the log message.
         **/
        QSortFilterProxyModel *m_filter;

        /**
         * Contains log table (the table view) and combo box (to interact with the filter)
         * to decide what kind of message is displayed in the log table.
         **/
        QWidget *m_logWidget;

        /**
         * Creates a new entry in the QStandardItemModel, represented by one row in the QTableView.
         *
         * \param entry the entry from whom the new row is created
         **/
        void createLogEntry( const WLogEntry& entry );

    private slots:
        /**
         * Gets called when the log level is changed in the log widget (through the combo box)
         *
         * \param logLevel the new log level for the filter
         **/
        void logLevelChanged( int logLevel );
};

#endif  // WQTSTATUSBAR_H
