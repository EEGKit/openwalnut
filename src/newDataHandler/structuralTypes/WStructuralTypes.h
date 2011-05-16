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

#include "WParameterTypes.h"

namespace mpl = boost::mpl;

/**
 * This class represents the samples which are needed to reconstruct the real type using a structural type. It therefore needs the parameter
 * vector from the structural type. This vector consists of boost::variant variables describing the possible types and integrals which define all
 * possible types represented by the structural type. The class stores these variants as instances in a recursive fashion. It therefore uses the
 * first variant type in the given ParameterVector template parameter as member and derives from itself specifying the vector reduced by one (the
 * first) element. Recursion is terminated using a partial specialization for the IsEmpty template parameter.
 *
 * \tparam ParameterVector The parameter vector. For details, have a look at the structural type example \ref WStructuralTypeExample.
 * \tparam IsEmpty is mpl::bool_< false >
 *
 * \note You need to initialize the variants. Normally, this is done using the structural type.
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
     * The next level.
     */
    typedef WStructuralTypeStore< typename mpl::template pop_front< ParameterVector >::type > NextStructuralTypeStoreLevel;

    /**
     * The current variant instance of this level.
     */
    VariantType m_variant;

    /**
     * Returns the next recursion level. This is useful for getVariant to initialize the different variants.
     *
     * \return casted instance.
     */
    NextStructuralTypeStoreLevel& next()
    {
        return static_cast< NextStructuralTypeStoreLevel& >( *this );
    }

    /**
     * Returns the next recursion level. This is useful for getVariant to initialize the different variants.
     *
     * \return casted instance.
     */
    const NextStructuralTypeStoreLevel& next() const
    {
        return static_cast< const NextStructuralTypeStoreLevel& >( *this );
    }

    /**
     * Returns the variant at the current level. Use next() to get the next variants. It is not possible to provide an recursive version of this
     * function, since full specialization of template member functions in a template class is not allowed. Use next() to go through the levels.
     *
     * \return the variant.
     */
    VariantType& getVariant()
    {
        return m_variant;
    }

    /**
     * Returns the variant at the current level. Use next() to get the next variants. It is not possible to provide an recursive version of this
     * function, since full specialization of template member functions in a template class is not allowed. Use next() to go through the levels.
     *
     * \return the variant.
     */
    const VariantType& getVariant() const
    {
        return m_variant;
    }
};

/**
 * Partial specialization which terminates recursive variant storage.
 *
 * \tparam ParameterVector the current parameter vector which is empty
 */
template< typename ParameterVector >
class WStructuralTypeStore< ParameterVector, true >
{
public:
    /**
     * The next level.
     */
    typedef WStructuralTypeStore< ParameterVector, true > NextStructuralTypeStoreLevel;
};

// ----------------------------------------------------------------------------------

/**
 * This class is able to recursively resolve the real types of a structural type sample, which is stored as \ref WStructuralTypeStore. The idea
 * behind this resolution is simple. The class uses two type vectors. ParameterVector: the vector of variants describing the possible
 * compile-time types describing the type it represents. This vector initially is the ParameterVector of the StructuralT. The second vector
 * ResolvedTypeVector is empty by default. The first level applies the boost visitor to the first variant in the structural type and resolves its
 * real type. This real type is then added to ResolvedTypeVector. The ParameterVector is reduced by one since we now know the real type of the
 * variant at the first position of ParameterVector. To do recursion, a new instance of WStructuralTypeResolution is created with these NEW
 * vectors as parameters and resolution is done for them. This recursion stops if the ParameterVector is empty. At this recursion level, the
 * ResolvedTypeVector contains all the real types. Using them, the StructuralT::ToRealType template is used to reconstruct the real type
 * represented by the WStructuralTypeStore sample and the Functor is called.
 *
 * \tparam StructuralT The underlying structural type to resolve
 * \tparam FunctorT The functor to call after resolution
 * \tparam ParameterVector The list of possible compile-time parameters the StructuralT represents
 * \tparam ResolvedTypeVector The growing list of real types.
 */
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
     * The same as the ParameterVector but reduced by the front element. This describes the remaining types to resolve.
     */
    typedef typename mpl::template pop_front< ParameterVector >::type RemainingParameterVector;

    /**
     * Similar to the CurrentParameterStore, but this describes the correct level in the inheritance hierarchy of the WStructuralTypeStore.
     */
    typedef typename CurrentParameterStore::NextStructuralTypeStoreLevel RemainingParameterStore;

    /**
     * The variant type at this level (for the first element int the given ParameterVector.
     */
    typedef typename mpl::front< ParameterVector >::type VariantType;

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
    void resolve() const
    {
        // forward the call and provide proper parameters
        applyFunction( mpl::bool_< false >() );
    }

    /**
     * The functor getting called by boost::apply_visitor during recursion. This handles the current recursion level and the real type of it. It
     * then adds the current RealT to the resolved type vector and reduces the current parameter vector by one and recursively calls another
     * WStructuralTypeResolution.
     *
     * \tparam RealT the real type of the variant in the store at the current recursion level.
     */
    template < typename RealT >
    void operator()( const RealT& /* x */ ) const
    {
        // now we know the real type of the variant for the first element in the given vector
        typedef typename mpl::push_back< ResolvedTypeVector, RealT >::type NextResolvedTypeVector;
        typedef WStructuralTypeResolution< StructuralT, FunctorT, RemainingParameterVector, NextResolvedTypeVector > NextResolutionLevel;

        NextResolutionLevel next( static_cast< const RemainingParameterStore& >( m_parameters ), m_functor );
        next.applyFunction( typename mpl::empty< RemainingParameterVector >::type() );
    }

    /**
     * Applies the resolving mechanism to the next recursive level. The stop criterion for recursion (on compile-time) is the empty parameter
     * type. This function terminates recursion as the ParameterVector is empty.
     */
    void applyFunction( mpl::bool_< true >  /* empty */ ) const
    {
        // time to create the real type from the resolved types
        typedef typename StructuralT::template ToRealType< ResolvedTypeVector >::Type RealType;
        // call the actual functor:
        m_functor( RealType() );
    }

    /**
     * Applies the resolving mechanism to the next recursive level. The stop criterion for recursion (on compile-time) is the empty parameter
     * type. This function does recursion as the vector is not empty.
     */
    void applyFunction( mpl::bool_< false > /* empty */ ) const
    {
        // let boost's visiting mechanism do the job
        boost::apply_visitor( *this, const_cast< VariantType& >( m_parameters.m_variant ) );
    }

