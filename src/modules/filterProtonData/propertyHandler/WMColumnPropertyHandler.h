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

#include "../WMProtonData.h"

/**
 * Creates, updates and handles the column properties.
 */
class WMColumnPropertyHandler
{
public:
    /**
     * Function variables for updating the data 
     */
    typedef boost::function< void( ) > CallbackPtr;

    /**
     * shared_ptr that points to itself 
     */
    typedef boost::shared_ptr< WMColumnPropertyHandler > SPtr;

    /**
     * constructor
     *
     * \param protonData Pointer to the content and header of the CSV 
     * \param properties A property variable that is generated by the WModul 
     * \param dataUpdate A function variable that reinitializes the WDataSets 
     */
    explicit WMColumnPropertyHandler( WMProtonData::SPtr protonData,
                                WPropertyGroup::SPtr properties,
                                WMColumnPropertyHandler::CallbackPtr dataUpdate );

    /**
     * creates the group property and the subproperty    
     */
    void createProperties();

private:
    /**
     * Pointer to the content and header of the CSV 
     */
    WMProtonData::SPtr m_protonData;

    /**
     * A property variable that is generated by the WModul 
     */
    WPropertyGroup::SPtr m_properties;

    /**
     * A function variable that reinitializes the WDataSets 
     */
    WMColumnPropertyHandler::CallbackPtr m_dataUpdate;

    /**
     * Creates the individual WItemSelection   
     */
    WPropSelection addHeaderProperty( std::string headerName, WPropertyBase::PropertyChangeNotifierType notifier );

    /**
     * Event function when WItemSelection is triggered  
     */
    void propertyNotifier( WPropertyBase::SPtr property );

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
     * represents the item type for item-selection
     */
    typedef WItemSelectionItemTyped< std::string > ItemType;

    /**
     * Get column number by name from header
     *
     * \param columnNameToMatch Search for the specified column name.
     * \param headerToSearchIn Search in specified header.
     * \return Column number, where columnToMatch is in headerToSearchIn
     */
    int getColumnNumberByName( std::string columnNameToMatch, std::vector<std::string> headerToSearchIn );

    /**
     * creates the content (options) of the WItemSelection 
     */
    void InitializeSelectionItem();
};

#endif  // WMCOLUMNPROPERTYHANDLER_H
