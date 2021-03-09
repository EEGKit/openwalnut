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
    std::string col;
    std::string cell;

    std::vector< std::string >  row;
    WDataSetCSV::UnseperatedRowSPtr rawRow = WDataSetCSV::UnseperatedRowSPtr( new std::vector< std::string >() );
    WDataSetCSV::ContentSPtr header = WDataSetCSV::ContentSPtr( new WDataSetCSV::Content() );
    WDataSetCSV::ContentSPtr data = WDataSetCSV::ContentSPtr( new WDataSetCSV::Content() );

    std::fstream file;
    file.open( m_fname );

    if( !file.is_open() )
    {
        throw WException( "File could not be opened!" );
    }

    while( std::getline( file, col ) )
    {
        rawRow->push_back( col );
        std::istringstream col_in( col );
        row.clear();

        while( std::getline( col_in, cell, ',' ) )
        {
            row.push_back( cell );
        }
        data->push_back( row );
    }

    file.close();

    header->push_back( data->at( 0 ) );
    data->erase( data->begin() );

    boost::shared_ptr< WDataSetCSV > datasetcsv = boost::shared_ptr< WDataSetCSV >( new WDataSetCSV( header, data ) );
    datasetcsv->setRawDataSet( rawRow );

    return datasetcsv;
}
