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

#ifndef WDATASET2_H
#define WDATASET2_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "WStillNeedsAName.h"
#include "WValueSet2.h"

#include "../common/WTransferable.h"

/**
 * A base class for the datasets.
 */
class WDataSet2Base : public WTransferable
{
public:

    //! A conveciance typedef for a boost::shared_ptr.
    typedef boost::shared_ptr< WDataSet2Base > SPtr;

    //! A conveciance typedef for a boost::shared_ptr to a const object.
    typedef boost::shared_ptr< WDataSet2Base const > ConstSPtr;

    /**
     * Constructor.
     */
    WDataSet2Base();

    /**
     * Destructor.
     */
    virtual ~WDataSet2Base();

    /**
     * Returns the name of this dataset.
     *
     * \return The name of this dataset.
     */
    virtual std::string const getName() const;

    /**
     * Returns a description of this dataset.
     *
     * \return A description of this dataset.
     */
    virtual std::string const getDescription() const;

    /**
     * Returns a prototype for this class.
     *
     * \return The prototype instance.
     */
    static boost::shared_ptr< WPrototyped > getPrototype();

protected:

    //! The prototype instance.
    static boost::shared_ptr< WPrototyped > m_prototype;
};

/**
 * The DataSet class. This class handles all which is needed for keeping data. It provides interfaces to apply visitor functions to it and to
 * process data. Please note, that the Grid is const after creation of the instance. The WDataSet2 disguises the real integral type and uses
 * structural types, describing other properties of the real type. This way, we do not need to completely specify the real integral types or
 * orders, dimensions and so on. Basically, this class can be seen as interface to WStillNeedsAName.
 *
 * \note If you want to add new DataSets, please specialize this class for your types. If you use a completely custom type, where no structural
 * type has been specified yet, have a look at the WXXXStructural.h files for an example.
 *
 * \tparam GridT The grid type to use.
 * \tparam StructuralT The structural type to use.
 */
template< typename GridT, typename StructuralT >
class WDataSet2 : public WDataSet2Base
{
public:

    /**
     * The Grid type.
     */
    typedef GridT GridType;

    /**
     * The structural type.
     */
    typedef StructuralT StructuralType;

    /**
     * The type of the WStillNeedsAName.
     */
    typedef WStillNeedsAName< GridType, StructuralType > ValueMapperType;

    /**
     * Convenience typedef for a boost::shared_ptr< WStillNeedsAName<...> >.
     */
    typedef boost::shared_ptr< WDataSet2< GridType, StructuralType > > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WStillNeedsAName<...> >.
     */
    typedef boost::shared_ptr< const WDataSet2< GridType, StructuralType > > ConstSPtr;

    /**
     * Creates an invalid dataset.
     */
    // TODO( reichenbach ): check if this is still nessessary
    WDataSet2()
        : WDataSet2Base()
    {
    }

    /**
     * A special constructor that allows to create an instance of a dataset using runtime
     * values that describe the stored datatype. A type store object for the structural type of this dataset
     * must be created from the runtime values using the typeStoreFromRuntimeVariables function and
     * provided to this constructor. Use this to create datasets for data read from files, or in any other case where
     * you do not know the type of the data at compile-time.
     *
     * \tparam TypeStoreT The type of the provided typestore object.
     *
     * \param grid The grid to use.
     * \param ts The type store instance.
     */
    template< typename TypeStoreT >
    WDataSet2( typename GridType::ConstSPtr grid, TypeStoreT const& ts )
        : WDataSet2Base()
    {
        typedef WStructuralTypeResolution< StructuralType, InitDataSetFunctor > TypeResolver;
        InitDataSetFunctor func( grid );
        TypeResolver resolver( ts, &func );
        resolver.resolve();
        m_valuemapper = func.getResult();
    }

    /**
     * Creates instance of WDataSet2. It uses a sample to determine the correct WValueSet2 type. The value of the sample is completely
     * unimportant. You can simply use the default constructor. Depending on the ValueSet, the memory is allocated for the data and you can
     * afterwards use the visitor scheme to fill it. To use this constructor, you need to know the data type to be stored at compile-time.
     *
     * \tparam SampleT Sample type.
     * \param grid the grid to use.
     * \param sample a sample. Needed to determine the real type of the values in this WStillNeedsAName.
     */
    template< typename SampleT >
    WDataSet2( const GridType& grid, const SampleT& sample )
        : WDataSet2Base(),
          m_valuemapper( typename ValueMapperType::SPtr( new ValueMapperType( grid, sample ) ) )
    {
        // Initialize
    }

    /**
     * Destructor. Cleans up. Please note, that the destruction does not necessarily removes the Grid and Data instances as they are
     * reference counted.
     */
    virtual ~WDataSet2()
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
        m_valuemapper->getGrid();
    }

    /**
     * Returns the name of this dataset.
     *
     * \return The name of this dataset.
     */
    virtual std::string const getName() const
    {
        return "WDataSet2";
    }

    /**
     * Returns a description of this dataset.
     *
     * \return A description of this dataset.
     */
    virtual std::string const getDescription() const
    {
        return "The class that handles all data and data interaction.";
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
        m_valuemapper->applyVisitor( visitor );
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
        static_cast< ValueMapperType const* const >( m_valuemapper.get() )->applyVisitor( visitor );
    }

    /**
     * Returns a prototype for this class.
     *
     * \return The prototype instance.
     */
    static boost::shared_ptr< WPrototyped > getPrototype()
    {
        if ( !m_prototype )
        {
            m_prototype = boost::shared_ptr< WPrototyped >( new WDataSet2() );
        }

        return m_prototype;
    }

protected:

    //! The prototype instance.
    static boost::shared_ptr< WPrototyped > m_prototype;

private:

    //! The WStillNeedsAName. This instance is responsible for handling the grid and values properly.
    typename ValueMapperType::SPtr m_valuemapper;

    /**
     * A functor/visitor that creates the correct type of WStillNeedsAName for a type T.
     *
     * This is used for the construction of a dataset using runtime information about the type to store.
     */
    class InitDataSetFunctor
    {
    public:

        /**
         * Constructor.
         *
         * \param grid The grid to use.
         */
        explicit InitDataSetFunctor( typename GridType::ConstSPtr const& grid )
            : m_grid( grid )
        {
        }

        /**
         * Creates the WStillNeedsAName with the correct data type.
         *
         * \tparam The type of the data.
         */
        template< typename T >
        void operator()( T /* not used */ )
        {
            m_result = typename ValueMapperType::SPtr( new ValueMapperType( m_grid, T() ) );
        }

        /**
         * Returns the created WStillNeedsAName instance.
         *
         * \return The WStillNeedsAName instance of the correct type.
         */
        typename ValueMapperType::SPtr const& getResult() const
        {
            return m_result;
        }

    private:

        //! The resulting WStillNeedsAName instance.
        typename ValueMapperType::SPtr m_result;

        //! The grid to use for the dataset.
        typename GridType::ConstSPtr const& m_grid;
    };
};

template< typename GridT, typename StructuralT >
boost::shared_ptr< WPrototyped > WDataSet2< GridT, StructuralT >::m_prototype = boost::shared_ptr< WPrototyped >();

#endif  // WDATASET2_H

