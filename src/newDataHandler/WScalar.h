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

#ifndef WSCALAR_H
#define WSCALAR_H

#include <stdint.h>

#include "WScalarBaseImpl.h"

/**
 * TODO(ebaum): write.
 */
class WScalar
{
public:

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors:
    //   These constructors allow simple and intuitive use of WScalar as a integral type
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Default constructor.
     */
    WScalar():
        m_impl( new WScalarImplDouble( 0.0 ) )
    {
        // init empty
    };

    /**
     * Construct a new WScalar with a special native type and value. As this constructor is not explicit, one may use WScalar in a nice way:
     * "WScalar b = 5.0;".
     *
     * \param val the value to set as initial value
     */
    WScalar( double val ):
        m_impl( new WScalarImplDouble( val ) )
    {
        // initialize
    };

    /**
     * Construct a new WScalar with a special native type and value. As this constructor is not explicit, one may use WScalar in a nice way:
     * "WScalar b = 5;".
     *
     * \note for integer types, the compiler can not decide for you which actual type (char, int, long) you really want, which is why WScalar
     * uses int32_t as default integral type.
     *
     * \param val the value to set as initial value
     */
    WScalar( int32_t val ):
        m_impl( new WScalarImplInt32( val ) )
    {
        // initialize
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Copying:
    //   Providing copy constructor and copy assignment with a light-weight copying allows fast use of WScalar instances in more complex
    //   formula and helps "faking" the integral nature of it.
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Copy constructor.
     *
     * \param other original instance to copy.
     */
    WScalar( const WScalar& other ):
        m_impl( other.m_impl )  // TODO(ebaum): thats not a deep copy as it would be needed. Implement this.
    {
        // copy
    };

    /**
     * Copy assignment operator for copying the specified instance of WScalar.
     *
     * \param other the instance to copy
     *
     * \return this instance copied from the other one
     */
    WScalar& operator=( const WScalar& other )
    {
        // copy and return this
        //m_impl = other.m_impl;
        // TODO(ebaum): implement this
        return *this;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Operators:
    //   Comparison, add, sub, multiply, divide, ...
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool operator==( const WScalar& other ) const
    {
        return m_impl->compare( *other.m_impl );
    };

    bool operator!=( const WScalar& other ) const
    {
        return !m_impl->compare( *other.m_impl );
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // The other stuff:
    //   Destruction.
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Destructor.
     */
    virtual ~WScalar()
    {
        // clean up
        delete m_impl;
    }

protected:

    /**
     * The actual instance containing the real value and all the possible operations.
     */
    WScalarImpl* m_impl;

private:
};

#endif  // WSCALAR_H

