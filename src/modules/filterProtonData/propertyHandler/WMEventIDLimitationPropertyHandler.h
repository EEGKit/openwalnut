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

#ifndef WMEVENTIDLIMITATIONPROPERTYHANDLER_H
#define WMEVENTIDLIMITATIONPROPERTYHANDLER_H

#include "../WMProtonData.h"

class WMEventIDLimitationPropertyHandler
{
public:
    typedef boost::shared_ptr< WMEventIDLimitationPropertyHandler > SPtr;

    typedef boost::function< void( ) > CallbackPtr;

    explicit WMEventIDLimitationPropertyHandler( WMProtonData::SPtr protonData,
                                    WPropertyGroup::SPtr properties,
                                    WMEventIDLimitationPropertyHandler::CallbackPtr dataUpdate );

    void createProperties();

    WPropInt getMinCap();

    WPropInt getMaxCap();
    
private:
    WMProtonData::SPtr m_protonData;

    WPropertyGroup::SPtr m_properties;

    WMEventIDLimitationPropertyHandler::CallbackPtr m_dataUpdate;

    /**
     * Set lower border of range of eventID selection.
     */
    WPropInt m_minCap;

    /**
     * Set upper border of range of eventID selection.
     */
    WPropInt m_maxCap;
    
    /**
     * Update your mesh when changing properties.
     */
    void updateMesh( );

     /**
     * Determines smalles und biggest eventID.
     */
    void determineMinMaxEventID();
};



#endif  // WMEVENTIDLIMITATIONPROPERTYHANDLER_H