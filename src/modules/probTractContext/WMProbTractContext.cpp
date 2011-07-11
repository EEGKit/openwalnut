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

#include "WMProbTractContext.h"
#include "WMProbTractContext.xpm"

#include "../../core/kernel/WKernel.h"
#include "../../core/kernel/WModuleFactory.h"
#include "../../core/kernel/WPrototypeRequirement.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMProbTractContext )

WMProbTractContext::WMProbTractContext():
    WModuleContainer()
{
}

WMProbTractContext::~WMProbTractContext()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMProbTractContext::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMProbTractContext() );
}

const char** WMProbTractContext::getXPMIcon() const
{
    return probtractcontext_xpm;
}
const std::string WMProbTractContext::getName() const
{
    return "ProbTractContext";
}

const std::string WMProbTractContext::getDescription() const
{
    return "Provide context for probablistic tractograms.";
}

void WMProbTractContext::connectors()
{
    // TODO(aberres): document
//    m_input = WModuleInputData < WDataSetScalar >::createAndAdd( shared_from_this(), "in",
//                                                "The scalar dataset." );

//    m_gradients = WModuleInputData< WDataSetVector >::createAndAdd( shared_from_this(), "gradients", "The gradient field of the dataset to display" );

    WModule::connectors();
}

void WMProbTractContext::properties()
{
    // Put the code for your properties here. See "src/modules/template/" for an extensively documented example.

    WModule::properties();
}

void WMProbTractContext::requirements()
{
    // Put the code for your requirements here. See "src/modules/template/" for an extensively documented example.
}

void WMProbTractContext::moduleMain()
{
    // create an instance using the prototypes
//    m_ptvModule = WModuleFactory::getModuleFactory()->create( WModuleFactory::getModuleFactory()->getPrototypeByName( "ProbTractVis" ) );

    // if probtractvis properties are needed
//    boost::shared_ptr< WProperties > ptvProp = m_ptvModule->getProperties();

//    boost::shared_ptr< WModuleInputData< WDataSetVector > > m_gradients; //!< The gradient field input

    // get NavSlice positions
//    m_axial = WKernel::getRunnungKernel()->getSelectionManager()->getPropAxialPos();

}
