//
// Created by top2021 on 11.02.21.
//

#include "WReaderCSV.h"

WReaderCSV::WReaderCSV(std::string fname) : WReader(fname){}

WReaderCSV::~WReaderCSV() throw() {}

/*
boost::shared_ptr< WDataSetPoints > WReaderCSV::read()
{
    m_vertices = boost::shared_ptr< std::vector< float > >( new std::vector< float >() );
    m_colors = boost::shared_ptr< std::vector< float > >( new std::vector< float >() );

    std::ifstream file(m_fname);
    std:: string input;

    bool isSkipFirstElement = false;

    while(std::getline(file, input))
    {
        //skip the first element (headers)
        if(!isSkipFirstElement)
        {
            isSkipFirstElement = true;
            continue;
        }

        //Seperated string with ","
        char* ptrToInput = new char[input.length() + 1];
        strcpy(ptrToInput, input.c_str());
        char* token = std::strtok(ptrToInput, ",");

        //skip the first 8 columns
        for (int i = 0; i < 7; ++i)
        {
            token = std::strtok(NULL, ",");
        }

        //X, Y, Z pos
        std::string x = std::strtok(NULL, ",");
        std::string y = std::strtok(NULL, ",");
        std::string z = std::strtok(NULL, ",");

        float posX = std::stof(x);
        float posY = std::stof(y);
        float posZ = std::stof(z);

        std::vector< float > point {posX, posY, posZ};

        m_vertices->push_back(posX);
        m_vertices->push_back(posY);
        m_vertices->push_back(posZ);

        m_colors->push_back(0);
        m_colors->push_back(0);
        m_colors->push_back(0);
    }

    boost::shared_ptr< WDataSetPoints > ds = boost::shared_ptr< WDataSetPoints >(
            new WDataSetPoints(
                    m_vertices,
                    m_colors
            )
    );

    return ds;
}
 */

boost::shared_ptr< WDataSetCSV > WReaderCSV::read()
{
    std::ifstream file(m_fname);
    std:: string input;

    m_content = boost::shared_ptr< std::vector< std::vector< std::string > > >( new std::vector< std::vector< std::string > >() );

    // CSV as Vector<Vector>
    //std::vector< std::vector< std::string > > csvMirror;
    while(std::getline(file, input))
    {
        char* ptrToInput = new char[input.length() + 1];
        strcpy(ptrToInput, input.c_str());
        char* token = std::strtok(ptrToInput, ",");

        std::vector< std::string > rowContent;

        while(token != NULL)
        {
            rowContent.push_back(token);
            token = std::strtok(NULL, ",");
        }

        m_content->push_back(rowContent);
    }

    return boost::shared_ptr< WDataSetCSV >(new WDataSetCSV(m_content));
}