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

#ifndef WSPANSPACE_H
#define WSPANSPACE_H

#include <vector>
#include <algorithm>
#include <utility>
#include <tr1/functional>

#include "../../common/WLogger.h"
#include "../../common/WProgressCombiner.h"

/**
 * Contains the minimum and maximum iso value
 */
template< typename T > struct MinMax
{
    /**
     * Initial min/max iso values
     *
     * \param min minimum isovalue
     * \param max maximum isovalue
     */
    MinMax( T min, T max ):
    m_max( max ), m_min( min )
    {
    }
    T m_max; //!< max iso value
    T m_min; //!< min iso value
};

/**
 * Contains the KdCell coordinates
 */
struct KdCellId
{
    /**
     * Initial KdCell coordinates
     *
     * \param x X coordinate
     * \param y Y coordinate
     * \param z Z coordinate
     */
    KdCellId( unsigned int x, unsigned int y, unsigned int z ):
    m_x( x ), m_y( y ), m_z( z )
    {
    }

    unsigned int m_x; //!< X coordinate
    unsigned int m_y; //!< Y coordinate
    unsigned int m_z; //!< Z coordinate
};

/**
 * Contains the min/max values and the Kd coordinates
 */
template< typename T > struct KdCells
{
    /**
     * Initial KdCell coordinates and min/max iso value
     *
     * \param minMax minimum and maximum isovalue
     * \param x X coordinate
     * \param y Y coordinate
     * \param z Z coordinate
     */
    KdCells( MinMax< T > minMax, unsigned int x, unsigned int y, unsigned int z ):
    m_minMax( minMax ), m_id( KdCellId( x, y, z ) )
    {
    }
    MinMax< T > m_minMax; //!< min/max iso value of KdCell
    KdCellId m_id; //!< KdCell coordinates
};

/**
 * This class saves the node of the KdTree nodes
 */
template< class T > class SSKdTreeNode
{
public:
    /**
     * Initial min/max iso values
     */
    SSKdTreeNode():
        m_location( minmax_t( T(), T() ), ( unsigned int ) 0, ( unsigned int ) 0, ( unsigned int ) 0 )
    {
    }

    typedef MinMax< T > minmax_t; //!< minmax type used for KdCells
    typedef KdCells< T > cells_t; //!< typ for KdCells
    typedef std::vector< cells_t > kdTree; //!< vector of KdCells

    boost::shared_ptr< SSKdTreeNode< T > > m_leftTree; //!< left tree of kd-tree median is NOT included
    boost::shared_ptr< SSKdTreeNode< T > > m_rightTree; //!< right tree of kd-tree median is included
    cells_t m_location; //!< location of KdTree Node contains cell cordinates and min/max iso values
};

/**
 * This class creates the KdTree
 */
template< class T > class WSpanSpaceKdTree
{
public:
    typedef MinMax< T > minmax_t; //!< minmax type used for KdCells
    typedef KdCells< T > cells_t; //!< the typ for KdCells
    typedef std::vector< cells_t > kdTree_t; //!< define vector of KdCells

    WSpanSpaceKdTree()
    {
    }

    /**
     * Returns a SpanSpace Kd-Tree node.
     *
     * \param points vector of MinMax points
     * \param depth Kd-Tree depth
     * \param first (this element is included)
     * \param last  (this element isn't included)
     *
     * \return return a SpanSpace Kd-Tree node.
     */
    boost::shared_ptr< SSKdTreeNode< T > > createTree(  kdTree_t& points, unsigned int depth, size_t first, size_t last );

private:
    int m_depth; //!< depth of KdTree
    
    /**
     * Sort method for the nth_element function
     *
     * \param i first cell which is compared to secound cell
     * \param j secound cell which is compared to the first cell
     *
     * \return for even numbers true if the first cell has a smaller minimum isovalue and 
     *         for odd numbers true if the secound cell has a bigger maximum isovalue
     */
    bool sortValues( cells_t i, cells_t j );
};

