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
#include <iomanip>
#include <list>
#include <string>
#include <utility>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <osg/Geode>
#include <osg/Geometry>

#include "../../common/WColor.h"
#include "../../common/WLogger.h"
#include "../../common/datastructures/WFiber.h"
#include "../../dataHandler/WDataSetFiberVector.h"
#include "../../dataHandler/WSubject.h"
#include "../../graphicsEngine/WGEGeodeUtils.h"
#include "../../graphicsEngine/WGEGeometryUtils.h"
#include "../../graphicsEngine/WGEUtils.h"
#include "../../kernel/WKernel.h"
#include "../../modules/fiberDisplay/WMFiberDisplay.h"
#include "WBresenham.h"
#include "WBresenhamDBL.h"
#include "WMVoxelizer.h"
#include "WRasterAlgorithm.h"
#include "voxelizer.xpm"

WMVoxelizer::WMVoxelizer()
    : WModule(),
      m_fullUpdate( new WCondition() )
{
}

WMVoxelizer::~WMVoxelizer()
{
}

boost::shared_ptr< WModule > WMVoxelizer::factory() const
{
    return boost::shared_ptr< WModule >( new WMVoxelizer() );
}

const char** WMVoxelizer::getXPMIcon() const
{
    return voxelizer_xpm;
}


void WMVoxelizer::moduleMain()
{
    m_moduleState.setResetable();
    m_moduleState.add( m_input->getDataChangedCondition() );  // additional fire-condition: "data changed" flag
    m_moduleState.add( m_fullUpdate );

    ready();

    while ( !m_shutdownFlag() ) // loop until the module container requests the module to quit
    {
        if ( !m_input->getData() ) // ok, the output has not yet sent data
        {
            m_moduleState.wait();
            continue;
        }

        // full update
        if( m_antialiased->changed() ||
            m_drawVoxels->changed() ||
            m_rasterAlgo->changed() ||
            m_voxelsPerUnit->changed() ||
            m_clusters != m_input->getData() )
        {
            m_drawVoxels->get( true );
            m_rasterAlgo->get( true );
            m_antialiased->get( true );
            m_clusters = m_input->getData();
            update();
        }

        if( m_drawfibers->changed() )
        {
            updateFibers();
        }

        if( m_drawCenterLine->changed() )
        {
            updateCenterLine();
        }

        m_moduleState.wait(); // waits for firing of m_moduleState ( dataChanged, shutdown, etc. )
    }
}

void WMVoxelizer::properties()
{
    m_antialiased     = m_properties->addProperty( "Antialiasing", "Enable/Disable antialiased drawing of voxels.", true, m_fullUpdate );
    m_drawfibers      = m_properties->addProperty( "Fiber Tracts", "Enable/Disable drawing of the fibers of a cluster.", true, m_fullUpdate );
    m_drawBoundingBox = m_properties->addProperty( "Bounding BoxEnable Feature", "Enable/Disable drawing of a clusters BoundingBox.", true );
    m_drawCenterLine  = m_properties->addProperty( "CenterLine", "Enable/Disable display of the CenterLine", true );
    m_lighting        = m_properties->addProperty( "Lighting", "Enable/Disable lighting.", true );
    m_drawVoxels      = m_properties->addProperty( "Display Voxels", "Enable/Disable drawing of marked voxels.", true, m_fullUpdate );
    m_rasterAlgo      = m_properties->addProperty( "RasterAlgo", "Specifies the algorithm you may want to use for voxelization.",
                                                    std::string( "WBresenham" ), m_fullUpdate );
    m_voxelsPerUnit   = m_properties->addProperty( "Voxels per Unit", "Specified the number of voxels per unit in the coordinate system. This "
                                                                       "is useful to increase the resolution of the grid", 1, m_fullUpdate );
}

void WMVoxelizer::activate()
{
    if( m_osgNode )
    {
        if( m_active->get() )
        {
            m_osgNode->setNodeMask( 0xFFFFFFFF );
        }
        else
        {
            m_osgNode->setNodeMask( 0x0 );
        }
    }

    WModule::activate();
}

