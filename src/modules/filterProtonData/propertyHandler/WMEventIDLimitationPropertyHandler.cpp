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


#include "WMEventIDLimitationPropertyHandler.h"

WMEventIDLimitationPropertyHandler::WMEventIDLimitationPropertyHandler( WMProtonData::SPtr protonData,
                                            WPropertyGroup::SPtr properties,
                                            WMEventIDLimitationPropertyHandler::CallbackPtr dataUpdate ):
    m_protonData( protonData ),
    m_properties( properties ),
    m_dataUpdate( dataUpdate )
{
}

void WMEventIDLimitationPropertyHandler::createProperties()
{
    WPropertyBase::PropertyChangeNotifierType eventIDNotifier = boost::bind( &WMEventIDLimitationPropertyHandler::updateMesh, this);
    WPropGroup eventIDGroup = m_properties->addPropertyGroup( "Event ID Limitation", "Adjust the range of eventIDs to show.", 0 );
    m_minCap = eventIDGroup->addProperty( "Min Cap", "Set your minium border of your range.", 1, eventIDNotifier );
    m_maxCap = eventIDGroup->addProperty( "Max Cap", "Set your maximum border of your range.", 2000, eventIDNotifier );

    determineMinMaxEventID();
}

void WMEventIDLimitationPropertyHandler::updateMesh( )
{
    determineMinMaxEventID();
    m_dataUpdate( );
}

void WMEventIDLimitationPropertyHandler::determineMinMaxEventID()
{
    int eventIDIndex = m_protonData->getColumnIndex( "eventID" );
    
    int minCap = std::stoi( m_protonData->getCSVData()->front().at( eventIDIndex ) );
    int maxCap = std::stoi( m_protonData->getCSVData()->back().at( eventIDIndex ) );

    m_minCap->setMin( minCap );
    m_minCap->setMax( maxCap );
    m_maxCap->setMin( minCap );
    m_maxCap->setMax( maxCap );

    int currentMinCap = m_minCap->get();
    int currentMaxCap = m_maxCap->get();

    if( currentMaxCap < currentMinCap )
        m_maxCap->set( currentMinCap );

    if( currentMinCap < 0 )
        m_minCap->set( 0 );
}

WPropInt WMEventIDLimitationPropertyHandler::getMinCap()
{
    return m_minCap;
}

WPropInt WMEventIDLimitationPropertyHandler::getMaxCap()
{
    return m_maxCap;
}