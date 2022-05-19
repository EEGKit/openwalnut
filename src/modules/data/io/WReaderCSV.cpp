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

#include <fstream>
#include <memory>
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


static std::istream& safeGetLine( std::istream& is, std::string& t ) // NOLINT
{
    // reads a line with a non specific line ending
    t.clear();
    std::istream::sentry se( is, true );
    std::streambuf* sb = is.rdbuf();

    int c;
    while( ( c = sb->sbumpc() ) != std::streambuf::traits_type::eof() )
    {
        switch( c )
        {
            case '\n':                      // Handle Unix linebreaks
                return is;
            case '\r':                      // Handle old mac linebreaks
                if( sb->sgetc() == '\n' )   // Handle windows linebreaks
                {
                    sb->sbumpc();
                }
                return is;
            default:
                t.push_back( c );
        }
    }

    if( t.empty() )
    {
        is.setstate( std::ios::eofbit );
    }
    return is;
}

std::shared_ptr< WDataSetCSV > WReaderCSV::read()
{
    size_t columnCount = 0;
    std::string line;
    std::vector< std::string >  row;
    WDataSetCSV::SeperatedRowSPtr rawRow = WDataSetCSV::SeperatedRowSPtr( new std::vector< std::string >() );

    WDataSetCSV::ContentSPtr header = WDataSetCSV::ContentSPtr( new WDataSetCSV::Content() );
    WDataSetCSV::ContentSPtr data = WDataSetCSV::ContentSPtr( new WDataSetCSV::Content() );

    std::fstream file;
    file.open( m_fname );

    if( !file.is_open() )
    {
        throw WException( "File could not be opened!" );
    }

    // treat first line as header
    safeGetLine( file, line );
    if( line == "" )
    {
        throw WException( "CSV file is empty!" );
    }

    rawRow->push_back( line );
    header->push_back( transformLineToVector( line ) );
    columnCount = header->at( 0 ).size();

    // treat remaining lines as data
    while( safeGetLine( file, line ) )
    {
        if( line.empty() && file.eof() )    // Ignore empty line at the end of a file.
        {
            break;
        }
        rawRow->push_back( line );
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

    std::shared_ptr< WDataSetCSV > datasetcsv = std::shared_ptr< WDataSetCSV >( new WDataSetCSV( header, data ) );
    datasetcsv->setRawDataSet( rawRow );

    return datasetcsv;
}

std::vector< std::string > WReaderCSV::transformLineToVector( std::string line )
{
    std::string cell;
    std::vector< std::string > row;

    size_t pos = 0;
    while( ( pos = line.find( ',' ) ) != std::string::npos )
    {
        cell = line.substr( 0, pos );
        row.push_back( cell );
        line.erase( 0, pos + 1 );
    }

    row.push_back( line );

    return row;
}
