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

#ifndef WLOGOUTPUTPROVIDER_H
#define WLOGOUTPUTPROVIDER_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "WLogEntry.h"

/**
 * Basic output class for the WLogger logging system. This is an abstract class providing the needed interface for WLogger.
 */
class WLogOutputProvider
{
public:

    /**
     * Convenience typedef for a boost::shared_ptr< WLogOutputProvider >.
     */
    typedef boost::shared_ptr< WLogOutputProvider > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WLogOutputProvider >.
     */
    typedef boost::shared_ptr< const WLogOutputProvider > ConstSPtr;

    /**
     * Constructor. Create a new output instance.
     *
     * \param logLevel logging level, i.e. verboseness
     * \param format the format used for output
     * \param colored true if coloring should be used.
     */
    WLogOutputProvider( LogLevel logLevel = LL_DEBUG, std::string format = "*%l [%s] %m \n", bool colored = true );

    /**
     * Destructor.
     */
    virtual ~WLogOutputProvider();

    /**
     * Prints the specified entry to the output. Implement this method for each specific output system
     *
     * \param entry the entry to print.
     */
    virtual void printEntry( const WLogEntry& entry ) = 0;

    /**
     * Sets the new log level. All new incoming logs will be filtered according to this level.
     *
     * \param logLevel the level
     */
    void setLogLevel( LogLevel logLevel );

    /**
     * Gets the currently set log level.
     *
     * \return the current log level
     */
    LogLevel getLogLevel() const;

    /**
     * Sets the format string.
     *
     * \param format the format string.
     */
    void setFormat( std::string format );

    /**
     * Returns the currently set format string.
     *
     * \return format string.
     */
    std::string getFormat() const;

    /**
     * Set whether to use colors or not. Note: this is only useful on Linux systems currently.
     *
     * \param colors true if colors should be used.
     */
    void setColored( bool colors );

    /**
     * Getter determining whether to use colors or not.
     *
     * \return true if colors should be used.
     */
    bool isColored() const;
protected:
    /**
     * The logging level. All messages below this level are discarded.
     */
    LogLevel m_logLevel;

    /**
     * The format of the message.
     */
    std::string m_format;

    /**
     * True if colors should be used. This requires a compatible terminal.
     */
    bool m_color;
private:
    /**
     * Disallow copy.
     *
     * \param rhs the stream to copy
     */
    WLogOutputProvider( const WLogOutputProvider& rhs );

    /**
     * Disallow assignment.
     *
     * \param rhs the stream to assign to this
     *
     * \return this
     */
    WLogOutputProvider& operator=( const WLogOutputProvider& rhs );
};

#endif  // WLOGOUTPUTPROVIDER_H

