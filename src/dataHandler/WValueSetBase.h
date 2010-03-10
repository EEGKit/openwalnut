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

#ifndef WVALUESETBASE_H
#define WVALUESETBASE_H

#include <cstddef>
#include <cmath>
#include "WDataHandlerEnums.h"

/**
 * Abstract base class to all WValueSets. This class doesn't provide any genericness.
 * \ingroup dataHandler
 */
class WValueSetBase
{
public:
    /**
     * Despite this is an abstract class all subclasses should have an order
     * and dimension.
     * \param order the tensor order of the values stored in this WValueSet
     * \param dimension the tensor dimension of the values stored in this WValueSet
     * \param inDataType indication of the primitive data type used to store the values
     */
    WValueSetBase( size_t order, size_t dimension, dataType inDataType );

    /**
     * Dummy since each class with virtual member functions needs one.
     */
    virtual ~WValueSetBase() = 0;

    /**
     * \return The number of tensors in this ValueSet.
     */
    virtual size_t size() const = 0;

    /**
     * \return The number of integrals (POD like int, double) in this ValueSet.
     */
    virtual size_t rawSize() const = 0;

    /**
     * \return Dimension of the values in this ValueSet
     */
    virtual size_t dimension() const
    {
        return m_dimension;
    }

    /**
     * \return Order of the values in this ValueSet
     */
    virtual size_t order() const
    {
        return m_order;
    }

    /**
     * Returns the number of elements of type T per value.
     * \note this is dimension to the power of order.
     * \return number of elements per value
     */
    virtual size_t elementsPerValue() const
    {
        return static_cast< size_t >( std::pow( static_cast< double >( m_dimension ), static_cast< int >( m_order ) ) );
    }

    /**
     * \return Dimension of the values in this ValueSet
     */
    virtual dataType getDataType() const
    {
        return m_dataType;
    }

protected:
    /**
     * The order of the tensors for this ValueSet
     */
    const size_t m_order;

    /**
     * The dimension of the tensors for this ValueSet
     */
    const size_t m_dimension;

    /**
     * The data type of the values' elements.
     */
    const dataType m_dataType;

private:
};

#endif  // WVALUESETBASE_H
