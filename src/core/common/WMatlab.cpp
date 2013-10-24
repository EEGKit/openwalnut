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

// #include "/local/apps/matlab/extern/include/engine.h"
#include <engine.h>

#include "WMatlab.h"

WMatlab::WMatlab( const std::string startCommand ) : m_engine( NULL )
{
    m_engine = engOpen( startCommand.c_str() );
    WAssert( m_engine, "Matlab start failed." );
}


WMatlab::~WMatlab()
{
    engClose( m_engine );
}


void WMatlab::putVariable( const std::string& varName, const WMatrix<double>& matrix )
{
    size_t cols = matrix.getNbCols();
    size_t rows = matrix.getNbRows();
    mxArray* matlabMatrix = mxCreateDoubleMatrix( rows, cols, mxREAL );
    // TODO(philips): do this by std::copy (we use c++)
    memcpy( ( void* ) mxGetPr( matlabMatrix ), ( void* ) &matrix[0], sizeof( double ) * cols * rows );
    // for ( size_t col = 0; col < cols; ++col  )
    // {
    //     for ( size_t row = 0; row < rows; ++row  )
    //     {
    //         matlabMatrixPtr[ row * cols + col ] = matrix( row, col );
    //     }
    // }
    engPutVariable( m_engine, varName.c_str(), matlabMatrix );
    mxDestroyArray( matlabMatrix );
}

void WMatlab::putVariable( const std::string& varName, const WValue<double>& vector )
{
    size_t cols = 1;
    size_t rows = vector.size();
    mxArray* matlabMatrix = mxCreateDoubleMatrix( rows, cols, mxREAL );
    // TODO(philips): do this by std::copy (we use c++)
    memcpy( ( void* ) mxGetPr( matlabMatrix ), ( void* ) &vector[0], sizeof( double ) * cols * rows );
    engPutVariable( m_engine, varName.c_str(), matlabMatrix );
    mxDestroyArray( matlabMatrix );
}

void WMatlab::putVariable( const std::string& varName, const WVector3d& vector )
{
    mxArray* matlabMatrix = mxCreateDoubleMatrix( 3, 1, mxREAL );
    memcpy( ( void* ) mxGetPr( matlabMatrix ), ( void* ) &vector[0], sizeof( double ) * 3 );
    engPutVariable( m_engine, varName.c_str(), matlabMatrix );
    mxDestroyArray( matlabMatrix );
}

void WMatlab::putVariable( const std::string& varName, double val )
{
    // mxArray* matlabMatrix = mxCreateDoubleMatrix( 1, 1, mxREAL );
    mxArray* matlabMatrix = mxCreateDoubleScalar( val );
    // double* matlabMatrixPtr = mxGetPr( matlabMatrix );
    // ( *matlabMatrixPtr ) = val;
    // matlabMatrixPtr = NULL;
    engPutVariable( m_engine, varName.c_str(), matlabMatrix );
    mxDestroyArray( matlabMatrix );
}

void WMatlab::putVariable( const std::string& varName, mxArray* matlabArray )
{
    engPutVariable( m_engine, varName.c_str(), matlabArray );
    mxDestroyArray( matlabArray );
}

WMatrix<double> WMatlab::getVariable( const std::string& varName ) const
{
    // TODO(philips): is this with memcpy possible? - faster
    // memcpy( (void *)mxGetPr(matlabMatrix), (void *)time, sizeof(time) );

    mxArray* matlabMatrix = engGetVariable( m_engine, varName.c_str() );
    WAssert( matlabMatrix, "Variable doesn't exist!" );
    size_t rows = mxGetM( matlabMatrix );
    size_t cols = mxGetN( matlabMatrix );
    double* matlabMatrixPtr = mxGetPr( matlabMatrix );
    WMatrix<double> result( rows, cols );
    // TODO(philips): do this by std::copy (we use c++)
    memcpy( ( void* ) &result[0], ( void* )mxGetPr( matlabMatrix ), sizeof( double ) * cols * rows );
    // for ( size_t col = 0; col < cols; ++col  )
    // {
    //     for ( size_t row = 0; row < rows; ++row  )
    //     {
    //         result( row, col ) = matlabMatrixPtr[ row * cols + col ];
    //     }
    // }
    matlabMatrixPtr = NULL;
    mxDestroyArray( matlabMatrix );
    return result;
}

double WMatlab::getScalarVariable( const std::string& varName ) const
{
    mxArray* matlabMatrix = engGetVariable( m_engine, varName.c_str() );
    WAssert( matlabMatrix, "Variable doesn't exist!" );
    WAssert( mxGetM( matlabMatrix ) == 1, "Variable isn't a scalar!" );
    WAssert( mxGetN( matlabMatrix ) == 1, "Variable isn't a scalar!" );
    double result( mxGetScalar( matlabMatrix ) );
    mxDestroyArray( matlabMatrix );
    return result;
}


void WMatlab::evalString( const std::string& function )
{
    int ret = engEvalString( m_engine, function.c_str() );
    WAssert( !ret, "Evaluation failed." );
}
