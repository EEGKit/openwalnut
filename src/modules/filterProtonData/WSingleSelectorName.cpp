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

std::string WSingleSelectorName::getEDEP()
{
    return "Energy deposition";
}

std::string WSingleSelectorName::getEVENTID()
{
    return "Event id";
}

std::string WSingleSelectorName::getPARENTID()
{
    return "Parent id";
}

WSingleSelectorName::NameDescriptionSearch WSingleSelectorName::getXwithDescription()
{
    return WSingleSelectorName::NameDescriptionSearch( getX(),
                        "Choose the column which should be used to determine the x coordinate.",
                        "posX" );
}

WSingleSelectorName::NameDescriptionSearch WSingleSelectorName::getYwithDescription()
{
    return WSingleSelectorName::NameDescriptionSearch( getY(),
                        "Choose the column which should be used to determine the y coordinate.",
                        "posY" );
}

WSingleSelectorName::NameDescriptionSearch WSingleSelectorName::getZwithDescription()
{
    return WSingleSelectorName::NameDescriptionSearch( getZ(),
                        "Choose the column which should be used to determine the z coordinate.",
                        "posZ" );
}

WSingleSelectorName::NameDescriptionSearch WSingleSelectorName::getPDGwithDescription()
{
    return WSingleSelectorName::NameDescriptionSearch( getPDG(),
                        "Choose the column which should be used to determine the particle data group.",
                        "PDGEncoding" );
}

WSingleSelectorName::NameDescriptionSearch WSingleSelectorName::getEDEPwithDescription()
{
    return WSingleSelectorName::NameDescriptionSearch( getEDEP(),
                        "Choose the column which should be used to determine the energy deposition.",
                        "edep" );
}

WSingleSelectorName::NameDescriptionSearch WSingleSelectorName::getEVENTIDwithDescription()
{
    return WSingleSelectorName::NameDescriptionSearch( getEVENTID(),
                        "Choose the column which should be used to determine the event id."
                        "Tracks will be drawn based on the the event id, all particles with the same event id will be connected.",
                        "eventID" );
}

WSingleSelectorName::NameDescriptionSearch WSingleSelectorName::getPARENTIDwithDescription()
{
    return WSingleSelectorName::NameDescriptionSearch( getPARENTID(),
                        "Choose the column which should be used to determine the parent id."
                        "Primaries and secondaries filtering is based on that id, if a "
                        "particle has the parent id 0 it is a primary otherwise it is a secondary.",
                        "parentID" );
}

std::list< WSingleSelectorName::NameDescriptionSearch > WSingleSelectorName::getListOfSelectorContent()
{
    std::list< WSingleSelectorName::NameDescriptionSearch > list;

    list.push_back( getXwithDescription() );
    list.push_back( getYwithDescription() );
    list.push_back( getZwithDescription() );
    list.push_back( getPDGwithDescription() );
    list.push_back( getEDEPwithDescription() );
    list.push_back( getEVENTIDwithDescription() );
    list.push_back( getPARENTIDwithDescription() );

    return list;
}
