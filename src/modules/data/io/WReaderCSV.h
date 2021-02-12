//
// Created by top2021 on 11.02.21.
//

#ifndef OPENWALNUT_WREADERCSV_H
#define OPENWALNUT_WREADERCSV_H

#include <boost/shared_ptr.hpp>
#include <core/dataHandler/WDataSetPoints.h>
#include <core/dataHandler/WDataSetCSV.h>
#include "core/dataHandler/io/WReader.h"


class WReaderCSV : WReader
{
public:
    explicit WReaderCSV(std::string fname);
    virtual ~WReaderCSV() throw();
    //virtual boost::shared_ptr< WDataSetPoints > read();
    virtual boost::shared_ptr< WDataSetCSV > read();

private:
    boost::shared_ptr< std::vector<float>> m_vertices;
    boost::shared_ptr< std::vector<float>> m_colors;
    boost::shared_ptr<std::vector<std::vector<std::string>>> m_content;
};


#endif //OPENWALNUT_WREADERCSV_H