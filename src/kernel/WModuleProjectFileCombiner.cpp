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

#include <iostream>
#include <map>
#include <utility>

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include "WKernel.h"
#include "WModuleCombiner.h"
#include "WModuleFactory.h"
#include "WModuleConnector.h"
#include "WModule.h"
#include "WModuleInputConnector.h"
#include "WModuleOutputConnector.h"
#include "exceptions/WModuleConnectorNotFound.h"

#include "../common/exceptions/WFileNotFound.h"
#include "../common/WLogger.h"

#include "WModuleProjectFileCombiner.h"

WModuleProjectFileCombiner::WModuleProjectFileCombiner( boost::filesystem::path project, boost::shared_ptr< WModuleContainer > target ):
    WModuleCombiner( target ),
    m_project( project )
{
    // initialize members
}

WModuleProjectFileCombiner::WModuleProjectFileCombiner( boost::filesystem::path project ):
    WModuleCombiner( WKernel::getRunningKernel()->getRootContainer() ),
    m_project( project )
{
    // initialize members
}

WModuleProjectFileCombiner::~WModuleProjectFileCombiner()
{
    // cleanup
}

void WModuleProjectFileCombiner::apply()
{
    wlog::info( "Kernel" ) << "Loading project file \"" << m_project.file_string() << "\".";

    // read the file
    std::ifstream input( m_project.file_string().c_str() );
    if ( !input.is_open() )
    {
        throw WFileNotFound( "The project file \"" + m_project.file_string() + "\" does not exist." );
    }

    // this is the proper regular expression for modules
    static const boost::regex modRe ( "^MODULE:([0-9]*):(.*)$" );
    static const boost::regex conRe ( "^CONNECTION:\\(([0-9]*),(.*)\\)->\\(([0-9]*),(.*)\\)$" );
    static const boost::regex propRe( "^PROPERTY:\\(([0-9]*),(.*)\\)=(.*)$" );
    static const boost::regex commentRe( "^//.*$" );

    // some maps which describe the module graph
    // These maps are used to read in the file and to later on build the actual graph
    typedef std::pair< unsigned int, boost::shared_ptr< WModule > > ModuleID;
    std::map< unsigned int, boost::shared_ptr< WModule > > modules;

    // list of connections
    typedef std::pair< unsigned int, std::string > Connector;
    typedef std::pair< Connector, Connector > Connection;
    std::list< Connection > connections;

    // list of properties
    typedef std::pair< unsigned int, std::string > Property;
    typedef std::pair< Property, std::string > PropertyValue;
    std::list< PropertyValue > properties;

    // read it line by line
    std::string line;       // the current line
    boost::smatch matches;  // the list of matches
    int i = 0;              // line counter

    while ( std::getline( input, line ) )
    {
        i++;
        if ( boost::regex_match( line, matches, modRe ) )
        {
            // it is a module line
            // matches[1] is the ID
            // matches[2] is the name of the module

            wlog::debug( "Project Loader [Parser]" ) << "Line " << i << ": Module \"" << matches[2] << "\" with ID " << matches[1];

            // create a module instance
            boost::shared_ptr< WModule > proto = WModuleFactory::getModuleFactory()-> isPrototypeAvailable( matches[2] );
            if ( !proto )
            {
                wlog::error( "Project Loader" ) << "There is no prototype available for module \"" << matches[2] << "\". Skipping.";
            }
            else
            {
                boost::shared_ptr< WModule > module = WModuleFactory::getModuleFactory()->create( proto );
                modules.insert( ModuleID( boost::lexical_cast< unsigned int >( matches[1] ), module ) );
            }
        }
        else if ( boost::regex_match( line, matches, conRe ) )
        {
            // it is a connector line
            // matches[1] and [2] are the module ID and connector name of the output connector
            // matches[3] and [4] are the module ID and connector name of the target input connector

            wlog::debug( "Project Loader [Parser]" ) << "Line " << i << ": Connection between \"" << matches[2] << "\" of module " << matches[1] <<
                                                               " and \"" << matches[4] << "\" of module " << matches[3] << ".";

            // now we search in modules[ matches[1] ] for an output connector named matches[2]
            connections.push_back( Connection( Connector( boost::lexical_cast< unsigned int >( matches[1] ), matches[2] ),
                                               Connector( boost::lexical_cast< unsigned int >( matches[3] ), matches[4] ) ) );
        }
        else if ( boost::regex_match( line, matches, propRe ) )
        {
            // it is a property line
            // matches[1] is the module ID
            // matches[2] is the property name
            // matches[3] is the property value

            wlog::debug( "Project Loader [Parser]" ) << "Line " << i << ": Property \"" << matches[2] << "\" of module " << matches[1] << " set to " <<
                                                                matches[3];

            properties.push_back( PropertyValue( Property( boost::lexical_cast< unsigned int >( matches[1] ), matches[2] ), matches[3] ) );
        }
        else if ( !line.empty() && !boost::regex_match( line, matches, commentRe ) )
        {
            wlog::debug( "Project Loader [Parser]" ) << "Line " << i << ": Malformed. Skipping.";
        }
    }
    // close it
    input.close();

    // now, as we have created the modules, we need to set the properties for each of it.
    for ( std::list< PropertyValue >::const_iterator iter = properties.begin(); iter != properties.end(); ++iter )
    {
        // grab corresponding module
        if ( !modules.count( ( *iter ).first.first ) )
        {
            wlog::error( "Project Loader" ) << "There is no module with ID \"" << ( *iter ).first.first <<  "\" to set the property \"" <<
                                               ( *iter ).first.second << "\" for. Skipping.";
            continue;
        }
        boost::shared_ptr< WModule > m = modules[ ( *iter ).first.first ];

        // has this module the specified property?
        boost::shared_ptr< WPropertyBase > prop = m->getProperties2()->findProperty( ( *iter ).first.second );
        if ( !prop )
        {
            wlog::error( "Project Loader" ) << "The module \"" << m->getName() << "\" has no property named \"" <<
                         ( *iter ).first.second << "\". Skipping.";
            continue;
        }
        else
        {
            // set the property here
            prop->setAsString( ( *iter ).second );
        }
    }

    // now add each module to the target container
    for ( std::map< unsigned int, boost::shared_ptr< WModule > >::const_iterator iter = modules.begin(); iter != modules.end(); ++iter )
    {
        m_container->add( ( *iter ).second );
    }

    // and finally, connect them all together
    for ( std::list< Connection >::const_iterator iter = connections.begin(); iter != connections.end(); ++iter )
    {
        // each connection contains two connectors
        Connector c1 = ( *iter ).first;
        Connector c2 = ( *iter ).second;

        // each of these connectors contains the module ID and the connector name
        // grab corresponding module 1
        boost::shared_ptr< WModule > m1;
        if ( !modules.count( c1.first ) )
        {
            wlog::error( "Project Loader" ) << "There is no module with ID \"" << c1.first <<  "\" for the connection "
                                            << "(" << c1.first << "," << c1.second << ")->(" << c2.first << "," << c2.second << "). Skipping.";

            continue;
        }
        m1 = modules[ c1.first ];

        boost::shared_ptr< WModule > m2;
        if ( !modules.count( c2.first ) )
        {
            wlog::error( "Project Loader" ) << "There is no module with ID \"" << c2.first <<  "\" for the connection "
                                            << "(" << c1.first << "," << c1.second << ")->(" << c2.first << "," << c2.second << "). Skipping.";

            continue;
        }
        m2 = modules[ c2.first ];

        // now we have the modules referenced by the ID
        // -> query the connectors
        // NOTE: we assume the first connector to be an output connector!
        boost::shared_ptr< WModuleOutputConnector > con1;
        try
        {
            con1 = m1->getOutputConnector( c1.second );
        }
        catch ( WModuleConnectorNotFound& e )
        {
            wlog::error( "Project Loader" ) << "There is no output connector \"" << c1.second << "\" in module \"" << m1->getName() << "\"";
            continue;
        }
        boost::shared_ptr< WModuleInputConnector > con2;
        try
        {
            con2 = m2->getInputConnector( c2.second );
        }
        catch ( WModuleConnectorNotFound& e )
        {
            wlog::error( "Project Loader" ) << "There is no input connector \"" << c2.second << "\" in module \"" << m2->getName() << "\"";
            continue;
        }

        // finally, connect them
        try
        {
            con1->connect( con2 );
        }
        catch ( WException& e )
        {
            wlog::error( "Project Loader" ) << "Connection " << "(" << c1.first << "," << c1.second << ")->(" << c2.first << "," << c2.second <<
                                                ") could not be created. Incompatible connectors?. Skipping.";
            continue;
        }
    }
}

