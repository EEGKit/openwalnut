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

#ifndef WMMERGEMESHES_H
#define WMMERGEMESHES_H

#include <memory>
#include <string>

#include "core/graphicsEngine/WTriangleMesh.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

/**
 * This modules takes some mesh data and merges them into one new mesh dataset
 *
 * \ingroup modules
 */
class WMMergeMeshes: public WModule
{
public:
    /**
     * Default constructor.
     */
    WMMergeMeshes();

    /**
     * Destructor.
     */
    virtual ~WMMergeMeshes();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description to module.
     */
    virtual const std::string getDescription() const;

    /**
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual std::shared_ptr< WModule > factory() const;

    /**
     * Get the icon for this module in XPM format.
     * \return The icon.
     */
    virtual const char** getXPMIcon() const;

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
     * The first input data.
     */
    std::shared_ptr< WModuleInputData< WTriangleMesh > > m_meshInput1;

    /**
     * The second input data.
     */
    std::shared_ptr< WModuleInputData< WTriangleMesh > > m_meshInput2;

    /**
     * The output connector used to provide the calculated mesh data to other modules.
     */
    std::shared_ptr< WModuleOutputData< WTriangleMesh > > m_meshOutput;

    /**
     * A condition used to notify about changes in several properties.
     */
    std::shared_ptr< WCondition > m_propCondition;
};

#endif  // WMMERGEMESHES_H

