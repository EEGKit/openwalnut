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

#include "../WMProtonData.h"

class WMFilterPropertyHandler
{
public:
    typedef boost::shared_ptr< WMFilterPropertyHandler > SPtr;

    typedef boost::function< void( ) > CallbackPtr;

    explicit WMFilterPropertyHandler( WMProtonData::SPtr protonData,
                                    WPropertyGroup::SPtr properties,
                                    WMFilterPropertyHandler::CallbackPtr dataUpdate );

    void createProperties();
    
    
    WPropBool getShowPrimaries();
    WPropBool getShowSecondaries();

    bool isPDGTypeSelected( int pdgType );

private:
    WMProtonData::SPtr m_protonData;

    WPropertyGroup::SPtr m_properties;

    WMFilterPropertyHandler::CallbackPtr m_dataUpdate;

    void updateSelectedPDGTypes( WPropertyBase::SPtr property );

    void createCheckBoxForPrimaryAndSecondary();

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
     * 
     */
    std::vector < std::string > m_selectedPDGTypes;

};



#endif  // WMFILTERPROPERTYHANDLER_H