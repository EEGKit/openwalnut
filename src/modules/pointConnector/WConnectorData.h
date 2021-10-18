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

#ifndef WCONNECTORDATA_H
#define WCONNECTORDATA_H

#include <memory>
#include <vector>

#include <osg/Geode>


/**
 * Holds the data of the WMPointConnector.
 */
class WConnectorData
{
public:
    /**
     * A shared_ptr to this class.
     */
    typedef std::shared_ptr< WConnectorData > SPtr;

    /**
     * Creates the internal vector arrays.
     */
    WConnectorData();

    /**
     * Clears the internal vector arrays.
     */
    void clear();

    /**
     * \return osg::ref_ptr< osg::Vec3Array > The vertices in this data.
     */
    osg::ref_ptr< osg::Vec3Array > getVertices();

    /**
     * \return osg::ref_ptr< osg::Vec4Array > The colors in this data.
     */
    osg::ref_ptr< osg::Vec4Array > getColors();

    /**
     * \return std::shared_ptr< std::vector< float > > The energy deposition in this data.
     */
    std::shared_ptr< std::vector< float > > getEdeps();

    /**
     * \return std::shared_ptr< std::vector< float > > The cluster size in this data.
     */
    std::shared_ptr< std::vector< float > > getClusterSizes();

    /**
     * Adds a vertex with its color to the currently selected fiber.
     * \param vertex The vertex to add.
     * \param color The color of the vertex.
     * \param edep The energy deposition of the vertex.
     * \param clusterSize The cluster size of the vertex.
     */
    void addVertex( osg::Vec3 vertex, osg::Vec4 color, float edep = 0.0, float clusterSize = 0.0 );

    /**
     * Selects a point by its index in m_vertices.
     * \param idx The index of the point.
     */
    void selectPoint( size_t idx );

    /**
     * Selects a poit by its coordinates.
     * \param vertex The coordinates of the point.
     */
    void selectPoint( osg::Vec3 vertex );

    /**
     * Deselects the currently selected point.
     */
    void deselectPoint();

    /**
     * 
     * \param idx Where to write the index of the selected point
     * \return true A point is selected
     * \return false A point is not selected
     */
    bool getSelectedPoint( size_t* idx );

    /**
     * \return true Cluster size should be used.
     * \return false Cluster size should not be used.
     */
    bool hasClusterSize();

    /**
     * Sets whether the cluster size should be used.
     * \param clusterSize 
     */
    void setHasClusterSize( bool clusterSize = true );

private:
    /**
     * The vertices that are drawn.
     */
    osg::ref_ptr< osg::Vec3Array > m_vertices;

    /**
     * The color of the vertices that are drawn.
     */
    osg::ref_ptr< osg::Vec4Array > m_colors;

    /**
     * The energy deposition of the vertices.
     */
    std::shared_ptr< std::vector< float > > m_edeps;

    /**
     * The cluster size of the vertices.
     */
    std::shared_ptr< std::vector< float > > m_clusterSizes;

    /**
     * The index of the selected vertex.
     */
    size_t m_selectedIndex;

    /**
     * Whether a selection has been done or not.
     */
    bool m_hasSelected = false;

    /**
     * Whether cluster size should be used.
     */
    bool m_hasClusterSize = false;
};

#endif  // WCONNECTORDATA_H
