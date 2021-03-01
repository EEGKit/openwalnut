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

#ifndef WMFILTERPROPERTYHANDLER_H
#define WMFILTERPROPERTYHANDLER_H

#include <string>
#include <vector>

#include "../WMProtonData.h"

class WMFilterPropertyHandler
{
public:
    /**
     * Function variables for updating the data 
     */
    typedef boost::shared_ptr< WMFilterPropertyHandler > SPtr;

    /**
     * shared_ptr that points to itself 
     */
    typedef boost::function< void( ) > CallbackPtr;

    /**
     * constructor
     *
     * \param protonData Pointer to the content and header of the CSV 
     * \param properties A property variable that is generated by the WModul 
     * \param dataUpdate A function variable that reinitializes the WDataSets 
     */
    explicit WMFilterPropertyHandler( WMProtonData::SPtr protonData,
                                    WPropertyGroup::SPtr properties,
                                    WMFilterPropertyHandler::CallbackPtr dataUpdate );

    /**
     * creates the group property and the subproperty    
     */
    void createProperties();

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

private:
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
    WMFilterPropertyHandler::CallbackPtr m_dataUpdate;

    /**
     * update the m_selectedPDGTypes
     */
    void updateSelectedPDGTypes( WPropertyBase::SPtr property );

    /**
     * create the Checkbox for primaries and secondaries
     */
    void createCheckBoxForPrimaryAndSecondary();

    /**
     * create the multiselection for the option of PDG
     */
    void createMultiSelectionForPDG();

    /**
     * Collect all particle types from your input data.
     */
    void searchPDGTypes();

     /**
     * Reload data when properties for selection of primaries and secondaries changed
     *
     * \param property contains reference to the property which called updateProperty()
     */
    void updateCheckboxProperty( WPropertyBase::SPtr property );

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
     * Stores selectable items.
     */
    boost::shared_ptr< WItemSelection > m_possibleSelection;

    /**
     * vector of the options of PDG
     */
    std::vector < std::string > m_selectedPDGTypes;
};

#endif  // WMFILTERPROPERTYHANDLER_H
