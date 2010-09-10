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

#include <string>
#include <vector>

#include "../common/WAssert.h"
#include "WDataSetSingle.h"

#include "WDataSetRawHARDI.h"

// prototype instance as singleton
boost::shared_ptr< WPrototyped > WDataSetRawHARDI::m_prototype = boost::shared_ptr< WPrototyped >();

WDataSetRawHARDI::WDataSetRawHARDI( boost::shared_ptr< WValueSetBase > newValueSet,
                                    boost::shared_ptr< WGrid > newGrid,
                                    boost::shared_ptr< std::vector< wmath::WVector3D > > newGradients )
    : WDataSetSingle( newValueSet, newGrid ), m_gradients( newGradients )
{
    WAssert( newValueSet, "No value set given." );
    WAssert( newGrid, "No grid given." );
    WAssert( newGradients, "No gradients given." );
    WAssert( newValueSet->size() == newGrid->size(), "Number of voxel entries unequal number of positions in grid." );
    WAssert( newValueSet->order() != newGradients->size(), "Number of gradients unequal number of entries in value set." );
}

WDataSetRawHARDI::WDataSetRawHARDI()
    : WDataSetSingle()
{
}

WDataSetRawHARDI::~WDataSetRawHARDI()
{
}

boost::shared_ptr< WPrototyped > WDataSetRawHARDI::getPrototype()
{
    if ( !m_prototype )
    {
        m_prototype = boost::shared_ptr< WPrototyped >( new WDataSetRawHARDI() );
    }

    return m_prototype;
}

const wmath::WVector3D& WDataSetRawHARDI::getGradient( size_t index ) const
{
#ifdef DEBUG
  return m_gradients->at( index );
#else
  return (*m_gradients)[ index ];
#endif
}

std::vector< wmath::WVector3D > const& WDataSetRawHARDI::getOrientations() const
{
    return *m_gradients;
}

std::size_t WDataSetRawHARDI::getNumberOfMeasurements() const
{
  return m_gradients->size();
}

const std::string WDataSetRawHARDI::getName() const
{
    return "WDataSetRawHARDI";
}

const std::string WDataSetRawHARDI::getDescription() const
{
    return "Contains HARDI measurements.";
}
