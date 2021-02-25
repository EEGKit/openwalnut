#ifndef WMPROTONDATA_H
#define WMPROTONDATA_H

#include "core/dataHandler/WDataSetCSV.h"

class WMProtonData {
public:
    explicit WMProtonData( boost::shared_ptr< WDataSetCSV::Content > csvHeader, boost::shared_ptr< WDataSetCSV::Content > csvData );

    boost::shared_ptr< WDataSetCSV::Content > getCSVHeader()
    {
        return m_csvHeader;
    }

    boost::shared_ptr< WDataSetCSV::Content > getCSVData()
    {
        return m_csvData;
    }

    void setCSVHeader( boost::shared_ptr< WDataSetCSV::Content > csvHeader );

    void setCSVData( boost::shared_ptr< WDataSetCSV::Content > csvData )
    {
        m_csvData = csvData;
    }

    void setColumnIndex( std::string columnName, int index );

    int getColumnIndex( std::string columnName );

    bool columnsInitialized();

private:
    /**
     * Stores column index of data.
     */
    boost::shared_ptr< WDataSetCSV::Content > m_csvHeader;

    /**
     * Stores data from obtained input file.
     */
    boost::shared_ptr< WDataSetCSV::Content > m_csvData;

    std::map< std::string, int > m_columnMap;
};

#endif  // WMPROTONDATA_H