osg::ref_ptr< osg::Geode > WMVoxelizer::genFiberGeode() const
{
    using osg::ref_ptr;
    ref_ptr< osg::Vec3Array > vertices = ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    ref_ptr< osg::Vec4Array > colors = ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    ref_ptr< osg::Geometry > geometry = ref_ptr< osg::Geometry >( new osg::Geometry );

    const WDataSetFiberVector& fibs = *m_clusters->getDataSetReference();

    const std::list< size_t >& fiberIDs = m_clusters->getIndices();
    std::list< size_t >::const_iterator cit = fiberIDs.begin();

    assert( fibs.size() > 0 && "no empty fiber dataset for clusters allowed in WMVoxelizer::createBoundingBox" );
    assert( fibs[0].size() > 0 && "no empty fibers in a cluster allowed in WMVoxelizer::createBoundingBox" );
    assert( fiberIDs.size() > 0 && "no empty clusters allowed in WMVoxelizer::createBoundingBox" );

    for( cit = fiberIDs.begin(); cit != fiberIDs.end(); ++cit )
    {
        const wmath::WFiber& fib = fibs[*cit];
        vertices->push_back( osg::Vec3( fib[0][0], fib[0][1], fib[0][2] ) );
        for( size_t i = 1; i < fib.size(); ++i )
        {
            vertices->push_back( osg::Vec3( fib[i][0], fib[i][1], fib[i][2] ) );
            colors->push_back( wge::osgColor( wge::getRGBAColorFromDirection( fib[i], fib[i-1] ) ) );
        }
        colors->push_back( colors->back() );
        geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, vertices->size() - fib.size(), fib.size() ) );
    }

    geometry->setVertexArray( vertices );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
    osg::ref_ptr< osg::Geode > geode = osg::ref_ptr< osg::Geode >( new osg::Geode );
    geode->addDrawable( geometry.get() );
    return geode;
}

boost::shared_ptr< WGridRegular3D > WMVoxelizer::constructGrid( const std::pair< wmath::WPosition, wmath::WPosition >& bb ) const
{
    int32_t nbVoxelsPerUnit = m_voxelsPerUnit->get( true );

    // TODO(math): implement the snap-to-grid (of the T1 image) feature for fll and bur.
    // TODO(math): remove hardcoded meta grid here.
    // the "+1" in the following three statements is because there are may be some more voxels
    // The first and last voxel are only half sized! hence one more position is needed
    size_t nbPosX = std::ceil( bb.second[0] - bb.first[0] ) + 1;
    size_t nbPosY = std::ceil( bb.second[1] - bb.first[1] ) + 1;
    size_t nbPosZ = std::ceil( bb.second[2] - bb.first[2] ) + 1;

    boost::shared_ptr< WGridRegular3D > grid( new WGridRegular3D( nbVoxelsPerUnit * nbPosX,
                                                                  nbVoxelsPerUnit * nbPosY,
                                                                  nbVoxelsPerUnit * nbPosZ,
                                                                  bb.first, 1, 1, 1 ) );
    return grid;
}

void WMVoxelizer::updateFibers()
{
    assert( m_osgNode );
    // TODO(math): instead of recompute the fiber geode, hide/unhide would be cool, but when data has changed recomputation is necessary
    if( m_drawfibers->get( true ) )
    {
        m_fiberGeode = genFiberGeode();
        m_osgNode->insert( m_fiberGeode );
    }
    else
    {
        m_osgNode->remove( m_fiberGeode );
    }
}

void WMVoxelizer::updateCenterLine()
{
    assert( m_osgNode );
    if( m_drawCenterLine->get( true ) )
    {
        boost::shared_ptr< wmath::WFiber > centerLine = m_clusters->getCenterLine();
        if( centerLine )
        {
            m_centerLineGeode = wge::generateLineStripGeode( *centerLine, 2.f );
        }
        else
        {
            warnLog() << "CenterLine update on non existing CenterLine (null)";
            m_centerLineGeode = osg::ref_ptr< osg::Geode >( new osg::Geode() );
        }
        m_osgNode->insert( m_centerLineGeode );
    }
    else
    {
        m_osgNode->remove( m_centerLineGeode );
    }
}

