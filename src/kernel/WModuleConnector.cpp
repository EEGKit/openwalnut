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

#include <iostream>
#include <list>
#include <string>
#include <sstream>
#include <set>

#include <boost/exception.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/signals2/connection.hpp>

#include "WModule.h"
#include "WModuleContainer.h"
#include "exceptions/WModuleConnectionFailed.h"
#include "exceptions/WModuleConnectionInvalid.h"
#include "exceptions/WModuleDisconnectFailed.h"
#include "exceptions/WModuleSignalSubscriptionFailed.h"
#include "exceptions/WModuleConnectorsIncompatible.h"
#include "WModuleConnectorSignals.h"

#include "WModuleConnector.h"

WModuleConnector::WModuleConnector( boost::shared_ptr<WModule> module, std::string name, std::string description ):
    boost::enable_shared_from_this<WModuleConnector>()
{
    // initialize members
    m_module = module;

    m_name = name;
    m_description = description;

    // connect standard signals
    // NOTE: these signals are NOT emitted by the connector this one is connected to, since a module can't send a "connection
    // closed" message if the connection is closed.
    subscribeSignal( CONNECTION_ESTABLISHED, boost::bind( &WModuleConnector::notifyConnectionEstablished, this, _1, _2 ) );
    subscribeSignal( CONNECTION_CLOSED, boost::bind( &WModuleConnector::notifyConnectionClosed, this, _1, _2 ) );

    signal_ConnectionEstablished.connect( getSignalHandler( CONNECTION_ESTABLISHED ) );
    signal_ConnectionClosed.connect( getSignalHandler( CONNECTION_CLOSED ) );
}

WModuleConnector::~WModuleConnector()
{
    disconnectAll();

    // cleanup
    signal_ConnectionEstablished.disconnect_all_slots();
    signal_ConnectionClosed.disconnect_all_slots();
}

bool WModuleConnector::isConnectedTo( boost::shared_ptr<WModuleConnector> con )
{
    boost::shared_lock<boost::shared_mutex> slock;
    slock = boost::shared_lock<boost::shared_mutex>( m_connectionListLock );
    int c1 = m_connected.count( con );
    slock.unlock();

    slock = boost::shared_lock<boost::shared_mutex>( con->m_connectionListLock );
    int c2 = con->m_connected.count( shared_from_this() );
    slock.unlock();

    // if the count is different the connection is invalid
    if ( c1 != c2 )
    {
        std::ostringstream s;
        s << "Connection between " << getCanonicalName() << " and " << con->getCanonicalName() << " failed.";
        throw WModuleConnectionInvalid( s.str() );
    }

    return ( c1 == 1 );
}

void WModuleConnector::connect( boost::shared_ptr<WModuleConnector> con )
{
    boost::shared_ptr< WModuleContainer > container = m_module->getAssociatedContainer();
    std::string containerName = container.get() ? container->getName() : "Unknown";
    WLogger::getLogger()->addLogMessage( "Connecting " + con->getCanonicalName() + " with " + getCanonicalName()
            , "ModuleContainer (" + containerName + ")", LL_INFO );

    // are both partners compatible to each other?
    if ( !( con->connectable( shared_from_this() ) && connectable( con ) ) )
    {
        std::ostringstream s;
        s << "Connection between " << getCanonicalName() << " and " << con->getCanonicalName() << " failed.";
        throw WModuleConnectorsIncompatible( s.str() );
    }

    // check whether they are already connected
    if ( isConnectedTo( con ) )
    {
        // is this worth an exception?
        return;
    }

    boost::unique_lock<boost::shared_mutex> lock;
    try
    {
        // add to list
        lock = boost::unique_lock<boost::shared_mutex>( m_connectionListLock );
        m_connected.insert( con );
        lock.unlock();

        // add to list of partner
        lock = boost::unique_lock<boost::shared_mutex>( con->m_connectionListLock );
        con->m_connected.insert( shared_from_this() );
        lock.unlock();
    }
    catch( const std::exception& e )
    {
        lock.unlock();

        // undo changes
        m_connected.erase( con );
        con->m_connected.erase( con );

        std::ostringstream s;
        s << "Connection between " << getCanonicalName() << " and " << con->getCanonicalName() << " failed.";
        throw WModuleConnectionFailed( s.str() );
    }
    catch( const boost::exception& e )
    {
        lock.unlock();

        // undo changes
        m_connected.erase( con );
        con->m_connected.erase( con );

        std::ostringstream s;
        s << "Connection between " << getCanonicalName() << " and " << con->getCanonicalName() << " failed.";
        throw WModuleConnectionFailed( s.str() );
    }

    // let them connect their signals
    connectSignals( con );
    con->connectSignals( shared_from_this() );

    // signal "connection established"
    signal_ConnectionEstablished( shared_from_this(), con );
    // signal to my partner, of course with the parameters the other way round
    con->signal_ConnectionEstablished( con, shared_from_this() );
}

