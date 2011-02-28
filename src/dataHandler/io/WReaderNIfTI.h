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

#ifndef WREADERNIFTI_H
#define WREADERNIFTI_H

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "WReader.h"
#include "../WDataSet.h"
#include "../../common/math/WMatrix.h"

#include "../../ext/nifti/nifti1_io.h"
#include "../WExportDataHandler.h"

/**
 * Reader for the NIfTI file format. For NIfTI just see http://nifti.nimh.nih.gov/.
 * \ingroup dataHandler
 */
class OWDATAHANDLER_EXPORT WReaderNIfTI : public WReader // NOLINT
{
/**
 * Only UnitTests may be friends.
 */
friend class WReaderNIfTITest;

public:
    /**
     * Constructs a loader to be executed in its own thread and ets the data needed
     * for the loader when executed in its own thread.
     * \param fileName this file will be loaded
     */
    explicit WReaderNIfTI( std::string fileName );

    /**
     * Loads the dataset.
     *
     * \param dataSetType This parameter can be used to tell the  function in advance how it should interprete the data.
     *
     * \return the dataset loaded.
     */
    virtual boost::shared_ptr< WDataSet > load( DataSetType dataSetType = W_DATASET_NONE  );

protected:
private:
    /**
     * This function allows to copy the data given as a T*
     * by niftilibio into a std::vector< T >
     * \param dataArray data to copy
     * \param countVoxels number of voxels stored in dataArray
     * \param vDim number of values per voxel
     */
    template < typename T > boost::shared_ptr< std::vector< T > > copyArray( const T* dataArray, const size_t countVoxels, const size_t vDim );

    /**
     * This function converts a 4x4 matrix from the NIfTI libs into the format
     * used by OpenWalnut.
     * \param in this matrix will be converted.
     */
    WMatrix< double > convertMatrix( const mat44& in );
};

#endif  // WREADERNIFTI_H
