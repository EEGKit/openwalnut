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

#include "WMPropertyStatus.h"

WMPropertyStatus::WMPropertyStatus()
{
    m_showPrimaries = NULL;
    m_showSecondaries = NULL;
    m_minCap = NULL;
    m_maxCap = NULL;
    m_colorFromEdep = NULL;
    m_sizesFromEdep = NULL;
    m_filteringGroup = NULL;
    m_visualizationGroup = NULL;
    m_colorSelection = NULL;
    m_columnPropertyHandler = NULL;
}

WPropBool WMPropertyStatus::getShowPrimaries()
{
    return m_showPrimaries;
}

void WMPropertyStatus::setShowPrimaries( WPropBool showPrimaries )
{
    m_showPrimaries = showPrimaries;
}

WPropBool WMPropertyStatus::getShowSecondaries()
{
    return m_showSecondaries;
}

void WMPropertyStatus::setShowSecondaries( WPropBool showSecondaries )
{
    m_showSecondaries = showSecondaries;
}

WPropInt WMPropertyStatus::getMinCap()
{
    return m_minCap;
}

void WMPropertyStatus::setMinCap( WPropInt minCap )
{
    m_minCap = minCap;
}

WPropInt WMPropertyStatus::getMaxCap()
{
    return m_maxCap;
}

void WMPropertyStatus::setMaxCap( WPropInt maxCap )
{
    m_maxCap = maxCap;
}

WPropBool WMPropertyStatus::getColorFromEdep()
{
    return m_colorFromEdep;
}

void WMPropertyStatus::setColorFromEdep( WPropBool colorFromEdep)
{
    m_colorFromEdep = colorFromEdep;
}

WPropBool WMPropertyStatus::getSizesFromEdep()
{
    return m_sizesFromEdep;
}

void WMPropertyStatus::setSizesFromEdep( WPropBool sizesFromEdep)
{
    m_sizesFromEdep = sizesFromEdep;
}

WPropGroup WMPropertyStatus::getFilteringGroup()
{
    return m_filteringGroup;
}

void WMPropertyStatus::setFilteringGroup( WPropGroup filteringGroup)
{
    m_filteringGroup = filteringGroup;
}

WPropGroup WMPropertyStatus::getVisualizationGroup()
{
    return m_visualizationGroup;
}

void WMPropertyStatus::setVisualizationGroup( WPropGroup visualizationGroup)
{
    m_visualizationGroup = visualizationGroup;
}

WPropColor WMPropertyStatus::getColorSelection()
{
    return m_colorSelection;
}

void WMPropertyStatus::setColorSelection( WPropColor colorSelection)
{
    m_colorSelection = colorSelection;
}

WMColumnPropertyHandler::SPtr WMPropertyStatus::getColumnPropertyHandler()
{
    return m_columnPropertyHandler;
}
    
void WMPropertyStatus::setColumnPropertyHandler( WMColumnPropertyHandler::SPtr columnPropertyHandler)
{
    m_columnPropertyHandler = columnPropertyHandler;
}

#include "WMPropertyStatus.h"