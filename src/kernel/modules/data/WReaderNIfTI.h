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

#include "../../../newDataHandler/reader/WReader2.h"
#include "../../../newDataHandler/WDataSet2.h"
#include "../../../newDataHandler/WGridRegular3D2.h"
#include "../../../newDataHandler/WGridRegular3D2Specializations.h"
#include "../../../newDataHandler/structuralTypes/WScalarStructural.h"
#include "../../../common/math/WMatrix.h"

#include "../../../ext/nifti/nifti1_io.h"

/**
 * Reader for the NIfTI file format. For NIfTI just see http://nifti.nimh.nih.gov/.
 * \ingroup dataHandler
 */
class WReaderNIfTI : public WReader2 // NOLINT
{
public:

    /**
     * Constructs a loader to be executed in its own thread and gets the data needed
     * for the loader when executed in its own thread.
     */
    WReaderNIfTI();

    /**
     * Destructor.
     */
    virtual ~WReaderNIfTI();

    /**
     * Returns the name of this reader.
     *
     * \return The name of this reader.
     */
    virtual std::string const getName() const;

    /**
     * Returns a description for this reader.
     *
     * \return A description for this reader.
     */
    virtual std::string const getDescription() const;

    /**
     * Check if this file can be loaded by this reader.
     *
     * \param filename The name or path to the file to be loaded.
     * \return true if a dataset can be contructed from the content of the file by this reader.
     */
    virtual bool acceptFile( boost::filesystem::path const& filename ) const;

    /**
     * Suggest the type of the datasets that could be constructed from the content of the file.
     * It is assumed that you checked the file with acceptFile first.
     *
     * \param filename The name or path to the file to be loaded.
     * \return A vector of pointers to the different dataset types that can be constructed from the file.
     */
    virtual std::vector< WDataSet2Base::SPtr > suggestDataSets( boost::filesystem::path const& filename ) const;

    /**
     * Actually constructs the dataset.
     *
     * \param filename The name or path to the file to be loaded.
     * \param dataSetSample A pointer from which the type of the dataset to be constructed can be inferred.
     * \return The loaded dataset.
     */
    virtual WDataSet2Base::SPtr load( boost::filesystem::path const& filename,
                                     WDataSet2Base::ConstSPtr const& dataSetSample ) const;

    /**
     * Returns a standard transformation.
     *
     * \return A Wmatrix that represents the dataset's transformation.
     */
    WMatrix< double > getStandardTransform() const;

    /**
     * Returns the SForm transformation stored in the nifti file's header.
     *
     * \return A Wmatrix that represents the dataset's transformation.
     */
    WMatrix< double > getSFormTransform() const;

    /**
     * Returns the QForm transformation stored in the nifti file's header.
     *
     * \return A Wmatrix that represents the dataset's transformation.
     */
    WMatrix< double > getQFormTransform() const;

protected:
private:

    /**
     * This function converts a 4x4 matrix from the NIfTI libs into the format
     * used by OpenWalnut.
     *
     * \param in this matrix will be converted.
     * \return The converted matrix.
     */
    WMatrix< double > convertMatrix( const mat44& in );

    //! the sform transform stored in the file header
    WMatrix< double > m_sform;

    //! the qform transform stored in the file header
    WMatrix< double > m_qform;

    /**
     * A functor used to copy the data from the loaded file to a dataset using the correct
     * data type (e.g. double, int etc.).
     *
     * Additional datasets can be included by adding the appropriate operator().
     */
    class CopyDataFunctor
    {
    public:

        /**
         * Constructor.
         *
         * \param data A pointer to the nifti image data.
         */
        explicit CopyDataFunctor( nifti_image const* const data )
            : m_data( data )
        {
        }

        /**
         * An operator thats loads scalar data using the correct type.
         *
         * \tparam T The data type of the dataset to be copied to.
         * \param access The access object of the correct type provided by the type resolving mechanism.
         */
        template< typename T >
        void operator()( WDataAccess< WGridRegular3D2, T > access )
        {
            typedef typename WDataAccess< WGridRegular3D2, T >::VoxelIterator VI;

            T const* const dataptr = reinterpret_cast< T const* >( m_data->data );
            VI vi, ve;
            std::size_t count = 0;
            for( tie( vi, ve ) = access.voxels(); vi != ve; ++vi )
            {
                *vi = dataptr[ count ];
                ++count;
            }
        }

        // to add other dataset types, for example vector data, add another operator():
        //
        // example for vector data:
        //
        // template< typename T >
        // void operator()( WDataAccess< WGridRegular3D2, WMatrixFixed< T, 3, 1 > > access )
        // {
        //     ...
        // }

    private:

        //! The nifti image data.
        nifti_image const* const m_data;
    };
};

#endif  // WREADERNIFTI_H
