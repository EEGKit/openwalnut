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

#ifndef WPROPERTYCONSTRAINTNOTEMPTY_H
#define WPROPERTYCONSTRAINTNOTEMPTY_H

#include "WPropertyTypes.h"
#include "WPropertyConstraintTypes.h"

template< typename T >
class WPropertyVariable;

/**
 * This class allows constraining properties to be not empty. This is especially useful for strings. This works on all types
 * providing an empty() member function (as std::string and boost::filesystem::path do).
 */
template< typename T >
class WPropertyConstraintNotEmpty: public WPropertyVariable< T >::PropertyConstraint
{
public:
    /**
     * Constructor.
     */
    explicit WPropertyConstraintNotEmpty();

    /**
     * Destructor.
     */
    virtual ~WPropertyConstraintNotEmpty();

    /**
     * Checks whether the specified new value is larger or equal to the specified min value.
     *
     * \param property the property whose new value should be set.
     * \param value the new value to check
     *
     * \return true if value >= m_min
     */
    virtual bool accept( boost::shared_ptr< WPropertyVariable< T > > property, T value );

    /**
     * Allows simple identification of the real constraint type.
     *
     * \return the type
     */
    virtual PROPERTYCONSTRAINT_TYPE getType();

private:
};

template < typename T >
WPropertyConstraintNotEmpty< T >::WPropertyConstraintNotEmpty()
{
}

template < typename T >
WPropertyConstraintNotEmpty< T >::~WPropertyConstraintNotEmpty()
{
}

template < typename T >
bool WPropertyConstraintNotEmpty< T >::accept( boost::shared_ptr< WPropertyVariable< T > > /* property */, T value )
{
    return !value.empty();
}

template < typename T >
PROPERTYCONSTRAINT_TYPE WPropertyConstraintNotEmpty< T >::getType()
{
    return PC_NOTEMPTY;
}

#endif  // WPROPERTYCONSTRAINTNOTEMPTY_H

