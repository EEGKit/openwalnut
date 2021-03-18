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

#include <list>
#include <string>

#include "WDatatype.h"
#include "WSingleSelectorName.h"

std::string WSingleSelectorName::getX()
{
    return "X";
}

std::string WSingleSelectorName::getY()
{
    return "Y";
}

std::string WSingleSelectorName::getZ()
{
    return "Z";
}

std::string WSingleSelectorName::getPDG()
{
    return "Particle Data Group";
}

std::string WSingleSelectorName::getEdep()
{
    return "Energy deposition";
}

std::string WSingleSelectorName::getEventId()
{
    return "Event id";
}

std::string WSingleSelectorName::getParentId()
{
    return "Parent id";
}

WSingleSelectorName::NameDescriptionSearchTyp WSingleSelectorName::getXwithDescription()
{
    return WSingleSelectorName::NameDescriptionSearchTyp(
                        getX(),
                        "Choose the column which should be used to determine the x coordinate.",
                        "posX",
                        WDatatype::getDouble() );
}

WSingleSelectorName::NameDescriptionSearchTyp WSingleSelectorName::getYwithDescription()
{
    return WSingleSelectorName::NameDescriptionSearchTyp(
                        getY(),
                        "Choose the column which should be used to determine the y coordinate.",
                        "posY",
                        WDatatype::getDouble() );
}

WSingleSelectorName::NameDescriptionSearchTyp WSingleSelectorName::getZwithDescription()
{
    return WSingleSelectorName::NameDescriptionSearchTyp(
                        getZ(),
                        "Choose the column which should be used to determine the z coordinate.",
                        "posZ",
                        WDatatype::getDefault() );
}

WSingleSelectorName::NameDescriptionSearchTyp WSingleSelectorName::getPDGwithDescription()
{
    return WSingleSelectorName::NameDescriptionSearchTyp(
                        getPDG(),
                        "Choose the column which should be used to determine the particle data group.",
                        "PDGEncoding",
                        WDatatype::getInt() );
}

WSingleSelectorName::NameDescriptionSearchTyp WSingleSelectorName::getEdepWithDescription()
{
    return WSingleSelectorName::NameDescriptionSearchTyp(
                        getEdep(),
                        "Choose the column which should be used to determine the energy deposition.",
                        "edep",
                        WDatatype::getDouble() );
}

WSingleSelectorName::NameDescriptionSearchTyp WSingleSelectorName::getEventIdWithDescription()
{
    return WSingleSelectorName::NameDescriptionSearchTyp(
                        getEventId(),
                        "Choose the column which should be used to determine the event id."
                        "Tracks will be drawn based on the the event id, all particles with the same event id will be connected.",
                        "eventID",
                        WDatatype::getInt() );
}

WSingleSelectorName::NameDescriptionSearchTyp WSingleSelectorName::getParentIdWithDescription()
{
    return WSingleSelectorName::NameDescriptionSearchTyp(
                        getParentId(),
                        "Choose the column which should be used to determine the parent id."
                        "Primaries and secondaries filtering is based on that id, if a "
                        "particle has the parent id 0 it is a primary otherwise it is a secondary.",
                        "parentID",
                        WDatatype::getInt() );
}

std::list< WSingleSelectorName::NameDescriptionSearchTyp > WSingleSelectorName::getListOfSelectorContent()
{
    std::list< WSingleSelectorName::NameDescriptionSearchTyp > list;

    list.push_back( getXwithDescription() );
    list.push_back( getYwithDescription() );
    list.push_back( getZwithDescription() );
    list.push_back( getPDGwithDescription() );
    list.push_back( getEdepWithDescription() );
    list.push_back( getEventIdWithDescription() );
    list.push_back( getParentIdWithDescription() );

    return list;
}
