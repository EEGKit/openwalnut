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

#ifndef WFIBERACTIONMULTIDESELECT_H
#define WFIBERACTIONMULTIDESELECT_H

#include <vector>

#include <boost/shared_ptr.hpp>
#include <osg/Geode>

#include "WFiberAction.h"
#include "../WFiberHandler.h"


/**
 * The action when adding multiple Vertices.
 */
class WFiberActionMultideselect : public WFiberAction
{
public:
    /**
     * A shared_ptr to this class.
     */
    typedef std::shared_ptr< WFiberActionMultideselect > SPtr;

    /**
     * Creates this action.
     * \param vertices A vector of vertices to add.
     * \param fiberIdx The index of the fiber this vertex was added to.
     * \param fiberHandler The WFiberHandler of the action.
     */
    WFiberActionMultideselect( std::vector< osg::Vec3 > vertices, size_t fiberIdx, WFiberHandler* fiberHandler );

    /**
     * Empty destructor.
     */
    ~WFiberActionMultideselect();

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
     * The vertices to add.
     */
    std::vector< osg::Vec3 > m_vertices;

    /**
     * The index of the fiber this vertex was added to.
     */
    size_t m_fiberIdx;

    /**
     * The WFiberHandler of the action.
     */
    WFiberHandler* m_fiberHandler;
};

#endif  // WFIBERACTIONMULTIDESELECT_H
