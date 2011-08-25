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

#ifndef WSTILLNEEDSANAME_H
#define WSTILLNEEDSANAME_H

#include <boost/shared_ptr.hpp>
#include "WValueSet2.h"
#include "WDataSetVisitor.h"
#include "WDataAccess.h"
#include "structuralTypes/WStructuralTypes.h"

/**
 * The value-mapper class. It handles the grid and value-sets and their interaction. It provides interfaces for accessing and processing them.
 * The WStillNeedsAName does not know the real integral type or other value-type specifics. It only knows the structural type. If you want your own
 * WDataSet types, you should probably modify WDataSet. It is an interface/adapter to this class.
 *
 * \tparam GridT The grid type to use.
 * \tparam StructuralT The structural type to use.
 */
template< typename GridT, typename StructuralT >
class WStillNeedsAName
{
public:
    template< typename, typename > friend class WDataSetVisitor;

    /**
     * The Grid type.
     */
    typedef GridT GridType;

    /**
     * The structural type.
     */
    typedef StructuralT StructuralType;

    /**
     * The type of the value set with the specified structural type
     */
    typedef WValueSet2Base ValueSetBaseType;

    /**
     * The value-mapper to which this class can be applied.
     */
    typedef WStillNeedsAName< GridType, StructuralType > ValueMapperType;

    /**
     * Convenience typedef for a boost::shared_ptr< WStillNeedsAName<...> >.
     */
    typedef boost::shared_ptr< WStillNeedsAName< GridType, StructuralType > > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WStillNeedsAName<...> >.
     */
    typedef boost::shared_ptr< const WStillNeedsAName< GridType, StructuralType > > ConstSPtr;

    /**
     * Creates instance of WStillNeedsAName. It uses a sample to determine the correct WValueSet2 type. The value of the sample is completely
     * unimportant. You can simply use the default constructor. Depending on the ValueSet, the memory is allocated for the data and you can
     * afterwards use the visitor scheme to fill it.
     *
     * \tparam SampleT Sample type.
     * \param grid the grid to use.
     * \param sample a sample. Needed to determine the real type of the values in this WStillNeedsAName.
     */
    template< typename SampleT >
    WStillNeedsAName( typename GridType::ConstSPtr grid, const SampleT& sample ):
        m_structuralTypeSample( StructuralT::FromRealType( sample ) ),
        m_grid( grid ),
        m_valueSet( typename ValueSetBaseType::SPtr( new WValueSet2< SampleT >( grid->size() ) ) )
    {
        // Initialize
    }

    /**
     * Destructor. Cleans up. Please note, that the destruction does not necessarily removes the Grid and Data instances as they are
     * reference counted.
     */
    virtual ~WStillNeedsAName()
    {
        // Cleans up.
    }

    /**
     * Returns the grid of the dataset. It returns a const grid only, as the grid is not modifiable.
     *
     * \return the grid.
     */
    typename GridType::ConstSPtr getGrid() const
    {
        return m_grid;
    }

    /**
     * Applies the specified visitor to the data and resolves the correct type. The visitor you specify needs to match certain requirements. Have
     * a look at \ref WDataSetVisitor for details.
     *
     * \tparam VisitorT the type of the visitor.
     * \param visitor the visitor instance.
     */
    template< typename VisitorT >
    void applyVisitor( VisitorT* visitor ) const
    {
        // the visitor is not called directly. Its done by WDataSetVisitor which acts as dispatcher.
        typedef const WDataSetVisitor< const ValueMapperType, VisitorT > DataSetVisitorType;
        // forward. The DataSetVisitorType is const here
        applyVisitorImpl< DataSetVisitorType >( visitor, this );
    }

    /**
     * Applies the specified visitor to the data and resolves the correct type. The visitor you specify needs to match certain requirements. Have
     * a look at \ref WDataSetVisitor for details.
     *
     * \tparam VisitorT the type of the visitor.
     * \param visitor the visitor instance.
     */
    template< typename VisitorT >
    void applyVisitor( VisitorT* visitor )
    {
        // the visitor is not called directly. Its done by WDataSetVisitor which acts as dispatcher.
        typedef WDataSetVisitor< ValueMapperType, VisitorT > DataSetVisitorType;
        // forward. The DataSetVisitorType is non-const here
        applyVisitorImpl< DataSetVisitorType >( visitor, this );
    }

    /**
     * This generates an accessor object with userdefined type. This is used if a user
     * wants to get/set data using a user-specified type, for example if the user just wants to
     * use double values without having to know the type of the actual data stored in the valueset.
     *
     * Be aware that there is additional overhead per data access in the form of casting, copies
     * and virtual function calls.
     *
     * All functions and iterators offered by the returned access object will return data proxy objects
     * instead of direct references to the data stored in the valueset. These proxy objects can then be
     * implicitly or explicitly cast to the user-defined type or be assigned from the user-defined
     * type (or anything that can be implicitly cast to the user-defined type).
     *
     * This creates special problems you should be aware of. For example, if you had a voxel iterator pointing
     * to a valid voxel, you could do this:
     *
     * int i = *vi;
     * *vi = 1;
     *
     * But not this:
     *
     * int i = *vi + 1;
     *
     * Instead you need to do this:
     *
     * int i = 1 + *vi;    or
     * int i = static_cast< int >( *vi ) + 1;
     *
     * But you can do all this even of the dataset actually contained double data (with loss of precision of course).
     *
     * The integral data type is not the only thing that can be different, it could be other properties defined by the structural
     * type too, for example spherical harmonic order. However, you need the correct assignment and/or cast operators between the
     * types, for example an operator:
     *
     * template< typename T, std::size_t order >
     * SH< double, 4 > operator=( SH< T, order > const& s )
     *
     * \tparam The user-defined type.
     *
     * \return An access object for a user-defined type.
     */
    template< typename T >
    WDataAccess< GridType, WDataProxy< T > > getAccess()
    {
        WGetProxyValueSetVisitor< T > visitor( m_valueSet );
        typedef WStructuralTypeResolution< StructuralType, WGetProxyValueSetVisitor< T > > TypeResolution;
        TypeResolution tr( m_structuralTypeSample, &visitor );
        tr.resolve();

        return WDataAccess< GridType, WDataProxy< T > >( m_grid, visitor.get() );
    }

