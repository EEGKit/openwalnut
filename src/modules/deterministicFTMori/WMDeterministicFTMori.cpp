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

#include <sstream>
#include <string>
#include <vector>

#include "../../common/math/WTensorSym.h"
#include "../../common/math/WTensorFunctions.h"
#include "../../common/WAssert.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../../dataHandler/io/WWriterFiberVTK.h"
#include "../../dataHandler/WValueSet.h"
#include "../../common/math/WLinearAlgebraFunctions.h"

#include "WMDeterministicFTMori.h"
#include "WMDeterministicFTMori.xpm"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMDeterministicFTMori )

WMDeterministicFTMori::WMDeterministicFTMori()
    : WModule(),
      m_dataSet(),
      m_fiberSet(),
      m_eigenField(),
      m_eigenOperation(),
      m_eigenPool()
{
}

WMDeterministicFTMori::~WMDeterministicFTMori()
{
}

boost::shared_ptr< WModule > WMDeterministicFTMori::factory() const
{
    return boost::shared_ptr< WModule >( new WMDeterministicFTMori() );
}

const char** WMDeterministicFTMori::getXPMIcon() const
{
    return moriTracking_xpm;
}

const std::string WMDeterministicFTMori::getName() const
{
    return "Mori Det. Tracking";
}

const std::string WMDeterministicFTMori::getDescription() const
{
    return "Implements the simple deterministic fibertracking algorithm by Mori et al.";
}

void WMDeterministicFTMori::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    ready();

    while ( !m_shutdownFlag() )
    {
        debugLog() << "Waiting.";
        m_moduleState.wait();

        if( m_trackingPool && m_trackingPool->status() == W_THREADS_FINISHED )
        {
            m_fiberSet = m_fiberAccu.buildDataSet();
            m_fiberAccu.clear();
            m_currentProgress->finish();
            debugLog() << "Tracking done.";
            // forward result
            m_output->updateData( m_fiberSet );
            m_trackingPool = boost::shared_ptr< TrackingFuncType >();
        }

        boost::shared_ptr< WDataSetSingle > inData = m_input->getData();
        bool dataChanged = ( m_dataSet != inData );
        if( dataChanged || !m_dataSet )
        {
            m_dataSet = inData;
            if( !m_dataSet )
            {
                continue;
            }
        }

        if( dataChanged && m_dataSet )
        {
            resetEigenFunction();
            // start the eigenvector computation
            // when the computation finishes, we'll be notified by the threadspool's
            // threadsDoneCondition
            resetProgress( m_dataSet->getValueSet()->size() );
            m_eigenPool->run();
            debugLog() << "Running computation of eigenvectors.";
        }
        else if( m_eigenPool && m_eigenPool->status() == W_THREADS_FINISHED )
        {
            // the computation of the eigenvectors has finished
            // we have a new eigenvectorfield
            m_currentProgress->finish();
            WAssert( m_eigenOperation, "" );
            m_eigenField = m_eigenOperation->getResult();
            m_eigenPool = boost::shared_ptr< EigenFunctionType >();
            debugLog() << "Eigenvectors computed.";

            m_currentMinFA = m_minFA->get( true );
            m_currentMinPoints = static_cast< std::size_t >( m_minPoints->get( true ) );
            m_currentMinCos = m_minCos->get( true );

            // perform the actual tracking
            resetTracking();
            resetProgress( m_dataSet->getValueSet()->size() );
            m_trackingPool->run();
            debugLog() << "Running tracking function.";
        }
        else if( !m_eigenPool && ( m_minFA->changed() || m_minPoints->changed() || m_minCos->changed() ) )
        {
            m_currentMinFA = m_minFA->get( true );
            m_currentMinPoints = static_cast< std::size_t >( m_minPoints->get( true ) );
            m_currentMinCos = m_minCos->get( true );

            // if there are no new eigenvectors or datasets,
            // restart the tracking, as there are changes to the parameters
            resetTracking();
            boost::shared_ptr< WGridRegular3D > g( boost::shared_dynamic_cast< WGridRegular3D >( m_eigenField->getGrid() ) );
            std::size_t todo = ( g->getNbCoordsX() - 2 ) * ( g->getNbCoordsY() - 2 ) * ( g->getNbCoordsZ() - 2 );
            resetProgress( todo );
            m_trackingPool->run();
            debugLog() << "Running tracking function.";
        }
    }

    // module shutdown
    debugLog() << "Shutting down module.";
    if( m_eigenPool )
    {
        if( m_eigenPool->status() == W_THREADS_RUNNING || m_eigenPool->status() == W_THREADS_STOP_REQUESTED )
        {
            m_eigenPool->stop();
            m_eigenPool->wait();
            debugLog() << "Aborting computation of eigenvalues because the module was ordered to shut down.";
        }
    }
    if( m_trackingPool )
    {
        if( m_trackingPool->status() == W_THREADS_RUNNING || m_trackingPool->status() == W_THREADS_STOP_REQUESTED )
        {
            m_trackingPool->stop();
            m_trackingPool->wait();
            debugLog() << "Aborting fiber tracking because the module was ordered to shut down.";
        }
    }
}

