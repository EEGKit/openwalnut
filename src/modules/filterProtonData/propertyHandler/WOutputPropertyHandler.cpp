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

#include "WOutputPropertyHandler.h"

WOutputPropertyHandler::WOutputPropertyHandler( WProtonData::SPtr protonData,
                                                WPropertyGroup::SPtr properties,
                                                WOutputPropertyHandler::CallbackPtr dataUpdate ):
    m_protonData( protonData ),
    m_properties( properties ),
    m_dataUpdate( dataUpdate )
{
}

void WOutputPropertyHandler::createProperties()
{
    WPropGroup outputGroup = m_properties->addPropertyGroup( "Output", "Configures the third output" );
    m_energyDeposition = outputGroup->addProperty( "Energy deposition", "Enables the energy deposition output", true );
    m_eventID = outputGroup->addProperty( "Event id", "Enables the event id output", false );

    updateProperties();
}

void WOutputPropertyHandler::updateProperties()
{
}

WPropBool WOutputPropertyHandler::getEnergyDeposition()
{
    return m_energyDeposition;
}

WPropBool WOutputPropertyHandler::getEventID()
{
    return m_eventID;
}