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

#include <algorithm>
#include <functional>
#include <map>
#include <numeric>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <osg/Geode>
#include <osg/LightModel>

#include "../../common/datastructures/WTriangleMesh.h"
#include "../../common/math/WPlane.h"
#include "../../common/WColor.h"
#include "../../graphicsEngine/WGEGeodeUtils.h"
#include "../../graphicsEngine/WGEGeometryUtils.h"
#include "../../graphicsEngine/WGEUtils.h"
#include "../../kernel/WKernel.h"
#include "WMClusterSlicer.h"

WMClusterSlicer::WMClusterSlicer()
    : WModule(),
      m_rootNode( osg::ref_ptr< WGEGroupNode >( new WGEGroupNode() ) ),
      m_fullUpdate( new WCondition ),
      m_maxMean( 0.0 ),
      m_minMean( 0.0 )
{
}

WMClusterSlicer::~WMClusterSlicer()
{
}

boost::shared_ptr< WModule > WMClusterSlicer::factory() const
{
    return boost::shared_ptr< WModule >( new WMClusterSlicer() );
}

const std::string WMClusterSlicer::getName() const
{
    return "Cluster Slicer";
}

const std::string WMClusterSlicer::getDescription() const
{
    return "Slices a cluster";
}

void WMClusterSlicer::connectors()
{
    m_fiberClusterInput = boost::shared_ptr< InputClusterType >( new InputClusterType( shared_from_this(), "cluster", "A cluster of fibers" ) );
    addConnector( m_fiberClusterInput );

    m_clusterDataSetInput = boost::shared_ptr< InputDataSetType >( new InputDataSetType( shared_from_this(), "clusterDS", "DataSet from cluster" ) );
    addConnector( m_clusterDataSetInput );

    m_paramDataSetInput = boost::shared_ptr< InputDataSetType >( new InputDataSetType( shared_from_this(), "paramDS", "DataSet of the parameters" ) );
    addConnector( m_paramDataSetInput );

    m_triangleMeshInput = boost::shared_ptr< InputMeshType >( new InputMeshType( shared_from_this(), "meshIN", "TrianglMesh" ) );
    addConnector( m_triangleMeshInput );

    m_colorMapOutput = boost::shared_ptr< OutputColorMapType >( new OutputColorMapType( shared_from_this(), "colorMap", "VertexID and colors" ) );
    addConnector( m_colorMapOutput );

    m_meshOutput = boost::shared_ptr< OutputMeshType >( new OutputMeshType( shared_from_this(), "meshOUT", "The Mesh to forward it for rendering" ) );
    addConnector( m_meshOutput );

    WModule::connectors();
}

void WMClusterSlicer::properties()
{
    m_drawISOVoxels     = m_properties2->addProperty( "Show/Hide ISO Voxels", "Show/Hide voxels withing a given ISOSurface.", true );
    m_drawSlices        = m_properties2->addProperty( "Show/Hide Slices", "Show/Hide slices along center line", false );
    m_isoValue          = m_properties2->addProperty( "Iso Value", "", 0.01 );
    m_meanSelector      = m_properties2->addProperty( "Mean Type", "Selects the mean type, must be on of:"
                                                                   " 0==arithmetic, 1==geometric, 2==median", 2, m_fullUpdate );
    m_planeNumX         = m_properties2->addProperty( "Planes #X-SamplePoints", "#samplePoints in first direction", 40, m_fullUpdate );
    m_planeNumY         = m_properties2->addProperty( "Planes #Y-SamplePoints", "#samplePoints in second direction", 40, m_fullUpdate );
    m_planeStepWidth    = m_properties2->addProperty( "Planes Step Width", "Distance between sample points", 0.5, m_fullUpdate );
    m_centerLineScale   = m_properties2->addProperty( "#Planes", "Scales the center line to have more or less samples", 1.0, m_fullUpdate );
    m_selectBiggestComponentOnly = m_properties2->addProperty( "Biggest Component Only",
       "If true, first the mesh is decomposed into its components (expensive!) and the biggest will be drawn", false );

    m_meanSelector->setMin( 0 );
    m_meanSelector->setMax( 2 );
    m_planeNumX->setMin( 1 );
    m_planeNumY->setMin( 1 );
    m_planeStepWidth->setMin( 0.0 );
}

