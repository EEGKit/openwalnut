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
#include "WDataSetVector.h"

// prototype instance as singleton
boost::shared_ptr< WPrototyped > WDataSetVector::m_prototype = boost::shared_ptr< WPrototyped >();

WDataSetVector::WDataSetVector( boost::shared_ptr< WValueSetBase > newValueSet,
                                boost::shared_ptr< WGrid > newGrid )
    : WDataSetSingle( newValueSet, newGrid )
{
    WAssert( newValueSet, "No value set given." );
    WAssert( newGrid, "No grid given." );
    WAssert( newValueSet->size() == newGrid->size(), "Number of values unequal number of positions in grid." );
    WAssert( newValueSet->order() == 1, "The value set does not contain vectors." );
}

WDataSetVector::WDataSetVector()
    : WDataSetSingle()
{
}

WDataSetVector::~WDataSetVector()
{
}

boost::shared_ptr< WPrototyped > WDataSetVector::getPrototype()
{
    if ( !m_prototype )
    {
        m_prototype = boost::shared_ptr< WPrototyped >( new WDataSetVector() );
    }

    return m_prototype;
}

wmath::WVector3D WDataSetVector::interpolate( const wmath::WPosition& pos, bool *success ) const
{
    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_grid );

    WAssert( grid,  "This data set has a grid whose type is not yet supported for interpolation." );
    // TODO(wiebel): change this to WAssert.
//    if( grid->getTransformationMatrix() != wmath::WMatrix<double>( 4, 4 ).makeIdentity()  )
//    {
//        throw WException( std::string( "Only feasible for untranslated grid so far." ) );
//    }
    WAssert( ( m_valueSet->order() == 1 &&  m_valueSet->dimension() == 3 ),
             "Only implemented for 3D Vectors so far." );

    *success = grid->encloses( pos );

    if( !*success )
    {
        return wmath::WVector3D( 0, 0, 0 );
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

    wmath::WVector3D result( 0, 0, 0 );
    for( size_t i = 0; i < 8; ++i )
    {
        result += h[i] * getVectorAt( vertexIds[i] );
    }

    *success = true;
    return result;
}

wmath::WVector3D WDataSetVector::getVectorAt( size_t index ) const
{
        switch( getValueSet()->getDataType() )
    {
        case W_DT_UNSIGNED_CHAR:
        {
            return boost::shared_dynamic_cast< WValueSet< unsigned char > >( getValueSet() )->getVector3D( index );
        }
        case W_DT_INT16:
        {
            return boost::shared_dynamic_cast< WValueSet< int16_t > >( getValueSet() )->getVector3D( index );
        }
        case W_DT_SIGNED_INT:
        {
            return boost::shared_dynamic_cast< WValueSet< int32_t > >( getValueSet() )->getVector3D( index );
        }
        case W_DT_FLOAT:
        {
            return boost::shared_dynamic_cast< WValueSet< float > >( getValueSet() )->getVector3D( index );
        }
        case W_DT_DOUBLE:
        {
            return boost::shared_dynamic_cast< WValueSet< double > >( getValueSet() )->getVector3D( index );
        }
        default:
            WAssert( false, "Unknow data type in dataset." );
    }

    return wmath::WVector3D( 0, 0, 0 );
}
