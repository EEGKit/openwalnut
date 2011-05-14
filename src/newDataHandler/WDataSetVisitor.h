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

#ifndef WDATASETVISITOR_H
#define WDATASETVISITOR_H

#include <boost/mpl/vector.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/size_t.hpp>
#include <boost/mpl/size.hpp>
#include <boost/variant.hpp>
#include <boost/variant/static_visitor.hpp>

#include "structuralTypes/WStructuralTypes.h"

namespace mpl = boost::mpl;

template< typename ParameterVector,
          bool IsEmpty = mpl::empty< ParameterVector >::value
        >
class ParameterStore: public ParameterStore< typename mpl::template pop_front< ParameterVector >::type >
{
public:
    typedef typename mpl::front< ParameterVector >::type VariantType;
    VariantType m_variant;
};

template< typename ParameterVector >
class ParameterStore< ParameterVector, true >
{
    // terminates recursive element-pop from ParameterVector
};

template< typename StructuralT,
          typename FunctorT,
          typename ParameterVector = typename StructuralT::ParameterVector,
          typename ResolvedTypeVector = mpl::vector<> >
class ParameterResolution: public boost::static_visitor<>
{
public:
    typedef typename mpl::template pop_front< ParameterVector >::type RemainingParameterVector;
    typedef typename mpl::front< ParameterVector >::type VariantType;
    typedef ParameterStore< ParameterVector > CurrentParameterStore;
    typedef ParameterStore< RemainingParameterVector > RemainingParameterStore;

    const CurrentParameterStore& m_parameters;
    FunctorT& m_functor;

    ParameterResolution( const CurrentParameterStore& parameters, FunctorT& functor ):
        m_parameters( parameters ),
        m_functor( functor )
    {
    }

    template < typename RealT >
    void operator()( const RealT& /* x */ ) const
    {
        // now we know the real type of the variant for the first element in the given vector
        typedef typename mpl::push_back< ResolvedTypeVector, RealT >::type NextResolvedTypeVector;
        typedef ParameterResolution< StructuralT, FunctorT, RemainingParameterVector, NextResolvedTypeVector > NextResolutionLevel;

        NextResolutionLevel next( static_cast< const RemainingParameterStore& >( m_parameters ), m_functor );
        next.applyFunction( typename mpl::empty< RemainingParameterVector >::type() );
    }

    void applyFunction( mpl::bool_< true >  /* empty */ ) const
    {
        // time to create the real type from the resolved types
        typedef typename StructuralT::template ToRealType< ResolvedTypeVector >::Type RealType;
        // call the actual functor:
        m_functor( RealType() );
    }

    void applyFunction( mpl::bool_< false > /* empty */ ) const
    {
        boost::apply_visitor( *this, const_cast< VariantType& >( m_parameters.m_variant ) );
    }

    void resolv() const
    {
        applyFunction( mpl::bool_< false >() );
    }
};




class WDataSetVisitor;

/**
 * This functor is used to unveil the real value-type in a value-set. Its purpose is to allow the value-mapper to apply a function with the real
 * value-type and grid-type. This class handles this and forwards the call to the visiting class. The name is telling. This class dispatches the
 * WDataSetVisitor through the WValueSetTyped::unveilValueType mechanism.
 */
class WDataSetVisitorDispatcher
{
public:
    template< typename VisitorType >
    explicit WDataSetVisitorDispatcher( VisitorType& visitor )
        //:        m_visitor( visitor )
    {
        // intialize
    }

    /**
     * Operator getting called by WValueSet::unveilValueType. The template parameter then is the correct value-type of the value-set calling it.
     *
     * \tparam ValueType the callers real value-type
     */
    template< typename ValueType >
    void operator()()
    {
        // unveiled real value-type in typename ValueType
    }

    /**
     * Operator getting called by WValueSet::unveilValueType. The template parameter then is the correct value-type of the value-set calling it.
     *
     * \tparam ValueType the callers real value-type
     */
    template< typename ValueType >
    void operator()() const
    {
        // unveiled real value-type in typename ValueType
    }
};


#endif  // WDATASETVISITOR_H

