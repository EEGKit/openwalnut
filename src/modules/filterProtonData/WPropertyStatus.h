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

#ifndef WPROPERTYSTATUS_H
#define WPROPERTYSTATUS_H

#include <memory>


#include "core/kernel/WModule.h"
#include "propertyHandler/WColumnPropertyHandler.h"
#include "propertyHandler/WEventIDLimitationPropertyHandler.h"
#include "propertyHandler/WFilterPropertyHandler.h"
#include "propertyHandler/WVisualizationPropertyHandler.h"
#include "propertyHandler/WOutputPropertyHandler.h"


/**
 * Holds references to all the property handlers.
 */
class WPropertyStatus
{
public:
    /**
     * A shared_ptr to this class.
     */
    typedef std::shared_ptr< WPropertyStatus > SPtr;

    /**
     * An empty constructor for class creation.
     */
    WPropertyStatus();

    /**
     * getter for m_columnPropertyHandler
     * \return return m_columnPropertyHandler
     */
    WColumnPropertyHandler::SPtr getColumnPropertyHandler();

    /**
     * setter for m_columnPropertyHandler
     * \param columnPropertyHandler setter variable for m_columnPropertyHandler
     */
    void setColumnPropertyHandler( WColumnPropertyHandler::SPtr columnPropertyHandler );

    /**
     * getter for m_filterPropertyHandler
     * \return return m_filterPropertyHandler
     */
    WFilterPropertyHandler::SPtr getFilterPropertyHandler();

    /**
     * setter for m_filterPropertyHandler
     * \param filterPropertyHandler setter variable for m_filterPropertyHandler
     */
    void setFilterPropertyHandler( WFilterPropertyHandler::SPtr filterPropertyHandler );

    /**
     * getter for m_visualizationPropertyHandler
     * \return return m_visualizationPropertyHandler
     */
    WVisualizationPropertyHandler::SPtr getVisualizationPropertyHandler();

    /**
     * setter for m_visualizationPropertyHandler
     * \param visualizationPropertyHandler setter variable for m_visualizationPropertyHandler
     */
    void setVisualizationPropertyHandler( WVisualizationPropertyHandler::SPtr visualizationPropertyHandler );

    /**
     * getter for m_eventIDLimitationPropertyHandler
     * \return return m_eventIDLimitationPropertyHandler
     */
    WEventIDLimitationPropertyHandler::SPtr getEventIDLimitationPropertyHandler();

    /**
     * setter for m_eventIDLimitationPropertyHandler
     * \param eventIDLimitationPropertyHandler setter variable for m_eventIDLimitationPropertyHandler
     */
    void setEventIDLimitationPropertyHandler( WEventIDLimitationPropertyHandler::SPtr eventIDLimitationPropertyHandler );

    /**
     * getter for m_outputPropertyHandler
     * \return WOutputPropertyHandler::SPtr m_outputPropertyHandler
     */
    WOutputPropertyHandler::SPtr getOutputPropertyHandler();

    /**
     * setter for m_outputPropertyHandler
     * \param outputPropertyHandler setter variable for m_outputPropertyHandler
     */
    void setOutputPropertyHandler( WOutputPropertyHandler::SPtr outputPropertyHandler );

private:
    /**
     * pointer to group property and the subproperty for columnselection   
     */
    WColumnPropertyHandler::SPtr m_columnPropertyHandler;

    /**
     * pointer to group property and the subproperty for filter the CSV-Data   
     */
    WFilterPropertyHandler::SPtr m_filterPropertyHandler;

    /**
     * pointer to group property and the subproperty for scaling and color handling
     */
    WVisualizationPropertyHandler::SPtr m_visualizationPropertyHandler;

    /**
     * pointer to group property and the subproperty for Limitation of eventID
     */
    WEventIDLimitationPropertyHandler::SPtr m_eventIDLimitationPropertyHandler;

    /**
     * pointer to group property and subproperty for the output
     */
    WOutputPropertyHandler::SPtr m_outputPropertyHandler;
};
#endif  // WPROPERTYSTATUS_H
