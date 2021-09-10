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

#include "WPropertyStatus.h"


WPropertyStatus::WPropertyStatus()
{
}

WColumnPropertyHandler::SPtr WPropertyStatus::getColumnPropertyHandler()
{
    return m_columnPropertyHandler;
}

void WPropertyStatus::setColumnPropertyHandler( WColumnPropertyHandler::SPtr columnPropertyHandler )
{
    m_columnPropertyHandler = columnPropertyHandler;
}

WFilterPropertyHandler::SPtr WPropertyStatus::getFilterPropertyHandler()
{
    return m_filterPropertyHandler;
}

void WPropertyStatus::setFilterPropertyHandler( WFilterPropertyHandler::SPtr filterPropertyHandler )
{
    m_filterPropertyHandler = filterPropertyHandler;
}

WVisualizationPropertyHandler::SPtr WPropertyStatus::getVisualizationPropertyHandler()
{
    return m_visualizationPropertyHandler;
}

void WPropertyStatus::setVisualizationPropertyHandler( WVisualizationPropertyHandler::SPtr visualizationPropertyHandler )
{
    m_visualizationPropertyHandler = visualizationPropertyHandler;
}

WEventIDLimitationPropertyHandler::SPtr WPropertyStatus::getEventIDLimitationPropertyHandler()
{
    return m_eventIDLimitationPropertyHandler;
}

void WPropertyStatus::setEventIDLimitationPropertyHandler( WEventIDLimitationPropertyHandler::SPtr eventIDLimitationPropertyHandler )
{
    m_eventIDLimitationPropertyHandler = eventIDLimitationPropertyHandler;
}

WOutputPropertyHandler::SPtr WPropertyStatus::getOutputPropertyHandler()
{
    return m_outputPropertyHandler;
}

void WPropertyStatus::setOutputPropertyHandler( WOutputPropertyHandler::SPtr outputPropertyHandler )
{
    m_outputPropertyHandler = outputPropertyHandler;
}
