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

#ifndef WJOINCONTOURTREE_H
#define WJOINCONTOURTREE_H

#include <set>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "../WDataSetSingle.h"

/**
 * Processes a dataset for join tree computation. This is a part of those famous contur trees.
 *
 * Every leaf in that tree represents a local maximum. A branch is a collection of vertices belonging to the
 * same component and nodes joining branches represent a data point which melds multiple (at least two)
 * branches.
 *
 * With the Split tree then you may compute the contour tree, but for that you may need to fullfil at least two
 * conditions:
 *  - You operate on a simplicial mesh (WGridRegular3D is not simplicial!!!)
 *  - All data points are pairwise disjoint
 *
 * \note You may use this join tree also for finding the vertices belonging to the volume enclosed by the
 * biggest ISO surface for a given ISO value. Then you don't need "simulation of simplicity" to make the
 * data points disjoint also you don't need simplicial meshes.
 */
class WJoinContourTree
{
friend class WJoinContourTreeTest;
public:
    /**
     * Initialize this with a data set for which the join tree should be computed.
     *
     * \throw WNotImplemented If the dataset is not a scalar double field
     *
     * \param dataset Reference to the dataset.
     */
    explicit WJoinContourTree( boost::shared_ptr< WDataSetSingle > dataset );

    /**
     * Build the join tree.
     */
    void buildJoinTree();

    /**
     * For a given ISO value all the voxel which are enclosed by the biggest ISO surface are computed.
     *
     * \param isoValue The ISO value
     *
     * \return Set of voxel indices
     */
    boost::shared_ptr< std::set< size_t > > getVolumeVoxelsEnclosedByISOSurface( const double isoValue ) const;

protected:
    /**
     * Sort the indices on their element value of the value set in descending order.
     */
    void sortIndexArray();

private:
    boost::shared_ptr< WGridRegular3D > m_grid; //!< Stores the reference to the grid of the given dataset to get the neighbours of a voxel
    boost::shared_ptr< WValueSet< double > > m_valueSet; //!< Stores reference to the ISO values, so we may sort them indirect on their value

    std::vector< size_t > m_elementIndices; //!< Stores the component number for the i'th vertex in the value set
    std::vector< size_t > m_joinTree; //!< For each index stores which node it is connected to
    std::vector< size_t > m_lowestVoxel; //!< Stores the index of lowest element for the i'th component

    /**
     * Comperator for indirect sort so the value set is not modified.
     */
    class IndirectCompare
    {
    public: // NOLINT
        /**
         * Since we must have access to the value set we need a reference to it.
         *
         * \param valueSet Value set on which the comparision is done.
         */
        explicit IndirectCompare( boost::shared_ptr< WValueSet< double > > valueSet );

        /**
         * Compares the ISO value of the elments with index i and j.
         *
         * \param i The index of the first element
         * \param j The index of the other element
         *
         * \return True if the element in the value set at position i is
         * greater than the the element at position j
         */
        bool operator()( size_t i, size_t j );

    private: // NOLINT
        boost::shared_ptr< WValueSet < double > > m_valueSet; //!< Reference to the ISO values
    };
};

#endif  // WJOINCONTOURTREE_H
