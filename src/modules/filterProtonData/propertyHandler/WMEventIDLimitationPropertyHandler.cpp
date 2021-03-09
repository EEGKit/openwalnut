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
    WPropertyBase::PropertyChangeNotifierType eventIDNotifier = boost::bind( &WMEventIDLimitationPropertyHandler::updateMesh, this );
    WPropGroup eventIDGroup = m_properties->addPropertyGroup( "Event iD limitation", "Adjust the range of eventIDs to be shown.", 0 );
    m_minCap = eventIDGroup->addProperty( "Minimum event id", "Filters out every event id which is lower than the set value.", 1 );
    m_maxCap = eventIDGroup->addProperty( "Maximum event id", "Filters out every event id which is higher than the set value.", 2000 );
    m_applySelection = eventIDGroup->addProperty( "Set selection", "Apply", WPVBaseTypes::PV_TRIGGER_READY, eventIDNotifier );

    determineMinMaxEventID();

    updateProperty();
}

void WMEventIDLimitationPropertyHandler::updateProperty()
{
    if(m_protonData->isColumnAvailable("eventID"))
    {
        m_minCap->setHidden( false );
        m_maxCap->setHidden( false );
    }
    else
    {
        m_minCap->setHidden( true );
        m_maxCap->setHidden( true );
    }
}

void WMEventIDLimitationPropertyHandler::updateMesh()
{
    determineMinMaxEventID();
    m_dataUpdate( );

    if( m_applySelection->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
    {
        m_applySelection->set( WPVBaseTypes::PV_TRIGGER_READY, false );
    }
}

void WMEventIDLimitationPropertyHandler::determineMinMaxEventID()
{
    int eventIDIndex = m_protonData->getColumnIndex( "eventID" );

    if(eventIDIndex < 0)
    {
        return;
    }

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
