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

#ifndef WDATASETCSV_H
#define WDATASETCSV_H


#include "WDataSet.h"
#include "string"
#include "vector"

/**
 * Represents a CSV dataset.
 */
class WDataSetCSV : public WDataSet
{
    /**
     * Only test class may be friend
    */
    friend class WDataSetCSVTest;
public:
    /**
     * represents a vector containing a vector of strings.
     */
    typedef std::vector< std::vector< std::string > > Content;

    /**
     * represents a pointer to the Content
     */
    typedef boost::shared_ptr< std::vector< std::vector< std::string > > > ContentSPtr;

    /**
     * represents a pointer to a vector of csv-raw-row as string
     */
    typedef boost::shared_ptr< std::vector< std:: string > > SeperatedRowSPtr;

    /**
     * represents an element within WDataSetCSV object.
     */
    typedef std::vector< std::string > ContentElem;

    /**
     * Construct WDataSetCSV object
     *
     * \param header Column names of the CSV file.
     * \param data Data content of the CSV file.
     */
    explicit WDataSetCSV( WDataSetCSV::ContentSPtr header, WDataSetCSV::ContentSPtr data );

    /**
     * The standard constructor.
     */
    WDataSetCSV();

    /**
     * Destructs this dataset.
     */
    virtual ~WDataSetCSV();

    /**
     * Getter method to receive csv header
     *
     * \return m_header as WDataSetCSV::Content object
     */
    WDataSetCSV::ContentSPtr getHeader();

    /**
     * Getter method to receive csv data
     *
     * \return m_data as WDataSetCSV::Content object
     */
    WDataSetCSV::ContentSPtr getData();

    /**
     * Getter method to receive csv rawdata
     *
     * \return m_data as WDataSetCSV::Content object
     */
    void setRawDataSet( WDataSetCSV::SeperatedRowSPtr rawDataSetIn );

    /**
     * Getter method to receive csv data
     *
     * \return rawDataSet as std::vector< std:: string > object
     */
    SeperatedRowSPtr getRawDataSet();

private:
    /**
     * Stores the column titles of a loaded CSV file.
     */
    WDataSetCSV::ContentSPtr m_header;

    /**
     * Stores the data of a loaded CSV file.
     */
    WDataSetCSV::ContentSPtr m_data;

    /**
     * Stores the rawdata of a loaded CSV file.
     */
    SeperatedRowSPtr rawDataSet;
};

#endif  // WDATASETCSV_H
