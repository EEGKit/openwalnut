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

/**
 * TODO(ebaum): write.
 */
template< typename StructuralT >
class WValueSet
{
public:

    /**
     * The real type used for storing the values.
     */
    typedef StructuralT StructuralType;

    /**
     * Convenience typedef for a boost::shared_ptr< WValueSet<...> >.
     */
    typedef boost::shared_ptr< WValueSet< StructuralType > > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WValueSet<...> >.
     */
    typedef boost::shared_ptr< const WValueSet< StructuralType > > ConstSPtr;

    /**
     * Default constructor. Creates an instance of WValueSet and allocates the needed memory. It uses the specified sample (structural type)
     * to create correct real types. The specified number of samples is mandatory. This information is normally retrieved in the WValueMapper
     * and provided for construction.
     *
     * \tparam SampleT The type of the sample. Determines real type of data array.
     * \param elements number of elements in the array
     * \param sample the sample. See SampleT.
     */
    template< typename SampleT >
    WValueSet( size_t elements, const SampleT& sample ):
        m_elements( elements )
    {
        // Initialize
        //StructuralType::BySample( sample )
    }

    /**
     * Destructor. Does not necessarily remove the data array as it is reference counted and may be used by other value-sets.
     */
    virtual ~WValueSet()
    {
        // clean-up
    }

protected:
private:

    /**
     * The plain data. Since the destruction is handled by boost::shared_array, m_data can be distributed along several copies of the valueset
     * instance which created it. This is a very often needed feature, since we want to keep several WValueSets and Grid in different
     * combinations.
     */
    boost::shared_array< double > m_data;

    /**
     * The number of elements in the data array.
     */
    size_t m_elements;
};

#endif  // WVALUESET_H

