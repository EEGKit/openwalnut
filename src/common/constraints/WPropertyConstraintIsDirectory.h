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

#ifndef WPROPERTYCONSTRAINTISDIRECTORY_H
#define WPROPERTYCONSTRAINTISDIRECTORY_H

#include "../WPropertyTypes.h"
#include "WPropertyConstraintTypes.h"

template< typename T >
class WPropertyVariable;

/**
 * This class allows constraining properties to be existing filenames. This is especially useful for WPropFilename.
 */
template < typename T >
class WPropertyConstraintIsDirectory: public WPropertyVariable< T >::PropertyConstraint
{
public:
    /**
     * Constructor.
     */
    explicit WPropertyConstraintIsDirectory();

    /**
     * Destructor.
     */
    virtual ~WPropertyConstraintIsDirectory();

    /**
     * Checks whether the specified value is a directory or not.
     *
     * \param property the property whose new value should be set.
     * \param value the new value to check
     *
     * \return true if the file/path is a directory
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
WPropertyConstraintIsDirectory< T >::WPropertyConstraintIsDirectory()
{
}

template < typename T >
WPropertyConstraintIsDirectory< T >::~WPropertyConstraintIsDirectory()
{
}

template < typename T >
bool WPropertyConstraintIsDirectory< T >::accept( boost::shared_ptr< WPropertyVariable< T > > /* property */, T value )
{
    return boost::filesystem::is_directory( value );
}

template < typename T >
PROPERTYCONSTRAINT_TYPE WPropertyConstraintIsDirectory< T >::getType()
{
    return PC_ISDIRECTORY;
}

#endif  // WPROPERTYCONSTRAINTISDIRECTORY_H

