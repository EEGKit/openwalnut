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

#include <regex>
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>

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

    if( csvData->empty() )
    {
        throw WException( "Can not set data! No data content found!" );
    }

    detectColumnTypesFromCsvData( csvData );

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

WDataSetCSV::ContentElemSPtr WProtonData::getColumnTypes()
{
    return m_columnTypes;
}

void WProtonData::detectColumnTypesFromCsvData( WDataSetCSV::ContentSPtr csvData )
{
    m_columnTypes = WDataSetCSV::ContentElemSPtr( new std::vector< std::string >() );

    auto currentRow = csvData->begin();

    // determine column types based on first csv data row
    for( auto cell : *currentRow )
    {
        m_columnTypes->push_back( determineColumnTypeByString( cell ) );
    }

    assert( m_columnTypes != nullptr );
    assert( !m_columnTypes->empty() );
    assert( m_columnTypes->size() == m_csvHeader->at( 0 ).size() );
}

std::string WProtonData::determineColumnTypeByString( std::string cellValue )
{
    std::regex regexInt( R"(^-?[[:d:]]+$)" );
    std::regex regexDouble( R"(^([+-]?(?:[[:d:]]+\.?|[[:d:]]*\.[[:d:]]+))(?:[Ee][+-]?[[:d:]]+)?$)" );

    if( std::regex_search( cellValue, regexInt ) )
    {
        return WDatatype::getInt();
    }
    else if( std::regex_search( cellValue, regexDouble ) )
    {
        return WDatatype::getDouble();
    }
    else
    {
        return WDatatype::getString();
    }
}

std::vector< std::string > WProtonData::getHeaderFromType( std::string typeName )
{
    std::vector< std::string > header = m_csvHeader->at( 0 );
    std::vector< std::string > columnTypes = *m_columnTypes;
    std::vector< std::string > filterHeader;

    if( typeName == WDatatype::getDefault() )
    {
        return header;
    }

    for( size_t i = 0; i < columnTypes.size(); i++)
    {
        if( columnTypes[i] == typeName )
        {
            filterHeader.push_back( header[i] );
        }
    }

    return filterHeader;
}
