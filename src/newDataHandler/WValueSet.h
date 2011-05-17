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

#ifndef WVALUESET_H
#define WVALUESET_H

#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

class WValueSetBase
{
public:
    /**
     * Convenience typedef for a boost::shared_ptr< WValueSet<...> >.
     */
    typedef boost::shared_ptr< WValueSetBase > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WValueSet<...> >.
     */
    typedef boost::shared_ptr< const WValueSetBase > ConstSPtr;

    /**
     * Constructor to create an empty value-set. Instantiating this class directly is quite useless as it does not store any data. Derive from
     * it.
     *
     * \param elements the number of elements to be used.
     */
    explicit WValueSetBase( size_t elements ):
        m_elements( elements )
    {
        // initialize
    }

    /**
     * Destructor.
     */
    virtual ~WValueSetBase()
    {
        // clean-up
    }

    /**
     * Returns the size of the valueset. It is the number of elements in it.
     *
     * \return number of elements
     */
    size_t size() const
    {
        return m_elements;
    }

private:
    /**
     * The number of elements in the data array.
     */
    size_t m_elements;
};

template< typename ValueT >
class WValueSet: public WValueSetBase
{
public:

    /**
     * The real type used for storing the values.
     */
    typedef ValueT ValueType;

    /**
     * Convenience typedef for a boost::shared_ptr< WValueSet<...> >.
     */
    typedef boost::shared_ptr< WValueSet< ValueType > > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WValueSet<...> >.
     */
    typedef boost::shared_ptr< const WValueSet< ValueType > > ConstSPtr;

    /**
     * Default constructor. This knows the real value-type and provides the needed mechanism to unveil it to an specified operator. It
     * additionally stores data as array in memory. If you want to write your own value-set, derive from WValueSetTyped.
     *
     * \param elements number of elements in the array
     */
    explicit WValueSet( size_t elements ):
        WValueSetBase( elements )
    {
        // initialize
    }

    /**
     * Destructor. Does not necessarily remove the data array as it is reference counted and may be used by other value-sets.
     */
    virtual ~WValueSet()
    {
        // clean-up
    }

    /**
     * Provides access to an item at a given index.
     *
     * \param index the index to access
     *
     * \return the value at index.
     */
    const ValueType& operator[]( size_t index ) const
    {
        return m_data.operator[]( index );
    }

    /**
     * Provides access to an item at a given index.
     *
     * \param index the index to access
     *
     * \return the value at index.
     */
    ValueType& operator[]( size_t index )
    {
        return m_data.operator[]( index );
    }

protected:
private:
    /**
     * The plain data. Since the destruction is handled by boost::shared_array, m_data can be distributed along several copies of the valueset
     * instance which created it. This is a very often needed feature, since we want to keep several WValueSets and Grid in different
     * combinations.
     */
    boost::shared_array< ValueType > m_data;
};



#endif  // WVALUESET_H

