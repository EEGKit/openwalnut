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

#ifndef WMCOLUMNPROPERTYHANDLER_H
#define WMCOLUMNPROPERTYHANDLER_H

#include <string>
#include <vector>

#include <boost/function.hpp>

#include "core/common/WItemSelectionItemTyped.h"

#include "WMProtonData.h"

class WMColumnPropertyHandler
{
public:
    typedef boost::function< void( WDataSetCSV::ContentSPtr, WDataSetCSV::ContentSPtr ) > CallbackPtr;

    typedef boost::shared_ptr< WMColumnPropertyHandler > SPtr;

    explicit WMColumnPropertyHandler( WMProtonData::SPtr protonData,
                                WPropertyGroup::SPtr properties,
                                WMColumnPropertyHandler::CallbackPtr dataUpdate );

    void createProperties();

    void updateProperty();

    /**
     * Collect all particle types from your input data.
     */
    void searchPDGTypes();

    bool isPDGTypeSelected( int pdgType );

private:
    WPropSelection addHeaderProperty( std::string headerName, WPropertyBase::PropertyChangeNotifierType notifier );

    void propertyNotifier( WPropertyBase::SPtr property );

    void updateSelectedPDGTypes( WPropertyBase::SPtr property );

    /**
     * Get column number by name from header
     *
     * \param columnNameToMatch Search for the specified column name.
     * \param headerToSearchIn Search in specified header.
     * \return Column number, where columnToMatch is in headerToSearchIn
     */
    int getColumnNumberByName( std::string columnNameToMatch, std::vector<std::string> headerToSearchIn );

    WMProtonData::SPtr m_protonData;

    WPropertyGroup::SPtr m_properties;

    WMColumnPropertyHandler::CallbackPtr m_dataUpdate;

    /**
     * Property group for column selection
     */
    WPropGroup m_columnSelectionGroup;

    /**
     * A list of items that can be selected using m_aSingleSelectionUsingTypes property.
     */
    boost::shared_ptr< WItemSelection > m_possibleSelectionsUsingTypes;

    /**
     * A property allowing the user to select ONE item. This additionally demonstrates how to use your own types/classes in selections.
     */
    WPropSelection m_singleSelectionForPosX;

    /**
     * A property allowing the user to select ONE item. This additionally demonstrates how to use your own types/classes in selections.
     */
    WPropSelection m_singleSelectionForPosY;

    /**
     * A property allowing the user to select ONE item. This additionally demonstrates how to use your own types/classes in selections.
     */
    WPropSelection m_singleSelectionForPosZ;

    /**
     * A property allowing the user to select ONE item. This additionally demonstrates how to use your own types/classes in selections.
     */
    WPropSelection m_singleSelectionForEdep;

    /**
     * A property allowing the user to select ONE item. This additionally demonstrates how to use your own types/classes in selections.
     */
    WPropSelection m_singleSelectionForEventID;

    /**
     * A property allowing the user to select ONE item. This additionally demonstrates how to use your own types/classes in selections.
     */
    WPropSelection m_singleSelectionForTrackID;

    /**
     * A property allowing the user to select ONE item. This additionally demonstrates how to use your own types/classes in selections.
     */
    WPropSelection m_singleSelectionForParentID;

    /**
     * Stores every unique particle id.
     */
    std::vector < int > m_pdgTypes;

    /**
     * Stores selectable items.
     */
    boost::shared_ptr< WItemSelection > m_possibleSelection;

    /**
     * Stores users selected items.
     */
    WPropSelection m_multiSelection;

    /**
     * 
     */
    std::vector < std::string > m_selectedPDGTypes;

     /**
     * represents the item type for item-selection
     */
    typedef WItemSelectionItemTyped< std::string > ItemType;
};

#endif  // WMCOLUMNPROPERTYHANDLER_H
