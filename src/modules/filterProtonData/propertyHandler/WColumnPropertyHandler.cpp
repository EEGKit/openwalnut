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

    m_columnSelectionGroup = m_properties->addPropertyGroup( "Select columns", "Select the columns which should be used" );

    std::list< WColumnPropertyHandler::NameDescriptionSearchTyp > names = WSingleSelectorName::getListOfSelectorContent();
    for( WColumnPropertyHandler::NameDescriptionSearchTyp selectorElement : names )
    {
        std::string columnName = std::get< 0 >( selectorElement );

        mapPropSelectionsToString.insert(
            std::map< WPropSelection, std::string >::value_type( addHeaderProperty( selectorElement, notifier ), columnName )
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

boost::shared_ptr< WItemSelection > WColumnPropertyHandler::initializeSelectionItem( std::list< std::string > typeNames )
{
    boost::shared_ptr< WItemSelection > possibleSelectionsUsingTypes = WItemSelection::SPtr( new WItemSelection() );

    std::vector< std::string > header = m_protonData->getHeaderFromType( typeNames );

    for( std::vector<std::string>::iterator colName = header.begin(); colName != header.end(); colName++ )
    {
        possibleSelectionsUsingTypes->addItem( ItemType::create( *colName, *colName, "",  NULL ) );
    }

    possibleSelectionsUsingTypes->addItem( ItemType::create( "- no selection -", "- no selection -", "",  NULL ) );

    return possibleSelectionsUsingTypes;
}

int WColumnPropertyHandler::getFilterIndex( int index, std::list< std::string > typeName )
{
    std::vector< std::string > headerToSearch = m_protonData->getCSVHeader()->at( 0 );

    std::string refheader = headerToSearch.at( index );

    std::vector< std::string > singleSelectorContent = m_protonData->getHeaderFromType( typeName );

    size_t indexCounter = 0;

    for( std::vector<std::string>::iterator colName = singleSelectorContent.begin(); colName != singleSelectorContent.end(); colName++ )
    {
        if( *colName == refheader )
        {
            return indexCounter;
        }
        indexCounter++;
    }

    return -1;
}

WPropSelection WColumnPropertyHandler::addHeaderProperty( WColumnPropertyHandler::NameDescriptionSearchTyp ndst,
                                                        WPropertyBase::PropertyChangeNotifierType notifier )
{
    std::string columnName = std::get< 0 >( ndst );
    std::string description = std::get< 1 >( ndst );
    std::string defName = std::get< 2 >( ndst );
    std::list< std::string > type = std::get< 3 >( ndst );

    int index = m_protonData->getColumnIndex( defName );

    int indexSingleSelector = index < 0 ? -1 : getFilterIndex( index, type );

    m_protonData->setStateIndex( columnName, index );

    boost::shared_ptr< WItemSelection > possibleSelectionsUsingTypes = initializeSelectionItem( type );

    WItemSelector selector = index < 0 ? possibleSelectionsUsingTypes->getSelectorLast() :
                                         possibleSelectionsUsingTypes->getSelector( indexSingleSelector );

    WPropSelection selection = m_columnSelectionGroup->addProperty(
                                                columnName,
                                                description,
                                                selector,
                                                notifier );

    WPropertyHelper::PC_SELECTONLYONE::addTo( selection );
    WPropertyHelper::PC_NOTEMPTY::addTo( selection );

    return selection;
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

        m_protonData->setStateIndex( columnName, m_protonData->getColumnIndex( selectedValue ) );

        m_dataUpdate();
        if(m_externEventMethod != NULL)
        {
            m_externEventMethod();
        }
    }
}
