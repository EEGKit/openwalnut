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
#include <regex>

#include "WMFilterPropertyHandler.h"


WMFilterPropertyHandler::WMFilterPropertyHandler( WMProtonData::SPtr protonData,
                                            WPropertyGroup::SPtr properties,
                                            WMColumnPropertyHandler::SPtr columnPropertyHandler,
                                            WMFilterPropertyHandler::CallbackPtr dataUpdate ):
    m_protonData( protonData ),
    m_properties( properties ),
    m_columnPropertyHandler( columnPropertyHandler ),
    m_dataUpdate( dataUpdate )
{
}

void WMFilterPropertyHandler::createProperties()
{
    m_filteringGroup = m_properties->addPropertyGroup( "Filtering", "Filter primaries and secondaries" );

    createCheckBoxForPrimaryAndSecondary();
    createMultiSelectionForPDG();
    createPropToSetParticleNames();

    updateProperty();
}


void WMFilterPropertyHandler::updateProperty()
{
    //m_filteringGroup->removeProperty( m_multiSelection );
    //createMultiSelectionForPDG();

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

    m_selectedPDGTypes.push_back( m_pdgTypes[0] );

    m_multiSelection = m_filteringGroup->addProperty( "Particle Type", "Choose particle type(s) you want show",
                                                            m_particleItemSelectionList->getSelectorFirst(), pdgEncodingnotifier );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_multiSelection );
}

void WMFilterPropertyHandler::createPropToSetParticleNames()
{
    WPropertyBase::PropertyChangeNotifierType notifierSetParticleName = boost::bind(
        &WMFilterPropertyHandler::saveRenameParticleButtonClick, this, boost::placeholders::_1 );

    WPropGroup filteringsubGroup = m_filteringGroup->addPropertyGroup( "Rename or Delete Particle-Name",
                                                            "Change, set a new name or delete for a particlename" );

    m_aString = filteringsubGroup->addProperty( "New Name (press enter)", "Input a new name for the choosen particle",
                                                            std::string( "" ), notifierSetParticleName );

    m_PdgForRenameSelection = filteringsubGroup->addProperty( "Select Particle",  "Select the Particle PDG to rename",
                                                            m_particleItemSelectionList->getSelectorFirst(),
                                                            notifierSetParticleName );

    m_saveButton= filteringsubGroup->addProperty( "Set Changes", "Save", WPVBaseTypes::PV_TRIGGER_READY, notifierSetParticleName );

    WPropertyHelper::PC_NOTEMPTY::addTo( m_aString );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_PdgForRenameSelection );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_PdgForRenameSelection );
}

void WMFilterPropertyHandler::setPDGTypes( int columnIndex )
{
    m_pdgTypes.clear();

    if(columnIndex < 0)
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
            m_selectedPDGTypes.push_back( getPdgFromName( selectedItems.at( i )->getName() ));
        }
    }
    m_dataUpdate( );
}

bool WMFilterPropertyHandler::isPDGTypeSelected( int pdgType )
{
    for( auto idx = 0; idx < m_selectedPDGTypes.size(); idx++ )
    {
        if(pdgType == m_selectedPDGTypes[idx] )
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
    if( !m_pdgTypes.empty() )
    {
        particleItemSelectionList->clear();

        for( auto pdgType : m_pdgTypes )
        {
            particleItemSelectionList->addItem(
                    getParticleNameFromPdg( pdgType ) + "(" + std::to_string( pdgType ) + ")"
            );
        }
    }
}

void WMFilterPropertyHandler::onSingleSelectionChanged( WPropertyBase::SPtr property )
{
    WItemSelector itemSelector = m_columnItemSelectionList->getSelectorAll();
    std::string columnName = m_singleSelection->get().at( 0 )->getName();
    bool columnIsValid = m_singleSelection->isValid();

    m_multiSelection->setHidden
    ( !columnIsValid );

    m_currentColumnIndex = selectColumn( columnName, itemSelector );

    setPDGTypes( m_currentColumnIndex );

    updatePDGTypesProperty( m_particleItemSelectionList );

    m_multiSelection->set( m_particleItemSelectionList->getSelectorFirst() );
}

void WMFilterPropertyHandler::saveRenameParticleButtonClick( WPropertyBase::SPtr property )
{
    if(property == m_saveButton && m_saveButton->get(true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
    {
        WItemSelector selectedItems = m_PdgForRenameSelection->get( true );  
        std::string newParticleName =  m_aString->get( true );
        int pdg  = getPdgFromName( selectedItems.at( 0 )->getName() );

        changePdgBiMap( pdg, newParticleName );
    }
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

void WMFilterPropertyHandler::createPDGMap( std::string path )
{
    m_PdgParticelNamePath= path;

    std::fstream pdgSignFile;
    std::string dataRow;
    bool firstLine = true;
    std::string particleName;
    int pdg;

    pdgSignFile.open( path );

    if( !pdgSignFile.is_open() )
    {
        throw WException( "File could not be opened!" );
    }
    while( std::getline( pdgSignFile, dataRow ) )
    {
        if( firstLine )
        {
            firstLine = false;
            continue;
        }
        std::istringstream iss( dataRow );
        iss >> particleName >> pdg;

        m_PdgNamesByID.insert( PdgElement( particleName, pdg ) );
    }
    pdgSignFile.close();
}

std::string WMFilterPropertyHandler::getParticleNameFromPdg( int pdg )
{
    BM_PDG::right_const_iterator pdg_iter = m_PdgNamesByID.right.find( pdg );

    if( pdg_iter != m_PdgNamesByID.right.end() )
    {
        return pdg_iter->second;
    }
    else
    {
        return "unknown";
    }
}

int WMFilterPropertyHandler::getPdgFromName( std::string particleName )
{
    BM_PDG::left_const_iterator pdg_iter = m_PdgNamesByID.left.find( particleName.substr( 0, particleName.find( "(" ) ) );
    if( pdg_iter != m_PdgNamesByID.left.end() )
    {
        return pdg_iter->second;
    }
    else
    {
        std::regex regexp( "[-+0-9]+" );
        std::smatch m;
        std::regex_search( particleName, m, regexp );
        return std::stoi( m[0] );
    }
}

void WMFilterPropertyHandler::changePdgBiMap( int pdg, std::string newParticleName )
{
    BM_PDG::right_iterator pdg_iter = m_PdgNamesByID.right.find( pdg );

    if( pdg_iter != m_PdgNamesByID.right.end() )
    {
        bool success_replace = m_PdgNamesByID.right.replace_data( pdg_iter, newParticleName );
        if( !success_replace )
        {
             throw WException( "Fail to replace new particle name" );
        }
    }
    else    //unknown particle
    {
        m_PdgNamesByID.insert( PdgElement( newParticleName, pdg ) );
    }

    writePdgMapInParticleNameFile();
}
void WMFilterPropertyHandler::writePdgMapInParticleNameFile()
{
    std::ofstream pdgSignFile;

    pdgSignFile.open( m_PdgParticelNamePath, std::ios_base::out | std::ios::trunc );

    if( !pdgSignFile.is_open() )
    {
        throw WException( "File could not be opened!" );
    }

    for( auto pdgNameIterator = m_PdgNamesByID.begin(); pdgNameIterator != m_PdgNamesByID.end(); ++pdgNameIterator )
    {
        pdgSignFile << pdgNameIterator->left << " " << pdgNameIterator->right << "\n";
    }

    m_saveButton->set( WPVBaseTypes::PV_TRIGGER_READY, false );
    pdgSignFile.close();
}
