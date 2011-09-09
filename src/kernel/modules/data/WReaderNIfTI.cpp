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

#include "../../../common/WIOTools.h"
#include "../../../common/WLogger.h"

#include "WReaderNIfTI.h"

WReaderNIfTI::WReaderNIfTI()
    : WReader2(),
      m_sform( 4, 4 ),
      m_qform( 4, 4 )
{
}

WReaderNIfTI::~WReaderNIfTI()
{
}

std::string const WReaderNIfTI::getName() const
{
    return "WReaderNIfTI";
}

std::string const WReaderNIfTI::getDescription() const
{
    return "Loads datasets from NIfTI files.";
}

bool WReaderNIfTI::acceptFile( boost::filesystem::path const& filename ) const
{
    std::string suffix = getSuffix( filename.file_string() );

    if( suffix == ".nii" || ( suffix == ".gz" && ::nifti_compiled_with_zlib() ) )
    {
        if( suffix == ".gz" )
        {
            boost::filesystem::path p( filename );
            p.replace_extension( "" );
            suffix = getSuffix( p.file_string() );

            if( suffix != ".nii" )
            {
                return false;
            }
        }
        return true;
    }
    return false;
}

std::vector< WDataSet2Base::SPtr > WReaderNIfTI::suggestDataSets( boost::filesystem::path const& filename ) const
{
    std::vector< WDataSet2Base::SPtr > suggestions;

    nifti_image* header = nifti_image_read( filename.file_string().c_str(), 0 );

    if( !header || header->ndim < 3 || header->dim[ 1 ] < 1 || header->dim[ 2 ] < 1 || header->dim[ 3 ] < 1 )
    {
        return suggestions;
    }

    if( header->dim[ 4 ] == 1 && ( header->dim[ 5 ] == 0 || header->dim[ 5 ] == 1 ) )
    {
        suggestions.push_back( WDataSet2Base::SPtr( new WDataSet2< WGridRegular3D2, WScalarStructural >() ) );
    }

    if( header->dim[ 4 ] == 3 && ( header->dim[ 5 ] == 0 || header->dim[ 5 ] == 1 ) )
    {
        suggestions.push_back( WDataSet2Base::SPtr( new WDataSet2< WGridRegular3D2, WVectorFixedStructural< 3 > >() ) );
    }

    if( header )
    {
        nifti_image_free( header );
    }

    return suggestions;
}

WDataSet2Base::SPtr WReaderNIfTI::load( boost::filesystem::path const& filename,
                                       WDataSet2Base::ConstSPtr const& dataSetSample ) const
{
    // dataset
    WDataSet2Base::SPtr ds;

    // load header
    nifti_image* header = nifti_image_read( filename.file_string().c_str(), 0 );
    if( !header )
    {
        return ds;
    }

    // load data
    nifti_image* filedata = nifti_image_read( filename.file_string().c_str(), 1 );
    if( !filedata )
    {
        nifti_image_free( header );
        return ds;
    }

    // build grid
    WGridRegular3D2::SPtr grid( new WGridRegular3D2( header->dim[ 1 ], header->dim[ 2 ], header->dim[ 3 ] ) );

    // init copy functor
    CopyDataFunctor copy( filedata );

    // init dataset depending on suggested type
    if( boost::shared_dynamic_cast< WDataSet2< WGridRegular3D2, WScalarStructural > const >( dataSetSample ) )
    {
        // scalar dataset
        WStructuralTypeStore< WScalarStructural::ParameterVector > ts =
            typeStoreFromRuntimeVariables< WScalarStructural::ParameterVector >( static_cast< std::size_t >( header->datatype ) );

        typedef WDataSet2< WGridRegular3D2, WScalarStructural > DataSetType;
        DataSetType::SPtr ptr = DataSetType::SPtr( new DataSetType( grid, ts ) );
        ds = ptr;

        ptr->applyVisitor( &copy );
    }
    if( boost::shared_dynamic_cast< WDataSet2< WGridRegular3D2, WVectorFixedStructural< 3 > > const >( dataSetSample ) )
    {
        // vector dataset
        WStructuralTypeStore< WVectorFixedStructural< 3 >::ParameterVector > ts =
            typeStoreFromRuntimeVariables< WVectorFixedStructural< 3 >::ParameterVector >(
                                            static_cast< std::size_t >( header->datatype ) );

        typedef WDataSet2< WGridRegular3D2, WVectorFixedStructural< 3 > > DataSetType;
        DataSetType::SPtr ptr = DataSetType::SPtr( new DataSetType( grid, ts ) );
        ds = ptr;

        ptr->applyVisitor( &copy );
    }
    else
    {
        nifti_image_free( header );
        nifti_image_free( filedata );
        return WDataSet2Base::SPtr();
    }

    // return
    nifti_image_free( header );
    nifti_image_free( filedata );
    return ds;
}

WMatrix< double > WReaderNIfTI::convertMatrix( const mat44& in )
{
    WMatrix< double > out( 4, 4 );

    bool isZero = true;
    for( size_t i = 0; i < 4; ++i )
    {
        for( size_t j = 0; j < 4; ++j )
        {
            out( i, j ) = in.m[i][j];
            isZero = isZero && out( i, j ) == 0.0;
        }
    }

    if( isZero )
    {
        out.makeIdentity();
    }
    return out;
}

WMatrix< double > WReaderNIfTI::getStandardTransform() const
{
    return WMatrix< double >( 4, 4 ).makeIdentity();
}

WMatrix< double > WReaderNIfTI::getSFormTransform() const
{
    return m_sform;
}

WMatrix< double > WReaderNIfTI::getQFormTransform() const
{
    return m_qform;
}
