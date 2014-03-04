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

#include <string>

#include <boost/regex.hpp>
#include <boost/tokenizer.hpp>

#include "core/common/WLogger.h"
#include "core/common/WStringUtils.h"
#include "core/kernel/WProjectFile.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WModuleContainer.h"
#include "core/kernel/WModule.h"

#include "WQtNetworkEditor.h"
#include "WQtNetworkItemGrid.h"
#include "WQtNetworkEditorProjectFileIO.h"

WQtNetworkEditorProjectFileIO::WQtNetworkEditorProjectFileIO( WQtNetworkEditor* ne ):
    WProjectFileIO(),
    m_networkEditor( ne )
{
    // initialize members
    // We want to set the layout before actually starting the network.
    setApplyOrder( PRE_MODULES );
}

WQtNetworkEditorProjectFileIO::~WQtNetworkEditorProjectFileIO()
{
    // cleanup
}

WProjectFileIO::SPtr WQtNetworkEditorProjectFileIO::clone( WProjectFile* project ) const
{
    // nothing special. Simply create new instance.
    WProjectFileIO::SPtr p( new WQtNetworkEditorProjectFileIO( m_networkEditor ) );
    p->setProject( project );
    return p;
}

/**
 * Interpret a string as a ";"- separated sequence of doubles.
 *
 * \param seq   the sequence
 * \param size  the number of doubles needed
 *
 * \return the values
 */
unsigned int* parseIntSequence( std::string seq, unsigned int size )
{
    // parse the string
    // -> tokenize it and fill pointer appropriately
    typedef boost::tokenizer<boost::char_separator< char > > tokenizer;
    boost::char_separator< char > sep( ";" );   // separate by ;
    tokenizer tok( seq, sep );

    // each value must be stored at the proper position
    unsigned int* values = new unsigned int[ size ];
    unsigned int i = 0;
    for( tokenizer::iterator it = tok.begin(); ( it != tok.end() ) && ( i < size ); ++it )
    {
        values[ i ] = string_utils::fromString< unsigned int >( ( *it ) );
        ++i;
    }

    // finally, set the values
    return values;
}

bool WQtNetworkEditorProjectFileIO::parse( std::string line, unsigned int lineNumber )
{
    static const boost::regex networkCoordRe( "^ *QT4GUI_NETWORK:([0-9]*)=(.*)$" );

    // use regex to parse it
    boost::smatch matches;  // the list of matches
    if( boost::regex_match( line, matches, networkCoordRe ) )
    {
        // network editor coordinate
        // matches[1] is the ID of the module
        // matches[2] is the network coordinate
        wlog::debug( "Project Loader [Parser]" ) << "Line " << lineNumber << ": Network \"" << matches[2] << "\" with module ID " << matches[1];

        unsigned int id = string_utils::fromString< unsigned int >( matches[1] );
        unsigned int* coordRaw = parseIntSequence( string_utils::toString( matches[2] ), 2 );

        // store. Applied later.
        m_networkCoords[ id ] = QPoint( coordRaw[0], coordRaw[1] );
        return true;
    }
    return false;
}

void WQtNetworkEditorProjectFileIO::done()
{
    // apply changes

    // get bounding box
    QPoint bbTL;
    QPoint bbBR;
    for( ModuleNetworkCoordinates::const_iterator it = m_networkCoords.begin(); it != m_networkCoords.end(); ++it )
    {
        // init first element
        if( it == m_networkCoords.begin() )
        {
            bbTL = ( *it ).second;
            bbBR = ( *it ).second;
        }
        else
        {
            // min/max
            bbTL.rx() = std::min( bbTL.x(), ( *it ).second.x() );
            bbTL.ry() = std::min( bbTL.y(), ( *it ).second.y() );
            bbBR.rx() = std::max( bbBR.x(), ( *it ).second.x() );
            bbBR.ry() = std::max( bbBR.y(), ( *it ).second.y() );
        }
    }

    // request a reserved grid zone:
    //unsigned int reservedAredID = m_networkEditor->getLayout()->reserveRect( bbTL, bbBR );

    // iterate our list.
    for( ModuleNetworkCoordinates::const_iterator it = m_networkCoords.begin(); it != m_networkCoords.end(); ++it )
    {
        // map the ID to a real module
        WModule::SPtr module = getProject()->mapToModule( ( *it ).first );
        if( module )
        {
            // register in network editor instance as a default position
            m_networkEditor->getLayout()->setModuleDefaultPosition( module, ( *it ).second );
        }
    }
}

void WQtNetworkEditorProjectFileIO::save( std::ostream& output ) // NOLINT
{
    output << "//////////////////////////////////////////////////////////////////////////////////////////////////////////////////" << std::endl <<
              "// Qt4GUI Network Information" << std::endl <<
              "//////////////////////////////////////////////////////////////////////////////////////////////////////////////////" << std::endl <<
              std::endl;

    // write the relative grid positions of each module
    WQtNetworkItemGrid* grid = m_networkEditor->getLayout()->getGrid();

    // IMPORTANT: we store relative coordinates. So we need the bounding rect to get relative coords
    QPoint bb = grid->getGridBoundingRect().topLeft();

    // iterate each module:
    // Grab container and lock
    WModuleContainer::ModuleSharedContainerType::ReadTicket container = WKernel::getRunningKernel()->getRootContainer()->getModules();
    // Iterate
    for( WModuleContainer::ModuleConstIterator iter = container->get().begin(); iter != container->get().end(); ++iter )
    {
        // a module has its own project file specific ID:
        unsigned int id = getProject()->mapFromModule( *iter );

        // get the graphical item for this module
        WQtNetworkItem* item = m_networkEditor->findItemByModule( *iter );

        // as grid for position
        if( grid->isInGrid( item ) )
        {
            QPoint p = grid->whereIs( item );

            // NOTE: we write relative coordinates! Si subtract bounding box top-left corner from each coordinate.
            output << "QT4GUI_NETWORK:" << id << "=" << p.x() - bb.x() << ";" << p.y() - bb.y() << std::endl;
        }
        // else: not in grid. We do not save info for this module
    }
}

