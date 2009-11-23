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
        return m_dimension;
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
    size_t m_order;

    /**
     * The dimension of the tensors for this ValueSet
     */
    size_t m_dimension;

    /**
     * The data type of the values' elements.
     */
    dataType m_dataType;

private:
};

#endif  // WVALUESETBASE_H
