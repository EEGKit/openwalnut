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

#ifndef WLOADERFIBERS_H
#define WLOADERFIBERS_H

#include <fstream>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "../WLoader.h"
#include "../exceptions/WDHIOFailure.h"
#include "../../math/WPosition.h"

class WDataHandler;

/**
 * Loader for the VTK file formats. For VTK just see http://www.vtk.org.
 * Currently only a subset of the legacy format is supported: MedInria's
 * Fib-VTK format. The byte order of the files is assumed to be big endian by
 * default.
 *
 * \ingroup dataHandler
 */
class WLoaderFibers : public WLoader
{
friend class WLoaderFibersTest;
public:
    /**
     * Constructs and makes a new VTK loader for separate thread start.
     */
    WLoaderFibers( std::string fname, boost::shared_ptr< WDataHandler >
            dataHandler ) throw( WDHIOFailure );

    /**
     * Destroys this instance and closes the file.
     */
    virtual ~WLoaderFibers() throw();

    /**
     * This function is automatically called when creating a new thread for the
     * loader with boost::thread. It calls the methods of the NIfTI I/O library.
     */
    virtual void operator()() throw();

protected:
    /**
     * Read header from file.
     *
     * \return The offset where header ends, so we may skip this next operation.
     */
    void readHeader() throw( WDHIOFailure, WDHException );

    /**
     * Read points from file while starting at the given position.
     */
    void readPoints();

    /**
     * First four lines of ASCII text describing this file
     */
    std::vector< std::string > m_header;

    /**
     * Point vector for all fibers
     */
    std::vector< wmath::WPosition > m_points;

    /**
     * Pointer to the input file stream reader.
     */
    std::ifstream *m_ifs;

private:
};

#endif  // WLOADERFIBERS_H
