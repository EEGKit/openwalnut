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

#include "WLogStream.h"


WLogStream::WLogStream( std::ostream& output, LogLevel logLevel, std::string format, bool colored ): // NOLINT - we need this non-const ref here
    WLogOutputProvider( logLevel, format, colored ),
    m_output( output )
{
    // do nothing
}

void WLogStream::printEntry( const WLogEntry& entry )
{
    // level test
    if( m_logLevel > entry.getLogLevel() )
    {
        return;
    }

    m_output << entry.getLogString( m_format, m_color );
    m_output.flush();
}

