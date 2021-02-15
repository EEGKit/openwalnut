//
// Created by top2021 on 11.02.21.
//

#include "WReaderCSV.h"

WReaderCSV::WReaderCSV(std::string fname) : WReader(fname){}

WReaderCSV::~WReaderCSV() throw() {}


boost::shared_ptr< WDataSetCSV > WReaderCSV::read()
{
    std::string col;
    std::string cell;

    std::vector< std::string>  row;
    m_content = boost::shared_ptr< std::vector< std::vector< std::string > > >( new std::vector< std::vector< std::string > >() );

    std::fstream file;
    file.open(m_fname);

    if(!file.is_open())
    {
        //throw()
    }

    while (std::getline(file, col))
    {
        std::istringstream col_in(col);
        row.clear();

        while (std::getline(col_in, cell, ','))
        {
                row.push_back(cell);
        }
        m_content->push_back(row);
    }

    file.close();

    return boost::shared_ptr< WDataSetCSV >(new WDataSetCSV(m_content));
}