void WMDeterministicFTMori::connectors()
{
    m_output = boost::shared_ptr< WModuleOutputData< WDataSetFibers > >( new WModuleOutputData< WDataSetFibers >( shared_from_this(),
                "det FT Mori output fibers", "A set of fibers extracted from the input set." )
            );

    m_input = boost::shared_ptr< WModuleInputData< WDataSetSingle > >( new WModuleInputData< WDataSetSingle >( shared_from_this(),
                "det FT Mori input tensor field", "An input set of 2nd-order tensors on a regular 3d-grid." )
            );

    addConnector( m_input );
    addConnector( m_output );

    WModule::connectors();
}

void WMDeterministicFTMori::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_minFA = m_properties->addProperty( "Min. FA", "The fractional anisotropy threshold value needed by "
                                                    "Mori's fiber tracking algorithm.", 0.2, m_propCondition );
    m_minFA->setMax( 1.0 );
    m_minFA->setMin( 0.0 );

    m_minPoints = m_properties->addProperty( "Min. Points", "The minimum number of points per fiber.", 30, m_propCondition );
    m_minPoints->setMax( 100 );
    m_minPoints->setMin( 1 );

    m_minCos = m_properties->addProperty( "Min. Cosine", "Minimum cosine of the angle between two"
                                           " adjacent fiber segments.", 0.80, m_propCondition );
    m_minCos->setMax( 1.0 );
    m_minCos->setMin( 0.0 );

    WModule::properties();
}

void WMDeterministicFTMori::activate()
{
    WModule::activate();
}

void WMDeterministicFTMori::resetEigenFunction()
{
    // check if we need to stop the currently running eigencomputation
    if( m_eigenPool )
    {
        debugLog() << "Stopping eigencomputation.";
        WThreadedFunctionStatus s = m_eigenPool->status();
        if( s != W_THREADS_FINISHED && s != W_THREADS_ABORTED )
        {
            m_eigenPool->stop();
            m_eigenPool->wait();
            s = m_eigenPool->status();
            WAssert( s == W_THREADS_FINISHED || s == W_THREADS_ABORTED, "" );
        }
        m_moduleState.remove( m_eigenPool->getThreadsDoneCondition() );
    }
    // the threadpool should have finished computing by now

    // create a new one
    m_eigenOperation = boost::shared_ptr< TPVO >( new TPVO( m_dataSet, boost::bind( &WMDeterministicFTMori::eigenFunc, this, _1 ) ) );
    m_eigenPool = boost::shared_ptr< EigenFunctionType >( new EigenFunctionType( WM_MORI_NUM_CORES, m_eigenOperation ) );
    m_moduleState.add( m_eigenPool->getThreadsDoneCondition() );
}

void WMDeterministicFTMori::resetTracking()
{
    // check if we need to stop the currently running tracking
    if( m_trackingPool )
    {
        debugLog() << "Stopping tracking.";
        WThreadedFunctionStatus s = m_trackingPool->status();
        if( s != W_THREADS_FINISHED && s != W_THREADS_ABORTED )
        {
            m_trackingPool->stop();
            m_trackingPool->wait();
            s = m_trackingPool->status();
            WAssert( s == W_THREADS_FINISHED || s == W_THREADS_ABORTED, "" );
        }
        m_moduleState.remove( m_trackingPool->getThreadsDoneCondition() );
    }
    // the threadpool should have finished computing by now

    m_fiberAccu.clear();

    // create a new one
    boost::shared_ptr< Tracking > t( new Tracking( m_eigenField,
                                                   boost::bind( &This::getEigenDirection, this, _1, _2 ),
                                                   boost::bind( &wtracking::WTrackingUtility::followToNextVoxel, _1, _2, _3 ),
                                                   boost::bind( &This::fiberVis, this, _1 ),
                                                   boost::bind( &This::pointVis, this, _1 ) ) );
    m_trackingPool = boost::shared_ptr< TrackingFuncType >( new TrackingFuncType( WM_MORI_NUM_CORES, t ) );
    m_moduleState.add( m_trackingPool->getThreadsDoneCondition() );
}

