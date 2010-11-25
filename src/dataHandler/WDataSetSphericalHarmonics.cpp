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

#include <stdint.h>

#include <string>
#include <vector>

#include "../common/WAssert.h"
#include "WDataSetSingle.h"
#include "WDataSetSphericalHarmonics.h"

// prototype instance as singleton
boost::shared_ptr< WPrototyped > WDataSetSphericalHarmonics::m_prototype = boost::shared_ptr< WPrototyped >();

WDataSetSphericalHarmonics::WDataSetSphericalHarmonics( boost::shared_ptr< WValueSetBase > newValueSet,
                                                        boost::shared_ptr< WGrid > newGrid ) :
    WDataSetSingle( newValueSet, newGrid )
{
    m_valueSet = boost::shared_dynamic_cast< WValueSet<double> >( newValueSet );
    WAssert( newValueSet, "No value set given." );
    WAssert( newGrid, "No grid given." );
    WAssert( m_valueSet, "No WValueSet<double> given." );
}

WDataSetSphericalHarmonics::WDataSetSphericalHarmonics()
    : WDataSetSingle()
{
}

WDataSetSphericalHarmonics::~WDataSetSphericalHarmonics()
{
}

boost::shared_ptr< WPrototyped > WDataSetSphericalHarmonics::getPrototype()
{
    if ( !m_prototype )
    {
        m_prototype = boost::shared_ptr< WPrototyped >( new WDataSetSphericalHarmonics() );
    }

    return m_prototype;
}

wmath::WSymmetricSphericalHarmonic WDataSetSphericalHarmonics::interpolate( const wmath::WPosition& pos, bool* success ) const
{
    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_grid );

    *success = grid->encloses( pos );

    bool isInside = true;
    size_t cellId = grid->getCellId( pos, &isInside );

    if( !isInside )
    {
        *success = false;
        return wmath::WSymmetricSphericalHarmonic();
    }

    // ids of vertices for interpolation
    std::vector< size_t > vertexIds = grid->getCellVertexIds( cellId );

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

    // take
    wmath::WValue< double > interpolatedCoefficients( m_valueSet->dimension() );
    for( size_t i = 0; i < 8; ++i )
    {
        interpolatedCoefficients += h[i] * m_valueSet->getWValue( vertexIds[i] );
    }

    *success = true;

    return wmath::WSymmetricSphericalHarmonic( interpolatedCoefficients );
}

wmath::WSymmetricSphericalHarmonic WDataSetSphericalHarmonics::getSphericalHarmonicAt( size_t index ) const
{
    if ( index < m_valueSet->size() ) return wmath::WSymmetricSphericalHarmonic( m_valueSet->getWValue( index ) );
    return wmath::WSymmetricSphericalHarmonic();
}

const std::string WDataSetSphericalHarmonics::getName() const
{
    return "WDataSetSphericalHarmonics";
}

const std::string WDataSetSphericalHarmonics::getDescription() const
{
    return "Contains factors for spherical harmonics.";
}

bool WDataSetSphericalHarmonics::isTexture() const
{
    return false;
}
