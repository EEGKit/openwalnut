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

#ifndef WSTRUCTURALTYPES_H
#define WSTRUCTURALTYPES_H

#include <boost/mpl/at.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/size_t.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/variant.hpp>
#include <boost/variant/static_visitor.hpp>

namespace mpl = boost::mpl;

/**
 * Example: TODO(ebaum): write
 */
class WStructuralTypeExample
{
public:
    // TODO(ebaum): write
};

// ----------------------------------------------------------------------------------

/**
 * This class represents the samples which are needed to reconstruct the real type using a structural type. It therefore needs the parameter
 * vector from the structural type. This vector consists of boost::variant variables describing the possible types and integrals which define all
 * possible types represented by the structural type. The class stores these variants as instances in a recursive fashion. It therefore uses the
 * first variant type in the given ParameterVector template parameter as member and derives from itself specifying the vector reduced by one (the
 * first) element. Recursion is terminated using a partial specialization for the IsEmpty template parameter.
 *
 * \tparam ParameterVector The parameter vector. For details, have a look at the structural types in structrualTypes/WMatrixFixedStructural.h for
 * example.
 *
 * \tparam IsEmpty is mpl::bool_< false >
 *
 * \note You need to initialize the variants. Normally, this is done using the structural type.
 *
 */
template< typename ParameterVector,
          bool IsEmpty = mpl::empty< ParameterVector >::value
        >
class WStructuralTypeStore: public WStructuralTypeStore< typename mpl::template pop_front< ParameterVector >::type >
{
public:
    /**
     * The current variant type of this level.
     */
    typedef typename mpl::front< ParameterVector >::type VariantType;
    /**
     * The current variant instance of this level.
     */
    VariantType m_variant;
};

/**
 * Partial specialization which terminates recursive variant storage.
 *
 * \tparam ParameterVector the current parameter vector which is empty
 */
template< typename ParameterVector >
class WStructuralTypeStore< ParameterVector, true >
{
    // terminates recursive element-pop from ParameterVector
};

// ----------------------------------------------------------------------------------

template< typename StructuralT,
          typename FunctorT,
          typename ParameterVector = typename StructuralT::ParameterVector,
          typename ResolvedTypeVector = mpl::vector<> >  // NOLINT - No, this is not a std::vector
class WStructuralTypeResolution: public boost::static_visitor<>
{
public:
    /**
     * Type abbreviation for the WStructuralTypeStore handling the sample of the structural type's parameter vector at the current
     * level.
     */
    typedef WStructuralTypeStore< ParameterVector > CurrentParameterStore;

    /**
     * Constructs the resolution object. It needs the parameter store and a function which is called upon resolution.
     *
     * \param parameters the parameter store. Use the structural type to create this from a sample.
     * \param functor the functor to call.
     */
    WStructuralTypeResolution( const CurrentParameterStore& parameters, FunctorT& functor ):
        m_parameters( parameters ),
        m_functor( functor )
    {
    }

    /**
     * Actually does type resolution and calls the functor.
     */
    void resolv() const
    {
        // forward the call and provide proper parameters
        applyFunction( mpl::bool_< false >() );
    }

    typedef typename mpl::template pop_front< ParameterVector >::type RemainingParameterVector;
    typedef WStructuralTypeStore< RemainingParameterVector > RemainingParameterStore;
    typedef typename mpl::front< ParameterVector >::type VariantType;

    const CurrentParameterStore& m_parameters;
    FunctorT& m_functor;

    template < typename RealT >
    void operator()( const RealT& /* x */ ) const
    {
        // now we know the real type of the variant for the first element in the given vector
        typedef typename mpl::push_back< ResolvedTypeVector, RealT >::type NextResolvedTypeVector;
        typedef WStructuralTypeResolution< StructuralT, FunctorT, RemainingParameterVector, NextResolvedTypeVector > NextResolutionLevel;

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
};

// ----------------------------------------------------------------------------------

#endif  // WSTRUCTURALTYPES_H

