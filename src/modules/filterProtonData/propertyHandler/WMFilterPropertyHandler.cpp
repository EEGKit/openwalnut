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

#include "WMFilterPropertyHandler.h"

WMFilterPropertyHandler::WMFilterPropertyHandler( WMProtonData::SPtr protonData,
                                            WPropertyGroup::SPtr properties,
                                            WMFilterPropertyHandler::CallbackPtr dataUpdate ):
    m_protonData( protonData ),
    m_properties( properties ),
    m_dataUpdate( dataUpdate )
{
}

void WMFilterPropertyHandler::createProperties()
{
    m_filteringGroup = m_properties->addPropertyGroup( "Filtering", "Filter primaries and secondaries" );

    createCheckBoxForPrimaryAndSecondary();
    createMultiSelectionForPDG();

    updateProperty();
}


void WMFilterPropertyHandler::updateProperty()
{
    if(m_protonData->IsColumnAvailable("parentID") && m_protonData->IsColumnAvailable("trackID"))
    {
        m_showPrimaries->setHidden( false );
        m_showSecondaries->setHidden( false );
    }
    else
    {
        m_showPrimaries->setHidden( true );
        m_showSecondaries->setHidden( true );
    }

    if(m_protonData->IsColumnAvailable("PDGEncoding"))
    {
        m_multiSelection->setHidden( false );
    }
    else
    {
        m_multiSelection->setHidden( true );
    }
}

void WMFilterPropertyHandler::createCheckBoxForPrimaryAndSecondary()
{
    WPropertyBase::PropertyChangeNotifierType notifierCheckBox = boost::bind(
        &WMFilterPropertyHandler::updateCheckboxProperty, this, boost::placeholders::_1 );

    m_showPrimaries = m_filteringGroup->addProperty( "Show primaries", "Show/hide primaries", true, notifierCheckBox );
    m_showSecondaries = m_filteringGroup->addProperty( "Show secondaries", "Show/hide secondaries", true, notifierCheckBox );
}

void WMFilterPropertyHandler::createMultiSelectionForPDG()
{
    WPropertyBase::PropertyChangeNotifierType pdgColumnNotifier = boost::bind(
        &WMFilterPropertyHandler::onSingleSelectionChanged, this, boost::placeholders::_1 );

    WPropertyBase::PropertyChangeNotifierType pdgEncodingnotifier = boost::bind(
        &WMFilterPropertyHandler::updateSelectedPDGTypes, this, boost::placeholders::_1 );

    m_columnItemSelectionList = m_columnPropertyHandler->getColumnItems();

    WItemSelector itemSelector = m_columnItemSelectionList->getSelectorAll();

    m_currentColumnIndex = selectColumn( "PDGEncoding", itemSelector );

    m_singleSelection = m_filteringGroup->addProperty( "Particle Data Group", "", itemSelector, pdgColumnNotifier );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_singleSelection );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_singleSelection );

    m_particleItemSelectionList = WItemSelection::SPtr( new WItemSelection() );

    setPDGTypes( m_currentColumnIndex );

    updatePDGTypesProperty( m_particleItemSelectionList );

    m_selectedPDGTypes.push_back( std::to_string( m_pdgTypes[0] ) );

    m_multiSelection = m_filteringGroup->addProperty( "Particle Type", "Choose particle type(s) you want show",
                                                            m_particleItemSelectionList->getSelectorFirst(), pdgEncodingnotifier );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_multiSelection );
}

void WMFilterPropertyHandler::setPDGTypes( int columnIndex )
{
    m_pdgTypes.clear();

    if(pdgColumnIndex < 0)
    {
        return;
    }

    for( auto idx = 0; idx < m_protonData->getCSVData()->size(); idx++)
    {
        std::vector< std::string > row = m_protonData->getCSVData()->at( idx );
        int currentParticleID = std::stoi( row.at( columnIndex ) );

        if( std::find( m_pdgTypes.begin(), m_pdgTypes.end(), currentParticleID ) == m_pdgTypes.end() )
        {
            m_pdgTypes.push_back( currentParticleID );
        }
    }
}

void WMFilterPropertyHandler::updateSelectedPDGTypes( WPropertyBase::SPtr property )
{
    m_selectedPDGTypes.clear();

    if( m_multiSelection->changed() )
    {
        WItemSelector selectedItems = m_multiSelection->get( true );

        for( int i = 0; i < selectedItems.size(); ++i )
        {
            m_selectedPDGTypes.push_back( selectedItems.at( i )->getName() );
        }
    }

    m_dataUpdate( );
}

bool WMFilterPropertyHandler::isPDGTypeSelected( int pdgType )
{
    for( auto idx = 0; idx < m_selectedPDGTypes.size(); idx++ )
    {
        if(pdgType == std::stoi( m_selectedPDGTypes[idx] ) )
        {
            return true;
        }
    }
    return false;
}

int WMFilterPropertyHandler::getCurrentColumnIndex()
{
    return m_currentColumnIndex;
}

int WMFilterPropertyHandler::selectColumn( std::string columnName, WItemSelector& itemSelector )
{
    for( int i = 0; i < itemSelector.size(); i++ )
    {
        WItemSelectionItem item = *itemSelector.at( i );

        if( item.getName() == columnName )
        {
            itemSelector = m_columnItemSelectionList->getSelector( i );
            return i;
        }
    }

    itemSelector = m_columnItemSelectionList->getSelectorNone();

    return 0;
}

void WMFilterPropertyHandler::updatePDGTypesProperty( WItemSelection::SPtr particleItemSelectionList )
{
    if ( !m_pdgTypes.empty() )
    {
        particleItemSelectionList->clear();

        for( auto pdgType : m_pdgTypes )
        {
            particleItemSelectionList->addItem( std::to_string( pdgType ) );
        }
    }
}

void WMFilterPropertyHandler::onSingleSelectionChanged( WPropertyBase::SPtr property )
{
    WItemSelector itemSelector = m_columnItemSelectionList->getSelectorAll();
    std::string columnName = m_singleSelection->get().at( 0 )->getName();
    bool columnIsValid = m_singleSelection->isValid();

    m_multiSelection->setHidden( !columnIsValid );

    m_currentColumnIndex = selectColumn( columnName, itemSelector );

    setPDGTypes( m_currentColumnIndex );

    updatePDGTypesProperty( m_particleItemSelectionList );

    m_multiSelection->set( m_particleItemSelectionList->getSelectorFirst() );
}

void WMFilterPropertyHandler::updateCheckboxProperty( WPropertyBase::SPtr property )
{
    if( m_showPrimaries->get() || m_showSecondaries->get() )
    {
        m_dataUpdate( );
    }
    else
    {
        if( property == m_showPrimaries )
        {
            m_showPrimaries->set( true );
        }

        if( property == m_showSecondaries )
        {
            m_showSecondaries->set( true );
        }
    }
}

WPropBool WMFilterPropertyHandler::getShowPrimaries()
{
    return m_showPrimaries;
}

WPropBool WMFilterPropertyHandler::getShowSecondaries()
{
    return m_showSecondaries;
}