template< class T > boost::shared_ptr< SSKdTreeNode< T > > WSpanSpaceKdTree< T >::createTree(
        kdTree_t& points, unsigned int depth, size_t first, size_t last )
{
    size_t size = last - first;
    size_t median = size / 2;
    boost::shared_ptr< SSKdTreeNode< T > > kdNode( new SSKdTreeNode< T >() );
    m_depth = depth;

    if( size <=  1 )
    {
        kdNode->m_location = points[ first + median ];
        return kdNode;
    }

    std::nth_element(  points.begin() + first, points.begin() + first + median, points.begin() + last,
                        std::tr1::bind( &WSpanSpaceKdTree<T>::sortValues,
                                        this,
                                        std::tr1::placeholders::_1, std::tr1::placeholders::_2 ) );

    kdNode->m_location = points[ first + median ];
    kdNode->m_leftTree = createTree( points, depth + 1, first, first + median );
    kdNode->m_rightTree = createTree( points, depth + 1, first + median, last );

    return kdNode;
}

template< class T > bool WSpanSpaceKdTree< T >::sortValues( cells_t i, cells_t j )
{
    /*
        ^ max = y = 0
        |
        |
        |
        |
        +-------> min = x = 1
    */
    if( m_depth % 2 == 1  ) // choos x or y axis
    {   
        // for even numbers like (0, 2, 4)
        // the function returns true if the first cell has a smaller minimum isovalue
        return ( i.m_minMax.m_min < j.m_minMax.m_min );
    }
    else
    {
        // for odd numbers like (1, 3, 5)
        // the function returns true if the secound cell has a bigger maximum isovalue
        return ( i.m_minMax.m_max < j.m_minMax.m_max );
    }
}

/**
 * This class is the virtual base class for the spanSpace algorithm.
 */
class WSpanSpaceBase
{
public:
    typedef std::vector< KdCellId* > cellids_t; //!< vector of pointers on KdCells
    typedef boost::shared_ptr< WSpanSpaceBase > SPtr; //!< pointer on SpanSpaceBase class
    virtual ~WSpanSpaceBase()
    {
    }

    /**
     * Virtual function to find cells with given isovalue
     *
     * \param iso the value which is searched for
     *
     * \return a pointer on the KdCell Vector
     */
    virtual boost::shared_ptr< cellids_t > findCells( double iso ) const = 0;
};

/**
 * This class does the calculation work for the spanSpace algorithm
 */
