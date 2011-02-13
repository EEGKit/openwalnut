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

// TESTING
#include <iostream>

#include <QtGui/QLabel>
#include <QtGui/QTableWidget>
#include <QtGui/QStandardItemModel>
#include <QtGui/QHeaderView>

#include "events/WEventTypes.h"
#include "WQtStatusBar.h"

// public
WQtStatusBar::WQtStatusBar( QWidget* parent ):
    QStatusBar( parent ),
    m_statusIcon( new WQtStatusIcon( Qt::green, this ) )
{
    //QHBoxLayout* layout = new QHBoxLayout( this );
    m_statusIcon->setMaximumSize( QSize( 17, 17 ) );
    this->addPermanentWidget( m_statusIcon, 1 );
    m_label = new QLabel( this );
    m_label->setText( "OpenWalnut" );
    this->addPermanentWidget( m_label, 2 );
    m_model = new QStandardItemModel;
    /*m_model->setHeaderData(1, Qt::Horizontal, tr("source"), 0);
    m_model->setHeaderData(2, Qt::Horizontal, tr("message"), 0);
    m_model->setHeaderData(3, Qt::Horizontal, tr("time"), 0);
    */
    QStandardItem* item = new QStandardItem( QString( "source " ) );
    m_model->setHorizontalHeaderItem( 0, item );
    item = new QStandardItem( QString( "message " ) );
    m_model->setHorizontalHeaderItem( 1, item );
    item = new QStandardItem( QString( "time " ) );
    m_model->setHorizontalHeaderItem( 2, item );

}

WQtStatusBar::~WQtStatusBar()
{
    delete m_statusIcon;
    delete m_label;
    delete m_model;
}


//private
bool WQtStatusBar::event( QEvent* event )
{
    if( event->type() == WQT_LOG_EVENT )
    {
        WLogEvent* updateEvent = dynamic_cast< WLogEvent* >( event );
        const WLogEntry& entry = updateEvent->getEntry();
        if( updateEvent && entry.getLogLevel() >= LL_DEBUG && entry.getLogLevel() <= LL_ERROR)
        {
            //WAssert( updateEvent, "Error with WUpdateStatusBarEvent" );

            QString msg = QString::fromStdString( entry.getSource() );
            msg += ": ";
            msg += QString::fromStdString( entry.getMessage() );

            QColor color = m_statusIcon->getColor();
            if( entry.getLogLevel() == LL_INFO )
            {
                if( color != Qt::red && color != QColor( 255, 140, 0 ) )
                {
                    m_statusIcon->setColor( Qt::green );
                }
            }

            if( entry.getLogLevel() == LL_WARNING )
            {
                if( color != Qt::red )
                {
                    // 'Dark Orange' = 255, 140, 0
                    m_statusIcon->setColor( QColor( 255, 140, 0 ) );
                    m_label->setText( msg );
                }
            }

            if( entry.getLogLevel() == LL_ERROR )
            {
                m_statusIcon->setColor( Qt::red );
                m_label->setText( msg );
            }
        }
        createLogEntry( entry );
        return true;
    }
    else
    {
        return QStatusBar::event( event );
    }
}

void WQtStatusBar::mousePressEvent( QMouseEvent* event )
{
    QTableView* tv = new QTableView();
    tv->horizontalHeader()->setResizeMode( QHeaderView::ResizeToContents );
    tv->verticalHeader()->hide();
    tv->setSelectionBehavior( QAbstractItemView::SelectRows );
    tv->setEditTriggers( QAbstractItemView::NoEditTriggers );

    tv->setModel( m_model );

    QRect rect = tv->geometry();
    rect.setWidth( 2 + tv->verticalHeader()->width() +
        tv->columnWidth( 0 ) + tv->columnWidth( 1 ) + tv->columnWidth( 2 ) + tv->columnWidth( 3 ) );
    rect.setHeight( 2 + tv->verticalHeader()->height() );
    tv->setGeometry(rect);

    tv->show();
    return QStatusBar::mousePressEvent( event );
}

void WQtStatusBar::createLogEntry( const WLogEntry& entry )
{
    QList< QStandardItem* > entryRow;
    //entryRow << new QStandardItem( QString( entry.getLogLevel().c_str() ) ); // loglevel is enum
    entryRow << new QStandardItem( QString( entry.getSource().c_str() ) );
    entryRow << new QStandardItem( QString( entry.getMessage().c_str() ) );
    entryRow << new QStandardItem( QString( entry.getTime().c_str() ) );

    m_model->appendRow( entryRow );
}

