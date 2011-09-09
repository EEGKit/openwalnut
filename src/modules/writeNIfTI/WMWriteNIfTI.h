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

#ifndef WMWRITENIFTI_H
#define WMWRITENIFTI_H

#include <map>
#include <string>
#include <vector>

#include <osg/Node>
#include <osg/Geode>
#include <osg/Uniform>

#include "../../newDataHandler/WDataSet2.h"
#include "../../newDataHandler/WGridRegular3D2.h"
#include "../../newDataHandler/WGridRegular3D2Specializations.h"
#include "../../newDataHandler/structuralTypes/WScalarStructural.h"
#include "../../newDataHandler/structuralTypes/WVectorFixedStructural.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"

#include "../../common/math/linearAlgebra/WLinearAlgebra.h"

class WPickHandler;

/**
 * Write the input dataset to a NIfTI file
 * \problem It works only on uint8_t values sets so far.
 * \problem Might crash OpenWalnut
 *
 * \ingroup modules
 */
class WMWriteNIfTI : public WModule
{
public:
    /**
     * Standard constructor.
     */
    WMWriteNIfTI();

    /**
     * Destructor.
     */
    ~WMWriteNIfTI();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description of module.
     */
    virtual const std::string getDescription() const;

    /**
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     * Get the icon for this module in XPM format.
     */
    virtual const char** getXPMIcon() const;


protected:
    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void moduleMain();

    /**
     * Initialize the connectors this module is using.
     */
    virtual void connectors();

    /**
     * Initialize the properties for this module.
     */
    virtual void properties();

private:
    /**
     * Allows to get a void* out of WValueSet.
     * \param returnData the casted data will be returned through this pointer.
     */
    template< typename T > void castData( void*& returnData );

    /**
     * This performs all work necessary to actually write the data to the file
     */
    void writeToFile();

    /**
     * The filename property -> where to write the nifty file
     */
    WPropFilename m_filename;
    WPropTrigger  m_saveTriggerProp; //!< This property triggers the actual writing,

    boost::shared_ptr< WModuleInputData< WDataSet2Base > > m_input;  //!< Input connector required by this module.
    boost::shared_ptr< WDataSet2Base > m_dataSet; //!< Pointer providing access to the treated data set in the whole module.

    /**
     *
     * Additional datasets can be included by adding the appropriate operator().
     */
    class SaveDataVisitor
    {
    public:

        /**
         * Constructor.
         *
         * \param data A pointer to the nifti image data.
         */
        explicit SaveDataVisitor( nifti_image* const data )
            : m_data( data )
        {
        }

        /**
         * Write grid properties to the data struct.
         *
         * \param grid The grid.
         */
        void writeGridProps( WGridRegular3D2::ConstSPtr const& grid )
        {
            m_data->nx = grid->getNbVoxelsX();
            m_data->ny = grid->getNbVoxelsY();
            m_data->nz = grid->getNbVoxelsZ();
            m_data->dx = 1.0;
            m_data->dy = 1.0;
            m_data->dz = 1.0;
        }

        /**
         * An operator thats writes scalar data using the correct type.
         *
         * \tparam T The data type of the dataset to be copied to.
         * \param access The access object of the correct type provided by the type resolving mechanism.
         */
        template< typename T >
        void operator()( WDataAccessConst< WGridRegular3D2, T > access )
        {
            typedef typename WDataAccessConst< WGridRegular3D2, T >::VoxelIterator VI;

            WGridRegular3D2::ConstSPtr grid = access.getGrid();

            writeGridProps( grid );

            m_data->nvox = grid->numVoxels();
            m_data->nt = 1;
            m_data->dim[ 0 ] = 4;
            m_data->ndim = 4;
            m_data->datatype = DataType< T >::type;
            m_data->nbyper = sizeof( T );

            T* t = new T[ grid->numVoxels() ];
            VI vi, ve;
            std::size_t count = 0;
            for( tie( vi, ve ) = access.voxels(); vi != ve; ++vi )
            {
                t[ count ] = *vi;
                ++count;
            }
            m_data->data = reinterpret_cast< void* >( t );
        }

        /**
         * An operator thats writes vector data using the correct type.
         *
         * \tparam T The data type of the dataset to be copied to.
         * \param access The access object of the correct type provided by the type resolving mechanism.
         */
        template< typename T, std::size_t k >
        void operator()( WDataAccessConst< WGridRegular3D2, WMatrixFixed< T, k, 1 > > access )
        {
            typedef typename WDataAccessConst< WGridRegular3D2, WMatrixFixed< T, k, 1 > >::VoxelIterator VI;

            WGridRegular3D2::ConstSPtr grid = access.getGrid();

            writeGridProps( grid );

            m_data->nvox = k * grid->numVoxels();
            m_data->nt = k;
            m_data->dim[ 4 ] = k;
            m_data->dim[ 0 ] = 4;
            m_data->ndim = 4;
            m_data->datatype = DataType< T >::type;
            m_data->nbyper = sizeof( T );

            T* t = new T[ grid->numVoxels() * k ];
            VI vi, ve;
            std::size_t count = 0;
            for( tie( vi, ve ) = access.voxels(); vi != ve; ++vi )
            {
                for( std::size_t i = 0; i < k; ++i )
                {
                    t[ i * grid->numVoxels() + count ] = ( *vi )[ i ];
                }
                ++count;
            }
            m_data->data = reinterpret_cast< void* >( t );
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
        nifti_image* const m_data;
    };
};

#endif  // WMWRITENIFTI_H
