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

#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "../../common/math/linearAlgebra/WLinearAlgebra.h"
#include "../../common/WAssert.h"
#include "../../common/WProgress.h"
#include "../../common/WStringUtils.h"
#include "../../kernel/WKernel.h"

#include "../../newDataHandler/WNeighborhoodIterator.h"

#include "WMGaussFiltering2.xpm"
#include "WMGaussFiltering2.h"

/**
 * A visitor/functor object used to perform single-direction gauss filtering on regular 3D datsets.
 */
class GaussVisitor
{
public:

    //! The type of the dataset.
    typedef WDataSet2< WGridRegular3D2, WScalarStructural > DataSetType;

    /**
     * Constructor.
     *
     * \param result The dataset to write to or NULL.
     * \param direction In which direction to smooth, 0 = x, 1 = y, 2 = z.
     */
    GaussVisitor( DataSetType::SPtr& result, int direction )  // NOLINT reference
        : m_result( result ),
          m_direction( direction )
    {
        m_nbh[ 0 ].add( -1, 0, 0 );
        m_nbh[ 0 ].add( 1, 0, 0 );
        m_nbh[ 1 ].add( 0, -1, 0 );
        m_nbh[ 1 ].add( 0, 1, 0 );
        m_nbh[ 2 ].add( 0, 0, -1 );
        m_nbh[ 2 ].add( 0, 0, 1 );
    }

    /**
     * Performs one iteration of gaussian blur with a 1x1x3 kernel.
     *
     * \tparam T The type of the data stored in the dataset.
     * \param access The access object of the dataset to blur.
     */
    template< typename T >
    void operator()( WDataAccessConst< WGridRegular3D2, T > access )
    {
        typedef WDataAccessConst< WGridRegular3D2, T > AccessInputType;
        typedef WDataAccess< WGridRegular3D2, WDataProxy< T > > AccessOutputType;

        if( !m_result )
        {
            m_result = DataSetType::SPtr( new DataSetType( access.getGrid(), T() ) );
        }
        AccessOutputType accessResult = m_result->getAccess< T >();

        typedef typename AccessInputType::VoxelIterator VI;
        typedef typename AccessInputType::VoxelNeighborIteratorWrap NI;
        typedef typename AccessOutputType::VoxelIterator RI;

        RI ri = accessResult.voxels().first;
        VI vi, ve;

        WBoundaryStrategyWrap< T > bs;

        for( tie( vi, ve ) = access.voxels(); vi != ve; ++vi )
        {
            NI ni;
            ni = vi.neighbors( &m_nbh[ m_direction ], &bs ).first;

            *ri = ( *ni + *vi * 2 + *( ++ni ) ) / 4;
            ++ri;
        }
    }

private:
    //! The dataset containing the result of the filtering.
    DataSetType::SPtr& m_result;

    //! The direction
    int m_direction;

    //! The 3 neighborhoods, one for each direction.
    WNeighborhood m_nbh[ 3 ];
};

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMGaussFiltering2 )

WMGaussFiltering2::WMGaussFiltering2() :
    WModule()
{
    // WARNING: initializing connectors inside the constructor will lead to an exception.
    // Implement WModule::initializeConnectors instead.
}

WMGaussFiltering2::~WMGaussFiltering2()
{
    // cleanup
    removeConnectors();
}

boost::shared_ptr< WModule > WMGaussFiltering2::factory() const
{
    return boost::shared_ptr< WModule >( new WMGaussFiltering2() );
}

const char** WMGaussFiltering2::getXPMIcon() const
{
    return gaussfiltering_xpm;
}

const std::string WMGaussFiltering2::getName() const
{
    return "Gauss Filtering 2";
}

const std::string WMGaussFiltering2::getDescription() const
{
    return "Runs a discretized Gauss filter as mask over a simple scalar field.";
}

void WMGaussFiltering2::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    ready();

    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        if( m_shutdownFlag() )
        {
            break;
        }

        boost::shared_ptr< DataSetType > newDataSet = m_input->getData();
        bool dataChanged = ( m_dataSet != newDataSet );

        if( dataChanged )
        {
            debugLog() << "Received new data.";
            m_dataSet = newDataSet;
        }

        if( dataChanged || m_iterations->changed() )
        {
            int iterations = m_iterations->get( true );

            // do the filtering
            DataSetType::SPtr ds[ 2 ];

            infoLog() << "Filtering...";

            clock_t t = clock();

            bool b = false;

            for( int i = 0; i < iterations; ++i )
            {
                for( int j = 0; j < 3; ++j )
                {
                    if( i == 0 && j == 0 )
                    {
                        GaussVisitor gv( ds[ 0 ], 0 );
                        static_cast< DataSetType const* const >( m_dataSet.get() )->applyVisitor( &gv );
                    }
                    else
                    {
                        GaussVisitor gv( ds[ static_cast< int >( !b ) ], j );
                        static_cast< DataSetType const* const >( ds[ static_cast< int >( b ) ].get() )->applyVisitor( &gv );
                        b = !b;
                    }
                }
            }

            infoLog() << "Time: " << clock() - t << "!";

            m_output->updateData( ds[ static_cast< int >( b ) ] );

            infoLog() << "Done.";
        }
    }

    infoLog() << "Shutting down, bye!";
}

void WMGaussFiltering2::connectors()
{
    // initialize connectors
    m_input = boost::shared_ptr< WModuleInputData< DataSetType > >(
            new WModuleInputData< DataSetType > ( shared_from_this(), "in",
                    "The dataset to filter" ) );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_input );

    // initialize connectors
    m_output = boost::shared_ptr< WModuleOutputData< DataSetType > >(
            new WModuleOutputData< DataSetType > ( shared_from_this(), "out",
                    "The filtered data set." ) );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_output );

    // call WModules initialization
    WModule::connectors();
}

void WMGaussFiltering2::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_iterations      = m_properties->addProperty( "Iterations",         "How often should the filter be applied.", 1, m_propCondition );
    m_iterations->setMin( 1 );
    m_iterations->setMax( 100 );

    WModule::properties();
}