void WModuleConnector::connectSignals( boost::shared_ptr<WModuleConnector> /*con*/ )
{
    // Add extra signal- connections here that are COMMON to ALL connectors.
    // NOTE: connection established and connection closed are not signals to connect, since you can not send an connection closed
    // signal to somebody with whom you are not connected anymore ;-).
}

void WModuleConnector::disconnectSignals( boost::shared_ptr<WModuleConnector> /*con*/ )
{
    // The base module does not subscribe to any signal -> no disconnection needed here
}

boost::signals2::connection WModuleConnector::subscribeSignal( MODULE_CONNECTOR_SIGNAL signal,
                                                               t_GenericSignalHandlerType notifier )
{
    switch (signal)
    {
        case CONNECTION_ESTABLISHED:
            return signal_ConnectionEstablished.connect( notifier );
        case CONNECTION_CLOSED:
            return signal_ConnectionClosed.connect( notifier );
        default:
            std::ostringstream s;
            s << "Could not subscribe to unknown signal. You need to implement this signal type explicitly.";
            throw WModuleSignalSubscriptionFailed( s.str() );
            break;
    }
}

const t_GenericSignalHandlerType WModuleConnector::getSignalHandler( MODULE_CONNECTOR_SIGNAL signal )
{
    // the module instance knows that
    return m_module->getSignalHandler( signal );
}

void WModuleConnector::disconnect( boost::shared_ptr<WModuleConnector> con, bool removeFromOwnList )
{
    if ( !isConnectedTo( con ) )
    {
        return;
    }

    // write lock
    boost::unique_lock<boost::shared_mutex> lock;
    try
    {
        // disconnect all signals
        con->disconnectSignals( shared_from_this() );
        disconnectSignals( con );

        // remove from list
        if ( removeFromOwnList )
        {
            lock = boost::unique_lock<boost::shared_mutex>( m_connectionListLock );
            // since we use shared pointers, erasing the item should be enough
            m_connected.erase( con );
            lock.unlock();
        }

        // remove me from his list
        lock = boost::unique_lock<boost::shared_mutex>( con->m_connectionListLock );
        con->m_connected.erase( shared_from_this() );
        lock.unlock();

        // signal closed connection
        signal_ConnectionClosed( shared_from_this(), con );
        con->signal_ConnectionClosed( shared_from_this(), con );
    }
    catch( const std::exception& e )
    {
        lock.unlock();

        std::ostringstream s;
        s << "Unable to disconnect " << getCanonicalName() << " from " << con->getCanonicalName() << ".";
        throw WModuleDisconnectFailed( s.str() );
    }
    catch( const boost::exception& e )
    {
        lock.unlock();

        std::ostringstream s;
        s << "Unable to disconnect " << getCanonicalName() << " from " << con->getCanonicalName() << ".";
        throw WModuleDisconnectFailed( s.str() );
    }
}

void WModuleConnector::disconnectAll()
{
    // remove from list

    // acquire read lock
    boost::shared_lock<boost::shared_mutex> rlock( m_connectionListLock );

    // each connector needs to be notified and disconnected properly
    for( std::set<boost::shared_ptr<WModuleConnector> >::iterator listIter = m_connected.begin(); listIter != m_connected.end();
         ++listIter )
    {
        disconnect( *listIter, false );
    }
    rlock.unlock();

    // lock it for writing
    boost::unique_lock<boost::shared_mutex> lock( m_connectionListLock );
    m_connected.clear();
    lock.unlock();
}

const std::string WModuleConnector::getDescription() const
{
    return m_description;
}

const std::string WModuleConnector::getName() const
{
    return m_name;
}

const std::string WModuleConnector::getCanonicalName() const
{
    std::ostringstream s;
    s << m_module->getName() << ":" << getName();

    return s.str();
}

void WModuleConnector::setDescription( std::string desc )
{
    m_description = desc;
}

void WModuleConnector::setName( std::string name )
{
    m_name = name;
}

void WModuleConnector::notifyConnectionEstablished( boost::shared_ptr<WModuleConnector> /*here*/, boost::shared_ptr<WModuleConnector> /*there*/ )
{
    // by default: do nothing.
}

void WModuleConnector::notifyConnectionClosed( boost::shared_ptr<WModuleConnector> /*here*/, boost::shared_ptr<WModuleConnector> /*there*/ )
{
    // do nothing by default
}

