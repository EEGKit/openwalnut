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

#ifndef WHIERARCHICALTREE_H
#define WHIERARCHICALTREE_H

#include <utility>
#include <vector>
#include <queue>
#include <list>

#include <boost/shared_ptr.hpp>

#include "WColor.h"
#include "WExportCommon.h"


/**
 * base class for hierarchical tree implementations
 */
class OWCOMMON_EXPORT WHierarchicalTree
{
public:
    /**
     * standard constructor
     */
    WHierarchicalTree();

    /**
     * destructor
     */
    virtual ~WHierarchicalTree();

    /**
     * A leaf is at the very bottom of the tree, it represents a single fiber or voxel, for several purposes
     * a leaf also counts as a cluster
     */
    virtual void addLeaf() = 0;

    /**
     * adds a cluster to the set, it combines 2 already existing clusters
     *
     * \param cluster1 first cluster to add
     * \param cluster2 second cluster to add
     * \param customData some arbitrary data stored with the cluster
     */
    void addCluster( size_t cluster1, size_t cluster2, float customData );

    /**
     * getter
     * \return the number of leafes
     */
    size_t getLeafCount();

    /**
     * getter
     * \return the number of clusters
     */
    size_t getClusterCount();

    /**
     * getter
     * \return maxlevel, i.e. the level of the root cluster
     */
    size_t getMaxLevel();

    /**
     * getter
     * \param cluster
     * \return the level of the selected cluster
     */
    size_t getLevel( size_t cluster );

    /**
     * getter
     * \param cluster the cluster in question
     * \return the parent for the selected cluster
     */
    size_t getParent( size_t cluster );

    /**
     * getter
     * \param cluster
     * \return the custom data for the selected cluster
     */
    float getCustomData( size_t cluster );

    /**
     * setter sets the color for a single cluster
     * \param color
     * \param cluster
     */
    void setColor( WColor color, size_t cluster );

    /**
     * getter
     * \param cluster
     * \return the color for the selected cluster
     */
    WColor getColor( size_t cluster );

    /**
     * getter
     * \param cluster
     * \return children for the selected cluster
     */
    std::pair<size_t, size_t>getChildren( size_t cluster );

    /**
     * sets the color for a selected cluster and all sub clusters
     * \param cluster
     * \param color
     */
    virtual void colorCluster( size_t cluster, WColor color ) = 0;

    /**
     * getter
     * \param cluster
     * \return the leafes contained in the selected cluster
     */
    virtual std::vector<size_t>getLeafesForCluster( size_t cluster ) = 0;

    /**
     * getter
     * \param cluster
     * \return number of leafes for the selected cluster
     */
    size_t size( size_t cluster );

    /**
     * checks if a cluster is a leaf or a cluster
     * \param cluster
     * \return true if it is a leaf
     */
    bool isLeaf( size_t cluster );

protected:
    /**
     * overall number of cluster, counts both leafes ( clusters of size one ) and real clusters
     */
    size_t m_clusterCount;

    /**
     * number of leaf nodes
     */
    size_t m_leafCount;

    /**
     * the maximum level, naturally the level of the root node
     */
    size_t m_maxLevel;

    /**
     * to enforce valid datastructures there will be no leaf with an id higher than a cluster, thus when
     * the first cluster is inserted, no leafes may be added
     */
    bool m_leafesLocked;

    /**
     * vector that stores the level of each cluster, the level is the maximum of the levels of the child clusters +1
     */
    std::vector<size_t>m_level;

    /**
     * vector that stores the parent cluster for each cluster
     */
    std::vector<size_t>m_parents;

    /**
     * vector that stores the 2 children of each cluster, contains an empty pair for leafes
     */
    std::vector< std::pair< size_t, size_t> >m_children;

    /**
     * custom data for each cluster, this may some energy or similarity level generated by the clustering algorithm
     * or something completely different
     */
    std::vector<float>m_customData;

    /**
     * a color value for each cluster
     */
    std::vector<WColor>m_colors;
private:
};

inline size_t WHierarchicalTree::getLeafCount()
{
    return m_leafCount;
}

inline size_t WHierarchicalTree::getClusterCount()
{
    return m_clusterCount;
}

inline size_t WHierarchicalTree::getMaxLevel()
{
    return m_maxLevel;
}

inline size_t WHierarchicalTree::getLevel( size_t cluster )
{
    return m_level[cluster];
}

inline size_t WHierarchicalTree::getParent( size_t cluster )
{
    if ( m_level[cluster] < m_maxLevel )
    {
        return m_parents[cluster];
    }
    else
    {
        // this is just to quiet the compiler, this branch should never be reached
        return cluster;
    }
}

inline std::pair<size_t, size_t>WHierarchicalTree::getChildren( size_t cluster )
{
    if ( m_level[cluster] > 0 )
    {
        return m_children[cluster];
    }
    else
    {
        // this is just to quiet the compiler, this branch should never be reached
        return std::pair<size_t, size_t>( cluster, cluster );
    }
}

inline float WHierarchicalTree::getCustomData( size_t cluster )
{
    return m_customData[cluster];
}

inline size_t WHierarchicalTree::size( size_t cluster )
{
    return getLeafesForCluster( cluster ).size();
}

inline void WHierarchicalTree::setColor( WColor color, size_t cluster )
{
    m_colors[cluster] = color;
}

inline WColor WHierarchicalTree::getColor( size_t cluster )
{
    return m_colors[cluster];
}

inline bool WHierarchicalTree::isLeaf( size_t cluster )
{
    return ( cluster < m_leafCount ) ? true : false;
}

#endif  // WHIERARCHICALTREE_H
