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

#ifndef WGEZOOMTRACKBALLNODEVISITOR_H
#define WGEZOOMTRACKBALLNODEVISITOR_H

#include <limits>
#include <vector>

#include <osg/BoundingBox>
#include <osg/NodeVisitor>

/**
 * A visitor that ignores specific nodes that are irrelevant for the trackball.
 */
class WGEZoomTrackballNodeVisitor : public osg::NodeVisitor
{
public:
    /**
     * Creates a new node visitor.
     * \param traversalMode The traversal mode.
     */
    explicit WGEZoomTrackballNodeVisitor( TraversalMode traversalMode = TRAVERSE_ACTIVE_CHILDREN );

    /**
     * Resets this visitor.
     */
    virtual void reset();

    /**
     * Handles camera nodes.
     * \param node The camera node.
     */
    void apply( osg::Camera& node ); // NOLINT

    /**
     * Handles projection nodes.
     * \param node The projection node.
     */
    void apply( osg::Projection& node ); // NOLINT

    /**
     * Handles drawable nodes.
     * \param node The drawable node.
     */
    void apply( osg::Drawable& node ); // NOLINT

    /**
     * Handles matrixtransform nodes.
     * \param node The matrixtransform node.
     */
    void apply( osg::MatrixTransform& node ); // NOLINT

    /**
     * Gets the bounding box.
     * \return osg::BoundingBox& The bounding box.
     */
    osg::BoundingBox& getBoundingBox();

private:
    osg::BoundingBox m_bb; //!< The bounding box that is generated.
    std::vector< osg::Matrix > m_matrixStack; //!< The stack for the matrices.
};

#endif  // WGEZOOMTRACKBALLNODEVISITOR_H
