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

#include <string>
#include <vector>

#include "WMColumnPropertyHandler.h"

WMColumnPropertyHandler::WMColumnPropertyHandler( WMProtonData::SPtr protonData,
                                            WPropertyGroup::SPtr properties,
                                            WMColumnPropertyHandler::CallbackPtr dataUpdate ):
    m_protonData( protonData ),
    m_properties( properties ),
    m_dataUpdate( dataUpdate )
{
}

void WMColumnPropertyHandler::createProperties()
{
    WPropertyBase::PropertyChangeNotifierType notifier = boost::bind( &WMColumnPropertyHandler::propertyNotifier,
                                                                this, boost::placeholders::_1 );

    InitializeSelectionItem();

    m_columnSelectionGroup = m_properties->addPropertyGroup( "Select columns", "Select the columns which should be used" );

    m_singleSelectionForPosX = addHeaderProperty( "posX", notifier );
    m_singleSelectionForPosY = addHeaderProperty( "posY", notifier );
    m_singleSelectionForPosZ = addHeaderProperty( "posZ", notifier );
    m_singleSelectionForEdep = addHeaderProperty( "edep", notifier );
    m_singleSelectionForEventID = addHeaderProperty( "eventID", notifier );
    m_singleSelectionForTrackID = addHeaderProperty( "trackID", notifier );
    m_singleSelectionForParentID = addHeaderProperty( "parentID", notifier );
}

void WMColumnPropertyHandler::InitializeSelectionItem()
{
    m_possibleSelectionsUsingTypes = WItemSelection::SPtr( new WItemSelection() );

    std::vector< std::string > header = m_protonData->getCSVHeader()->at( 0 );
    for( std::vector<std::string>::iterator colName = header.begin(); colName != header.end(); colName++ )
    {
        m_possibleSelectionsUsingTypes->addItem( ItemType::create( *colName, *colName, "",  NULL ) );
    }
}

WPropSelection WMColumnPropertyHandler::addHeaderProperty( std::string headerName, WPropertyBase::PropertyChangeNotifierType notifier )
{
    int index = m_protonData->getColumnIndex( headerName );
    WItemSelector selector = index < 0 ? m_possibleSelectionsUsingTypes->getSelectorNone() : m_possibleSelectionsUsingTypes->getSelector( index );
    WPropSelection selection = m_columnSelectionGroup->addProperty(
                                                headerName,
                                                "Choose the " + headerName + " column from csv",
                                                selector,
                                                notifier );

    WPropertyHelper::PC_SELECTONLYONE::addTo( selection );
    WPropertyHelper::PC_NOTEMPTY::addTo( selection );

    return selection;
}

int WMColumnPropertyHandler::getColumnNumberByName( std::string columnNameToMatch, std::vector< std::string > headerToSearchIn )
{
    int pos = 0;
    for( std::vector< std::string >::iterator it = headerToSearchIn.begin(); it != headerToSearchIn.end(); it++ )
    {
        if( *it == columnNameToMatch ) return pos;
        pos++;
    }
    return -1;
}

void WMColumnPropertyHandler::propertyNotifier( WPropertyBase::SPtr property )
{
    const WItemSelector* selector;
    std::string columnName;

    if( property == m_singleSelectionForPosX )
    {
        selector = &m_singleSelectionForPosX->get( true );
        columnName = "posX";
    }
    else if( property == m_singleSelectionForPosY )
    {
        selector = &m_singleSelectionForPosY->get( true );
        columnName = "posY";
    }
    else if( property == m_singleSelectionForPosZ )
    {
        selector = &m_singleSelectionForPosZ->get( true );
        columnName = "posZ";
    }
    else if( property == m_singleSelectionForEdep )
    {
        selector = &m_singleSelectionForEdep->get( true );
        columnName = "edep";
    }
    else if( property == m_singleSelectionForEventID )
    {
        selector = &m_singleSelectionForEventID->get( true );
        columnName = "eventID";
    }
    else if( property == m_singleSelectionForTrackID )
    {
        selector = &m_singleSelectionForTrackID->get( true );
        columnName = "trackID";
    }
    else if( property == m_singleSelectionForParentID )
    {
        selector = &m_singleSelectionForParentID->get( true );
        columnName = "parentID";
    }

    if( selector != NULL )
    {
        std::string selectedValue = selector->at( 0 )->getAs< ItemType >()->getValue();

        
        m_protonData->setColumnIndex( columnName, getColumnNumberByName( selectedValue, m_protonData->getCSVHeader()->at( 0 ) ) );

        m_dataUpdate( );
    }
}



