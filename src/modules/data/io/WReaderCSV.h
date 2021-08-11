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

#include <memory>
#include <string>
#include <vector>

#include <core/dataHandler/WDataSetCSV.h>
#include <core/dataHandler/WDataSetPoints.h>

#include "core/dataHandler/io/WReader.h"


/**
 * Read content from a CSV file.
 * \ingroup dataHandler
 */
class WReaderCSV : WReader
{
    /**
     * Only WReaderCSVTest class may be friend
     */
     friend class WReaderCSVTest;
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
     * \return pointer to a WDataSetCSV.
     * \throws WException If the file could not be opened.
     */
    virtual std::shared_ptr< WDataSetCSV > read();
private:
    /**
     * Transforms a given string into a string vector, by a predefined delimiter
     * \param line The line, which shell be split into a string vector
     * \return The splitted string as a string vector
     */
    std::vector< std::string > transformLineToVector( std::string line );
};

#endif  // WREADERCSV_H
