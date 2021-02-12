//
// Created by top2021 on 11.02.21.
//

#ifndef OPENWALNUT_WDATASETCSV_H
#define OPENWALNUT_WDATASETCSV_H


#include "WDataSet.h"

class WDataSetCSV : public WDataSet
{
public:
    typedef boost::shared_ptr< WDataSetCSV > SPtr;
    typedef boost::shared_ptr< const WDataSetCSV > ConstSPtr;
    typedef boost::shared_ptr< std::vector<std::vector<std::string>>> Content;

    WDataSetCSV(Content content);
    WDataSetCSV();

    virtual ~WDataSetCSV();

private:
    Content m_content;
};


#endif //OPENWALNUT_WDATASETCSV_H