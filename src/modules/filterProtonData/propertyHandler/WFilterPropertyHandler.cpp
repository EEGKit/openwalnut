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

#include <regex>
#include <string>
#include <vector>

#include "core/kernel/WKernel.h"
#include "WFilterPropertyHandler.h"

WFilterPropertyHandler::WFilterPropertyHandler( WProtonData::SPtr protonData,
                                            WPropertyGroup::SPtr properties,
                                            WFilterPropertyHandler::CallbackPtr dataUpdate ):
    m_protonData( protonData ),
    m_properties( properties ),
    m_dataUpdate( dataUpdate )
{
}

void WFilterPropertyHandler::createProperties()
{
    m_filteringGroup = m_properties->addPropertyGroup( "Filtering", "Filter primaries, secondaries and particle types" );

    createCheckBoxForPrimaryAndSecondary();
    createMultiSelectionForPDG();
    createPropToSetParticleNames();

    updateProperty();
}

void WFilterPropertyHandler::updateProperty()
{
    updatePDGProperties();

    if( m_protonData->isColumnAvailable( "Parent id" ) && m_protonData->isColumnAvailable( "Track id" ) )
    {
        m_showPrimaries->setHidden( false );
        m_showSecondaries->setHidden( false );
    }
    else
    {
        m_showPrimaries->setHidden( true );
        m_showSecondaries->setHidden( true );
    }

    if( m_protonData->isColumnAvailable( "Particle Data Group" ) )
    {
        m_multiSelection->setHidden( false );
        m_filteringsubGroup->setHidden( false );
    }
    else
    {
        m_multiSelection->setHidden( true );
        m_filteringsubGroup->setHidden( true );
    }
}

void WFilterPropertyHandler::updatePDGProperties()
{
    m_currentColumnIndex = m_protonData->getColumnIndexBySelection( "Particle Data Group" );

    m_filteringGroup->removeProperty( m_multiSelection );
    m_filteringGroup->removeProperty( m_filteringsubGroup );

    createMultiSelectionForPDG();
    createPropToSetParticleNames();
}

void WFilterPropertyHandler::createCheckBoxForPrimaryAndSecondary()
{
    WPropertyBase::PropertyChangeNotifierType notifierCheckBox = boost::bind(
        &WFilterPropertyHandler::updateCheckboxProperty, this, boost::placeholders::_1 );

    m_showPrimaries = m_filteringGroup->addProperty( "Show primaries",
                                            "Show or hide primaries. One can either hide primaries or secondaries, but not both at the same time.",
                                            true, notifierCheckBox );
    m_showSecondaries = m_filteringGroup->addProperty( "Show secondaries",
                                            "Show or hide secondaries. One can either hide primaries or secondaries, but not both at the same time.",
                                            true, notifierCheckBox );
}

void WFilterPropertyHandler::searchPDGTypes()
{
    int pdgColumnIndex = m_protonData->getColumnIndexBySelection( "Particle Data Group" );

    if(pdgColumnIndex < 0)
    {
        return;
    }

    for( size_t idx = 0; idx < m_protonData->getCSVData()->size(); idx++)
    {
        std::vector< std::string > row = m_protonData->getCSVData()->at( idx );
        int currentParticleID = std::stoi( row.at( pdgColumnIndex ) );

        if( std::find( m_pdgTypes.begin(), m_pdgTypes.end(), currentParticleID ) == m_pdgTypes.end() )
        {
            m_pdgTypes.push_back( currentParticleID );
        }
    }
}

void WFilterPropertyHandler::createMultiSelectionForPDG()
{
    WPropertyBase::PropertyChangeNotifierType pdgEncodingnotifier = boost::bind(
        &WFilterPropertyHandler::updateSelectedPDGTypes, this );

    m_particleItemSelectionList = WItemSelection::SPtr( new WItemSelection() );

    m_pdgTypes.clear();
    searchPDGTypes();

    if(m_pdgTypes.size() <= 0)
    {
        m_multiSelection = m_filteringGroup->addProperty( "Show particles", "Choose particle type(s) to be shown.",
                                                            m_particleItemSelectionList->getSelectorNone(), pdgEncodingnotifier );
        return;
    }

    for( auto pdgType : m_pdgTypes )
    {
        m_particleItemSelectionList->addItem(
                getParticleNameFromPdg( pdgType ) + " (" + std::to_string( pdgType ) + ")"
        );
    }

    m_multiSelection = m_filteringGroup->addProperty( "Show particles", "Choose particle type(s) to be shown.",
                                                            m_particleItemSelectionList->getSelectorAll(), pdgEncodingnotifier );

    WPropertyHelper::PC_NOTEMPTY::addTo( m_multiSelection );
}

void WFilterPropertyHandler::updateSelectedPDGTypes()
{
    m_dataUpdate( );
}

