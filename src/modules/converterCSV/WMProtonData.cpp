#include "WMProtonData.h"

WMProtonData::WMProtonData( boost::shared_ptr< WDataSetCSV::Content > csvHeader, boost::shared_ptr< WDataSetCSV::Content > csvData )
{
    setCSVHeader( csvHeader );
    setCSVData( csvData );
}

void WMProtonData::setCSVHeader( boost::shared_ptr< WDataSetCSV::Content > csvHeader )
{
    if ( csvHeader->empty() )
    {
        throw WException( "No empty header allowed!" );
    }

    m_csvHeader = csvHeader;

    // TODO change csvheader type to vector< string > or maybe to map
    std::vector< std::string > header = m_csvHeader->at( 0 );

    for( auto i = 0; i < header.size(); i++ )
    {
        m_columnMap[header[i]] = i;
    }
}

void WMProtonData::setColumnIndex( std::string columnName, int index )
{
    m_columnMap[columnName] = index;
}

int WMProtonData::getColumnIndex( std::string columnName )
{
    if( m_columnMap.find( columnName ) == m_columnMap.end() )
    {
        return -1;
    }

    return m_columnMap[columnName];
}

bool WMProtonData::columnsInitialized()
{
    static const std::string necessaryColumns[] = {
        "PDGEncoding", "posX", "posY", "posZ", "edep", "eventID", "trackID", "parentID"
    };

    for( int i = 0; i < sizeof( necessaryColumns ) / sizeof( std::string ); i++ )
    {
        std::string column = necessaryColumns[i];
        if( getColumnIndex( column ) < 0 ) {
            return false;
        }
    }

    return true;
}