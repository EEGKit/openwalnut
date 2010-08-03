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
#include "../common/WLimits.h"
#include "datastructures/WValueSetHistogram.h"
#include "WDataSetSingle.h"

#include "WDataSetScalar.h"

// prototype instance as singleton
boost::shared_ptr< WPrototyped > WDataSetScalar::m_prototype = boost::shared_ptr< WPrototyped >();

WDataSetScalar::WDataSetScalar( boost::shared_ptr< WValueSetBase > newValueSet,
                                boost::shared_ptr< WGrid > newGrid )
    : WDataSetSingle( newValueSet, newGrid )
{
    WAssert( newValueSet, "No value set given." );
    WAssert( newGrid, "No grid given." );
    WAssert( newValueSet->size() == newGrid->size(), "Number of values unequal number of positions in grid." );
    WAssert( newValueSet->order() == 0, "The value set does not contain scalars." );

    // the histogram gets calculated on demand
    m_histogram = boost::shared_ptr< WValueSetHistogram >();
}

WDataSetScalar::WDataSetScalar()
    : WDataSetSingle()
{
    // default constructor used by the prototype mechanism
}

WDataSetScalar::~WDataSetScalar()
{
}

double WDataSetScalar::getMax() const
{
    return m_valueSet->getMaximumValue();
}

double WDataSetScalar::getMin() const
{
    return m_valueSet->getMinimumValue();
}

boost::shared_ptr< WPrototyped > WDataSetScalar::getPrototype()
{
    if ( !m_prototype )
    {
        m_prototype = boost::shared_ptr< WPrototyped >( new WDataSetScalar() );
    }

    return m_prototype;
}

double WDataSetScalar::interpolate( const wmath::WPosition& pos, bool* success )
{
    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_grid );

    WAssert( grid, "This data set has a grid whose type is not yet supported for interpolation." );
    WAssert( grid->isNotRotatedOrSheared(), "Only feasible for grids that are only translated or scaled so far." );
    WAssert( ( m_valueSet->order() == 0 &&  m_valueSet->dimension() == 1 ),
             "Only implemented for scalar values so far." );

    *success = grid->encloses( pos );

    if( !*success )
    {
        return 0;
    }

    std::vector< size_t > vertexIds = grid->getCellVertexIds( grid->getCellId( pos ) );

    wmath::WPosition localPos = pos - grid->getPosition( vertexIds[0] );

    double lambdaX = localPos[0] / grid->getOffsetX();
    double lambdaY = localPos[1] / grid->getOffsetY();
    double lambdaZ = localPos[2] / grid->getOffsetZ();
    std::vector< double > h( 8 );
//         lZ     lY
//         |      /
//         | 6___/_7
//         |/:    /|
//         4_:___5 |
//         | :...|.|
//         |.2   | 3
//         |_____|/ ____lX
//        0      1
    h[0] = ( 1 - lambdaX ) * ( 1 - lambdaY ) * ( 1 - lambdaZ );
    h[1] = (     lambdaX ) * ( 1 - lambdaY ) * ( 1 - lambdaZ );
    h[2] = ( 1 - lambdaX ) * (     lambdaY ) * ( 1 - lambdaZ );
    h[3] = (     lambdaX ) * (     lambdaY ) * ( 1 - lambdaZ );
    h[4] = ( 1 - lambdaX ) * ( 1 - lambdaY ) * (     lambdaZ );
    h[5] = (     lambdaX ) * ( 1 - lambdaY ) * (     lambdaZ );
    h[6] = ( 1 - lambdaX ) * (     lambdaY ) * (     lambdaZ );
    h[7] = (     lambdaX ) * (     lambdaY ) * (     lambdaZ );

    double result = 0;
    for( size_t i = 0; i < 8; ++i )
    {
        result += h[i] * WDataSetSingle::getValueAt( vertexIds[i] );
    }

    *success = true;
    return result;
}

double WDataSetScalar::getValueAt( int x, int y, int z ) const
{
    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_grid );
    size_t id = x + y * grid->getNbCoordsX() + z * grid->getNbCoordsX() * grid->getNbCoordsY();

    return WDataSetSingle::getValueAt( id );
}

boost::shared_ptr< const WValueSetHistogram > WDataSetScalar::getHistogram()
{
    if ( m_histogram )
    {
        return m_histogram;
    }

    boost::lock_guard<boost::mutex> lock( m_histogramLock );
    m_histogram = boost::shared_ptr< WValueSetHistogram >( new WValueSetHistogram( m_valueSet ) );

    return m_histogram;
}

