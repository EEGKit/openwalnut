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

#ifndef WMDISTANCEMAPISOSURFACE_H
#define WMDISTANCEMAPISOSURFACE_H

#include <string>

#include "../../kernel/WModule.h"
#include "../../kernel/WModuleContainer.h"
#include "../../kernel/WModuleInputData.h"
#include "../../kernel/WModuleInputForwardConnector.h"
#include "../../kernel/WModuleOutputData.h"

/**
 * Computes a distance map from an anatomy dataset and renders it as isosurface. It is a composition of marching cubes and the
 * distance map module.
 * \ingroup modules
 */
class WMDistanceMapIsosurface : public WModuleContainer
{
/**
 * Only UnitTests may be friends.
 */
friend class WMDistanceMapIsosurfaceTest;

public:
    /**
     * Standard constructor.
     */
    WMDistanceMapIsosurface();

    /**
     * Destructor.
     */
    ~WMDistanceMapIsosurface();

    /**
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

protected:
    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void moduleMain();

    /**
     * Initialize the connectors this module is using.
     */
    virtual void connectors();

    /**
     * Initialize the properties for this module.
     */
    virtual void properties();

private:

    /**
     * The description of the module. This is used for the container.
     */
    std::string m_name;

    /**
     * The description of the module. This is used for the container.
     */
    std::string m_description;

    boost::shared_ptr< WModuleInputForwardConnector< WDataSetSingle > > m_input;  //!< Input connector required by this module.

    /**
     * Connector to provide the distance map to other modules.
     */
    boost::shared_ptr< WModuleOutputData< WDataSetSingle > > m_output;

    /**
     * The marching cubes module used in this container.
     */
    boost::shared_ptr< WModule > m_marchingCubesModule;

    /**
     * The distance map module used in this container.
     */
    boost::shared_ptr< WModule > m_distanceMapModule;
};

#endif  // WMDISTANCEMAPISOSURFACE_H
