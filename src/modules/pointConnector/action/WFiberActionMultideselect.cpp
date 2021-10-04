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

#include <vector>

#include "../WConnectorData.h"
#include "WFiberActionMultideselect.h"


WFiberActionMultideselect::WFiberActionMultideselect( std::vector< osg::Vec3 > vertices, size_t fiberIdx, WFiberHandler* fiberHandler ):
    m_vertices( vertices ),
    m_fiberIdx( fiberIdx ),
    m_fiberHandler( fiberHandler )
{
}

WFiberActionMultideselect::~WFiberActionMultideselect()
{
}

void WFiberActionMultideselect::undo()
{
    m_fiberHandler->addVerticesToFiber( m_vertices, m_fiberIdx, true );

    m_fiberHandler->getPointConnector()->getConnectorData()->deselectPoint();
    m_fiberHandler->selectLastPoint();

    m_fiberHandler->getPointConnector()->updateAll();
}

void WFiberActionMultideselect::redo()
{
    m_fiberHandler->removeVerticesFromFiber( m_vertices, m_fiberIdx, true );

    m_fiberHandler->getPointConnector()->getConnectorData()->deselectPoint();
    m_fiberHandler->selectLastPoint();

    m_fiberHandler->getPointConnector()->updateAll();
}
