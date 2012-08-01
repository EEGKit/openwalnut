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

#include "WLogOutputProvider.h"

WLogOutputProvider::WLogOutputProvider( LogLevel logLevel, std::string format, bool colored ):
    m_logLevel( logLevel ),
    m_format( format ),
    m_color( colored )
{
    // initialize members
}

WLogOutputProvider::~WLogOutputProvider()
{
    // cleanup
}

void WLogOutputProvider::setLogLevel( LogLevel logLevel )
{
    m_logLevel = logLevel;
}

LogLevel WLogOutputProvider::getLogLevel() const
{
    return m_logLevel;
}

void WLogOutputProvider::setFormat( std::string format )
{
    m_format = format;
}

std::string WLogOutputProvider::getFormat() const
{
    return m_format;
}

void WLogOutputProvider::setColored( bool colors )
{
    m_color = colors;
}

bool WLogOutputProvider::isColored() const
{
    return m_color;
}

