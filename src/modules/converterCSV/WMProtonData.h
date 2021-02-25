#ifndef WMPROTONDATA_H
#define WMPROTONDATA_H

#include "core/dataHandler/WDataSetCSV.h"

class WMProtonData
{
public:
    typedef boost::shared_ptr< WMProtonData > SPtr;

    explicit WMProtonData( WDataSetCSV::ContentSPtr csvHeader, WDataSetCSV::ContentSPtr csvData );

    WDataSetCSV::ContentSPtr getCSVHeader()
    {
        return m_csvHeader;
    }

    WDataSetCSV::ContentSPtr getCSVData()
    {
        return m_csvData;
    }

    void setCSVHeader( WDataSetCSV::ContentSPtr csvHeader );

    void setCSVData( WDataSetCSV::ContentSPtr csvData )
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
    WDataSetCSV::ContentSPtr m_csvHeader;

    /**
     * Stores data from obtained input file.
     */
    WDataSetCSV::ContentSPtr m_csvData;

    std::map< std::string, int > m_columnMap;
};

#endif  // WMPROTONDATA_H