template< class T> class WSpanSpace
:public WSpanSpaceBase
{
public:
    typedef MinMax< T > minmax_t; //!< minmax type used for KdCells
    typedef KdCells< T > cells_t; //!< the typ for KdCells
    typedef std::vector< cells_t > kdCells_t; //!< vector of KdCells
    
    /**
     * Virtual function of SpanSpace algorithm
     *
     * \param nbCoordsX No. of coordinates in X direction
     * \param nbCoordsY No. of coordinates in Y direction
     * \param nbCoordsZ No. of coordinates in Z direction
     * \param vals the values at the vertices
     * \param mainProgress progress combiner used to report progress of spanSpace algorithm
     */
    WSpanSpace(  size_t nbCoordsX, size_t nbCoordsY, size_t nbCoordsZ,
                 const std::vector< T >* vals,  boost::shared_ptr< WProgressCombiner > mainProgress );
    virtual ~WSpanSpace()
    {
    }
    
    /**
     * Find cells with given isovalue
     *
     * \param iso the value which is searched for
     *
     * \return a pointer on the KdCell Vector
     */
    boost::shared_ptr< cellids_t > findCells( double iso ) const;


protected:
private:
    /**
     * Get the minimum and maximun isovalue of a special cell with given x,y,z coordinates
     *
     * \param coordX x coordinate of cell
     * \param coordY y coordinate of cell
     * \param coordZ z coordinate of cell
     *
     * \return minMax isovalue
     */
    minmax_t getCellMinMax( unsigned int coordX, unsigned int coordY, unsigned int coordZ );
    
    /**
     * Calculate the id of a cell
     *
     * \param x x coordinate of cell
     * \param y y coordinate of cell
     * \param z z coordinate of cell
     *
     * \return calculated cell id
     */
    T getValue( unsigned int x, unsigned int y, unsigned int z );

    size_t m_nCellsX; //!< X coordinate of KdCell
    size_t m_nCellsY; //!< Y coordinate of KdCell
    size_t m_nCellsZ; //!< Z coordinate of KdCell

    const std::vector< T >* m_vals; //!< vector of cellids
    kdCells_t m_spanSpace; //!< vector of KdCells
    boost::shared_ptr< SSKdTreeNode< T > > m_kdTreeNode; //!< pointer of KdTree Node

    /**
     * Search Cells with special isocalue
     * and write this cells in given cellVector
     *
     * \param isovalue search in KdTree for this isovalue
     * \param cellVector write in this vector the cells which this isovalue
     */
    void searchKdTree( T isovalue, cellids_t& cellVector ) const;

    /**
     * Search the minimun isovalues in KdTree
     *
     * \param root KdTreeNode in which is searched
     * \param isovalue which is search for
     * \param cellVector pointer of vector in which cells are written
     */
    void searchKdMinMax( boost::shared_ptr< SSKdTreeNode< T > > root, T isovalue, cellids_t& cellVector ) const;

    /**
     * Search the maximum values in KdTree
     *
     * \param root KdTreeNode in which is searched
     * \param isovalue which is search for
     * \param cellVector pointer of vector in which cells are written
     */
    void searchKdMaxMin( boost::shared_ptr< SSKdTreeNode< T > > root, T isovalue, cellids_t& cellVector ) const;
    cellids_t m_activeCells; //!< variable is used to get the number of active cells
};

template< class T > boost::shared_ptr< WSpanSpaceBase::cellids_t > WSpanSpace<T>::findCells( double iso ) const
{
        wlog::debug( "sleep" );
        sleep( 20 );
        boost::shared_ptr< cellids_t > cellVector( new cellids_t );
        T isoValue = static_cast< T >( iso );
        // fill cellVec with content
        searchKdTree( isoValue, *cellVector );
        wlog::debug( "sleep" );
        sleep( 20 );
        wlog::debug( "WSpanSpace" ) << "active cells:" << cellVector->size();
        return cellVector;
}