void WFilterPropertyHandler::createPropToSetParticleNames()
{
     m_notifierSetParticleName = boost::bind(
        &WFilterPropertyHandler::saveSelectPDGandSaveEvent, this, boost::placeholders::_1 );


    m_filteringsubGroup  = m_filteringGroup->addPropertyGroup( "Rename Particle Types",
                                                            "Filtering/Rename or Delete Particle-Name" );

    m_PdgForRenameSelection = m_filteringsubGroup->addProperty( "Select particle",  "Select the particle type to be renamed.",
                                                            m_particleItemSelectionList->getSelectorFirst(), m_notifierSetParticleName );

    m_inputNewParticleName = m_filteringsubGroup->addProperty( "New name (press enter)", "Type in a new name for the selected particle type. "
                                                            "To submit your entry press enter while you are in the textbox.",
                                                            setDefaultForRenameField(), m_notifierSetParticleName );

    WPropertyHelper::PC_NOTEMPTY::addTo( m_inputNewParticleName );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_PdgForRenameSelection );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_PdgForRenameSelection );
}

std::string WFilterPropertyHandler::setDefaultForRenameField()
{
    WItemSelector selectedPdg = m_PdgForRenameSelection->get( true );
    std::string particleName = selectedPdg.at( 0 )->getName();
    return particleName.substr( 0, particleName.find( " (" ) );
}

void WFilterPropertyHandler::selectPdgAndSaveEvent( WPropertyBase::SPtr property )
{
    if( property == m_PdgForRenameSelection )
    {
        m_filteringsubGroup->removeProperty( m_inputNewParticleName );
        m_inputNewParticleName = m_filteringsubGroup->addProperty( "New name (press enter)", "Type in a new name for the selected particle type. "
                                                            "To submit your entry press enter while you are in the textbox.",
                                                            setDefaultForRenameField(), m_notifierSetParticleName  );
    }
    if( property == m_inputNewParticleName )
    {
            WItemSelector selectedPdg = m_PdgForRenameSelection->get( true );
            changePdgBiMap( getPdgFromName( selectedPdg.at( 0 )->getName() ),  m_inputNewParticleName->get( true ) );
            updatePDGProperties();
    }
}

bool WFilterPropertyHandler::isPDGTypeSelected( int pdgType )
{
    WItemSelector selectedItems = m_multiSelection->get( true );

    for( size_t i = 0; i < selectedItems.size(); ++i )
    {
        if(getPdgFromName( selectedItems.at( i )->getName()) == pdgType)
        {
            return true;
        }
    }
    return false;
}

void WFilterPropertyHandler::updateCheckboxProperty( WPropertyBase::SPtr property )
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

WPropBool WFilterPropertyHandler::getShowPrimaries()
{
    return m_showPrimaries;
}

WPropBool WFilterPropertyHandler::getShowSecondaries()
{
    return m_showSecondaries;
}

void WFilterPropertyHandler::createPDGMap( std::string path )
{
    std::fstream pdgSignFile;
    std::string dataRow;
    std::string particleName;
    int pdg;

    pdgSignFile.open( getParticleNameFilePath() );

    if( !pdgSignFile.is_open() )
    {
        if( !copyFileToHomePath(path, getParticleNameFilePath() ) )
        {
            throw WException( "Failed to copy particle name file into homedir" );
        }
        pdgSignFile.open( getParticleNameFilePath() );
    }

    std::getline( pdgSignFile, dataRow );
    while( std::getline( pdgSignFile, dataRow ) )
    {
        std::istringstream iss( dataRow );
        iss >> particleName >> pdg;

        m_PdgNamesByID.insert( PdgElement( particleName, pdg ) );
    }
    pdgSignFile.close();
}

std::string WFilterPropertyHandler::getParticleNameFromPdg( int pdg )
{
    BM_PDG::right_const_iterator pdg_iter = m_PdgNamesByID.right.find( pdg );

    return pdg_iter != m_PdgNamesByID.right.end() ?  pdg_iter->second : "Unknown";
}

int WFilterPropertyHandler::getPdgFromName( std::string particleName )
{
    BM_PDG::left_const_iterator pdg_iter = m_PdgNamesByID.left.find( particleName.substr( 0, particleName.find( " (" ) ) );

    return pdg_iter != m_PdgNamesByID.left.end() ? pdg_iter->second : getPdgFromUnkownParticle( particleName );
}

int WFilterPropertyHandler::getPdgFromUnkownParticle( std::string particleName )
{
    std::regex regexp( "[-+0-9]+" );
    std::smatch m;
    std::regex_search( particleName, m, regexp );

    return std::stoi( m[0] );
}

void WFilterPropertyHandler::changePdgBiMap( int pdg, std::string newParticleName )
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

void WFilterPropertyHandler::writePdgMapInParticleNameFile()
{
    std::ofstream pdgSignFile;
    pdgSignFile.open( getParticleNameFilePath() , std::ios_base::out | std::ios::trunc );

    if( !pdgSignFile.is_open() )
    {
        throw WException( "File could not be opened!" );
    }
    for( auto pdgNameIterator = m_PdgNamesByID.begin(); pdgNameIterator != m_PdgNamesByID.end(); ++pdgNameIterator )
    {
        pdgSignFile << pdgNameIterator->left << " " << pdgNameIterator->right << "\n";
    }
    pdgSignFile.close();
}

std::string WFilterPropertyHandler::getParticleNameFilePath()
{
    return WPathHelper::getHomePath().string() + "\\PDGEncodingNameMap.txt";
}

bool WFilterPropertyHandler::copyFileToHomePath( std::string shareDirFile, std::string homeDirFile )
{
    std::ifstream src( shareDirFile, std::ios::binary );
    std::ofstream dest( homeDirFile, std::ios::binary );

    dest << src.rdbuf();
    return src && dest;
}
