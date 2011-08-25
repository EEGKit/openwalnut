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

#ifndef WDATAACCESS_H
#define WDATAACCESS_H

// forward declaration
template< typename > class WValueSet2;

/**
 * Provides functions for random access to the data of a dataset as well as
 * functions that provide different kinds of iterator ranges. Every grid needs to specialize this.
 */
template< typename GridT, typename ValueT >
class WDataAccess
{
public:
    /**
     * The grid type.
     */
    typedef GridT GridType;

    /**
     * The type of the values in the dataset
     */
    typedef ValueT ValueType;

    /**
     * Constructs accessor. Requires the valueset and grid.
     *
     * \param grid the grid needed to access the data
     * \param valueSet the values
     */
    WDataAccess( typename GridType::ConstSPtr grid, typename WValueSet2< ValueType >::SPtr valueSet ):
        m_grid( grid ),
        m_valueSet( valueSet )
    {
        // do something
    }

private:
    /**
     * The grid instance
     */
    typename GridType::ConstSPtr m_grid;

    /**
     * The valueset.
     */
    typename WValueSet2< ValueType >::SPtr m_valueSet;
};

/**
 * Provides functions for random access to the data of a CONST dataset as well as
 * functions that provide different kinds of const-iterator ranges. Every grid needs to specialize this.
 */
template< typename GridT, typename ValueT >
class WDataAccessConst
{
public:
    /**
     * The grid type.
     */
    typedef GridT GridType;

    /**
     * The type of the values in the dataset
     */
    typedef ValueT ValueType;

    /**
     * Constructs accessor. Requires the valueset and grid.
     *
     * \param grid the grid needed to access the data
     * \param valueSet the values
     */
    WDataAccessConst( typename GridType::ConstSPtr grid, typename WValueSet2< ValueType >::ConstSPtr valueSet ):
        m_grid( grid ),
        m_valueSet( valueSet )
    {
        // do something
    }

private:
    /**
     * The grid instance
     */
    typename GridType::ConstSPtr m_grid;

    /**
     * The valueset.
     */
    typename WValueSet2< ValueType >::ConstSPtr m_valueSet;
};

#endif  // WDATAACCESS_H

