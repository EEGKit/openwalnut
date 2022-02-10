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

#include <mutex>

#include "WConditionOneShot.h"

WConditionOneShot::WConditionOneShot()
    : WCondition()
{
    // initialize members
    m_isDone = false;
}

WConditionOneShot::~WConditionOneShot()
{
    // cleanup
}

void WConditionOneShot::wait() const
{
    if( !m_isDone )
    {
        std::unique_lock<std::shared_mutex> lock( m_mutex );
        m_condition.wait( m_mutex, [this]
        {
            return m_isDone.load();
        } );
    }
}

void WConditionOneShot::notify()
{
    m_isDone = true;
    WCondition::notify();
}
