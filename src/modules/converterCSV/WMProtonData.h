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

#ifndef WMPROTONDATA_H
#define WMPROTONDATA_H

#include <map>
#include <string>

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