private:
    /**
     * The current list of parameters for matching the current ParameterVector.
     */
    const CurrentParameterStore& m_parameters;

    /**
     * The functor to call.
     */
    FunctorT& m_functor;
};

// ----------------------------------------------------------------------------------

/**
 * Example class showing the correct structure of structural types. The idea behind structural types is to encapsulate real types and their
 * compile-time parameters which are not really useful/interesting to the programmer. A little example: WMatrixFixed< double, 3, 1 >, which is a
 * vector of double. But for your calculations, the integral type (double in this case) is not really important. Furthermore assume that you are
 * not interested in the dimension of the vector. You only want an arbitrary size, arbitrary typed vector. The WDataSet we are using in
 * OpenWalnut uses these structural types to allow the user exactly that. This simple example type shows how to write a structural type and which
 * methods it needs. We do it for the example of a arbitrary size/type vector.
 *
 * Generally, the structural type must provide the following:
 *  - ParameterVector: describing the compile-time parameters of the real type.
 *  - ToRealType: to convert the structural type to the real type if the parameter types are knwon
 *  - FromRealType: to create a structural type sample if an real type sample is known.
 *
 * \note Never use this class directly. It won't compile as we only use some forward declarations.
 */
class WStructuralTypeExample
{
public:
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Remove these forward declarations in your own structural types!
    //   - we need them here because we do not want to include WMatrixFixed.h
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Forward declaration of WMatrixFixed. This is needed as we do not want to include WMatrixFixed.h for an example.
     *
     * \note remove this in your own structural types.
     */
    template< typename, size_t, size_t > class WMatrixFixed;

    /**
     * This is the most important part of the structural type. This is a type vector, containing the allowed compile-time (template) types
     * describing the real type. Remember our WMatrixFixed< double, 3, 1 > example. We want to encapsulate arbitrary size/type vectors. So we add
     * the integral types as possible variation and a couple of natural numbers (not zero) as variation for the size template parameter of
     * WMatrixFixed by defining the ParameterVector the following:
     *
     * \note you can use an arbitrary number of types variants here
     */
    typedef boost::mpl::vector< WIntegralVariant, WNaturalNumbersNoZeroVariant > ParameterVector;   // NOLINT - no this is not a std::vector

    /**
     * The type resolver mechanism must be able to reconstruct the real type (in our case the WMatrixFixed< X, Y, 0 >) from a list of parameters.
     * This list is generated automatically and specified to ToRealType. This class needs to define the real type by using the resolved type
     * list. This is quite simple, as the mpl::vector provides direct indexing of elements. And you, as you defined the structural type exactly
     * know which index is which template parameter for the represented real type.
     *
     * \tparam ParameterVector the above defined ParameterVector.
     */
    template< typename RealParameterVector >
    struct ToRealType
    {
        /**
         * This typedef reconstructs the real type. It knows that the given RealParameterVector has exactly two elements since the
         * ParameterVector had two. The first element was the integral type. The second was a size_t element denoting the dimension of the
         * vector. These information are combined here.
         */
        typedef WMatrixFixed< typename mpl::at< RealParameterVector, mpl::size_t< 0 > >::type,
                                       mpl::at< RealParameterVector, mpl::size_t< 1 > >::type::value,
                              1 > Type;
    };

    /**
     * This function is the inverse of ToRealType. It creates an structural type sample instance describing the real type's compile time parameter
     * according to the ParameterVector. It therefore uses the \ref WStructuralTypeStore class. It provides the variants for each item in
     * ParameterVector, which need to be initialized with the real template types of SampleT.
     *
     * \param sample the sample. This is used to initialize the variants.
     * \tparam SampleT the sample of real type to convert to a structural type sample.
     *
     * \return the sample.
     */
    template< typename SampleT >
    static WStructuralTypeStore< ParameterVector > FromRealType( const SampleT& sample )
    {
        // First: create the StructuralTypeStore
        WStructuralTypeStore< ParameterVector > s;

        // get the variant for the integral type. This is the first element in the ParameterVector
        s.getVariant() = typename SampleT::IntegralType();

        // the second variant is the size variant. Use next() to get the variant for the second item.
        s.next().getVariant() = typename SampleT::TheSize();
        return s;
    }
};

#endif  // WSTRUCTURALTYPES_H

