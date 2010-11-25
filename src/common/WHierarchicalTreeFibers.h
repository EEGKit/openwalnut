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

#ifndef WHIERARCHICALTREEFIBERS_H
#define WHIERARCHICALTREEFIBERS_H

#include <utility>
#include <vector>
#include <queue>
#include <list>

#include <boost/shared_ptr.hpp>

#include "WColor.h"

#include "WHierarchicalTree.h"

/**
 * Class implements a hierarchical tree and provides helper functions for selection and navigation
 */
class WHierarchicalTreeFibers : public WHierarchicalTree
{
public:
    /**
     * standard constructor
     */
    WHierarchicalTreeFibers();

    /**
     * destructor
     */
    ~WHierarchicalTreeFibers();

    /**
     * A leaf is at the very bottom of the tree, it represents a single fiber or voxel, for several purposes
     * a leaf also counts as a cluster
     */
    void addLeaf();

    /**
     * adds a cluster to the set, it combines 2 already existing clusters
     *
     * \param cluster1 first cluster to add
     * \param cluster2 second cluster to add
     * \param level level of the new cluster
     * \param leafes vector of leafes the new cluster contains
     * \param customData some arbitrary data stored with the cluster
     */
    void addCluster( size_t cluster1, size_t cluster2, size_t level, std::vector<size_t> leafes, float customData );

    /**
     * generates a bitfield where for every leaf in the selected cluster the value is true, false otherwise
     *
     * \param cluster
     * \return shared pointer to the bitfield
     */
    boost::shared_ptr< std::vector<bool> >getOutputBitfield( size_t cluster );

    /**
     * generates a bitfield where for every leaf in the selected cluster the value is true, false otherwise
     *
     * \param clusters
     * \return shared pointer to the bitfield
     */
    boost::shared_ptr< std::vector<bool> >getOutputBitfield( std::vector<size_t>clusters );

    /**
     * finds clusters that match a given ROI up to a certain percentage
     *
     * \param ratio value of [0,1] of how many leafes have to be in the roi to activate the cluster
     * \param number number of cluster to select, if more than give number matches the ratio criteria only the
     * biggest clusters are returned
     */
    std::vector<size_t> getBestClustersFittingRoi( float ratio = 0.9, size_t number = 1 );

    /**
     * calculates the ratio of fibers in the roi for a given cluster
     * \param cluster
     * \return ratio
     */
    float getRatio( size_t cluster );

    /**
     * setter
     * \param bitfield
     */
    void setRoiBitField( boost::shared_ptr< std::vector<bool> > bitfield );

    /**
     * finds the X biggest clusters for a given cluster
     * \param cluster
     * \param number of sub clusters
     */
    std::vector< size_t >findXBiggestClusters( size_t cluster, size_t number = 10 );

    /**
     * helper function to sort a list of clusters depending on the number of leafes in them
     * \param input reference to the list to be sorted
     */
    void sortList( std::list<size_t> &input ); //NOLINT


    /**
     * sets the color for a selected cluster and all sub clusters
     * \param cluster
     * \param color
     */
    void colorCluster( size_t cluster, WColor color );

    /**
     * getter
     * \param cluster
     * \return the leafes contained in the selected cluster
     */
    std::vector<size_t>getLeafesForCluster( size_t cluster );

protected:
private:
    /**
     * stores a pointer to the bitfield by the current roi setting
     */
    boost::shared_ptr< std::vector<bool> > m_roiSelection;

    /**
     *vector that stores the leaf id's for each cluster, this is quite memory intensive but speeds up selection
     * of leafes for nodes at higher levels
     */
    std::vector< std::vector<size_t> >m_containsLeafes;
};


inline void WHierarchicalTreeFibers::setRoiBitField( boost::shared_ptr< std::vector<bool> > bitfield )
{
    m_roiSelection = bitfield;
}

inline std::vector<size_t>WHierarchicalTreeFibers::getLeafesForCluster( size_t cluster )
{
    return m_containsLeafes[cluster];
}

#endif  // WHIERARCHICALTREEFIBERS_H