    /**
     * As with the non-const version, but there is no (data) assignment operator for the data proxies
     * return by the various functions and iterators.
     *
     * \tparam The user-defined type.
     *
     * \return An access object for a user-defined type.
     */
    template< typename T >
    WDataAccessConst< GridType, WDataProxy< T > > getAccess() const
    {
        WGetProxyValueSetVisitor< T > visitor( m_valueSet );
        typedef WStructuralTypeResolution< StructuralType, WGetProxyValueSetVisitor< T > > TypeResolution;
        TypeResolution tr( m_structuralTypeSample, &visitor );
        tr.resolve();

        return WDataAccessConst< GridType, WDataProxy< T > >( m_grid, visitor.get() );
    }

protected:

    /**
     * Returns the value-set base instance as pointer.
     *
     * \return WValueSet2Base instance
     */
    ValueSetBaseType::SPtr getValueSetBase()
    {
        return m_valueSet;
    }

    /**
     * Returns the value-set base instance as const pointer.
     *
     * \return WValueSet2Base instance
     */
    ValueSetBaseType::ConstSPtr getValueSetBase() const
    {
        return m_valueSet;
    }

    /**
     * Returns the value-set instance as pointer with the specified value-type.
     *
     * \return WValueSet2 instance with specified type. NULL if specified type is wrong.
     */
    template < typename ValueT >
    typename WValueSet2< ValueT >::SPtr getValueSet()
    {
        return boost::shared_dynamic_cast< WValueSet2< ValueT > >( m_valueSet );
    }

    /**
     * Returns the value-set instance as const pointer with the specified value-type.
     *
     * \return WValueSet2 instance with specified type. NULL if specified type is wrong.
     */
    template < typename ValueT >
    typename WValueSet2< ValueT >::ConstSPtr getValueSet() const
    {
        return boost::shared_dynamic_cast< WValueSet2< ValueT > >( m_valueSet );
    }

    /**
     * Create an accessor object of given type. The type you specify here should allow non-const access to the data. Thy AccessorT needs a
     * constructor which accepts GridType::ConstSPtr and WValueSet2< SomeType >::SPtr.
     *
     * \tparam AccessorT some Type allowing non-const access to the data.
     *
     * \return the accessor instance.
     */
    template < typename AccessorT >
    AccessorT createAccessor()
    {
        AccessorT ac( getGrid(), getValueSet< typename AccessorT::ValueType >() );
        return ac;
    }

    /**
     * Create an accessor object of given type. The type you specify here should dis-allow non-const access to the data. Thy ConstAccessorT needs a
     * constructor which accepts GridType::ConstSPtr and WValueSet2< SomeType >::ConstSPtr.
     *
     * \tparam ConstAccessorT some Type allowing only const access to the data.
     *
     * \return the accessor instance.
     */
    template < typename ConstAccessorT >
    ConstAccessorT createAccessor() const
    {
        ConstAccessorT ac( getGrid(), getValueSet< typename ConstAccessorT::ValueType >() );
        return ac;
    }

private:

    /**
     * Does type resolution and utilizes the WDataSetVisitor to apply user defined-visitors.
     *
     * \tparam DataSetVisitorType the WDataSetVisitor instance. Can be const of the ValueMapper should be const during visit.
     *
     * \param visitor the user defined visitor
     * \param correctThis the this pointer as const or non-const pointer, depending on  DataSetVisitorType::ValueMapperType.
     */
    template< typename DataSetVisitorType >
    void applyVisitorImpl( typename DataSetVisitorType::VisitorType* visitor, typename DataSetVisitorType::ValueMapperType* correctThis ) const
    {
        // the visitor is not called directly. Its done by WDataSetVisitor which acts as dispatcher.
        DataSetVisitorType visitDispatcher( visitor, correctThis );

        // now, use the type resolution mechanism to call the correct WDataSetVisitor operator.
        typedef WStructuralTypeResolution< StructuralType, DataSetVisitorType > TypeResolution;
        TypeResolution tr( m_structuralTypeSample, &visitDispatcher );
        tr.resolve();
    }

    /**
     * The sample representing the real type of the data inside. This is needed for applying the visitor.
     */
    WStructuralTypeStore< typename StructuralType::ParameterVector > m_structuralTypeSample;

    /**
     * The grid in which the data exists.
     */
    typename GridType::ConstSPtr m_grid;

    /**
     * The value-set.
     */
    typename ValueSetBaseType::SPtr m_valueSet;
};

#endif  // WSTILLNEEDSANAME_H

