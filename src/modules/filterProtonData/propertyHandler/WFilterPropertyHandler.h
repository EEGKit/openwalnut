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

#ifndef WFILTERPROPERTYHANDLER_H
#define WFILTERPROPERTYHANDLER_H

#include <string>
#include <vector>

#include <boost/bimap.hpp>
#include <boost/lexical_cast.hpp>

#include "core/common/WPathHelper.h"

#include "WColumnPropertyHandler.h"
#include "../WProtonData.h"


/**
 * Creates, updates and handles the filter properties.
 */
class WFilterPropertyHandler
{
public:
    /**
     * Function variables for updating the data 
     */
    typedef boost::shared_ptr< WFilterPropertyHandler > SPtr;

    /**
     * shared_ptr that points to itself 
     */
    typedef boost::function< void( ) > CallbackPtr;

    /**
     * bimap to save the pdg to a particlename
     */
    typedef boost::bimap< std::string, int > BM_PDG;

    /**
     * represent an Element of a pdg and a particlename for the map
     */
    typedef BM_PDG::value_type PdgElement;

    /**
     * constructor
     *
     * \param protonData Pointer to the content and header of the CSV 
     * \param properties A property variable that is generated by the WModul 
     * \param dataUpdate A function variable that reinitializes the WDataSets 
     */
    explicit WFilterPropertyHandler( WProtonData::SPtr protonData,
                                    WPropertyGroup::SPtr properties,
                                    WFilterPropertyHandler::CallbackPtr dataUpdate );

    /**
     * creates the group property and the subproperty    
     */
    void createProperties();

    /**
     * update current group property and subproperty
     */
    void updateProperty();

    /**
     * Getter
     *
     * \return WPropBool of the ShowPrimaries
     */
    WPropBool getShowPrimaries();

    /**
     * Getter
     *
     * \return WPropBool of the ShowSecondaries
     */
    WPropBool getShowSecondaries();

    /**
     * Check whether the PDG type is contained in the m_selectedPDGTypes 
     *
     * \param pdgType the int value of the PDG-Type
     * \return true, if it includes. false, if it does not includes  
     */
    bool isPDGTypeSelected( int pdgType );

     /**
     * creates a bimap out of the names and ids of PDG txt
     *
     * \param path fielpath of the pdg particlename file
     * \throws WException
     */
    void createPDGMap( std:: string path );

private:
    /**
     * Pointer to the content and header of the CSV 
     */
    WProtonData::SPtr m_protonData;

    /**
     * A property variable that is generated by the WModul 
     */
    WPropertyGroup::SPtr m_properties;

    /**
     * A function variable that reinitializes the WDataSets 
     */
    WFilterPropertyHandler::CallbackPtr m_dataUpdate;

    /**
     * \return the path of the particle names file
     */
    std::string getParticleNameFilePath();

    /**
     * Helperfunction to copy the particlename file
     *
     * \param shareDirFile sourcepath of the particle name file in the share directory
     * \param homeDirFile destinypath of the local directory for the particle name file
     * \return true copy was succesful
     * \return false copy was unsuccesful
     */
    bool copyFileToHomePath( std::string shareDirFile, std::string homeDirFile );

    /**
     * Collect all particle types from your input data.
     */
    void searchPDGTypes();

    /**
     * update the m_selectedPDGTypes
     *
     * \param property contains reference to the property which called updateProperty()
     *
     */
    void updateSelectedPDGTypes();

    /**
     * event of the save button in the rename pdg subgroup
     *
     * \param property contains reference to the property which called updateProperty()
     *
     */
    void selectPdgAndSaveEvent( WPropertyBase::SPtr property );

    /**
     * update PDG Properties (Multiselector and change-Name-Properties)
     */
    void updatePDGProperties();

    /**
     * create the Checkbox for primaries and secondaries
     */
    void createCheckBoxForPrimaryAndSecondary();

    /**
     * create the multiselection for the option of PDG
     */
    void createMultiSelectionForPDG();

    /**
     * create the Subgroup for change of PDG names
     */
    void createPropToSetParticleNames();

    /**
     * set the default string for the string property textfield
     * 
     * \return the string of the selected item of the itemselector
     */
    std::string setDefaultForRenameField();

    /**
     * Updates possible selectable particle types in multiselection
     *
     * \param particleItemSelectionList selectable partyle types as itemSelectionList
     */
    void updatePDGTypesProperty( WItemSelection::SPtr particleItemSelectionList );

     /**
     * Reload data when properties for selection of primaries and secondaries changed
     *
     * \param property contains reference to the property which called updateProperty()
     */
    void updateCheckboxProperty( WPropertyBase::SPtr property );

    /**
     * Seatch the ParticleName in the map with a given pdg
     *
     * \param pdg pdg number of a particlename
     * \return the particle name
     */
    std::string getParticleNameFromPdg( int pdg );

    /**
     * Search the pdg in the map with a given particlename
     *
     * \param particleName particlename of a pdg number
     * \return pdg number
     */
    int getPdgFromName( std::string particleName );

    /**
     * get the PDG number of an unknown particle element with 
     * the help of regex
     * \param particleName particle name of an unknown number
     * \return pdg number
     */
    int getPdgFromUnkownParticle( std::string particleName );

    /**
     * write the BiMap in the Pdg particle name file
     */
    void writePdgMapInParticleNameFile();

    /**
     * set or replace a particlename and pdg in the bimap
     *
     * \param pdg pdg number of a selected particle name
     * \param newParticleName new particlename of a pdg number
     * \throws WException
     */
    void changePdgBiMap( int pdg, std::string newParticleName );

    /**
     * A Map of the PDG and their names
     */
    BM_PDG m_PdgNamesByID;

    /**
     * The current column that is selected as particle data group
     */
    int m_currentColumnIndex = 0;

     /**
     * Stores every unique particle id.
     */
    std::vector < int > m_pdgTypes;

    /**
     * Property group for filtering options
     */
    WPropGroup m_filteringGroup;

    /**
     * Decides whether to hide or show primaries.
     */
    WPropBool m_showPrimaries;

    /**
     * Decides whether to hide or show secondaries.
     */
    WPropBool m_showSecondaries;

    /**
     * Stores users selected items.
     */
    WPropSelection m_multiSelection;

    /**
     * Stores subgroup for change the pdg-Name
     */
    WPropGroup m_filteringsubGroup;

    /**
     * Stores selectable items.
     */
    boost::shared_ptr< WItemSelection > m_particleItemSelectionList;

    /**
     * vector of the options of PDG
    */
    std::vector < int > m_selectedPDGTypes;

    /**
     * A string for the new particle name
     */
    WPropString m_inputNewParticleName;

    /**
     * A trigger for the save button to save changes
     */
    WPropTrigger  m_saveButton;

    /**
     * A property to select one pdg to change
     */
    WPropSelection m_PdgForRenameSelection;

    /**
     * A notifier for the Changeevents of the Rename PDG property
     */
    WPropertyBase::PropertyChangeNotifierType m_notifierSetParticleName;
};

#endif  // WFILTERPROPERTYHANDLER_H
