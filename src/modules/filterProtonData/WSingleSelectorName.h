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

#ifndef WSINGLESELECTORNAME_H
#define WSINGLESELECTORNAME_H

#include <tuple>
#include <list>
#include <string>

#include "WDataType.h"


/**
 * Holds the single selector content.
 */
class WSingleSelectorName
{
public:
    /**
     * tuple with 3 string as value
     * 1. Name of single-selector,
     * 2. Desciption of single-selector,
     * 3. Value that is searched for in the csv header
     */
    typedef std::tuple< std::string, std::string, std::string, std::list < std::string > > NameDescriptionSearchTyp;

    /**
     * getter
     * \return the name of the single-selector (X)
     */
    static std::string getX();

    /**
     * getter
     * \return the name of the single-selector (Y)
     */
    static std::string getY();

    /**
     * getter
     * \return the name of the single-selector (Z)
     */
    static std::string getZ();

    /**
     * getter
     * \return the name of the single-selector (pdg)
     */
    static std::string getPDG();

    /**
     * getter
     * \return the name of the single-selector (edep)
     */
    static std::string getEdep();

    /**
     * getter
     * \return the name of the single-selector (Event id)
     */
    static std::string getEventId();

    /**
     * getter
     * \return the name of the single-selector (Parent id)
     */
    static std::string getParentId();

    /**
     * getter
     * \return the name of the single-selector (X) with Description
     */
    static WSingleSelectorName::NameDescriptionSearchTyp getXwithDescription();

    /**
     * getter
     * \return the name of the single-selector (Y) with Description
     */
    static WSingleSelectorName::NameDescriptionSearchTyp getYwithDescription();

    /**
     * getter
     * \return the name of the single-selector (Z) with Description
     */
    static WSingleSelectorName::NameDescriptionSearchTyp getZwithDescription();

    /**
     * getter
     * \return the name of the single-selector (pdg) with Description
     */
    static WSingleSelectorName::NameDescriptionSearchTyp getPDGwithDescription();

    /**
     * getter
     * \return the name of the single-selector (edep) with Description
     */
    static WSingleSelectorName::NameDescriptionSearchTyp getEdepWithDescription();

    /**
     * getter
     * \return the name of the single-selector (Event id) with Description
     */
    static WSingleSelectorName::NameDescriptionSearchTyp getEventIdWithDescription();

    /**
     * getter
     * \return the name of the single-selector (Parent id) with Description
     */
    static WSingleSelectorName::NameDescriptionSearchTyp getParentIdWithDescription();

    /**
     * getter
     * \return Vector of names of the single-selectors with Description and Value that is searched for in the csv header
     */
    static std::list< WSingleSelectorName::NameDescriptionSearchTyp > getListOfSelectorContent();
};

#endif  // WSINGLESELECTORNAME_H
