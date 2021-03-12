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

/**
 * Holds the csv data.
 */
class WMProtonData
{
    /**
     * only test class may be friend
     */
     friend class WMProtonDataTest;
public:
    /**
     * shared_ptr that points to itself 
     */
    typedef boost::shared_ptr< WMProtonData > SPtr;

    /**
     * constructor
     *
     * \param csvHeader pointer to a column-header of the CSV-file
     * \param csvData pointer to a content of the CSV file 
     */
    explicit WMProtonData( WDataSetCSV::ContentSPtr csvHeader, WDataSetCSV::ContentSPtr csvData );

    /**
     * getter
     *
     * \return pointer to a column-header of the CSV-file
     */
    WDataSetCSV::ContentSPtr getCSVHeader();

    /**
     * getter
     *
     * \return pointer to a content of the CSV file 
     */
    WDataSetCSV::ContentSPtr getCSVData();

    /**
     * setter
     *
     * \param csvHeader set m_csvHeader
     */
    void setCSVHeader( WDataSetCSV::ContentSPtr csvHeader );

    /**
     * setter
     *
     * \param csvData set m_csvData
     */
    void setCSVData( WDataSetCSV::ContentSPtr csvData );

    /**
     * setter
     *
     * \param columnName Name of column-header of the CSV-file
     * \param index position of column-header of the CSV-file
     */
    void setStateIndex( std::string columnName, int index );

    /**
     * getter
     *
     * \param columnName Name of selected name of single-selection
     * \return return the position current selected item
     */
    int getColumnIndexBySelection( std::string selectedName );

    /**
     * getter
     *
     * \param columnName Name of column-header of the CSV-file
     * \return return the position of column-header of the CSV-file as int
     */
    int getColumnIndex( std::string columnName );

    /**
     * checks whether columns are available 
     * \param columnName THe name of the column.
     * \return true if column is available, false if column is not available
     */
    bool isColumnAvailable( std::string columnName );

private:
    /**
     * Stores column index of data.
     */
    WDataSetCSV::ContentSPtr m_csvHeader;

    /**
     * Stores data from obtained input file.
     */
    WDataSetCSV::ContentSPtr m_csvData;

    /**
     * Stores data as map
     */
    std::map< std::string, int > m_columnMap;

    /**
     * Stores die Availability of the selected Column (ColumnPropertyHandler)
     */
    std::map< std::string, int > m_ColumnMapSelectedIndex;
};

#endif  // WMPROTONDATA_H