void WMClusterSlicer::moduleMain()
{
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );

    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_paramDataSetInput->getDataChangedCondition() );
    m_moduleState.add( m_triangleMeshInput->getDataChangedCondition() );
    m_moduleState.add( m_drawSlices->getCondition() );
    m_moduleState.add( m_drawISOVoxels->getCondition() );
    m_moduleState.add( m_fullUpdate );
    m_moduleState.add( m_selectBiggestComponentOnly->getCondition() );

    ready();

    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting...";
        m_moduleState.wait();

        if( m_shutdownFlag() )
        {
            break;
        }

        boost::shared_ptr< WDataSetSingle > newClusterDS = m_clusterDataSetInput->getData();
        boost::shared_ptr< WFiberCluster >  newCluster   = m_fiberClusterInput->getData();
        boost::shared_ptr< WDataSetSingle > newParamDS   = m_paramDataSetInput->getData();
        boost::shared_ptr< WTriangleMesh >  newMesh      = m_triangleMeshInput->getData();
        bool meshChanged = ( m_mesh != newMesh );
        bool paramDSChanged = ( m_paramDS != newParamDS );
        bool clusterChanged = ( m_cluster != newCluster );
        bool clusterDSChanged = ( m_clusterDS != newClusterDS );
        bool dataChanged = clusterDSChanged || clusterChanged || paramDSChanged || meshChanged;

        m_clusterDS = newClusterDS;
        m_cluster = newCluster;
        m_paramDS = newParamDS;
        m_mesh = newMesh;

        if( !( m_clusterDS.get() && m_cluster.get() && m_paramDS.get() && m_mesh.get() ) )
        {
            debugLog() << "Invalid data. Waiting for data change again.";
            continue;
        }

        if( dataChanged )
        {
            debugLog() << "Data changed...";
            if( meshChanged )
            {
                if( clusterDSChanged )
                {
                    debugLog() << "Building Join Tree";
                    m_joinTree = boost::shared_ptr< WJoinContourTree >( new WJoinContourTree( m_clusterDS ) );
                    m_joinTree->buildJoinTree();
                    debugLog() << "Finished building Join Tree";
                }
                // when the mesh has changed the isoValue must have had changed too
                m_isoVoxels = m_joinTree->getVolumeVoxelsEnclosedByISOSurface( m_isoValue->get() );
                if( m_selectBiggestComponentOnly->get( true ) )
                {
                    debugLog() << "Start mesh decomposition";
                    m_components = tm_utils::componentDecomposition( *m_mesh );
                    debugLog() << "Decomposing mesh done";
                }
            }
        }

        if( meshChanged || paramDSChanged || m_meanSelector->changed() || m_planeNumX->changed()
                        || m_planeNumY->changed() || m_planeStepWidth->changed() || m_centerLineScale->changed() )
        {
            debugLog() << "Performing full update";
            generateSlices();
            sliceAndColorMesh( m_mesh );
            updateDisplay( true ); // force display update here (erasing invalid planes)
            debugLog() << "Full update done.";
        }
        else if( m_drawSlices->changed() || m_drawISOVoxels->changed() )
        {
            updateDisplay();
        }
        else if( m_selectBiggestComponentOnly->changed() )
        {
            sliceAndColorMesh( m_mesh );
        }
    }

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
}

wmath::WValue< double > WMClusterSlicer::meanParameter( boost::shared_ptr< std::set< wmath::WPosition > > samplePoints ) const
{
    std::vector< double > samples;
    samples.reserve( samplePoints->size() );

    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_clusterDS->getGrid() );
    assert( grid != 0 );

    for( std::set< wmath::WPosition >::iterator pos = samplePoints->begin(); pos != samplePoints->end(); )
    {
        // ATTENTION: We don't interpolate in m_clusterDS since it might be the wrong component!!
        int id = grid->getVoxelNum( *pos );
        if( id >= 0 ) // check if the position is inside of the main component (biggest volume)
        {
            if( m_isoVoxels->count( static_cast< size_t >( id ) ) == 1 ) // check if its in main component
            {
                bool inParamGrid = false;
                double value = m_paramDS->interpolate( *pos, &inParamGrid );
                if( inParamGrid ) // check if its in paramDS
                {
                    samples.push_back( value );
                    ++pos;
                    continue;
                }
            }
        }
        samplePoints->erase( pos++ ); // erase in case the pos is not in main component or in paramDS or in clusterDS
    }
    double arithmeticMean = std::accumulate( samples.begin(), samples.end(), 0.0 );
    arithmeticMean = arithmeticMean / ( samples.empty() ? 1.0 : samples.size() );

    std::nth_element( samples.begin(), samples.begin() + samples.size() / 2, samples.end() );
    double median = ( samples.empty() ? 0.0 : samples[ samples.size() / 2 ] );

    // discard first all elements <= 0.0 since then the geometric mean does not make any sense

    std::vector< double >::iterator newEnd = std::remove_if( samples.begin(), samples.end(), std::bind2nd( std::less_equal< double >(), 0.0 ) );
    double geometricMean = std::accumulate( samples.begin(), newEnd, 1.0, std::multiplies< double >() );
    geometricMean = std::pow( geometricMean, ( samples.empty() ? 0.0 : 1.0 / samples.size() ) );

    wmath::WValue< double > result( 3 );
    result[0] = arithmeticMean;
    result[1] = geometricMean;
    result[2] = median;
    return result;
}

