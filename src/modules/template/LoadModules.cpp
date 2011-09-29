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

#include "WMTemplate.h"
#include "WMDataHandlerTemplate.h"

#include "LoadModules.h"

// The following function will be the "entry point" for this shared lib.
// Defining this by yourself is only needed if you want to load multiple modules
// from a single shared library. If you only have a single module, you can use the
// W_LOADABLE_MODULE macro, e.g.:
//
// W_LOADABLE_MODULE( WMMyModuleClass )
//
// Just add it on top of your WMMyModuleClass.cpp file.

#ifdef _MSC_VER
extern "C" __declspec(dllexport) void WLoadModule( WModuleList& m ) // NOLINT
#else
extern "C"                       void WLoadModule( WModuleList& m ) // NOLINT
#endif
{
    // Add prototypes for every module to the module list by using the standard constructor
    // do not forget to use a shared_ptr here.
    m.push_back( boost::shared_ptr< WModule >( new WMTemplate ) );
    m.push_back( boost::shared_ptr< WModule >( new WMDataHandlerTemplate ) );
}
