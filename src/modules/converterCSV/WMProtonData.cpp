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
#include <vector>

#include "WMProtonData.h"

WMProtonData::WMProtonData( WDataSetCSV::ContentSPtr csvHeader, WDataSetCSV::ContentSPtr csvData )
{
    setCSVHeader( csvHeader );
    setCSVData( csvData );
}

void WMProtonData::setCSVHeader( WDataSetCSV::ContentSPtr csvHeader )
{
    if( csvHeader->empty() )
    {
        throw WException( "No empty header allowed!" );
    }

    m_csvHeader = csvHeader;

    // TODO(robin.eschbach) change csvheader type to vector< string > or maybe to map
    std::vector< std::string > header = m_csvHeader->at( 0 );

    for( auto i = 0; i < header.size(); i++ )
    {
        m_columnMap[header[i]] = i;
    }
}

void WMProtonData::setColumnIndex( std::string columnName, int index )
{
    m_columnMap[columnName] = index;
}

int WMProtonData::getColumnIndex( std::string columnName )
{
    if( m_columnMap.find( columnName ) == m_columnMap.end() )
    {
        return -1;
    }

    return m_columnMap[columnName];
}

bool WMProtonData::columnsInitialized()
{
    static const std::string necessaryColumns[] = {
        "PDGEncoding", "posX", "posY", "posZ", "edep", "eventID", "trackID", "parentID"
    };

    for( int i = 0; i < sizeof( necessaryColumns ) / sizeof( std::string ); i++ )
    {
        std::string column = necessaryColumns[i];
        if( getColumnIndex( column ) < 0 )
        {
            return false;
        }
    }

    return true;
}
