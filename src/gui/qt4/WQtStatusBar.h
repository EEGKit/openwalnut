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

#include <QtGui/QStatusBar>
#include <QtGui/QMenu>
#include <QtGui/QLabel>
#include <QtGui/QStandardItemModel>

#include "events/WLogEvent.h"
#include "guiElements/WQtStatusIcon.h"


#include <QtGui/QTableView>
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
         * pointer to the "trafic light" element in the bottom left corner
         **/
        WQtStatusIcon* m_statusIcon;

        /**
         * pointer to the label in the status bar, displaying messages
         **/
        QLabel* m_label;

        /**
         * the model holding the event data
         **/
        QStandardItemModel* m_model;

        /**
         * view that displays the model
         **/
        QTableView* m_view;

        /**
         * TODO
         **/
        void createLogEntry( const WLogEntry& );
};

#endif  // WQTSTATUSBAR_H
