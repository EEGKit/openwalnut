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

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "../../common/WIOTools.h"
#include "../WDataHandlerEnums.h"
#include "../WDataSet.h"
#include "../WDataSetDTI.h"
#include "../WDataSetRawHARDI.h"
#include "../WDataSetScalar.h"
// #include "../WDataSetSegmentation.h"  // TODO(philips): polish WDataSetSegmentation for check in
#include "../WDataSetSingle.h"
#include "../WDataSetSphericalHarmonics.h"
#include "../WDataSetTimeSeries.h"
#include "../WDataSetVector.h"
#include "../WGrid.h"
#include "../WGridRegular3D.h"
#include "../WSubject.h"
#include "../WValueSet.h"
#include "../WValueSetBase.h"
#include "WReaderNIfTI.h"
#include "../../common/WLogger.h"

WReaderNIfTI::WReaderNIfTI( std::string fileName )
    : WReader( fileName )
{
}

template< typename T > std::vector< T > WReaderNIfTI::copyArray( const T* dataArray, const size_t countVoxels,
        const size_t vDim )
{
    std::vector< T > data( countVoxels * vDim );
    for( unsigned int i = 0; i < countVoxels; ++i )
    {
        for ( unsigned int j = 0; j < vDim; ++j )
        {
            data[i * vDim + j] = dataArray[( j * countVoxels ) + i];
        }
    }
    return data;
}


wmath::WMatrix< double > WReaderNIfTI::convertMatrix( const mat44& in )
{
    wmath::WMatrix< double > out( 4, 4 );
    for( size_t i = 0; i < 4; ++i )
    {
        for( size_t j = 0; j < 4; ++j )
        {
            out( i, j ) = in.m[i][j];
        }
    }
    return out;
}