void WMVoxelizer::update()
{
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_osgNode );
    m_osgNode = osg::ref_ptr< WGEGroupNode >( new WGEGroupNode );
    m_osgNode->addUpdateCallback( new OSGCB_HideUnhideBB( this ) );
    m_osgNode->addUpdateCallback( new OSGCB_ChangeLighting( this ) );

    updateFibers();

    updateCenterLine();

    std::pair< wmath::WPosition, wmath::WPosition > bb = createBoundingBox( *m_clusters );

    m_boundingBoxGeode = wge::generateBoundingBoxGeode( bb.first, bb.second, WColor( 0.3, 0.3, 0.3, 1 ) );
    m_osgNode->insert( m_boundingBoxGeode );

    boost::shared_ptr< WGridRegular3D > grid = constructGrid( bb );

    debugLog() << "Created grid of size: " << grid->size();

    boost::shared_ptr< WRasterAlgorithm > rasterAlgo;
    if( m_rasterAlgo->get() == std::string( "WBresenham" ) )
    {
        rasterAlgo = boost::shared_ptr< WBresenham >( new WBresenham( grid, m_antialiased ) );
    }
    else if( m_rasterAlgo->get() == std::string( "WBresenhamDBL" ) )
    {
        rasterAlgo =  boost::shared_ptr< WBresenhamDBL >( new WBresenhamDBL( grid, m_antialiased ) );
    }
    else
    {
        errorLog() << "Invalid rasterization algorithm: " << m_rasterAlgo->get();
        m_rasterAlgo->set( std::string( "WBresenham" ) );
        rasterAlgo = boost::shared_ptr< WBresenham >( new WBresenham( grid, m_antialiased ) );
    }
    debugLog() << "Using: " << m_rasterAlgo->get() << " as rasterization Algo.";
    raster( rasterAlgo );

    // update both outputs
    boost::shared_ptr< WDataSetSingle > outputDataSet = rasterAlgo->generateDataSet();
    m_output->updateData( outputDataSet );
    boost::shared_ptr< WDataSetSingle > outputDataSetDir = rasterAlgo->generateVectorDataSet();
    m_dirOutput->updateData( outputDataSetDir );

    if( m_drawVoxels->get() )
    {
        m_voxelGeode = genDataSetGeode( outputDataSet );
        m_osgNode->insert( m_voxelGeode );
    }

    m_osgNode->setNodeMask( m_active->get() ? 0xFFFFFFFF : 0x0 );
    m_osgNode->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_osgNode );
}

void WMVoxelizer::raster( boost::shared_ptr< WRasterAlgorithm > algo ) const
{
    const WDataSetFiberVector& fibs = *m_clusters->getDataSetReference();
    const std::list< size_t >& fiberIDs = m_clusters->getIndices();
    std::list< size_t >::const_iterator cit = fiberIDs.begin();

    debugLog() << "Cluster indices to voxelize: " << fiberIDs;
    debugLog() << "Using: " << m_clusters->getDataSetReference() << " as fiber dataset";

    assert( fibs.size() > 0 && "no empty fiber dataset for clusters allowed in WMVoxelizer::createBoundingBox" );
    assert( fibs[0].size() > 0 && "no empty fibers in a cluster allowed in WMVoxelizer::createBoundingBox" );
    assert( fiberIDs.size() > 0 && "no empty clusters allowed in WMVoxelizer::createBoundingBox" );

    for( cit = fiberIDs.begin(); cit != fiberIDs.end(); ++cit )
    {
        algo->raster( fibs.at( *cit ) );
    }
    // TODO(math): This is just a line for testing purposes
//    wmath::WLine l;
//    l.push_back( wmath::WPosition( 73, 38, 29 ) );
//    l.push_back( wmath::WPosition( 120, 150, 130 ) );
//    algo->raster( l );
}

void WMVoxelizer::connectors()
{
    typedef WModuleInputData< const WFiberCluster > InputType; // just an alias
    m_input = boost::shared_ptr< InputType >( new InputType( shared_from_this(), "voxelInput", "A loaded dataset with grid." ) );
    addConnector( m_input );

    typedef WModuleOutputData< WDataSetSingle > OutputType; // just an alias
    m_output = boost::shared_ptr< OutputType >( new OutputType( shared_from_this(), "voxelOutput", "The voxelized data set." ) );
    addConnector( m_output );

    m_dirOutput = boost::shared_ptr< OutputType >( new OutputType( shared_from_this(), "voxelDirectionOutput", "The voxelized direction dataset." ) );
    addConnector( m_dirOutput );

    WModule::connectors();  // call WModules initialization
}


