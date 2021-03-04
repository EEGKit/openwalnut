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
#include <map>

#include "WMColumnPropertyHandler.h"

WMColumnPropertyHandler::WMColumnPropertyHandler( WMProtonData::SPtr protonData,
                                            WPropertyGroup::SPtr properties,
                                            WMColumnPropertyHandler::CallbackPtr dataUpdate ):
    m_protonData( protonData ),
    m_properties( properties ),
    m_dataUpdate( dataUpdate )
{
    mapPropSelectionsToString = {};
}

void WMColumnPropertyHandler::createProperties()
{
    WPropertyBase::PropertyChangeNotifierType notifier = boost::bind( &WMColumnPropertyHandler::propertyNotifier,
                                                                this, boost::placeholders::_1 );

    InitializeSelectionItem();

    m_columnSelectionGroup = m_properties->addPropertyGroup( "Select columns", "Select the columns which should be used" );

    for( std::string colName : vecDefaultColumnNames )
    {
        mapPropSelectionsToString.insert(
            std::map< WPropSelection, std::string >::value_type( addHeaderProperty( colName, notifier ), colName )
        );
    }
}

void WMColumnPropertyHandler::setSelectionEventMethod( WMColumnPropertyHandler::CallbackPtr externEventMethod )
{
    m_externEventMethod = externEventMethod;
}

void WMColumnPropertyHandler::updateProperty()
{
}

void WMColumnPropertyHandler::InitializeSelectionItem()
{
    m_possibleSelectionsUsingTypes = WItemSelection::SPtr( new WItemSelection() );

    std::vector< std::string > header = m_protonData->getCSVHeader()->at( 0 );
    for( std::vector<std::string>::iterator colName = header.begin(); colName != header.end(); colName++ )
    {
        m_possibleSelectionsUsingTypes->addItem( ItemType::create( *colName, *colName, "",  NULL ) );
    }

    m_possibleSelectionsUsingTypes->addItem( ItemType::create( "- no selection -", "- no selection -", "",  NULL ) );
}

WPropSelection WMColumnPropertyHandler::addHeaderProperty( std::string columnName, WPropertyBase::PropertyChangeNotifierType notifier )
{
    int index = m_protonData->getColumnIndex( columnName );
    WItemSelector selector = index < 0 ? m_possibleSelectionsUsingTypes->getSelectorLast() : m_possibleSelectionsUsingTypes->getSelector( index );
    WPropSelection selection = m_columnSelectionGroup->addProperty(
                                                columnName,
                                                "Choose the " + columnName + " column from csv",
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
    const WItemSelector* selector = NULL;
    std::string columnName;

    for( PropMapEntry elem = mapPropSelectionsToString.begin();
         elem != mapPropSelectionsToString.end();
         elem++ )
    {
        if( property == elem->first )
        {
            selector = &elem->first->get( true );
            columnName = elem->second;
            break;
        }
    }

    if( selector != NULL)
    {
        std::string selectedValue = selector->at( 0 )->getAs< ItemType >()->getValue();

        m_protonData->setColumnIndex( columnName,
                                      getColumnNumberByName( selectedValue, m_protonData->getCSVHeader()->at( 0 ) )
                                     );

        if(m_externEventMethod != NULL)
        {
            m_externEventMethod();
        }
    }
}

boost::shared_ptr< WItemSelection > WMColumnPropertyHandler::getColumnItems() 
{
    return m_possibleSelectionsUsingTypes;
}