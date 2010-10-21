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

#ifdef OW_USE_OSSIM

#include <ossim/matrix/newmat.h>
#include <ossim/matrix/newmatap.h>

#include <boost/shared_ptr.hpp>

#include "WOSSIMHelper.h"

boost::shared_ptr< NEWMAT::Matrix > wmath::WOSSIMHelper::OWMatrixToOSSIMMatrix( const wmath::WMatrix<double> &input )
{
  boost::shared_ptr< NEWMAT::Matrix > result( new NEWMAT::Matrix( input.getNbRows(), input.getNbCols() ) );
  for ( size_t row = 0; row < input.getNbRows(); row++ )
  {
    for ( size_t col = 0; col < input.getNbCols(); col++ )
    {
      ( *result )( static_cast<int>( row+1 ), static_cast<int>( col+1 ) ) = input( row, col );
    }
  }
  return result;
}

wmath::WMatrix<double> wmath::WOSSIMHelper::OSSIMMatrixToOWMatrix( const NEWMAT::Matrix& input )
{
  wmath::WMatrix<double> result( static_cast<size_t>( input.Nrows() ), static_cast<size_t>( input.Ncols() ) );

  for ( size_t row = 0; row < result.getNbRows(); row++ )
  {
    for ( size_t col = 0; col < result.getNbCols(); col++ )
    {
      result( row, col ) = input( static_cast<int>( row+1 ), static_cast<int>( col+1 ) );
    }
  }
  return result;
}

wmath::WMatrix<double> wmath::WOSSIMHelper::OSSIMDiagonalMatrixToOWMatrix( const NEWMAT::DiagonalMatrix& input )
{
  wmath::WMatrix<double> result( static_cast<size_t>( input.Nrows() ), static_cast<size_t>( input.Ncols() ) );

  for ( size_t i = 0; i < result.getNbRows(); i++ )
  {
    result( i, i ) = input( static_cast<int>( i+1 ) );
  }
  return result;
}

wmath::WValue<double> wmath::WOSSIMHelper::OSSIMDiagonalMatrixToOWVector( const NEWMAT::DiagonalMatrix& input )
{
  wmath::WValue<double> result( static_cast<size_t>( input.Nrows() ) );

  for ( size_t i = 0; i < result.size(); i++ )
  {
    result[ i ] = input( static_cast<int>( i+1 ) );
  }
  return result;
}

// gsl_vector* wmath::OSSIMHelper::OWVectorToOSSIMVector( const WValue<double> &input )
// {
// }
//
// wmath::WValue<double> wmath::OSSIMHelper::OSSIMVectorToOWVector( const gsl_vector* input )
// {
// }


void wmath::WOSSIMHelper::computeSVD( const wmath::WMatrix< double >& A,
                                     wmath::WMatrix< double >& U,
                                     wmath::WMatrix< double >& V,
                                     wmath::WValue< double >& S )
{
  // create matrices in OSSIM format
  boost::shared_ptr< NEWMAT::Matrix > AA( OWMatrixToOSSIMMatrix( A ) );
  NEWMAT::DiagonalMatrix SS( static_cast<int>( S.size() ) );
  NEWMAT::Matrix UU( static_cast<int>( U.getNbRows() ), static_cast<int>( U.getNbCols() ) );
  NEWMAT::Matrix VV( static_cast<int>( V.getNbRows() ), static_cast<int>( V.getNbCols() ) );
  // do SVD
  NEWMAT::SVD( *AA, SS, UU, VV );
  // convert Matrices to OW format
  S = OSSIMDiagonalMatrixToOWVector( SS );
  U = OSSIMMatrixToOWMatrix( UU );
  V = OSSIMMatrixToOWMatrix( VV );
}

// wmath::WMatrix<double> wmath::WOSSIMHelper::pseudoInverse( const WMatrix<double>& input )
// {
//             // calc pseudo inverse
//             wmath::WMatrix< double > U( input.getNbRows(), input.getNbCols() );
//             wmath::WMatrix< double > V( input.getNbCols(), input.getNbCols() );
//             wmath::WValue< double > Svec( input.getNbCols() );
//             wmath::WOSSIMHelper::computeSVD( input, U, V, Svec );
//
//             // create diagonal matrix S
//             wmath::WMatrix< double > S( input.getNbCols(), input.getNbCols() );
//
//             for ( size_t i = 0; i < Svec.size() && i < S.getNbRows() && i < S.getNbCols(); i++ )
//               S( i, i ) = ( Svec[ i ] == 0.0 ) ? 0.0 : 1.0 / Svec[ i ];
//
//             return wmath::WMatrix< double >( V*S*U.transposed() );
// }
#endif
