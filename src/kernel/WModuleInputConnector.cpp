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

#include <string>

#include "WModule.h"
#include "WModuleOutputConnector.h"
#include "WModuleConnectorSignals.h"

#include "WModuleInputConnector.h"

WModuleInputConnector::WModuleInputConnector( boost::shared_ptr<WModule> module, std::string name, std::string description ):
    WModuleConnector( module, name, description )
{
    // initialize members

    // connect some signals
    // This signal is some kind of "forwarder" for the data_changed signal of an output connector.
    signal_DataChanged.connect( getSignalHandler( DATA_CHANGED ) );
}

WModuleInputConnector::~WModuleInputConnector()
{
    // cleanup
    m_DataChangedConnection.disconnect();
}

bool WModuleInputConnector::connectable( boost::shared_ptr<WModuleConnector> con )
{
    // output connectors are just allowed to get connected with input connectors
    if ( dynamic_cast<WModuleOutputConnector*>( con.get() ) )   // NOLINT - since we really need them here
    {
        return true;
    }
    return false;
}

void WModuleInputConnector::connectSignals( boost::shared_ptr<WModuleConnector> con )
{
    WModuleConnector::connectSignals( con );

    // connect dataChange signal with an internal handler to ensure we can add the "input" connector pointer, since the output
    // connector does not set this information.
    // NOTE: con will be an WModuleOutputConnector
    m_DataChangedConnection = con->subscribeSignal( DATA_CHANGED,
            boost::bind( &WModuleInputConnector::notifyDataChange, this, _1, _2 ) 
    );
}

void WModuleInputConnector::disconnectSignals( boost::shared_ptr<WModuleConnector> con )
{
    m_DataChangedConnection.disconnect();

    WModuleConnector::disconnectSignals( con );
}

void WModuleInputConnector::notifyDataChange( boost::shared_ptr<WModuleConnector> /*input*/,
                                              boost::shared_ptr<WModuleConnector> output )
{
    // since the output connector is not able to fill the parameter "input" we need to forward this message and fill it with the
    // proper information
    signal_DataChanged( shared_from_this(), output );
}

