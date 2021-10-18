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

#include "../WConnectorData.h"
#include "WFiberActionRemoveVertex.h"


WFiberActionRemoveVertex::WFiberActionRemoveVertex( osg::Vec3 vertex, size_t fiberIdx, size_t position, WFiberHandler* fiberHandler ):
    m_vertex( vertex ),
    m_fiberIdx( fiberIdx ),
    m_position( position ),
    m_fiberHandler( fiberHandler )
{
}

WFiberActionRemoveVertex::~WFiberActionRemoveVertex()
{
}

void WFiberActionRemoveVertex::undo()
{
    m_fiberHandler->addVertexToFiberAt( m_vertex, m_fiberIdx, m_position, true );

    m_fiberHandler->getPointConnector()->getConnectorData()->deselectPoint();
    m_fiberHandler->selectLastPoint();

    m_fiberHandler->getPointConnector()->updateAll();
}

void WFiberActionRemoveVertex::redo()
{
    m_fiberHandler->removeVertexFromFiber( m_vertex, m_fiberIdx, true );

    m_fiberHandler->getPointConnector()->getConnectorData()->deselectPoint();
    m_fiberHandler->selectLastPoint();

    m_fiberHandler->getPointConnector()->updateAll();
}