void WMClusterSlicer::generateSlices()
{
    debugLog() << "Generating Slices";
    wmath::WFiber centerLine( *m_cluster->getCenterLine() ); // copy the centerline
    if( centerLine.empty() )
    {
        errorLog() << "CenterLine of the bundle is empty => no slices are drawn";
        return;
    }
    centerLine.resample( static_cast< size_t >( m_centerLineScale->get( true ) * centerLine.size() ) );

    m_slices = boost::shared_ptr< std::vector< std::pair< double, WPlane > > >( new std::vector< std::pair< double, WPlane > > );
    m_maxMean = wlimits::MIN_DOUBLE;
    m_minMean = wlimits::MAX_DOUBLE;
    const int nbX = m_planeNumX->get( true );
    const int nbY = m_planeNumY->get( true );
    const double stepWidth = m_planeStepWidth->get( true );
    const int meanType = m_meanSelector->get( true );
    m_rootNode->remove( m_samplePointsGeode );
    m_samplePointsGeode = osg::ref_ptr< WGEGroupNode >( new WGEGroupNode ); // discard old geode
    for( size_t i = 1; i < centerLine.size(); ++i )
    {
        wmath::WVector3D tangent = centerLine[i] - centerLine[i-1];
        WPlane p( tangent, centerLine[i-1] );

        boost::shared_ptr< std::set< wmath::WPosition > > samplePoints = p.samplePoints( stepWidth, nbX, nbY  );
        double mean = meanParameter( samplePoints )[ meanType ];
        if( mean > m_maxMean )
        {
            m_maxMean = mean;
        }
        if( mean < m_minMean )
        {
            m_minMean = mean;
        }
        m_slices->push_back( std::make_pair( mean, p ) );
        if( m_drawSlices->get( true ) )
        {
            m_samplePointsGeode->insert( wge::genPointBlobs( samplePoints, 0.1 ) );
        }
    }
}

/**
 * Compares two WTrianglesMeshes on their size of vertices. This is private here since it makes sense only to this module ATM.
 */
struct WMeshSizeComp
{
    /**
     * Comparator on num vertex of two WTriangleMeshes
     *
     * \param m First Mesh
     * \param n Second Mesh
     *
     * \return True if and only if the first Mesh has less vertices as the second mesh.
     */
    bool operator()( const boost::shared_ptr< WTriangleMesh >& m, const boost::shared_ptr< WTriangleMesh >& n ) const
    {
        return m->getNumVertices() < n->getNumVertices();
    }
};

