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

#include "WConditionOneShot.h"

WConditionOneShot::WConditionOneShot()
    : WCondition()
{
    // initialize members
    m_lock = std::unique_lock<std::shared_mutex>( m_mutex );
}

WConditionOneShot::~WConditionOneShot()
{
    // cleanup
    try
    {
        m_lock.unlock();
    }
    catch( const std::system_error &e )
    {
        // ignore this particular error since it is thrown when the lock is not locked anymore
    }
}

void WConditionOneShot::wait() const
{
    if( m_lock.owns_lock() )
    {
        m_condition.wait( m_mutex );
    }
}

void WConditionOneShot::notify()
{
    try
    {
        m_lock.unlock();
    }
    catch( const std::system_error &e )
    {
        // ignore this particular error since it is thrown when the lock is not locked anymore
        // because the notify was called multiple times
    }

    WCondition::notify();
}

