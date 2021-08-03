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

#ifndef WFIBERACTIONADDVERTEX_H
#define WFIBERACTIONADDVERTEX_H

#include <memory>

#include <osg/Geode>

#include "../WFiberHandler.h"
#include "WFiberAction.h"


/**
 * The action when adding a Vertex.
 */
class WFiberActionAddVertex : public WFiberAction
{
public:
    /**
     * A shared_ptr to this class.
     */
    typedef std::shared_ptr< WFiberActionAddVertex > SPtr;

    /**
     * Creates this action.
     * \param vertex The vertex to add.
     * \param fiberIdx The index of the fiber this vertex was added to.
     * \param fiberHandler The WFiberHandler of the action.
     */
    WFiberActionAddVertex( osg::Vec3 vertex, size_t fiberIdx, WFiberHandler* fiberHandler );

    /**
     * Empty destructor.
     */
    ~WFiberActionAddVertex();

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
     * The WFiberHandler of the action.
     */
    WFiberHandler* m_fiberHandler;
};

#endif  // WFIBERACTIONADDVERTEX_H
