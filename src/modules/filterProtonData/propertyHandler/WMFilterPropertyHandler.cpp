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
}

void WMFilterPropertyHandler::createCheckBoxForPrimaryAndSecondary()
{
    WPropertyBase::PropertyChangeNotifierType notifierCheckBox = boost::bind( &WMFilterPropertyHandler::updateCheckboxProperty, this, boost::placeholders::_1 );
    m_showPrimaries = m_filteringGroup->addProperty( "Show primaries", "Show/hide primaries", true, notifierCheckBox );
    m_showSecondaries = m_filteringGroup->addProperty( "Show secondaries", "Show/hide secondaries", true, notifierCheckBox );
}

void WMFilterPropertyHandler::createMultiSelectionForPDG()
{
    WPropertyBase::PropertyChangeNotifierType pdgEncodingnotifier = boost::bind( &WMFilterPropertyHandler::updateSelectedPDGTypes,this, boost::placeholders::_1 );

    m_possibleSelection = WItemSelection::SPtr( new WItemSelection() );
    searchPDGTypes();
    for( auto pdgType : m_pdgTypes )
    {
        m_possibleSelection->addItem( std::to_string( pdgType ) );
    }

    m_selectedPDGTypes.push_back( std::to_string( m_pdgTypes[0] ) );

    m_multiSelection = m_filteringGroup->addProperty( "PDGEncoding", "Choose particle type(s) you want show",
                                                            m_possibleSelection->getSelectorFirst(), pdgEncodingnotifier );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_multiSelection );
}

WPropBool WMFilterPropertyHandler::getShowPrimaries()
{
    return m_showPrimaries;
}

WPropBool WMFilterPropertyHandler::getShowSecondaries()
{
    return m_showSecondaries;
}

void WMFilterPropertyHandler::searchPDGTypes()
{
    int pdgColumnIndex = m_protonData->getColumnIndex( "PDGEncoding" );

    for( auto idx = 0; idx < m_protonData->getCSVData()->size(); idx++)
    {
        std::vector< std::string > row = m_protonData->getCSVData()->at( idx );
        int currentParticleID = std::stoi( row.at( pdgColumnIndex ) );

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
