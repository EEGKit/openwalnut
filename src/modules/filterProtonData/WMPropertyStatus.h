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

#ifndef WMPROPERTYSTATUS_H
#define WMPROPERTYSTATUS_H


#include "core/kernel/WModule.h"
#include "WMColumnPropertyHandler.h"

class WMPropertyStatus
{

public:
    WMPropertyStatus();

    WPropBool getShowPrimaries();
    void setShowPrimaries( WPropBool showPrimaries );

    WPropBool getShowSecondaries();
    void setShowSecondaries( WPropBool showSecondaries );

    WPropInt getMinCap();
    void setMinCap( WPropInt minCap );

    WPropInt getMaxCap();
    void setMaxCap( WPropInt maxCap );

    WPropBool getColorFromEdep();
    void setColorFromEdep( WPropBool colorFromEdep);

    WPropBool getSizesFromEdep();
    void setSizesFromEdep( WPropBool sizesFromEdep);

    WPropGroup getFilteringGroup();
    void setFilteringGroup( WPropGroup filteringGroup);

    WPropGroup getVisualizationGroup();
    void setVisualizationGroup( WPropGroup visualizationGroup);

    WPropColor getColorSelection();
    void setColorSelection( WPropColor colorSelection);

    WMColumnPropertyHandler::SPtr getColumnPropertyHandler();
    void setColumnPropertyHandler( WMColumnPropertyHandler::SPtr columnPropertyHandler);

private:
    /**
     * Decides whether to hide or show primaries.
     */
    WPropBool m_showPrimaries;

    /**
     * Decides whether to hide or show secondaries.
     */
    WPropBool m_showSecondaries;

    /**
     * Set lower border of range of eventID selection.
     */
    WPropInt m_minCap;

    /**
     * Set upper border of range of eventID selection.
     */
    WPropInt m_maxCap;

     /**
     * Decides whether to color points based on the deposited energy or not.
     */
    WPropBool m_colorFromEdep;

    /**
     * Decides whether to scale the points sizes according to their deposited energy.
     */
    WPropBool m_sizesFromEdep;

    /**
     * Property group for filtering options
     */
    WPropGroup m_filteringGroup;

    /**
     * Property group for visualization modes
     */
    WPropGroup m_visualizationGroup;

    /**
     * Property that holds the current selected color when not coloring points based on their deposited energy.
     */
    WPropColor m_colorSelection;


    WMColumnPropertyHandler::SPtr m_columnPropertyHandler;

};




#endif  // WMPROPERTYSTATUS_H