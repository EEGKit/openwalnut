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

#ifndef WMGAUSSFILTERING2_H
#define WMGAUSSFILTERING2_H

#include <map>
#include <string>
#include <vector>

#include <osg/Node>
#include <osg/Geode>
#include <osg/Uniform>

#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"

#include "../../common/math/linearAlgebra/WLinearAlgebra.h"

#include "../../newDataHandler/WGridRegular3D2.h"
#include "../../newDataHandler/WGridRegular3D2Specializations.h"
#include "../../newDataHandler/structuralTypes/WScalarStructural.h"
#include "../../newDataHandler/WDataSet2.h"

/**
 * Gauss filtering for scalar datasets.
 *
 * \note Values outside of the grid will be considered zero.
 *
 * \ingroup modules
 */
class WMGaussFiltering2 : public WModule
{
public:

    //! The type of the dataset.
    typedef WDataSet2< WGridRegular3D2, WScalarStructural > DataSetType;

    /**
     * Standard constructor.
     */
    WMGaussFiltering2();

    /**
     * Destructor.
     */
    ~WMGaussFiltering2();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description of module.
     */
    virtual const std::string getDescription() const;

    /**
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     * Get the icon for this module in XPM format.
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
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * The number of iterations to use for filtering
     */
    WPropInt m_iterations;

    boost::shared_ptr< WModuleInputData< DataSetType > > m_input;  //!< Input connector required by this module.
    boost::shared_ptr< WModuleOutputData< DataSetType > > m_output; //!< The only output of this filter module.
    boost::shared_ptr< DataSetType > m_dataSet; //!< Pointer providing access to the treated data set in the whole module.
};
#endif  // WMGAUSSFILTERING2_H
