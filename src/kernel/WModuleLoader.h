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

#ifndef WMODULELOADER_H
#define WMODULELOADER_H

#include <set>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>

#include "../common/WSharedLib.h"
#include "../common/WSharedAssociativeContainer.h"
#include "../common/WLogger.h"
#include "WModule.h"

/**
 * Loads module prototypes from shared objects in a given directory and injects it into the module factory.
 */
class WModuleLoader
{
public:

	/**
	 * Constructor. It does not load any files. Use load to do this.
	 *
     */
    explicit WModuleLoader();

	/**
	 * Destructor, closes all handles to shared libraries.
	 */
    ~WModuleLoader();

	/**
	 * Load the module prototypes from the shared libraries.
	 *
	 * \param ticket A write ticket to a shared container.
	 */
    void load( WSharedAssociativeContainer< std::set< boost::shared_ptr< WModule > > >::WriteTicket ticket );

    /**
     * Returns the prefix of a shared module library filename.
     *
     * \return the prefix.
     */
    static std::string getModulePrefix();

private:

    /**
     * All the loaded shared libraries. Get freed on destruction. So do NOT free this instance while the libs are used.
     */
    std::vector< WSharedLib > m_libs;

};

#endif  // WMODULELOADER_H
