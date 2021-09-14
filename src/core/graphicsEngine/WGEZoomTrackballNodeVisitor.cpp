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

#include <osg/Camera>
#include <osg/Drawable>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/Projection>

#include "geodes/WGEGridNode.h"
#include "WGEZoomTrackballNodeVisitor.h"

WGEZoomTrackballNodeVisitor::WGEZoomTrackballNodeVisitor( TraversalMode traversalMode ): osg::NodeVisitor( traversalMode )
{
}

void WGEZoomTrackballNodeVisitor::reset()
{
    m_matrixStack.clear();
    m_bb.init();
}

void WGEZoomTrackballNodeVisitor::apply( osg::Camera& node ) // NOLINT
{
    // Ignore all cameras that are a 2D orthogonal projection.
    double left, right, bottom, top, zNear, zFar;
    if( node.getProjectionMatrixAsOrtho( left, right, bottom, top, zNear, zFar ) && zNear == -1 && zFar == 1 )
    {
        return;
    }
    traverse( node );
}

void WGEZoomTrackballNodeVisitor::apply( osg::Projection& node ) // NOLINT
{
    // Ignore all 2D orthogonal projections.
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
        if( m_matrixStack.empty() )
        {
            m_bb.expandBy( bb );
        }
        else
        {
            osg::Matrix matrix = m_matrixStack.back();
            m_bb.expandBy( bb.corner( 0 ) * matrix );
            m_bb.expandBy( bb.corner( 1 ) * matrix );
            m_bb.expandBy( bb.corner( 2 ) * matrix );
            m_bb.expandBy( bb.corner( 3 ) * matrix );
            m_bb.expandBy( bb.corner( 4 ) * matrix );
            m_bb.expandBy( bb.corner( 5 ) * matrix );
            m_bb.expandBy( bb.corner( 6 ) * matrix );
            m_bb.expandBy( bb.corner( 7 ) * matrix );
        }
    }
}

void WGEZoomTrackballNodeVisitor::apply( osg::MatrixTransform& node ) // NOLINT
{
    osg::Matrix matrix;
    if( !m_matrixStack.empty() )
    {
        matrix = m_matrixStack.back();
    }
    node.computeLocalToWorldMatrix( matrix, this );

    m_matrixStack.push_back( matrix );
    traverse( node );
    m_matrixStack.pop_back();
}

osg::BoundingBox& WGEZoomTrackballNodeVisitor::getBoundingBox()
{
    return m_bb;
}
