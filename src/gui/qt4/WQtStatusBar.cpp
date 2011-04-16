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

#include <QtGui/QComboBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QTableWidget>

#include "events/WEventTypes.h"

#include "WQtStatusBar.h"

// public
WQtStatusBar::WQtStatusBar( QWidget *parent ):
    QStatusBar( parent ),
    m_statusIcon( new WQtStatusIcon( Qt::green, this ) )
{
    m_statusIcon->setMaximumSize( QSize( 17, 17 ) );
    this->addPermanentWidget( m_statusIcon, 1 );
    m_label = new QLabel( this );
    m_label->setText( "OpenWalnut" );
    m_label->setMinimumSize( QSize( 750, 17 ) );
    this->addPermanentWidget( m_label, 2 );

    QVBoxLayout *layout = new QVBoxLayout();

    m_model = new QStandardItemModel();
    QStandardItem* item = new QStandardItem( QString( "type " ) );
    m_model->setHorizontalHeaderItem( 0, item );
    item = new QStandardItem( QString( "time " ) );
    m_model->setHorizontalHeaderItem( 1, item );
    item = new QStandardItem( QString( "source " ) );
    m_model->setHorizontalHeaderItem( 2, item );
    item = new QStandardItem( QString( "message " ) );
    m_model->setHorizontalHeaderItem( 3, item );

    m_filter = new QSortFilterProxyModel();
    m_filter->setSourceModel( m_model );
    // display log events beginning with log level info, ignore debug
    m_filter->setFilterRegExp( QRegExp( "^[1-3]" ) );

    QComboBox *comboBox = new QComboBox();
    comboBox->addItem( "Debug " );
    comboBox->addItem( "Info " );
    comboBox->addItem( "Warning " );
    comboBox->addItem( "Error " );
    comboBox->setCurrentIndex( 1 ); // set to 'Info'
    connect( comboBox, SIGNAL( currentIndexChanged( int ) ), this, SLOT( logLevelChanged( int ) ) );
    layout->addWidget( comboBox );

    m_view = new QTableView();
    m_view->setModel( m_filter );
    //m_view->setGeometry( QRect( 0, 0, 1000, 500 ) );
    m_view->setMinimumSize( QSize( 1000, 500 ) );
    m_view->verticalHeader()->hide();
    m_view->setSelectionBehavior( QAbstractItemView::SelectRows );
    m_view->setEditTriggers( QAbstractItemView::NoEditTriggers );
    m_view->setColumnHidden( 0, true );
    layout->addWidget( m_view );

    m_logWidget = new QWidget();
    m_logWidget->setLayout( layout );
}

WQtStatusBar::~WQtStatusBar()
{
    delete m_statusIcon;
    delete m_label;
    delete m_view;
    delete m_filter;
    delete m_model;
    delete m_logWidget;
}

void WQtStatusBar::closeView()
{
    m_logWidget->close();
}

//private
bool WQtStatusBar::event( QEvent *event )
{
    if( event->type() == WQT_LOG_EVENT )
    {
        WLogEvent* updateEvent = dynamic_cast< WLogEvent* >( event );
        const WLogEntry& entry = updateEvent->getEntry();
        if( updateEvent )
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

void WQtStatusBar::mousePressEvent( QMouseEvent *event )
{
    m_view->resizeColumnsToContents();
    m_view->horizontalHeader()->setStretchLastSection( true );
    m_logWidget->show();
    return QStatusBar::mousePressEvent( event );
}

void WQtStatusBar::createLogEntry( const WLogEntry& entry )
{
    QList< QStandardItem* > entryRow;

    // add type, is hidden
    QString type;
    switch( entry.getLogLevel() )
    {
        case LL_DEBUG:
            type = "0";
            break;
        case LL_INFO:
            type = "1";
            break;
        case LL_WARNING:
            type = "2";
            break;
        case LL_ERROR:
            type = "3";
            break;
    }
    entryRow << new QStandardItem( type );

    // add time
    entryRow << new QStandardItem( QString( entry.getTime().c_str() ) );

    // add source and color for severity
    QStandardItem *source = new QStandardItem( QString( entry.getSource().c_str() ) );
    if( entry.getLogLevel() == LL_WARNING )
    {
        source->setBackground( QColor( 255, 140, 0 ) );
    }
    else if( entry.getLogLevel() == LL_ERROR )
    {
        source->setBackground( Qt::red );
    }
    entryRow << source;

    // add message
    entryRow << new QStandardItem( QString( entry.getMessage().c_str() ) );

    m_model->appendRow( entryRow );
    m_view->resizeColumnsToContents();
    m_view->horizontalHeader()->setStretchLastSection( true );
    m_view->scrollToBottom();
}

void WQtStatusBar::logLevelChanged( int logLevel )
{
    QString level;
    switch( logLevel )
    {
        case 0:
            level = "^.";
            break;
        case 1:
            level = "^[1-3]";
            break;
        case 2:
            level = "^[2-3]";
            break;
        case 3:
            level = "^[3]";
            break;
    }
    m_filter->setFilterRegExp( QRegExp( level ) );
}

