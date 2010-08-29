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

#ifndef WSHAREDASSOCIATIVECONTAINER_H
#define WSHAREDASSOCIATIVECONTAINER_H

#include <boost/thread.hpp>

#include "WSharedObject.h"

/**
 * This class provides a common interface for thread-safe access to associative containers (set, multiset, map, multimap).
 */
template < typename T >
class WSharedAssociativeContainer: public WSharedObject< T >
{
public:
    // Some helpful typedefs

    /**
     * A typedef for the correct const iterator useful to traverse this sequence container.
     */
    typedef typename T::const_iterator   ConstIterator;

    /**
     * A typedef for the correct iterator to traverse this sequence container.
     */
    typedef typename T::iterator         Iterator;

    /**
     * Default constructor.
     */
    WSharedAssociativeContainer();

    /**
     * Destructor.
     */
    virtual ~WSharedAssociativeContainer();

    /**
     * Clears the container.
     */
    void clear();

protected:
private:
};

template < typename T >
WSharedAssociativeContainer< T >::WSharedAssociativeContainer():
    WSharedObject< T >()
{
    // init members
}

template < typename T >
WSharedAssociativeContainer< T >::~WSharedAssociativeContainer()
{
    // clean up
}

template < typename T >
void WSharedAssociativeContainer< T >::clear()
{
    typename WSharedAssociativeContainer< T >::WriteTicket w = WSharedObject< T >::getWriteTicket();
    w->get().clear();
}

#endif  // WSHAREDASSOCIATIVECONTAINER_H

