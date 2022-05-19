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

#ifndef WMSCALAROPERATOR_H
#define WMSCALAROPERATOR_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <osg/Geode>
#include <osg/Node>
#include <osg/Uniform>

#include "core/dataHandler/WDataSetScalar.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"

/**
 * Operators for processing two WDataSetScalar.
 *
 * \ingroup modules
 */
class WMScalarOperator: public WModule
{
public:
    /**
     * Standard constructor.
     */
    WMScalarOperator();

    /**
     * Destructor.
     */
    ~WMScalarOperator();

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
     * The second visitor which got applied to the second value set. It discriminates the integral type and applies the operator in a per value
     * style.
     *
     * \tparam VSetAType The integral type of the first valueset.
     */
    template< typename VSetAType >
    class VisitorVSetB: public boost::static_visitor< std::shared_ptr< WValueSetBase > >
    {
    public:
        /**
         * Creates visitor for the second level of cascading. Takes the first value set as parameter. This visitor applies the operation o to A and
         * B: o(A,B).
         *
         * \param vsetA the first value set
         * \param opIdx The operator index. Depending on the index, the right operation is selected
         */
        VisitorVSetB( const WValueSet< VSetAType >* const vsetA, size_t opIdx = 0 );

        /**
         * Visitor on the second valueset. This applies the operation.
         *
         * \tparam VSetBType the integral type of the currently visited valueset.
         * \param vsetB the valueset currently visited (B).
         *
         * \return the result of o(A,B)
         */
        template< typename VSetBType >
        result_type operator()( const WValueSet< VSetBType >* const& vsetB ) const;      // NOLINT

        /**
         * The first valueset.
         */
        const WValueSet< VSetAType >* const m_vsetA;

        /**
         * The operator index.
         */
        size_t m_opIdx;
    };

    /**
     * Visitor for discriminating the type of the first valueset. It simply creates a new instance of VisitorVSetB with the proper integral type of
     * the first value set.
     */
    class VisitorVSetA: public boost::static_visitor< std::shared_ptr< WValueSetBase > >
    {
    public:
        /**
         * Create visitor instance. The specified valueset gets visited if the first one is visited using this visitor.
         *
         * \param vsetB The valueset to visit during this visit.
         * \param opIdx The operator index. Forwarded to VisitorVSetB
         */
        VisitorVSetA( WValueSetBase* vsetB, size_t opIdx = 0 );

        /**
         * Called by boost::varying during static visiting. Creates a new VisitorVSetB which finally applies the operation.
         *
         * \tparam T the real integral type of the first value set.
         * \param vsetA the first valueset currently visited.
         *
         * \return the result from the operation with this and the second value set
         */
        template < typename T >
        result_type operator()( const WValueSet< T >* const& vsetA ) const;             // NOLINT

        /**
         * The valueset where to cascade.
         */
        WValueSetBase* m_vsetB;

        /**
         * The operator index.
         */
        size_t m_opIdx;
    };

    /**
     * Visitor for discriminating the type of the first valueset. It should be used for operations on ONE valueset.
     */
    class VisitorVSetSingleArgument: public boost::static_visitor< std::shared_ptr< WValueSetBase > >
    {
    public:
        /**
         * Create visitor instance. The specified valueset gets visited if the first one is visited using this visitor.
         *
         * \param opIdx The operator index. Forwarded to VisitorVSetB
         */
        explicit VisitorVSetSingleArgument( size_t opIdx = 0 );

        /**
         * Called by boost::varying during static visiting. Applies the operation to it
         *
         * \tparam T the real integral type of the first value set.
         * \param vsetA the first valueset currently visited.
         *
         * \return the result from the operation with this and the second value set
         */
        template < typename T >
        result_type operator()( const WValueSet< T >* const& vsetA ) const;             // NOLINT

        /**
         * Set lower and upper border needed for several ops.
         *
         * \param l lower border
         * \param u upper border
         */
        void setBorder( double l, double u );
        /**
         * The operator index.
         */
        size_t m_opIdx;

        /**
         * Lower border needed for several ops.
         */
        double m_lowerBorder;

        /**
         * Upper border needed for several ops.
         */
        double m_upperBorder;
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

    /**
     * Lower border used for clamping.
     */
    WPropDouble m_lowerBorder;

    /**
     * Upper border used for clamping.
     */
    WPropDouble m_upperBorder;

    std::shared_ptr< WModuleInputData< WDataSetScalar > > m_inputA;  //!< Input connector required by this module.
    std::shared_ptr< WModuleInputData< WDataSetScalar > > m_inputB;  //!< Input connector required by this module.

    std::shared_ptr< WModuleOutputData< WDataSetScalar > > m_output; //!< The only output of this filter module.
};

#endif  // WMSCALAROPERATOR_H

