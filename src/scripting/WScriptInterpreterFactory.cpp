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

#ifdef LUA_FOUND
    #include "lua/WScriptInterpreterLUA.h"
#endif

#ifdef PYTHON_FOUND
    #include "python/WScriptInterpreterPython.h"
#endif

#include "WScriptInterpreterFactory.h"

boost::shared_ptr< WScriptInterpreter > WScriptInterpreterFactory::constructByFileExtension( std::string const& ext )
{
    boost::shared_ptr< WScriptInterpreter > interp;

#ifdef PYTHON_FOUND
    if( ext == ".py" )
    {
        interp = boost::shared_ptr< WScriptInterpreter >( new WScriptInterpreterPython() );
    }
#endif
#ifdef LUA_FOUND
    if( ext == ".lua" )
    {
        interp = boost::shared_ptr< WScriptInterpreter >( new WScriptInterpreterLUA() );
    }
#endif
    return interp;
}

boost::shared_ptr< WScriptInterpreter > WScriptInterpreterFactory::constructByName( std::string const& name )
{
    boost::shared_ptr< WScriptInterpreter > interp;

#ifdef PYTHON_FOUND
    if( name == "python" )
    {
        interp = boost::shared_ptr< WScriptInterpreter >( new WScriptInterpreterPython() );
    }
#endif
#ifdef LUA_FOUND
    if( name == "lua" )
    {
        interp = boost::shared_ptr< WScriptInterpreter >( new WScriptInterpreterLUA() );
    }
#endif
    return interp;
}

std::string WScriptInterpreterFactory::getSupportedInterpreterList()
{
    std::string s;
#ifdef PYTHON_FOUND
    s += "\tpython \t(.py)\n";
#endif
#ifdef LUA_FOUND
    s += "\tlua \t(.lua)\n";
#endif
    return s;
}