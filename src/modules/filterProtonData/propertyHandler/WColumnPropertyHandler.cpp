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
#include <map>
#include <string>
#include <vector>

#include "WColumnPropertyHandler.h"

WColumnPropertyHandler::WColumnPropertyHandler( WProtonData::SPtr protonData,
                                            WPropertyGroup::SPtr properties,
                                            WColumnPropertyHandler::CallbackPtr dataUpdate ):
    m_protonData( protonData ),
    m_properties( properties ),
    m_dataUpdate( dataUpdate )
{
    mapPropSelectionsToString = {};
}

void WColumnPropertyHandler::createProperties()
{
    WPropertyBase::PropertyChangeNotifierType notifier = boost::bind( &WColumnPropertyHandler::propertyNotifier,
                                                                this, boost::placeholders::_1 );

    InitializeSelectionItem();

    m_columnSelectionGroup = m_properties->addPropertyGroup( "Select columns", "Select the columns which should be used" );

    std::list< std::tuple< std::string, std::string, std::string > > names = WSingleSelectorName::getListOfSelectorContent();
    for( std::tuple< std::string, std::string, std::string > selectorElement : names )
    {
        std::string columnName = std::get< 0 >( selectorElement );
        std::string desciption = std::get< 1 >( selectorElement );
        std::string defName = std::get< 2 >( selectorElement );

        mapPropSelectionsToString.insert(
            std::map< WPropSelection, std::string >::value_type( addHeaderProperty( columnName, desciption, defName, notifier ), columnName )
        );
    }
}

void WColumnPropertyHandler::setSelectionEventMethod( WColumnPropertyHandler::CallbackPtr externEventMethod )
{
    m_externEventMethod = externEventMethod;
}

void WColumnPropertyHandler::updateProperty()
{
}

void WColumnPropertyHandler::InitializeSelectionItem()
{
    m_possibleSelectionsUsingTypes = WItemSelection::SPtr( new WItemSelection() );

    std::vector< std::string > header = m_protonData->getCSVHeader()->at( 0 );
    for( std::vector<std::string>::iterator colName = header.begin(); colName != header.end(); colName++ )
    {
        m_possibleSelectionsUsingTypes->addItem( ItemType::create( *colName, *colName, "",  NULL ) );
    }

    m_possibleSelectionsUsingTypes->addItem( ItemType::create( "- no selection -", "- no selection -", "",  NULL ) );
}

WPropSelection WColumnPropertyHandler::addHeaderProperty( std::string columnName, std::string description, std::string defName,
                                                        WPropertyBase::PropertyChangeNotifierType notifier )
{
    int index = m_protonData->getColumnIndex( defName );

    m_protonData->setStateIndex( columnName, index );

    WItemSelector selector = index < 0 ? m_possibleSelectionsUsingTypes->getSelectorLast() : m_possibleSelectionsUsingTypes->getSelector( index );
    WPropSelection selection = m_columnSelectionGroup->addProperty(
                                                columnName,
                                                description,
                                                selector,
                                                notifier );

    WPropertyHelper::PC_SELECTONLYONE::addTo( selection );
    WPropertyHelper::PC_NOTEMPTY::addTo( selection );

    return selection;
}

int WColumnPropertyHandler::getColumnNumberByName( std::string columnNameToMatch )
{
    std::vector< std::string > headerToSearchIn = m_protonData->getCSVHeader()->at( 0 );

    int pos = 0;
    for( std::vector< std::string >::iterator it = headerToSearchIn.begin(); it != headerToSearchIn.end(); it++ )
    {
        if( *it == columnNameToMatch ) return pos;
        pos++;
    }
    return -1;
}

void WColumnPropertyHandler::propertyNotifier( WPropertyBase::SPtr property )
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

        m_protonData->setStateIndex( columnName,
                                      getColumnNumberByName( selectedValue )
                                     );

        m_dataUpdate();
        if(m_externEventMethod != NULL)
        {
            m_externEventMethod();
        }
    }
}

boost::shared_ptr< WItemSelection > WColumnPropertyHandler::getColumnItems()
{
    return m_possibleSelectionsUsingTypes;
}