wmath::WVector3D WMDeterministicFTMori::getEigenDirection( boost::shared_ptr< WDataSetSingle const > ds,
                                                           wtracking::WTrackingUtility::JobType const& j )
{
    WAssert( ds, "" );
    WAssert( ds->getGrid(), "" );
    WAssert( ds->getValueSet(), "" );
    boost::shared_ptr< WGridRegular3D > g = boost::shared_dynamic_cast< WGridRegular3D >( ds->getGrid() );
    int i = g->getVoxelNum( j.first );
    WAssert( i != -1, "" );
    boost::shared_ptr< WValueSet< double > > vs = boost::shared_dynamic_cast< WValueSet< double > >( ds->getValueSet() );
    WAssert( vs, "" );
    if( vs->rawData()[ 4 * i + 3 ] < m_currentMinFA )
    {
        return wmath::WVector3D( 0.0, 0.0, 0.0 );
    }
    else
    {
        wmath::WVector3D v;
        v[ 0 ] = vs->rawData()[ 4 * i + 0 ];
        v[ 1 ] = vs->rawData()[ 4 * i + 1 ];
        v[ 2 ] = vs->rawData()[ 4 * i + 2 ];
        v.normalize();
        if( j.second.norm() == 0 )
        {
            return v;
        }
        else if( v.dotProduct( j.second ) > m_currentMinCos )
        {
            return v;
        }
        else if( j.second.dotProduct( v * -1.0 ) > m_currentMinCos )
        {
            return v * -1.0;
        }
        else
        {
            return wmath::WVector3D( 0.0, 0.0, 0.0 );
        }
    }
}

void WMDeterministicFTMori::fiberVis( FiberType const& f )
{
    if( f.size() >= m_currentMinPoints )
    {
        m_fiberAccu.add( f );
    }
    ++*m_currentProgress;
}

void WMDeterministicFTMori::pointVis( wmath::WVector3D const& )
{
}

WMDeterministicFTMori::EigenOutArrayType const WMDeterministicFTMori::eigenFunc( EigenInArrayType const& input )
{
    EigenOutArrayType a;

    wmath::WTensorSym< 2, 3, double > m;
    m( 0, 0 ) = static_cast< double >( input[ 0 ] );
    m( 0, 1 ) = static_cast< double >( input[ 1 ] );
    m( 0, 2 ) = static_cast< double >( input[ 2 ] );
    m( 1, 1 ) = static_cast< double >( input[ 3 ] );
    m( 1, 2 ) = static_cast< double >( input[ 4 ] );
    m( 2, 2 ) = static_cast< double >( input[ 5 ] );

    std::vector< double > ev( 3 );
    std::vector< wmath::WVector3D > t( 3 );

    // calc eigenvectors
    wmath::jacobiEigenvector3D( m, &ev, &t );

    // find the eigenvector with largest absolute eigenvalue
    int u = 0;
    double h = 0.0;
    for( int n = 0; n < 3; ++n )
    {
        if( fabs( ev[ n ] ) > h )
        {
            h = fabs( ev[ n ] );
            u = n;
        }
    }

    // copy them to the output
    a[ 0 ] = t[ u ][ 0 ];
    a[ 1 ] = t[ u ][ 1 ];
    a[ 2 ] = t[ u ][ 2 ];

    // calc fa
    double d = ev[ 0 ] * ev[ 0 ] + ev[ 1 ] * ev[ 1 ] + ev[ 2 ] * ev[ 2 ];
    if( fabs( d ) == 0.0 )
    {
        a[ 3 ] = 0.0;
    }
    else
    {
        double trace = ( ev[ 0 ] + ev[ 1 ] + ev[ 2 ] ) / 3;
        a[ 3 ] = sqrt( 1.5 * ( ( ev[ 0 ] - trace ) * ( ev[ 0 ] - trace )
                             + ( ev[ 1 ] - trace ) * ( ev[ 1 ] - trace )
                             + ( ev[ 2 ] - trace ) * ( ev[ 2 ] - trace ) ) / d );
    }

    ++*m_currentProgress;

    return a;
}

void WMDeterministicFTMori::resetProgress( std::size_t todo )
{
    if( m_currentProgress )
    {
        m_currentProgress->finish();
    }
    m_currentProgress = boost::shared_ptr< WProgress >( new WProgress( "det Mori FT", todo ) );
    m_progress->addSubProgress( m_currentProgress );
}
