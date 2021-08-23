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

#include <iostream>

#include <osg/Drawable>
#include <osg/Projection>
#include <osg/Camera>
#include <osg/MatrixTransform>

#include "geodes/WGEGridNode.h"
#include "WGEZoomTrackballNodeVisitor.h"

WGEZoomTrackballNodeVisitor::WGEZoomTrackballNodeVisitor( TraversalMode traversalMode ): osg::NodeVisitor( traversalMode )
{
}

void WGEZoomTrackballNodeVisitor::reset()
{
    m_bb.init();
}

void WGEZoomTrackballNodeVisitor::apply( osg::Camera& node ) // NOLINT
{
    double left, right, bottom, top, zNear, zFar;
    if( node.getProjectionMatrixAsOrtho( left, right, bottom, top, zNear, zFar ) && zNear == -1 && zFar == 1 )
    {
        return;
    }
    traverse( node );
}

void WGEZoomTrackballNodeVisitor::apply( osg::Projection& node ) // NOLINT
{
    double left, right, bottom, top, zNear, zFar;
    if( node.getMatrix().getOrtho( left, right, bottom, top, zNear, zFar ) && zNear == -1 && zFar == 1 )
    {
        return;
    }
    traverse( node );
}

void WGEZoomTrackballNodeVisitor::apply( osg::Drawable& node ) // NOLINT
{
    osg::BoundingBox bb = node.getBoundingBox();
    if( bb.valid() )
    {
        m_bb.expandBy( bb );
    }
}

void WGEZoomTrackballNodeVisitor::apply( osg::MatrixTransform& node ) // NOLINT
{
    if( !dynamic_cast< WGEGridNode* >( &node ) )
    {
        traverse( node );
    }
}

osg::BoundingBox& WGEZoomTrackballNodeVisitor::getBoundingBox()
{
    return m_bb;
}
