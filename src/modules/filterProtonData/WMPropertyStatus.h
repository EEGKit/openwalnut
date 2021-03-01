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

#include <boost/shared_ptr.hpp>

#include "core/kernel/WModule.h"

#include "propertyHandler/WMColumnPropertyHandler.h"
#include "propertyHandler/WMFilterPropertyHandler.h"
#include "propertyHandler/WMVisualizationPropertyHandler.h"
#include "propertyHandler/WMEventIDLimitationPropertyHandler.h"

class WMPropertyStatus
{
public:
    typedef boost::shared_ptr< WMPropertyStatus > SPtr;

    WMPropertyStatus();

    /**
     * getter for m_columnPropertyHandler
     * \return return m_columnPropertyHandler
     */
    WMColumnPropertyHandler::SPtr getColumnPropertyHandler();

    /**
     * setter for m_columnPropertyHandler
     * \param columnPropertyHandler setter variable for m_columnPropertyHandler
     */
    void setColumnPropertyHandler( WMColumnPropertyHandler::SPtr columnPropertyHandler );

    /**
     * getter for m_filterPropertyHandler
     * \return return m_filterPropertyHandler
     */
    WMFilterPropertyHandler::SPtr getFilterPropertyHandler();

    /**
     * setter for m_filterPropertyHandler
     * \param filterPropertyHandler setter variable for m_filterPropertyHandler
     */
    void setFilterPropertyHandler( WMFilterPropertyHandler::SPtr filterPropertyHandler );

    /**
     * getter for m_visualizationPropertyHandler
     * \return return m_visualizationPropertyHandler
     */
    WMVisualizationPropertyHandler::SPtr getVisualizationPropertyHandler();

    /**
     * setter for m_visualizationPropertyHandler
     * \param visualizationPropertyHandler setter variable for m_visualizationPropertyHandler
     */
    void setVisualizationPropertyHandler( WMVisualizationPropertyHandler::SPtr visualizationPropertyHandler );

    /**
     * getter for m_eventIDLimitationPropertyHandler
     * \return return m_eventIDLimitationPropertyHandler
     */
    WMEventIDLimitationPropertyHandler::SPtr getEventIDLimitationPropertyHandler();

    /**
     * setter for m_eventIDLimitationPropertyHandler
     * \param eventIDLimitationPropertyHandler setter variable for m_eventIDLimitationPropertyHandler
     */
    void setEventIDLimitationPropertyHandler( WMEventIDLimitationPropertyHandler::SPtr eventIDLimitationPropertyHandler );

private:
    /**
     * pointer to group property and the subproperty for columnselection   
     */
    WMColumnPropertyHandler::SPtr m_columnPropertyHandler;

    /**
     * pointer to group property and the subproperty for filter the CSV-Data   
     */
    WMFilterPropertyHandler::SPtr m_filterPropertyHandler;

    /**
     * pointer to group property and the subproperty for ************
     */
    WMVisualizationPropertyHandler::SPtr m_visualizationPropertyHandler;

    /**
     * pointer to group property and the subproperty for Limitation of eventID
     */
    WMEventIDLimitationPropertyHandler::SPtr m_eventIDLimitationPropertyHandler;
};
#endif  // WMPROPERTYSTATUS_H
