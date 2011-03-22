//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

#include "../../kernel/WKernel.h"
#include "../WMProbTractVis.xpm" // Please put a real icon here.

#include "WMProbTractVis.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMProbTractVis )

WMProbTractVis::WMProbTractVis():
    WModule()
{
}

WMProbTractVis::~WMProbTractVis()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMProbTractVis::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMProbTractVis() );
}

const char** WMProbTractVis::getXPMIcon() const
{
    return probTractVis_xpm; // Please put a real icon here.
}
const std::string WMProbTractVis::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "ProbTractVis";
}

const std::string WMProbTractVis::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    // See "src/modules/template/" for an extensively documented example.
    return "Someone should add some documentation here. "
    "Probably the best person would be the modules's creator, i.e. \"berres\"";
}

void WMProbTractVis::connectors()
{
    // Put the code for your connectors here. See "src/modules/template/" for an extensively documented example.

    WModule::connectors();
}

void WMProbTractVis::properties()
{
    // Put the code for your properties here. See "src/modules/template/" for an extensively documented example.

    WModule::properties();
}

void WMProbTractVis::requirements()
{
    // Put the code for your requirements here. See "src/modules/template/" for an extensively documented example.
}

void WMProbTractVis::moduleMain()
{
    // Put the code for your module's main functionality here.
    // See "src/modules/template/" for an extensively documented example.
}
