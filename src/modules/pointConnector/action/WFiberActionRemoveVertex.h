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

#include <boost/shared_ptr.hpp>
#include <osg/Geode>

#include "WFiberAction.h"
#include "../WFiberHandler.h"

class WFiberActionRemoveVertex : public WFiberAction
{
public:
    typedef boost::shared_ptr< WFiberActionRemoveVertex > SPtr;

    WFiberActionRemoveVertex( osg::Vec3 vertex, size_t position, WFiberHandler* fiberHandler );

    ~WFiberActionRemoveVertex();

    virtual void undo();

    virtual void redo();
private:
    osg::Vec3 m_vertex;

    size_t m_position;

    WFiberHandler* m_fiberHandler;
};

#endif  // WFIBERACTIONREMOVEVERTEX_H
