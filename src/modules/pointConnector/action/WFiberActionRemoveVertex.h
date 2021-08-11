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

#ifndef WFIBERACTIONREMOVEVERTEX_H
#define WFIBERACTIONREMOVEVERTEX_H

#include <memory>
#include <string>

#include <osg/Geode>

#include "../WFiberHandler.h"
#include "WFiberAction.h"


/**
 * The action when removing a Vertex.
 */
class WFiberActionRemoveVertex : public WFiberAction
{
public:
    /**
     * A shared_ptr to this class.
     */
    typedef std::shared_ptr< WFiberActionRemoveVertex > SPtr;

    /**
     * Creates this action.
     * \param vertex The vertex to add.
     * \param fiberIdx The index of the fiber this vertex was added to.
     * \param position The position of the vertex in the vertices vector.
     * \param fiberHandler The WFiberHandler of the action.
     */
    WFiberActionRemoveVertex( osg::Vec3 vertex, size_t fiberIdx, size_t position, WFiberHandler* fiberHandler );

    /**
     * Empty destructor.
     */
    ~WFiberActionRemoveVertex();

    /**
     * Undos this action.
     */
    virtual void undo();

    /**
     * Redos this action.
     */
    virtual void redo();

private:
    /**
     * The vertex to add.
     */
    osg::Vec3 m_vertex;

    /**
     * The index of the fiber this vertex was added to.
     */
    size_t m_fiberIdx;

    /**
     * The position of the vertex in the vertices vector.
     */
    size_t m_position;

    /**
     * The WFiberHandler of the action.
     */
    WFiberHandler* m_fiberHandler;
};

#endif  // WFIBERACTIONREMOVEVERTEX_H