std::pair< wmath::WPosition, wmath::WPosition > WMVoxelizer::createBoundingBox( const WFiberCluster& cluster ) const
{
    const WDataSetFiberVector& fibs = *cluster.getDataSetReference();

    const std::list< size_t >& fiberIDs = cluster.getIndices();
    std::list< size_t >::const_iterator cit = fiberIDs.begin();

    assert( fibs.size() > 0 && "no empty fiber dataset for clusters allowed in WMVoxelizer::createBoundingBox" );
    assert( fibs[0].size() > 0 && "no empty fibers in a cluster allowed in WMVoxelizer::createBoundingBox" );
    assert( fiberIDs.size() > 0 && "no empty clusters allowed in WMVoxelizer::createBoundingBox" );

    wmath::WPosition fll = fibs[0][0]; // front lower left corner ( initialize with first WPosition of first fiber )
    wmath::WPosition bur = fibs[0][0]; // back upper right corner ( initialize with first WPosition of first fiber )
    for( cit = fiberIDs.begin(); cit != fiberIDs.end(); ++cit )
    {
        const wmath::WFiber& fiber = fibs[*cit];
        for( size_t i = 0; i < fiber.size(); ++i )
        {
            for( int x = 0; x < 3; ++x )
            {
                fll[x] = std::min( fiber[i][x], fll[x] );
                bur[x] = std::max( fiber[i][x], bur[x] );
            }
        }
    }
    return std::make_pair( fll, bur );
}

osg::ref_ptr< osg::Geode > WMVoxelizer::genDataSetGeode( boost::shared_ptr< WDataSetSingle > dataset ) const
{
    using osg::ref_ptr;
    ref_ptr< osg::Vec3Array > vertices = ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    ref_ptr< osg::Vec4Array > colors = ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    ref_ptr< osg::Geometry > geometry = ref_ptr< osg::Geometry >( new osg::Geometry );
    ref_ptr< osg::Vec3Array > normals = ref_ptr< osg::Vec3Array >( new osg::Vec3Array );

    // cycle through all positions in the dataSet
    boost::shared_ptr< WValueSet< double > > valueset = boost::shared_dynamic_cast< WValueSet< double > >( dataset->getValueSet() );
    assert( valueset != 0 );
    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( dataset->getGrid() );
    assert( grid != 0 );
    const std::vector< double >& values = *valueset->rawDataVectorPointer();
    for( size_t i = 0; i < values.size(); ++i )
    {
        if( values[i] != 0.0 )
        {
            wmath::WPosition pos = grid->getPosition( i );
            boost::shared_ptr< std::vector< wmath::WPosition > > voxelCornerVertices = grid->getVoxelVertices( pos, 0.01 );
            osg::ref_ptr< osg::Vec3Array > ver = wge::generateCuboidQuads( *voxelCornerVertices );
            vertices->insert( vertices->end(), ver->begin(), ver->end() );
            osg::ref_ptr< osg::Vec3Array > nor = wge::generateCuboidQuadNormals( *voxelCornerVertices );
            normals->insert( normals->end(), nor->begin(), nor->end() );
            geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUADS, vertices->size() - ver->size(), ver->size() ) );
            for( size_t j = 0; j < ver->size(); ++j )
            {
                double transparency = ( values[i] <= 1.0 ? values[i] : 1.0 );
                colors->push_back( wge::osgColor( WColor( 1, 0, 0, transparency ) ) );
            }
        }
    }

    geometry->setVertexArray( vertices );
    colors->push_back( wge::osgColor( WColor( 1, 0, 0, 0.1 ) ) );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
    geometry->setNormalArray( normals );
    geometry->setNormalBinding( osg::Geometry::BIND_PER_PRIMITIVE );
    osg::ref_ptr< osg::Geode > geode = osg::ref_ptr< osg::Geode >( new osg::Geode );
    geode->addDrawable( geometry );
    return geode;
}

void WMVoxelizer::OSGCB_HideUnhideBB::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    if( m_module->m_boundingBoxGeode )
    {
        if ( m_module->m_drawBoundingBox->get() )
        {
            m_module->m_boundingBoxGeode->setNodeMask( 0xFFFFFFFF );
        }
        else
        {
            m_module->m_boundingBoxGeode->setNodeMask( 0x0 );
        }
    }
    traverse( node, nv );
}

void WMVoxelizer::OSGCB_ChangeLighting::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    if ( m_module->m_lighting->get() )
    {
        m_module->m_osgNode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::ON );
    }
    else
    {
        m_module->m_osgNode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    }
    traverse( node, nv );
}