template< class T > WSpanSpace<T>::WSpanSpace( size_t nbCoordsX, size_t nbCoordsY, size_t nbCoordsZ,
                                             const std::vector< T >*  vals,  boost::shared_ptr< WProgressCombiner > mainProgress )
{
    m_nCellsX = nbCoordsX - 1;
    m_nCellsY = nbCoordsY - 1;
    m_nCellsZ = nbCoordsZ - 1;

    boost::shared_ptr< WProgress > progressSpan = boost::shared_ptr< WProgress >( new WProgress( "Span Space", m_nCellsZ ) );
    boost::shared_ptr< WProgress > progressKd = boost::shared_ptr< WProgress >( new WProgress( "Kd-Tree" ) );
    mainProgress->addSubProgress( progressSpan );
    mainProgress->addSubProgress( progressKd );

    m_vals = vals;

    m_spanSpace.reserve( m_nCellsX * m_nCellsY * m_nCellsZ );
    for( unsigned int z = 0; z < m_nCellsZ; z++ )
    {
        ++*progressSpan;
        for( unsigned int y = 0; y < m_nCellsY; y++ )
        {
            for( unsigned int x = 0; x < m_nCellsX; x++ )
            {
                m_spanSpace.push_back( cells_t( getCellMinMax( x, y, z ), x, y, z ) );
            }
        }
    }

    progressSpan->finish();
    wlog::debug( "SpanSpace" ) << "started";
    WSpanSpaceKdTree< T > kdTree;
    m_kdTreeNode = kdTree.createTree( m_spanSpace, 0, 0, m_spanSpace.size() );
    progressKd->finish();
    wlog::debug( "SpanSpace" ) << "finished";
}
template< class T > void WSpanSpace<T>::searchKdTree( T isovalue,
                                                      cellids_t& cellVector ) const
{
    if( m_kdTreeNode )
    {
        searchKdMinMax( m_kdTreeNode, isovalue, cellVector );
        wlog::debug( "veea" ) << "kj";
        searchKdMaxMin( m_kdTreeNode, isovalue, cellVector );
    }
    else
        cellVector.push_back( &m_kdTreeNode->m_location.m_id );
}
template< class T > void WSpanSpace<T>::searchKdMaxMin( boost::shared_ptr< SSKdTreeNode< T > > root, T isovalue,
                                         cellids_t& cellVector ) const
{
    if( root->m_location.m_minMax.m_max > isovalue )
    {
        if( root->m_location.m_minMax.m_min > isovalue )
        {
            if( !root->m_rightTree && !root->m_leftTree )
                cellVector.push_back( &root->m_location.m_id );
        }

        if( root->m_leftTree )
        {
            searchKdMinMax( root->m_leftTree, isovalue, cellVector );
        }
    }
    if( root->m_rightTree )
    {
        searchKdMinMax( root->m_rightTree, isovalue, cellVector );
    }
}
template< class T > void WSpanSpace<T>::searchKdMinMax( boost::shared_ptr< SSKdTreeNode< T > > root, T isovalue,
                                         cellids_t& cellVector ) const
{
    if( root->m_location.m_minMax.m_min < isovalue )
    {
        if( root->m_location.m_minMax.m_max > isovalue )
        {
            if( !root->m_rightTree && !root->m_leftTree )
                cellVector.push_back( &root->m_location.m_id );
        }
        if( root->m_rightTree )
        {
            searchKdMaxMin( root->m_rightTree, isovalue, cellVector );
        }
    }
    if( root->m_leftTree )
    {
        searchKdMaxMin( root->m_leftTree, isovalue, cellVector );
    }
}
template< class T > typename WSpanSpace< T >::minmax_t WSpanSpace< T >::getCellMinMax( unsigned int coordX, unsigned int coordY, unsigned int coordZ )
{
    T valMax, valMin;
    valMax = valMin = getValue( coordX, coordY, coordZ  );

    // get point neighbours
    valMin = std::min( valMin, getValue( coordX + 1, coordY, coordZ ) );
    valMax = std::max( valMax, getValue( coordX + 1, coordY, coordZ  ) );

    valMin = std::min( valMin, getValue( coordX + 1, coordY, coordZ + 1 ) );
    valMax = std::max( valMax, getValue( coordX + 1, coordY, coordZ + 1 ) );

    valMin = std::min( valMin, getValue( coordX + 1, coordY + 1, coordZ ) );
    valMax = std::max( valMax, getValue( coordX + 1, coordY + 1, coordZ ) );

    valMin = std::min( valMin, getValue( coordX + 1, coordY, coordZ + 1 ) );
    valMax = std::max( valMax, getValue( coordX + 1, coordY, coordZ + 1 ) );

    valMin = std::min( valMin, getValue( coordX, coordY + 1, coordZ ) );
    valMax = std::max( valMax, getValue( coordX, coordY + 1, coordZ ) );

    valMin = std::min( valMin, getValue( coordX, coordY + 1, coordZ + 1 ) );
    valMax = std::max( valMax, getValue( coordX, coordY + 1, coordZ + 1 ) );

    valMin = std::min( valMin, getValue( coordX, coordY, coordZ + 1 ) );
    valMax = std::max( valMax, getValue( coordX, coordY, coordZ + 1 ) );

    return minmax_t( valMin, valMax );
}

template< class T > T WSpanSpace< T >::getValue( unsigned int x, unsigned int y, unsigned int z )
{
    return ( *m_vals )[ z * ( m_nCellsX + 1 ) * ( m_nCellsY + 1 ) + y * ( m_nCellsX + 1 ) + x ];
}

#endif  // WSPANSPACE_H
