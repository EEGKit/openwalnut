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

#ifndef WMVISUALIZATIONPROPERTYHANDLER_H
#define WMVISUALIZATIONPROPERTYHANDLER_H

#include "../WMProtonData.h"

class WMVisualizationPropertyHandler
{
public:
    typedef boost::shared_ptr< WMVisualizationPropertyHandler > SPtr;

    typedef boost::function< void( ) > CallbackPtr;

    explicit WMVisualizationPropertyHandler( WMProtonData::SPtr protonData,
                                    WPropertyGroup::SPtr properties,
                                    WMVisualizationPropertyHandler::CallbackPtr dataUpdate );

    void createProperties();

    WPropBool getColorFromEdep();

    WPropBool getSizesFromEdep();

    WPropColor getColorSelection();

private:
    WMProtonData::SPtr m_protonData;

    WPropertyGroup::SPtr m_properties;

    WMVisualizationPropertyHandler::CallbackPtr m_dataUpdate;

    /**
     * Property group for visualization modes
     */
    WPropGroup m_visualizationGroup;

    /**
     * Decides whether to color points based on the deposited energy or not.
     */
    WPropBool m_colorFromEdep;

    /**
     * Decides whether to scale the points sizes according to their deposited energy.
     */
    WPropBool m_sizesFromEdep;

    /**
     * Property that holds the current selected color when not coloring points based on their deposited energy.
     */
    WPropColor m_colorSelection;

     /**
     * Reload data when properties for selection of primaries and secondaries changed
     *
     * \param property contains reference to the property which called updateProperty()
     */
    void updateCheckboxProperty( WPropertyBase::SPtr property );
};



#endif  // WMVISUALIZATIONPROPERTYHANDLER_H