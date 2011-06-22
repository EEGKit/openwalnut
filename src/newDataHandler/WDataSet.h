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

#ifndef WDATASET_H
#define WDATASET_H

#include <boost/shared_ptr.hpp>

#include "WStillNeedsAName.h"
#include "WValueSet.h"

/**
 * The DataSet class. This class handles all which is needed for keeping data. It provides interfaces to apply visitor functions to it and to
 * process data. Please note, that the Grid is const after creation of the instance. The WDataSet disguises the real integral type and uses
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
class WDataSet
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
    typedef boost::shared_ptr< WDataSet< GridType, StructuralType > > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WStillNeedsAName<...> >.
     */
    typedef boost::shared_ptr< const WDataSet< GridType, StructuralType > > ConstSPtr;

    /**
     * Creates instance of WDataSet. It uses a sample to determine the correct WValueSet type. The value of the sample is completely
     * unimportant. You can simply use the default constructor. Depending on the ValueSet, the memory is allocated for the data and you can
     * afterwards use the visitor scheme to fill it.
     *
     * \tparam SampleT Sample type.
     * \param grid the grid to use.
     * \param sample a sample. Needed to determine the real type of the values in this WStillNeedsAName.
     */
    template< typename SampleT >
    WDataSet( const GridType& grid, const SampleT& sample ):
        m_valuemapper( typename ValueMapperType::SPtr( new ValueMapperType( grid, sample ) ) )
    {
        // Initialize
    }

    /**
     * Destructor. Cleans up. Please note, that the destruction does not necessarily removes the Grid and Data instances as they are
     * reference counted.
     */
    virtual ~WDataSet()
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

protected:
private:

    /**
     * The WStillNeedsAName. This instance is responsible for handling the grid and values properly.
     */
    typename ValueMapperType::SPtr m_valuemapper;
};

#endif  // WDATASET_H

