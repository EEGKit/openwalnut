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

#ifndef WVALUEMAPPER_H
#define WVALUEMAPPER_H

#include <boost/shared_ptr.hpp>

#include "WValueSet.h"
#include "WDataSetVisitor.h"
#include "WDataSetAccessor.h"
#include "structuralTypes/WStructuralTypes.h"

/**
 * The value-mapper class. It handles the grid and value-sets and their interaction. It provides interfaces for accessing and processing them.
 * The WValueMapper does not know the real integral type or other value-type specifics. It only knows the structural type. If you want your own
 * WDataSet types, you should probably modify WDataSet. It is an interface/adapter to this class.
 *
 * \tparam GridT The grid type to use.
 * \tparam StructuralT The structural type to use.
 */
template< typename GridT, typename StructuralT >
class WValueMapper
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
    typedef WValueSetBase ValueSetBaseType;

    /**
     * The value-mapper to which this class can be applied.
     */
    typedef WValueMapper< GridType, StructuralType > ValueMapperType;

    /**
     * Convenience typedef for a boost::shared_ptr< WValueMapper<...> >.
     */
    typedef boost::shared_ptr< WValueMapper< GridType, StructuralType > > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WValueMapper<...> >.
     */
    typedef boost::shared_ptr< const WValueMapper< GridType, StructuralType > > ConstSPtr;

    /**
     * Creates instance of WValueMapper. It uses a sample to determine the correct WValueSet type. The value of the sample is completely
     * unimportant. You can simply use the default constructor. Depending on the ValueSet, the memory is allocated for the data and you can
     * afterwards use the visitor scheme to fill it.
     *
     * \tparam SampleT Sample type.
     * \param grid the grid to use.
     * \param sample a sample. Needed to determine the real type of the values in this WValueMapper.
     */
    template< typename SampleT >
    WValueMapper( typename GridType::ConstSPtr grid, const SampleT& sample ):
        m_structuralTypeSample( StructuralT::FromRealType( sample ) ),
        m_grid( grid ),
        m_valueSet( typename ValueSetBaseType::SPtr( new WValueSet< SampleT >( grid->size() ) ) )
    {
        // Initialize
    }

    /**
     * Destructor. Cleans up. Please note, that the destruction does not necessarily removes the Grid and Data instances as they are
     * reference counted.
     */
    virtual ~WValueMapper()
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

protected:

    /**
     * Returns the value-set base instance as pointer.
     *
     * \return WValueSetBase instance
     */
    ValueSetBaseType::SPtr getValueSetBase()
    {
        return m_valueSet;
    }

    /**
     * Returns the value-set base instance as const pointer.
     *
     * \return WValueSetBase instance
     */
    ValueSetBaseType::ConstSPtr getValueSetBase() const
    {
        return m_valueSet;
    }

    /**
     * Returns the value-set instance as pointer with the specified value-type.
     *
     * \return WValueSet instance with specified type. NULL if specified type is wrong.
     */
    template < typename ValueT >
    typename WValueSet< ValueT >::SPtr getValueSet()
    {
        return boost::shared_dynamic_cast< WValueSet< ValueT > >( m_valueSet );
    }

    /**
     * Returns the value-set instance as const pointer with the specified value-type.
     *
     * \return WValueSet instance with specified type. NULL if specified type is wrong.
     */
    template < typename ValueT >
    typename WValueSet< ValueT >::ConstSPtr getValueSet() const
    {
        return boost::shared_dynamic_cast< WValueSet< ValueT > >( m_valueSet );
    }

    /**
     * Create an accessor object of given type. The type you specify here should allow non-const access to the data. Thy AccessorT needs a
     * constructor which accepts GridType::ConstSPtr and WValueSet< SomeType >::SPtr.
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
     * constructor which accepts GridType::ConstSPtr and WValueSet< SomeType >::ConstSPtr.
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
        DataSetVisitorType visitDispacher( visitor, correctThis );

        // now, use the type resolution mechanism to call the correct WDataSetVisitor operator.
        typedef WStructuralTypeResolution< StructuralType, DataSetVisitorType > TypeResolution;
        TypeResolution tr( m_structuralTypeSample, &visitDispacher );
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

#endif  // WVALUEMAPPER_H

