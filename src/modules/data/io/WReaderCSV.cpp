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

#include "WReaderCSV.h"

WReaderCSV::WReaderCSV( std::string fname )
        : WReader( fname )
{
}

WReaderCSV::~WReaderCSV() throw()
{
}


boost::shared_ptr< WDataSetCSV > WReaderCSV::read()
{
    int columnCount = 0;
    std::string line;
    std::vector< std::string >  row;

    WDataSetCSV::ContentSPtr header = WDataSetCSV::ContentSPtr( new WDataSetCSV::Content() );
    WDataSetCSV::ContentSPtr data = WDataSetCSV::ContentSPtr( new WDataSetCSV::Content() );

    std::fstream file;
    file.open( m_fname );

    if( !file.is_open() )
    {
        throw WException( "File could not be opened!" );
    }

    // treat first line as header
    std::getline( file, line );
    if( line == "" )
    {
        throw WException( "CSV file is empty!" );
    }

    header->push_back( transformLineToVector( line ) );
    columnCount = header->at( 0 ).size();

    // treat remaining lines as data
    while( std::getline( file, line ) )
    {
        row = transformLineToVector( line );
        if( row.size() != columnCount )
        {
            throw WException( "Data row count does not equal header count!" );
        }
        else
        {
            data->push_back( row );
        }
    }

    if( data->size() == 0 )
    {
        throw WException( "CSV File does not contain data!" );
    }

    file.close();

    return boost::shared_ptr< WDataSetCSV >( new WDataSetCSV( header, data ) );
}

std::vector< std::string > WReaderCSV::transformLineToVector( std::string line )
{
    std::string cell;
    std::vector< std::string >  row;

    std::istringstream inLine( line );
    while( std::getline( inLine, cell, ',' ) )
    {
        row.push_back( cell );
    }

    return row;
}