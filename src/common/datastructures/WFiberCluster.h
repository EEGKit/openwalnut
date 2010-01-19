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

#ifndef WFIBERCLUSTER_H
#define WFIBERCLUSTER_H

#include <list>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "../../dataHandler/WDataSetFibers.h"
#include "../WColor.h"
#include "../WTransferable.h"

/**
 * Represents a cluster of indices of a WDataSetFibers.
 */
class WFiberCluster : public WTransferable
{
friend class WFiberClusterTest;
public:
    /**
     * Constructs an cluster with one fiber and a reference to the fiber dataset
     * to compute the intercluster distance.
     *
     * \param index The index of the first fiber belonging to this cluster
     */
    explicit WFiberCluster( size_t index );

    /**
     * Constructs an empty cluster.
     */
    WFiberCluster();

    /**
     * Returns true if there are no fibers in that cluster, false otherwise.
     */
    bool empty() const;

    /**
     * Merge the fibers of the other cluster with the fibers of this cluster.
     * Afterwards the other cluster is empty.
     *
     * \param other The other WFiberCluster which should merged into this one
     */
    void merge( WFiberCluster &other ); // NOLINT

    /**
     * Returns a const reference of all indices inside this cluster
     */
    const std::list< size_t >& getIndices() const;

    /**
     * Sort the indices of fibers associated with this cluster in ascending
     * order.
     */
    void sort();

    /**
     * \return Number of fibers associated with this cluster.
     */
    size_t size() const;

    /**
     * Make this cluster empty. Note: The real fibers from fiber dataset are
     * not deleted.
     */
    void clear();

    /**
     * Sets the color of which all fibers of this clusters should be painted
     * with.
     *
     * \param color The color for all fibers of this cluster.
     */
    void setColor( WColor color );

    /**
     * Gets the color of which all fibers of this clusters should be painted
     * with.
     */
    WColor getColor() const;

    /**
     * The only reason for implementing is here, to prevent this class from
     * beeing abstract.
     *
     * \return A name.
     */
    virtual const std::string getName() const;

    /**
     * The only reason for implementing is here, to prevent this class from
     * beeing abstract.
     *
     * \return A name.
     */
    virtual const std::string getDescription() const;

    /**
     * \param other The other fiber which should be compared
     * \return true If both clusters having same fibers IN SAME ORDER!
     */
    bool operator==( const WFiberCluster& other ) const;

    /**
     * The opposite of the operator==
     *
     * \param other The other fiber which should be compared
     * \return false If both clusters having same fibers IN SAME ORDER!
     */
    bool operator!=( const WFiberCluster& other ) const;

    // TODO(math): The only reason why we store here a Reference to the fiber
    // dataset is, we need it in the WMVoxelizer module as well as the clustering
    // information. Since we don't have the possibility of multiple
    // InputConnectors we must agglomerate those into one object. Please remove this.
    // DISABLE DOXYGEN HERE
    // \cond
    void setDataSetReference( boost::shared_ptr< const WDataSetFibers > fibs );
    boost::shared_ptr< const WDataSetFibers > getDataSetReference() const;
    static boost::shared_ptr< WPrototyped > getPrototype();

protected:
    // TODO(math): The only reason why we store here a Reference to the fiber
    // dataset is, we need it in the WMVoxelizer module as well as the clustering
    // information. Since we don't have the possibility of multiple
    // InputConnectors we must agglomerate those into one object. Please remove this.
    static boost::shared_ptr< WPrototyped > m_prototype;
    // \endcond
    // ENDABLE DOXYGEN HERE

private:

    /**
     * All indices in this set are members of this cluster
     */
    std::list< size_t > m_memberIndices;

    /**
     * Reference to the real fibers of the brain this cluster belongs to.
     */
    boost::shared_ptr< const WDataSetFibers > m_fibs;

    /**
     * Color which is used to paint the members of this cluster.
     */
    WColor m_color;
};

inline bool WFiberCluster::empty() const
{
    return m_memberIndices.empty();
}

inline void WFiberCluster::sort()
{
    m_memberIndices.sort();
}

inline size_t WFiberCluster::size() const
{
    return m_memberIndices.size();
}

inline void WFiberCluster::clear()
{
    m_memberIndices.clear();
}

inline void WFiberCluster::setColor( WColor color )
{
    m_color = color;
}

inline WColor WFiberCluster::getColor() const
{
    return m_color;
}

inline const std::string WFiberCluster::getName() const
{
    return "FiberCluster";
}

inline const std::string WFiberCluster::getDescription() const
{
    return "A collection of indices for fibers representing a fiber cluster";
}

inline bool WFiberCluster::operator==( const WFiberCluster& other ) const
{
    return m_memberIndices == other.m_memberIndices;
}

inline bool WFiberCluster::operator!=( const WFiberCluster& other ) const
{
    return m_memberIndices != other.m_memberIndices;
}

inline const std::list< size_t >& WFiberCluster::getIndices() const
{
    return m_memberIndices;
}

inline std::ostream& operator<<( std::ostream& os, const WFiberCluster& c )
{
    using string_utils::operator<<;
    return os << c.getIndices();
}

#endif  // WFIBERCLUSTER_H
