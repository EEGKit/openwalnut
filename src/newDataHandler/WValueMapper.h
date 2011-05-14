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

#include "WDataSetVisitor.h"
#include "WValueSet.h"

/**
 * The value-mapper class. It handles the grid and value-sets and their interaction. It provides interfaces for accessing and processing them.
 * The WValueMapper does not know the real integral type or other value-type specifics. It only knows the structural type. If you want your own
 * WDataSet2 types, you should probably modify WDataSet2. It is an interface/adapter to this class.
 *
 * \tparam GridT The grid type to use.
 * \tparam StructuralT The structural type to use.
 */
template< typename GridT, typename StructuralT >
class WValueMapper
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
     * The type of the value set with the specified structural type
     */
    typedef WValueSetBase ValueSetBaseType;

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
        m_grid( grid ),
        m_valueSet( typename ValueSetBaseType::ConstSPtr( new WValueSet< SampleT >( grid->size() ) ) )
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
     * Applies the given visitor. It therefore uses the value-set's dispatch mechanism to call a WDataSetVisitorDispatcher, which, in turn, calls
     * the WDataSetVisitor.
     *
     * \param visitor the visitor to apply.
     */
    void applyVisitor( WDataSetVisitor& visitor ) const
    {
        // do a double dispatch. The Value-set calls the WDataSetVisitorDispatcher. This unveils the real value-type. The dispatcher then calls
        // the WDataSetVisitor instance.
        WDataSetVisitorDispatcher d( visitor );
        // the dispatch function calls the const operator() in d for us. We do not need to further handle const-ness here.
        m_valueSet->dispatch( d );
    }

    /**
     * Applies the given visitor. It therefore uses the value-set's dispatch mechanism to call a WDataSetVisitorDispatcher, which, in turn, calls
     * the WDataSetVisitor.
     *
     * \param visitor the visitor to apply.
     */
    void applyVisitor( WDataSetVisitor& visitor )
    {
        // do a double dispatch. The Value-set calls the WDataSetVisitorDispatcher. This unveils the real value-type. The dispatcher then calls
        // the WDataSetVisitor instance.
        WDataSetVisitorDispatcher d( visitor );
        m_valueSet->dispatch( d );
    }

    /**
     * Applies the given visitor. It therefore uses the value-set's dispatch mechanism to call a WDataSetVisitorDispatcher, which, in turn, calls
     * the WDataSetVisitor.
     *
     * \param visitor the visitor to apply.
     */
    void applyVisitor( WDataSetVisitor* visitor ) const
    {
        // do a double dispatch. The Value-set calls the WDataSetVisitorDispatcher. This unveils the real value-type. The dispatcher then calls
        // the WDataSetVisitor instance.
        WDataSetVisitorDispatcher d( &visitor );
        // the dispatch function calls the const operator() in d for us. We do not need to further handle const-ness here.
        m_valueSet->dispatch( d );
    }

    /**
     * Applies the given visitor. It therefore uses the value-set's dispatch mechanism to call a WDataSetVisitorDispatcher, which, in turn, calls
     * the WDataSetVisitor.
     *
     * \param visitor the visitor to apply.
     */
    void applyVisitor( WDataSetVisitor* visitor )
    {
        // do a double dispatch. The Value-set calls the WDataSetVisitorDispatcher. This unveils the real value-type. The dispatcher then calls
        // the WDataSetVisitor instance.
        WDataSetVisitorDispatcher d( &visitor );
        m_valueSet->dispatch( d );
    }

protected:
private:

    /**
     * The grid in which the data exists.
     */
    typename GridType::ConstSPtr m_grid;

    /**
     * The value-set.
     */
    typename ValueSetBaseType::ConstSPtr m_valueSet;
};

#endif  // WVALUEMAPPER_H