boost::shared_ptr< WDataSet > WReaderNIfTI::load()
{
    nifti_image* header = nifti_image_read( m_fname.c_str(), 0 );

    WAssert( header, "Error during file access to NIfTI file. This probably means that the file is corrupted." );

    WAssert( header->ndim >= 3,
             "The NIfTI file contains data that has less than the three spatial dimension. OpenWalnut is not able to handle this." );

    int columns = header->dim[1];
    int rows = header->dim[2];
    int frames = header->dim[3];

    boost::shared_ptr< WValueSetBase > newValueSet;
    boost::shared_ptr< WGrid > newGrid;

    nifti_image* filedata = nifti_image_read( m_fname.c_str(), 1 );

    unsigned int vDim;
    if( header->ndim >= 4 )
    {
        vDim = header->dim[4];
    }
    else
    {
        vDim = 1;
    }

    unsigned int order = ( ( vDim == 1 ) ? 0 : 1 );  // TODO(all): Does recognize vectors and scalars only so far.
    unsigned int countVoxels = columns * rows * frames;

    try
    {
        // don't rearrange if this is a time series
        if( header->dim[ 5 ] <= 1 )
        {
            switch( header->datatype )
            {
            case DT_UNSIGNED_CHAR:
                {
                    std::vector< unsigned char > data = copyArray( reinterpret_cast< unsigned char* >( filedata->data ), countVoxels, vDim );
                    newValueSet = boost::shared_ptr< WValueSetBase >( new WValueSet< unsigned char >( order, vDim, data, W_DT_UNSIGNED_CHAR ) );
                    break;
                }

            case DT_SIGNED_SHORT:
                {
                    std::vector< int16_t > data = copyArray( reinterpret_cast< int16_t* >( filedata->data ), countVoxels, vDim );
                    newValueSet = boost::shared_ptr< WValueSetBase >( new WValueSet< int16_t >( order, vDim, data, W_DT_INT16 ) );
                    break;
                }

            case DT_INT32:
                {
                    std::vector< int32_t > data = copyArray( reinterpret_cast< int32_t* >( filedata->data ), countVoxels, vDim );
                    newValueSet = boost::shared_ptr< WValueSetBase >( new WValueSet< int32_t >( order, vDim, data, W_DT_SIGNED_INT ) );
                    break;
                }


            case DT_FLOAT:
                {
                    std::vector< float > data = copyArray( reinterpret_cast< float* >( filedata->data ), countVoxels, vDim );
                    newValueSet = boost::shared_ptr< WValueSetBase >( new WValueSet< float >( order, vDim, data, W_DT_FLOAT ) );
                    break;
                }

            case DT_DOUBLE:
                {
                    std::vector< double > data = copyArray( reinterpret_cast< double* >( filedata->data ), countVoxels, vDim );
                    newValueSet = boost::shared_ptr< WValueSetBase >( new WValueSet< double >( order, vDim, data, W_DT_DOUBLE ) );
                    break;
                }

            default:
                wlog::error( "WReaderNIfTI" ) << "unknown data type " << header->datatype << std::endl;
                newValueSet = boost::shared_ptr< WValueSetBase >();
            }
        }
    }
    catch( const std::exception& e )
    {
        nifti_image_free( filedata );
        throw e;
    }

    newGrid = boost::shared_ptr< WGridRegular3D >( new WGridRegular3D(
        columns, rows, frames,
        convertMatrix( header->qto_xyz ),
        convertMatrix( header->sto_xyz ),
        header->dx, header->dy, header->dz ) );

    boost::shared_ptr< WDataSet > newDataSet;
    // known description
    std::string description( header->descrip );
    // TODO(philips): polish WDataSetSegmentation for check in
    //     if ( !description.compare( "WDataSetSegmentation" ) )
    //     {
    //         wlog::debug( "WReaderNIfTI" ) << "Load as segmentation" << std::endl;
    //         newDataSet = boost::shared_ptr< WDataSet >( new WDataSetSegmentation( newValueSet, newGrid ) );
    //     }
    //     else

    if ( !description.compare( "WDataSetSphericalHarmonics" ) )
    {
        wlog::debug( "WReaderNIfTI" ) << "Load as spherical harmonics" << std::endl;
        newDataSet = boost::shared_ptr< WDataSet >( new WDataSetSphericalHarmonics( newValueSet, newGrid ) );
    }
    // 4th dimension is the time
    // note that in the nifti standard, dim[ 4 ] is the temporal dimension
    // we use dim[ 5 ] here
    else if( header->dim[ 5 ] > 1 )
    {
        WAssert( header->dim[ 4 ] == 1, "Only scalar datasets are supported for time series so far." );
        wlog::debug( "WReaderNIfTI" ) << "Load as WDataSetTimeSeries";
        std::size_t numTimeSlices = header->dim[ 5 ];
        float tw = header->pixdim[ 5 ];
        WAssert( tw != 0.0f, "" );

        std::vector< boost::shared_ptr< WDataSetScalar const > > ds;
        std::vector< float > times;
        float t = 0.0f;
        for( std::size_t k = 0; k < numTimeSlices; ++k )
        {
            times.push_back( t );
            t += tw;
            boost::shared_ptr< WValueSetBase > vs;
            switch( header->datatype )
            {
            case DT_UINT8:
                {
                    uint8_t* ptr = reinterpret_cast< uint8_t* >( filedata->data );
                    std::vector< uint8_t > values( ptr + k * countVoxels, ptr + ( k + 1 ) * countVoxels );
                    vs = boost::shared_ptr< WValueSetBase >( new WValueSet< uint8_t >( 0, 1, values, W_DT_UINT8 ) );
                }
                break;
            case DT_INT8:
                {
                    int8_t* ptr = reinterpret_cast< int8_t* >( filedata->data );
                    std::vector< int8_t > values( ptr + k * countVoxels, ptr + ( k + 1 ) * countVoxels );
                    vs = boost::shared_ptr< WValueSetBase >( new WValueSet< int8_t >( 0, 1, values, W_DT_INT8 ) );
                }
                break;
            case DT_UINT16:
                {
                    uint16_t* ptr = reinterpret_cast< uint16_t* >( filedata->data );
                    std::vector< uint16_t > values( ptr + k * countVoxels, ptr + ( k + 1 ) * countVoxels );
                    vs = boost::shared_ptr< WValueSetBase >( new WValueSet< uint16_t >( 0, 1, values, W_DT_UINT16 ) );
                }
                break;
            case DT_INT16:
                {
                    int16_t* ptr = reinterpret_cast< int16_t* >( filedata->data );
                    std::vector< int16_t > values( ptr + k * countVoxels, ptr + ( k + 1 ) * countVoxels );
                    vs = boost::shared_ptr< WValueSetBase >( new WValueSet< int16_t >( 0, 1, values, W_DT_INT16 ) );
                }
                break;
            case DT_UINT32:
                {
                    uint32_t* ptr = reinterpret_cast< uint32_t* >( filedata->data );
                    std::vector< uint32_t > values( ptr + k * countVoxels, ptr + ( k + 1 ) * countVoxels );
                    vs = boost::shared_ptr< WValueSetBase >( new WValueSet< uint32_t >( 0, 1, values, W_DT_UINT32 ) );
                }
                break;
            case DT_SIGNED_INT:
                {
                    int32_t* ptr = reinterpret_cast< int32_t* >( filedata->data );
                    std::vector< int32_t > values( ptr + k * countVoxels, ptr + ( k + 1 ) * countVoxels );
                    vs = boost::shared_ptr< WValueSetBase >( new WValueSet< int32_t >( 0, 1, values, W_DT_SIGNED_INT ) );
                }
                break;
            case DT_UINT64:
                {
                    uint64_t* ptr = reinterpret_cast< uint64_t* >( filedata->data );
                    std::vector< uint64_t > values( ptr + k * countVoxels, ptr + ( k + 1 ) * countVoxels );
                    vs = boost::shared_ptr< WValueSetBase >( new WValueSet< uint64_t >( 0, 1, values, W_DT_UINT64 ) );
                }
                break;
            case DT_INT64:
                {
                    int64_t* ptr = reinterpret_cast< int64_t* >( filedata->data );
                    std::vector< int64_t > values( ptr + k * countVoxels, ptr + ( k + 1 ) * countVoxels );
                    vs = boost::shared_ptr< WValueSetBase >( new WValueSet< int64_t >( 0, 1, values, W_DT_INT64 ) );
                }
                break;
            case DT_FLOAT:
                {
                    float* ptr = reinterpret_cast< float* >( filedata->data );
                    std::vector< float > values( ptr + k * countVoxels, ptr + ( k + 1 ) * countVoxels );
                    vs = boost::shared_ptr< WValueSetBase >( new WValueSet< float >( 0, 1, values, W_DT_FLOAT ) );
                }
                break;
            case DT_DOUBLE:
                {
                    double* ptr = reinterpret_cast< double* >( filedata->data );
                    std::vector< double > values( ptr + k * countVoxels, ptr + ( k + 1 ) * countVoxels );
                    vs = boost::shared_ptr< WValueSetBase >( new WValueSet< double >( 0, 1, values, W_DT_DOUBLE ) );
                }
                break;
            default:
                throw WException( std::string( "Unsupported datatype in WReaderNIfTI" ) );
                break;
            }
            ds.push_back( boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( vs, newGrid ) ) );
        }
        newDataSet = boost::shared_ptr< WDataSet >( new WDataSetTimeSeries( ds, times ) );
    }
    // unknown description
    else
    {
        if( vDim == 3 )
        {
            wlog::debug( "WReaderNIfTI" ) << "Load as WDataSetVector";
            newDataSet = boost::shared_ptr< WDataSet >( new WDataSetVector( newValueSet, newGrid ) );
        }
        else if( vDim == 1 )
        {
            wlog::debug( "WReaderNIfTI" ) << "Load as WDataSetScalar";
            newDataSet = boost::shared_ptr< WDataSet >( new WDataSetScalar( newValueSet, newGrid ) );
        }
        else if( vDim > 20 && header->dim[ 5 ] <= 1 ) // hardi data, order 1
        {
            wlog::debug( "WReaderNIfTI" ) << "Load as WDataSetRawHARDI";

            std::string gradientFileName = m_fname;
            using wiotools::getSuffix;
            std::string suffix = getSuffix( m_fname );

            if( suffix == ".gz" )
            {
                WAssert( gradientFileName.length() > 3, "" );
                gradientFileName.resize( gradientFileName.length() - 3 );
                suffix = getSuffix( gradientFileName );
            }
            WAssert( suffix == ".nii", "Input file is not a nifti file." );

            WAssert( gradientFileName.length() > 4, "" );
            gradientFileName.resize( gradientFileName.length() - 4 );
            gradientFileName += ".bvec";

            // check if the file exists
            std::ifstream i( gradientFileName.c_str() );
            if( i.bad() || !i.is_open() )
            {
                if( i.is_open() )
                {
                    i.close();
                }
                // cannot find the appropriate gradient vectors, build a dataSetSingle instead of hardi
                newDataSet = boost::shared_ptr< WDataSet >( new WDataSetSingle( newValueSet, newGrid ) );
                wlog::debug( "WReaderNIfTI" ) << "Could not find corresponding gradients file, loading as WDataSetSingle instead.";
            }
            else
            {
                // read gradients, there should be 3 * vDim values in the file
                typedef std::vector< wmath::WVector3D > GradVec;
                boost::shared_ptr< GradVec > newGradients = boost::shared_ptr< GradVec >( new GradVec( vDim ) );

                // the file should contain the x-coordinates in line 0, y-coordinates in line 1,
                // z-coordinates in line 2
                for( unsigned int j = 0; j < 3; ++j )
                {
                    for( unsigned int k = 0; k < vDim; ++k )
                    {
                        i >> newGradients->operator[] ( k )[ j ];
                    }
                }
                bool success = !i.eof();
                i.close();

                WAssert( success, "Gradient file did not contain enough gradients." );

                newDataSet = boost::shared_ptr< WDataSet >( new WDataSetRawHARDI( newValueSet, newGrid, newGradients ) );
            }
        }
        else if( header->intent_code == NIFTI_INTENT_SYMMATRIX )
        {
            wlog::debug( "WReaderNIfTI" ) << "Load as WDataSetDTI";
            newDataSet = boost::shared_ptr< WDataSetDTI >( new WDataSetDTI( newValueSet, newGrid ) );
        }
        else
        {
            wlog::debug( "WReaderNIfTI" ) << "Load as WDataSetSingle";
            newDataSet = boost::shared_ptr< WDataSet >( new WDataSetSingle( newValueSet, newGrid ) );
        }
    }
    newDataSet->setFileName( m_fname );

    nifti_image_free( filedata );

    return newDataSet;
}
