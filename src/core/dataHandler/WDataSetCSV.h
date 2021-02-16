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

class WDataSetCSV : public WDataSet
{
public:
    typedef boost::shared_ptr< WDataSetCSV > SPtr;
    typedef boost::shared_ptr< const WDataSetCSV > ConstSPtr;
    typedef boost::shared_ptr< std::vector<std::vector<std::string>> > Content;

    explicit WDataSetCSV( Content content );
    WDataSetCSV();

    virtual ~WDataSetCSV();

    Content m_content;
};

#endif  // WDATASETCSV_H
