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

#ifndef WSHAREDOBJECTTICKET_H
#define WSHAREDOBJECTTICKET_H
#include <iostream>
#include <boost/shared_ptr.hpp>

#include "WCondition.h"
#include "WSharedObjectTicket.h"

// The shared object class
template < typename T >
class WSharedObject;

/**
 * Class which represents granted access to a locked object. It contains a reference to the object and a lock. The lock is freed after the ticket
 * has been destroyed.
 */
template < typename Data >
class WSharedObjectTicket
{
// the shared object class needs protected access to create new instances
friend class WSharedObject< Data >;
public:

    /**
     * Destroys the ticket and releases the lock.
     */
    virtual ~WSharedObjectTicket()
    {
        // NOTE: the derived destructor already unlocks.
        if ( m_condition )
        {
            m_condition->notify();
        }
    };

    /**
     * Returns the protected data. As long as you own the ticket, you are allowed to use it.
     *
     * \return the data
     */
    Data& get() const
    {
        return m_data;
    };

protected:

    /**
     * Create a new instance. It is protected to avoid someone to create them. It locks the mutex.
     *
     * \param data the data to protect
     * \param mutex the mutex used to lock
     * \param condition a condition that should be fired upon unlock. Can be NULL.
     */
    WSharedObjectTicket( Data& data, boost::shared_ptr< boost::shared_mutex > mutex, boost::shared_ptr< WCondition > condition ): // NOLINT
        m_data( data ),
        m_mutex( mutex ),
        m_condition( condition )
    {
    };

    /**
     * The data to which access is allowed by the ticket
     */
    Data& m_data;

    /**
     * The mutex used for locking.
     */
    boost::shared_ptr< boost::shared_mutex > m_mutex;

    /**
     * A condition which gets notified after unlocking. Especially useful to notify waiting threads about a change in the object.
     */
    boost::shared_ptr< WCondition > m_condition;

    /**
     * Unlocks the mutex.
     */
    virtual void unlock() = 0;

private:
};

/**
 * Class which represents granted access to a locked object. It contains a reference to the object and a lock. The lock is freed after the ticket
 * has been destroyed.  This class especially implements the shared (read) lock.
 */
template < typename Data >
class WSharedObjectTicketRead: public WSharedObjectTicket< Data >
{
// the shared object class needs protected access to create new instances
friend class WSharedObject< Data >;
public:

    /**
     * Destroys the ticket and releases the lock.
     */
    virtual ~WSharedObjectTicketRead()
    {
        unlock();
    };

protected:

    /**
     * Create a new instance. It is protected to avoid someone to create them. It locks the mutex for read.
     *
     * \param data the data to protect
     * \param mutex the mutex used to lock
     * \param condition a condition that should be fired upon unlock. Can be NULL.
     */
    WSharedObjectTicketRead( Data& data, boost::shared_ptr< boost::shared_mutex > mutex, boost::shared_ptr< WCondition > condition ): // NOLINT
        WSharedObjectTicket< Data >( data, mutex, condition ),
        m_lock( boost::shared_lock< boost::shared_mutex >( *mutex ) )
    {
    };

    /**
     * The lock.
     */
    boost::shared_lock< boost::shared_mutex > m_lock;

    /**
     * Unlocks the mutex.
     */
    virtual void unlock()
    {
        m_lock.unlock();
    }

private:
};

/**
 * Class which represents granted access to a locked object. It contains a reference to the object and a lock. The lock is freed after the ticket
 * has been destroyed. This class especially implements the exclusive (write) lock.
 */
template < typename Data >
class WSharedObjectTicketWrite: public WSharedObjectTicket< Data >
{
// the shared object class needs protected access to create new instances
friend class WSharedObject< Data >;
public:

    /**
     * Destroys the ticket and releases the lock.
     */
    virtual ~WSharedObjectTicketWrite()
    {
        unlock();
    };

protected:

    /**
     * Create a new instance. It is protected to avoid someone to create them. It locks the mutex for write.
     *
     * \param data the data to protect
     * \param mutex the mutex used to lock
     * \param condition a condition that should be fired upon unlock. Can be NULL.
     */
    WSharedObjectTicketWrite( Data& data, boost::shared_ptr< boost::shared_mutex > mutex, boost::shared_ptr< WCondition > condition ): // NOLINT
        WSharedObjectTicket< Data >( data, mutex, condition ),
        m_lock( boost::unique_lock< boost::shared_mutex >( *mutex ) )
    {
    };

    /**
     * The lock.
     */
    boost::unique_lock< boost::shared_mutex > m_lock;

    /**
     * Unlocks the mutex.
     */
    virtual void unlock()
    {
        m_lock.unlock();
    }

private:
};

#endif  // WSHAREDOBJECTTICKET_H