void WMClusterSlicer::sliceAndColorMesh( boost::shared_ptr< WTriangleMesh > mesh )
{
    debugLog() << "Selecting mesh component...";
    boost::shared_ptr< WTriangleMesh > renderMesh = mesh;
    assert( renderMesh.get() );
    if( m_selectBiggestComponentOnly->get( true ) )
    {
        if( !m_components.get() )
        {
            debugLog() << "Start mesh decomposition";
            m_components = tm_utils::componentDecomposition( *renderMesh );
            debugLog() << "Decomposing mesh done";
        }
        renderMesh = *std::max_element( m_components->begin(), m_components->end(), WMeshSizeComp() );
    }

    debugLog() << "Mesh selected";

    if( renderMesh != m_meshOutput->getData() )
    {
        m_meshOutput->updateData( renderMesh );
    }

    debugLog() << "Building mesh color map...";
    m_colorMap = boost::shared_ptr< WColoredVertices >( new WColoredVertices );
    std::map< size_t, std::pair< double, int > > cm;

    for( std::vector< std::pair< double, WPlane > >::const_iterator slice = m_slices->begin(); slice != m_slices->end(); ++slice )
    {
        boost::shared_ptr< std::set< size_t > > coloredVertices = tm_utils::intersection( *renderMesh, slice->second );
        double scaledMean = ( m_maxMean == m_minMean ? 0.0 : ( slice->first - m_minMean ) / ( m_maxMean - m_minMean ) );
        for( std::set< size_t >::const_iterator coloredVertex = coloredVertices->begin(); coloredVertex != coloredVertices->end(); ++coloredVertex )
        {
            if( cm.find( *coloredVertex ) != cm.end() )
            {
                cm[ *coloredVertex ].first += scaledMean;
                cm[ *coloredVertex ].second++;
            }
            else
            {
                cm[ *coloredVertex ].first  = scaledMean;
                cm[ *coloredVertex ].second = 1;
            }
        }
    }

    std::map< size_t, WColor > cmData;
    for( std::map< size_t, std::pair< double, int > >::const_iterator vertexColor = cm.begin(); vertexColor != cm.end(); ++vertexColor )
    {
        cmData[ vertexColor->first ] = WColor( 0.0, vertexColor->second.first / vertexColor->second.second, 1.0 );
    }

    m_colorMap->setData( cmData );
    debugLog() << "Done with color map building";
    m_colorMapOutput->updateData( m_colorMap );
}

void WMClusterSlicer::updateDisplay( bool force )
{
    debugLog() << "Forced updating display: " << force;
    if( m_drawISOVoxels->changed() || force )
    {
        m_rootNode->remove( m_isoVoxelGeode );
        m_isoVoxelGeode = osg::ref_ptr< osg::Geode >( new osg::Geode() ); // discard old geode
        if( m_drawISOVoxels->get( true ) )
        {
            assert( m_isoVoxels && "JoinTree cannot be valid since there is no valid m_clusterDS." );
            m_isoVoxelGeode = generateISOVoxelGeode();
            m_rootNode->insert( m_isoVoxelGeode );
        }
    }

    if( m_drawSlices->changed() || force )
    {
        m_rootNode->remove( m_sliceGeode );
        m_sliceGeode = osg::ref_ptr< WGEGroupNode >( new WGEGroupNode ); // discard old geode
        if( m_drawSlices->get( true ) ) // regenerate
        {
            const double width = m_planeNumX->get() * m_planeStepWidth->get();
            const double height = m_planeNumY->get() * m_planeStepWidth->get();
            for( std::vector< std::pair< double, WPlane > >::const_iterator cit = m_slices->begin(); cit != m_slices->end(); ++cit )
            {
                double scaledMean = 0.0;
                if( m_maxMean == m_minMean )
                {
                    scaledMean = 0;
                }
                else
                {
                    scaledMean = ( cit->first - m_minMean ) / ( m_maxMean - m_minMean );
                }
                WColor color( scaledMean, scaledMean, 1 );
                m_sliceGeode->insert( wge::genFinitePlane( width, height, cit->second, color, true ) );
            }
            m_rootNode->insert( m_sliceGeode );
        }
    }
}

osg::ref_ptr< osg::Geode > WMClusterSlicer::generateISOVoxelGeode() const
{
    boost::shared_ptr< std::set< wmath::WPosition > > points( new std::set< wmath::WPosition > );
    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_clusterDS->getGrid() );
    assert( grid != 0 );
    for( std::set< size_t >::const_iterator cit = m_isoVoxels->begin(); cit != m_isoVoxels->end(); ++cit )
    {
        points->insert( grid->getPosition( *cit ) );
    }
    return wge::genPointBlobs< std::set< wmath::WPosition > >( points, grid->getOffsetX() );
}

void WMClusterSlicer::activate()
{
    if( m_rootNode )
    {
        if( m_active->get() )
        {
            m_rootNode->setNodeMask( 0xFFFFFFFF );
        }
        else
        {
            m_rootNode->setNodeMask( 0x0 );
        }
    }

    WModule::activate();
}
