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

#include <string>

#include "WQtNumberEdit.h"

WQtNumberEdit::WQtNumberEdit( QWidget* parent )
    : QLineEdit( parent )
{
    connect( this, SIGNAL( returnPressed() ), this, SLOT( numberChanged() ) );
}

WQtNumberEdit::~WQtNumberEdit()
{
}

void WQtNumberEdit::setInt( int number )
{
    setText( QString::number( number ) );
    m_boostSignal( m_name, number );
}

void WQtNumberEdit::numberChanged()
{
    bool ok;
    int number = text().toInt( &ok, 10 );
    if ( ok )
    {
        emit signalNumber( number );
        m_boostSignal( m_name, number );
    }
    else
    {
        setText( QString::number( 0 ) );
        emit signalNumber( 0 );
        m_boostSignal( m_name, 0 );
    }
}

boost::signal2< void, std::string, int >*WQtNumberEdit::getboostSignal()
{
    return &m_boostSignal;
}

void WQtNumberEdit::setName( std::string name )
{
    m_name = name;
}
