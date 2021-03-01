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
}

WMColumnPropertyHandler::SPtr WMPropertyStatus::getColumnPropertyHandler()
{
    return m_columnPropertyHandler;
}

void WMPropertyStatus::setColumnPropertyHandler( WMColumnPropertyHandler::SPtr columnPropertyHandler )
{
    m_columnPropertyHandler = columnPropertyHandler;
}

WMFilterPropertyHandler::SPtr WMPropertyStatus::getFilterPropertyHandler()
{
    return m_filterPropertyHandler;
}

void WMPropertyStatus::setFilterPropertyHandler( WMFilterPropertyHandler::SPtr filterPropertyHandler )
{
    m_filterPropertyHandler = filterPropertyHandler;
}

WMVisualizationPropertyHandler::SPtr WMPropertyStatus::getVisualizationPropertyHandler()
{
    return m_visualizationPropertyHandler;
}

void WMPropertyStatus::setVisualizationPropertyHandler( WMVisualizationPropertyHandler::SPtr visualizationPropertyHandler )
{
    m_visualizationPropertyHandler = visualizationPropertyHandler;
}

WMEventIDLimitationPropertyHandler::SPtr WMPropertyStatus::getEventIDLimitationPropertyHandler()
{
    return m_eventIDLimitationPropertyHandler;
}

void WMPropertyStatus::setEventIDLimitationPropertyHandler( WMEventIDLimitationPropertyHandler::SPtr eventIDLimitationPropertyHandler )
{
    m_eventIDLimitationPropertyHandler = eventIDLimitationPropertyHandler;
}
