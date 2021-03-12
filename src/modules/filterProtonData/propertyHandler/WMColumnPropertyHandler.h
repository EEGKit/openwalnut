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
#include <map>

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

    /**
     * \return boost::shared_ptr< WItemSelection > Reference of selectable column types
     */
    boost::shared_ptr< WItemSelection > getColumnItems();

    /**
     * update current group property and subproperty
     */
    void updateProperty();

    /**
     * setter to use a external function
     * \param externEventMethod A void function variable that can be use outside
     */
    void setSelectionEventMethod( WMColumnPropertyHandler::CallbackPtr externEventMethod );

private:
    /**
     * A vector, where all default ColumnPropertyNames are stored in
     */
    const std::vector< std::string > vecDefaultColumnNames = { "Particle Data Group", "X", "Y", "Z", "Energy deposition", "Event id", "Track id",
                                                              "Parent id" };
    
    /**
     * A vector, where all the default names are stored in
     */
    const std::vector< std::string > vecDefaultNames = { "PDGEncoding", "posX", "posY", "posZ", "edep", "eventID", "trackID",
                                                        "parentID" };

    /**
     * A vector where all default column desciptions are stored in
     */
    const std::vector< std::string > vecDefaultDescription = {
                                            "Choose the column which should be used to determine the particle data group.",
                                            "Choose the column which should be used to determine the x coordinate.",
                                            "Choose the column which should be used to determine the y coordinate.",
                                            "Choose the column which should be used to determine the z coordinate.",
                                            "Choose the column which should be used to determine the energy deposition.",
                                            "Choose the column which should be used to determine the event id."
                                            "Tracks will be drawn based on the the event id, all particles with the same event id will be connected.",
                                            "Choose the column which should be used to determine the track id.",
                                            "Choose the column which should be used to determine the parent id."
                                            "Primaries and secondaries filtering is based on that id, if a "
                                            "particle has the parent id 0 it is a primary otherwise it is a secondary."};

    /**
     * A map between WPropSelection items and column names as strings
     */
     std::map< WPropSelection, std::string > mapPropSelectionsToString;

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
     * A void function variable that can be use outside
     */
    WMColumnPropertyHandler::CallbackPtr m_externEventMethod;

    /**
     * Creates the individual WItemSelection
     * \param headerName The name of the selection
     * \param description The description of the selection
     * \param notifier The notifier for the selection
     * \param defName The default name of the selection
     * \return WPropSelection The created selection
     */
    WPropSelection addHeaderProperty( std::string headerName, std::string description, std::string defName,
                                    WPropertyBase::PropertyChangeNotifierType notifier );

    /**
     * Event function when WItemSelection is triggered
     * \param property The changed property
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
     * represents the item type for item-selection
     */
    typedef WItemSelectionItemTyped< std::string > ItemType;

    /**
     * Reresents an entry in a < WPropSelection, string > map
     */
     typedef std::map< WPropSelection, std::string >::iterator PropMapEntry;

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
