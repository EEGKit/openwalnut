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

#include <algorithm>
#include <vector>

#include "WConnectorData.h"


WConnectorData::WConnectorData()
{
    m_vertices = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array() );
    m_colors = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array() );
    m_edeps = std::shared_ptr< std::vector< float > >( new std::vector< float >() );
    m_clusterSizes = std::shared_ptr< std::vector< float > >( new std::vector< float> () );
}

void WConnectorData::clear()
{
    m_vertices->clear();
    m_colors->clear();
    m_edeps->clear();
    m_clusterSizes->clear();
    m_hasClusterSize = false;
}

osg::ref_ptr< osg::Vec3Array > WConnectorData::getVertices()
{
    return m_vertices;
}

osg::ref_ptr< osg::Vec4Array > WConnectorData::getColors()
{
    return m_colors;
}

std::shared_ptr< std::vector< float > > WConnectorData::getEdeps()
{
    return m_edeps;
}

std::shared_ptr< std::vector< float > > WConnectorData::getClusterSizes()
{
    return m_clusterSizes;
}


void WConnectorData::addVertex( osg::Vec3 vertex, osg::Vec4 color, float edep, float clusterSize )
{
    m_vertices->push_back( vertex );
    m_colors->push_back( color );
    m_edeps->push_back( edep );
    m_clusterSizes->push_back( clusterSize );
}

void WConnectorData::deselectPoint()
{
    m_hasSelected = false;
}

void WConnectorData::selectPoint( size_t idx )
{
    m_selectedIndex = idx;
    m_hasSelected = true;
}

void WConnectorData::selectPoint( osg::Vec3 vertex )
{
    std::vector< osg::Vec3 >::iterator vertexIterator = std::find( m_vertices->begin(), m_vertices->end(), vertex );
    size_t vIdx = std::distance( m_vertices->begin(), vertexIterator );

    selectPoint( vIdx );
}

bool WConnectorData::getSelectedPoint( size_t* idx )
{
    *idx = m_selectedIndex;
    return m_hasSelected;
}


bool WConnectorData::hasClusterSize()
{
    return m_hasClusterSize;
}

void WConnectorData::setHasClusterSize( bool clusterSize )
{
    m_hasClusterSize = clusterSize;
}
