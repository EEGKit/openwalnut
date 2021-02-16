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

#ifndef WREADERCSV_H
#define WREADERCSV_H

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <core/dataHandler/WDataSetPoints.h>
#include <core/dataHandler/WDataSetCSV.h>
#include "core/dataHandler/io/WReader.h"

class WReaderCSV : WReader
{
public:
    explicit WReaderCSV( std::string fname );
    virtual ~WReaderCSV() throw();
    virtual boost::shared_ptr< WDataSetCSV > read();

private:
    boost::shared_ptr< std::vector<float> > m_vertices;
    boost::shared_ptr< std::vector<float> > m_colors;
    boost::shared_ptr< std::vector<std::vector<std::string>> > m_content;
};


#endif  // WREADERCSV_H
