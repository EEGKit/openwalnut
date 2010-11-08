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

#include <QtGui/QLabel>

#include "events/WLogEvent.h"
#include "events/WEventTypes.h"

#include "WQtStatusBar.h"

// TESTING
#include <iostream>

// public

WQtStatusBar::WQtStatusBar( QWidget* parent ):
    QStatusBar( parent )
{
    //QHBoxLayout* layout = new QHBoxLayout( this );
    WQtStatusIcon* m_statusIcon = new WQtStatusIcon( Qt::green, this );
    //std::cout << "c: " << this << std::endl;
    //std::cout << m_statusIcon << std::endl;
    m_statusIcon->setMaximumSize( QSize( 17, 17 ) );
    this->addPermanentWidget( m_statusIcon, 1 );
    m_label = new QLabel( this );
    m_label->setText("OpenWalnut");
    this->addPermanentWidget( m_label, 2 );
}

WQtStatusBar::~WQtStatusBar()
{
    delete m_statusIcon;
    delete m_label;
}

//private

bool WQtStatusBar::event( QEvent* event )
{
    //std::cout << "e: " << this << std::endl;
    //std::cout << m_statusIcon << std::endl;
    bool returnValue = false;
    if( event->type() == WQT_LOG_EVENT )
    {
        if( !m_statusIcon )
            return returnValue;
        returnValue = true;
        WLogEvent* updateEvent = dynamic_cast< WLogEvent* >( event );
        //WAssert( updateEvent, "Error with WUpdateStatusBarEvent" ); // TODO check this, change error message
        const WLogEntry& entry = updateEvent->getEntry();

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
            }
        }

        if( entry.getLogLevel() == LL_ERROR )
        {
            m_statusIcon->setColor( Qt::red );
        }
        // TODO set message
    }
    return returnValue;
}

/*QMenu* WQtStatusBar::createPopupMenu()
{
    return new QMenu();
}*/

