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

#include "WProtonData.h"


WProtonData::WProtonData( WDataSetCSV::ContentSPtr csvHeader, WDataSetCSV::ContentSPtr csvData )
{
    setCSVHeader( csvHeader );
    setCSVData( csvData );
}

void WProtonData::setCSVHeader( WDataSetCSV::ContentSPtr csvHeader )
{
    if( csvHeader == nullptr )
    {
        throw WException( "Can not set header! No header specified!" );
    }

    if( csvHeader->empty() )
    {
        throw WException( "No empty header allowed!" );
    }

    m_csvHeader = csvHeader;

    // TODO(robin.eschbach) change csvheader type to vector< string > or maybe to map
    std::vector< std::string > header = m_csvHeader->at( 0 );

    //Set Name with Index of csv to map
    for( size_t i = 0; i < header.size(); i++ )
    {
        m_columnMap[header[i]] = i;
    }
}

void WProtonData::setCSVData( WDataSetCSV::ContentSPtr csvData )
{
    if( csvData == nullptr )
    {
        throw WException( "Can not set data! No data specified!" );
    }

    m_csvData = csvData;
}

WDataSetCSV::ContentSPtr WProtonData::getCSVData()
{
    return m_csvData;
}

WDataSetCSV::ContentSPtr WProtonData::getCSVHeader()
{
    return m_csvHeader;
}

void WProtonData::setStateIndex( std::string columnName, int index )
{
    //0 is not accepted as an index because it is the default value if no map has been created. (index + 1)
    m_ColumnMapSelectedIndex[ columnName ] = index + 1;
}

bool WProtonData::isColumnAvailable( std::string columnName )
{
    return m_ColumnMapSelectedIndex[ columnName ] > 0;
}

int WProtonData::getColumnIndexBySelection( std::string selectedName )
{
    //to get the original index value, have to calculate minus 1
    return m_ColumnMapSelectedIndex[ selectedName ] - 1;
}

int WProtonData::getColumnIndex( std::string columnName )
{
    if( m_columnMap.find( columnName ) == m_columnMap.end() )
    {
        return -1;
    }

    return m_columnMap[ columnName ];
}
