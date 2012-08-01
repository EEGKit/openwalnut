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

#ifndef WLOGSTREAM_H
#define WLOGSTREAM_H

#include <ostream>
#include <string>
#include <boost/shared_ptr.hpp>

#include "WLogOutputProvider.h"
#include "WLogEntry.h"

/**
 * Class implementing a capsule for an output stream and the needed level and format information. This implements WLogOutputProvider for
 * std::ostream.
 */
class WLogStream: public WLogOutputProvider
{
public:
    typedef boost::shared_ptr< WLogStream > SharedPtr;  //!< shared pointer type
    typedef WLogStream* Ptr; //!< pointer type
    typedef WLogStream& Ref; //!< reference
    typedef const WLogStream& ConstRef; //!< const reference

    /**
     * Constructor. Create a new stream instance. The output stream is a mandatory parameter. The others are predefined with some defaults.
     *
     * \param output the stream where to print log messages to
     * \param logLevel logging level, i.e. verboseness
     * \param format the format used for output
     * \param colored true if coloring should be used.
     */
    WLogStream( std::ostream& output, LogLevel logLevel = LL_DEBUG, std::string format = "*%l [%s] %m \n", bool colored = true );  // NOLINT - we need this non-const ref here

    /**
     * Prints the specified entry to the output stream in the right format if the log level matches.
     *
     * \param entry the entry to print.
     */
    virtual void printEntry( const WLogEntry& entry );

private:
    /**
     * The output stream.
     */
    std::ostream& m_output;
};

#endif  // WLOGSTREAM_H

