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

    WMColumnPropertyHandler::SPtr getColumnPropertyHandler();
    void setColumnPropertyHandler( WMColumnPropertyHandler::SPtr columnPropertyHandler);

    WMFilterPropertyHandler::SPtr getFilterPropertyHandler();
    void setFilterPropertyHandler( WMFilterPropertyHandler::SPtr filterPropertyHandler);

    WMVisualizationPropertyHandler::SPtr getVisualizationPropertyHandler();
    void setVisualizationPropertyHandler( WMVisualizationPropertyHandler::SPtr visualizationPropertyHandler);

    WMEventIDLimitationPropertyHandler::SPtr getEventIDLimitationPropertyHandler();
    void setEventIDLimitationPropertyHandler( WMEventIDLimitationPropertyHandler::SPtr eventIDLimitationPropertyHandler);

private:

    WMColumnPropertyHandler::SPtr m_columnPropertyHandler;

    WMFilterPropertyHandler::SPtr m_filterPropertyHandler;

    WMVisualizationPropertyHandler::SPtr m_visualizationPropertyHandler;

    WMEventIDLimitationPropertyHandler::SPtr m_eventIDLimitationPropertyHandler;
    
};




#endif  // WMPROPERTYSTATUS_H