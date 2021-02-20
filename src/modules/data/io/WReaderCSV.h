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

/**
 * Read content from a CSV file.
 * \ingroup dataHandler
 */
class WReaderCSV : WReader
{
public:
    /**
     * Constructs a CSV reader object.
     *
     * \param fname path of the file to load.
     */
    explicit WReaderCSV( std::string fname );

    /**
     * Destroys this instance.
     */
    virtual ~WReaderCSV() throw();

    /**
     * Read the file and create a dataset as a vector.
     *
     * \return pointer to a WDataSetCSV
     */
    virtual boost::shared_ptr< WDataSetCSV > read();
};

#endif  // WREADERCSV_H
