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

#ifndef WDATASETACCESSOR_H
#define WDATASETACCESSOR_H

template< typename > class WValueSet;

template< typename GridT, typename ValueT >
class WDataSetAccessor
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
    WDataSetAccessor( typename GridType::ConstSPtr grid, typename WValueSet< ValueType >::SPtr valueSet ):
        m_grid( grid ),
        m_valueSet( valueSet )
    {
        // do something
    }

public:
    /**
     * The grid instance
     */
    typename GridType::ConstSPtr m_grid;

    /**
     * The valueset.
     */
    typename WValueSet< ValueType >::SPtr m_valueSet;
};

template< typename GridT, typename ValueT >
class WDataSetAccessorConst
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
    WDataSetAccessorConst( typename GridType::ConstSPtr grid, typename WValueSet< ValueType >::ConstSPtr valueSet ):
        m_grid( grid ),
        m_valueSet( valueSet )
    {
        // do something
    }

public:
    /**
     * The grid instance
     */
    typename GridType::ConstSPtr m_grid;

    /**
     * The valueset.
     */
    typename WValueSet< ValueType >::ConstSPtr m_valueSet;
};

#endif  // WDATASETACCESSOR_H

