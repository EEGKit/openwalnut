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

#ifndef WMVECTOROPERATOR_H
#define WMVECTOROPERATOR_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <osg/Geode>
#include <osg/Node>
#include <osg/Uniform>

#include "core/dataHandler/WDataSetScalar.h"
#include "core/dataHandler/WDataSetVector.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"

/**
 * Operators for processing two WDataSetScalar.
 *
 * \ingroup modules
 */
class WMVectorOperator: public WModule
{
public:
    /**
     * Standard constructor.
     */
    WMVectorOperator();

    /**
     * Destructor.
     */
    ~WMVectorOperator();

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
     * Visitor for discriminating the type of the first valueset.
     */
    class VisitorVSetA: public boost::static_visitor< std::shared_ptr< WValueSetBase > >
    {
    public:
        /**
         * Create visitor instance.
         *
         * \param opIdx The operator index.
         * \param grid the underlying grid
         */
        VisitorVSetA( std::shared_ptr< WGridRegular3D > grid, size_t opIdx = 0 );

        /**
         * Called by boost::varying during static visiting.
         *
         * \tparam T the real integral type of the first value set.
         * \param vsetA the first valueset currently visited.
         *
         * \return the result from the operation
         */
        template < typename T >
        result_type operator()( const WValueSet< T >* const& vsetA ) const;             // NOLINT

        /**
         * The underlying grid.
         */
        std::shared_ptr< WGridRegular3D > m_grid;

        /**
         * The operator index.
         */
        size_t m_opIdx;
    };

    /**
     * A condition used to notify about changes in several properties.
     */
    std::shared_ptr< WCondition > m_propCondition;

    /**
     * A list of operations that are possible
     */
    std::shared_ptr< WItemSelection > m_operations;

    /**
     * The currently selected operation.
     */
    WPropSelection m_opSelection;

    std::shared_ptr< WModuleInputData< WDataSetVector > > m_inputA;  //!< Input connector required by this module.

    std::shared_ptr< WModuleOutputData< WDataSetScalar > > m_output; //!< The only output of this filter module.
};
#endif  // WMVECTOROPERATOR_H

