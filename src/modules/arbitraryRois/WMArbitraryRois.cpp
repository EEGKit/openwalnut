//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

#include <string>
#include <vector>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Material>
#include <osg/StateSet>
#include <osg/StateAttribute>
#include <osg/PolygonMode>
#include <osg/LightModel>

#include "../../common/WAssert.h"
#include "../../dataHandler/WDataSetScalar.h"
#include "../../kernel/WKernel.h"

#include "../../graphicsEngine/WROIArbitrary.h"
#include "../../graphicsEngine/WROIBox.h"

#include "../../graphicsEngine/algorithms/WMarchingCubesAlgorithm.h"

#include "WMArbitraryRois.h"
#include "WMArbitraryRois.xpm"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMArbitraryRois )

WMArbitraryRois::WMArbitraryRois():
    WModule(),
    m_textureChanged( true ),
    m_recompute( boost::shared_ptr< WCondition >( new WCondition() ) ),
    m_dataSet(),
    m_moduleNode( new WGEGroupNode() ),
    m_showSelector( true )
{
}

WMArbitraryRois::~WMArbitraryRois()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMArbitraryRois::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMArbitraryRois() );
}

const char** WMArbitraryRois::getXPMIcon() const
{
    return arbitraryROI_xpm;
}

const std::string WMArbitraryRois::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "Arbitrary Rois";
}

const std::string WMArbitraryRois::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    // See "src/modules/template/" for an extensively documented example.
    return "Someone should add some documentation here. ";
}

void WMArbitraryRois::connectors()
{
    // initialize connectors
    m_input = boost::shared_ptr< WModuleInputData < WDataSetScalar > >(
        new WModuleInputData< WDataSetScalar >( shared_from_this(), "in", "Dataset to cut roi from." ) );
    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_input );

    // call WModules initialization
    WModule::connectors();
}

void WMArbitraryRois::properties()
{
    m_aTrigger = m_properties->addProperty( "Create", "Create a ROI", WPVBaseTypes::PV_TRIGGER_READY  );
    m_bTrigger = m_properties->addProperty( "Finalize", "Finalize and add to ROI manager", WPVBaseTypes::PV_TRIGGER_READY  );
    m_threshold = m_properties->addProperty( "Threshold", "", 0. );
    m_surfaceColor = m_properties->addProperty( "Surface color", "", WColor( 1.0, 0.3, 0.3, 1.0 ) );

    WModule::properties();
}

void WMArbitraryRois::moduleMain()
{
    // use the m_input "data changed" flag
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_recompute );

    // signal ready state
    ready();

    // loop until the module container requests the module to quit
    while ( !m_shutdownFlag() )
    {
        m_moduleState.wait(); // waits for firing of m_moduleState ( dataChanged, shutdown, etc. )

        if ( m_shutdownFlag() )
        {
            break;
        }

        if( m_dataSet != m_input->getData() )
        {
            // acquire data from the input connector
            m_dataSet = m_input->getData();

            m_threshold->setMin( m_dataSet->getMin() );
            m_threshold->setMax( m_dataSet->getMax() );
            m_threshold->set( 0. );

            initSelectionRoi();
        }
        // this waits for m_moduleState to fire. By default, this is only the m_shutdownFlag condition.
        // NOTE: you can add your own conditions to m_moduleState using m_moduleState.add( ... )

        if ( m_aTrigger->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            debugLog() << "Creating cut dataset.";
            m_showSelector = true;
            createCutDataset();
            renderMesh();
            m_aTrigger->set( WPVBaseTypes::PV_TRIGGER_READY, false );
        }

        if ( m_bTrigger->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            debugLog() << "Creating cut dataset.";
            m_showSelector = false;
            createCutDataset();
            renderMesh();
            finalizeRoi();
            m_bTrigger->set( WPVBaseTypes::PV_TRIGGER_READY, false );
        }

        //m_moduleState.wait();
    }
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_moduleNode );
}

void WMArbitraryRois::initSelectionRoi()
{
    wmath::WPosition crossHairPos = WKernel::getRunningKernel()->getSelectionManager()->getCrosshair()->getPosition();
    wmath::WPosition minROIPos = crossHairPos - wmath::WPosition( 10., 10., 10. );
    wmath::WPosition maxROIPos = crossHairPos + wmath::WPosition( 10., 10., 10. );

    m_selectionRoi = osg::ref_ptr< WROIBox >( new WROIBox( minROIPos, maxROIPos ) );
    m_selectionRoi->setColor( osg::Vec4( 0., 0., 1.0, 0.4 ) );
    createCutDataset();
}

void WMArbitraryRois::createCutDataset()
{
    if( !m_active->get() )
    {
        return;
    }

    boost::shared_ptr< WValueSetBase > newValueSet;

    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() );

    size_t order = ( *m_dataSet ).getValueSet()->order();
    size_t vDim = ( *m_dataSet ).getValueSet()->dimension();

    float threshold = m_threshold->get();
    std::vector< float > data;

    switch( ( *m_dataSet ).getValueSet()->getDataType() )
    {
        case W_DT_UNSIGNED_CHAR:
        {
            boost::shared_ptr< WValueSet< unsigned char > > vals;
            vals =  boost::shared_dynamic_cast< WValueSet< unsigned char > >( ( *m_dataSet ).getValueSet() );
            WAssert( vals, "Data type and data type indicator must fit." );
            data = cutArea( ( *m_dataSet ).getGrid(), vals );
            break;
        }
        case W_DT_INT16:
        {
            boost::shared_ptr< WValueSet< int16_t > > vals;
            vals =  boost::shared_dynamic_cast< WValueSet< int16_t > >( ( *m_dataSet ).getValueSet() );
            WAssert( vals, "Data type and data type indicator must fit." );
            data = cutArea( ( *m_dataSet ).getGrid(), vals );
            break;
        }
        case W_DT_SIGNED_INT:
        {
            boost::shared_ptr< WValueSet< int32_t > > vals;
            vals =  boost::shared_dynamic_cast< WValueSet< int32_t > >( ( *m_dataSet ).getValueSet() );
            WAssert( vals, "Data type and data type indicator must fit." );
            cutArea( ( *m_dataSet ).getGrid(), vals );
            data = cutArea( ( *m_dataSet ).getGrid(), vals );
            break;
        }
        case W_DT_FLOAT:
        {
            boost::shared_ptr< WValueSet< float > > vals;
            vals =  boost::shared_dynamic_cast< WValueSet< float > >( ( *m_dataSet ).getValueSet() );
            WAssert( vals, "Data type and data type indicator must fit." );
            data = cutArea( ( *m_dataSet ).getGrid(), vals );
            break;
        }
        case W_DT_DOUBLE:
        {
            boost::shared_ptr< WValueSet< double > > vals;
            vals =  boost::shared_dynamic_cast< WValueSet< double > >( ( *m_dataSet ).getValueSet() );
            WAssert( vals, "Data type and data type indicator must fit." );
            data = cutArea( ( *m_dataSet ).getGrid(), vals );
            break;
        }
        default:
            WAssert( false, "Unknown data type in MarchingCubes module" );
    }
    m_newValueSet = boost::shared_ptr< WValueSet< float > >( new WValueSet< float >( order, vDim, data, W_DT_FLOAT ) );
    WMarchingCubesAlgorithm mcAlgo;
    m_triMesh = mcAlgo.generateSurface( grid->getNbCoordsX(), grid->getNbCoordsY(), grid->getNbCoordsZ(),
                                        grid->getTransformationMatrix(),
                                        m_newValueSet->rawDataVectorPointer(),
                                        threshold,
                                        m_progress );
}

template< typename T > std::vector< float > WMArbitraryRois::cutArea( boost::shared_ptr< WGrid > inGrid, boost::shared_ptr< WValueSet< T > > vals )
{
    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( inGrid );

    size_t nx = grid->getNbCoordsX();
    size_t ny = grid->getNbCoordsY();
    size_t nz = grid->getNbCoordsZ();

    double dx = grid->getOffsetX();
    double dy = grid->getOffsetY();
    double dz = grid->getOffsetZ();

    size_t xMin = static_cast<size_t>( m_selectionRoi->getMinPos()[0] / dx );
    size_t yMin = static_cast<size_t>( m_selectionRoi->getMinPos()[1] / dy );
    size_t zMin = static_cast<size_t>( m_selectionRoi->getMinPos()[2] / dz );
    size_t xMax = static_cast<size_t>( m_selectionRoi->getMaxPos()[0] / dx );
    size_t yMax = static_cast<size_t>( m_selectionRoi->getMaxPos()[1] / dy );
    size_t zMax = static_cast<size_t>( m_selectionRoi->getMaxPos()[2] / dz );

    std::vector< float >newVals( nx * ny * nz, 0 );

    for ( size_t z = 0; z < nz; ++z )
    {
        for ( size_t y = 0 ; y < ny; ++y )
        {
            for ( size_t x = 0 ; x < nx; ++x )
            {
                 if ( ( x > xMin ) && ( x < xMax ) && ( y > yMin ) && ( y < yMax ) && ( z > zMin ) && ( z < zMax ) )
                 {
                     newVals[ x + nx * y + nx * ny * z ] = static_cast<float>( vals->getScalar( x + nx * y + nx * ny * z ) );
                 }
            }
        }
    }
    return newVals;
}

void WMArbitraryRois::renderMesh()
{
    m_moduleNode->remove( m_outputGeode );

    if ( m_showSelector )
    {
        osg::Geometry* surfaceGeometry = new osg::Geometry();
        m_outputGeode = osg::ref_ptr< osg::Geode >( new osg::Geode );

        m_outputGeode->setName( "roi" );

        surfaceGeometry->setVertexArray( m_triMesh->getVertexArray() );

        // ------------------------------------------------
        // normals
        surfaceGeometry->setNormalArray( m_triMesh->getVertexNormalArray() );
        surfaceGeometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );

        // ------------------------------------------------
        // colors
        osg::Vec4Array* colors = new osg::Vec4Array;
        colors->push_back( osg::Vec4( 0.2f, 1.0f, 0.2f, 1.0f ) );
        surfaceGeometry->setColorArray( colors );
        surfaceGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );

        osg::DrawElementsUInt* surfaceElement = new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );

        std::vector< size_t >tris = m_triMesh->getTriangles();
        surfaceElement->reserve( tris.size() );

        for( unsigned int vertId = 0; vertId < tris.size(); ++vertId )
        {
            surfaceElement->push_back( tris[vertId] );
        }
        surfaceGeometry->addPrimitiveSet( surfaceElement );
        m_outputGeode->addDrawable( surfaceGeometry );

        osg::StateSet* state = m_outputGeode->getOrCreateStateSet();
        osg::ref_ptr<osg::LightModel> lightModel = new osg::LightModel();
        lightModel->setTwoSided( true );
        state->setAttributeAndModes( lightModel.get(), osg::StateAttribute::ON );
        state->setMode(  GL_BLEND, osg::StateAttribute::ON );

    //    {
    //        osg::ref_ptr< osg::Material > material = new osg::Material();
    //        material->setDiffuse(   osg::Material::FRONT, osg::Vec4( 1.0, 1.0, 1.0, 1.0 ) );
    //        material->setSpecular(  osg::Material::FRONT, osg::Vec4( 0.0, 0.0, 0.0, 1.0 ) );
    //        material->setAmbient(   osg::Material::FRONT, osg::Vec4( 0.1, 0.1, 0.1, 1.0 ) );
    //        material->setEmission(  osg::Material::FRONT, osg::Vec4( 0.0, 0.0, 0.0, 1.0 ) );
    //        material->setShininess( osg::Material::FRONT, 25.0 );
    //        state->setAttribute( material );
    //    }

        m_moduleNode->insert( m_outputGeode );
        WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_moduleNode );
    }
}

void WMArbitraryRois::finalizeRoi()
{
    if( !m_active->get() )
    {
        return;
    }

    if( !WKernel::getRunningKernel()->getSelectionManager()->getBitField() )
    {
        wlog::warn( "WMArbitraryRois" ) << "Refused to add ROI, as ROIManager does not have computed its bitfield yet.";
        return;
    }

    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() );
    osg::ref_ptr< WROI > newRoi = osg::ref_ptr< WROI >( new WROIArbitrary(  grid->getNbCoordsX(), grid->getNbCoordsY(), grid->getNbCoordsZ(),
                                                                            grid->getTransformationMatrix(),
                                                                            *m_newValueSet->rawDataVectorPointer(),
                                                                            m_triMesh,
                                                                            m_threshold->get(),
                                                                            m_dataSet->getMax(), m_surfaceColor->get( true ) ) );

    if ( WKernel::getRunningKernel()->getRoiManager()->getSelectedRoi() == NULL )
    {
        WKernel::getRunningKernel()->getRoiManager()->addRoi( newRoi );
    }
    else
    {
        WKernel::getRunningKernel()->getRoiManager()->addRoi( newRoi, WKernel::getRunningKernel()->getRoiManager()->getSelectedRoi() );
    }
}

void WMArbitraryRois::activate()
{
    if( m_selectionRoi )
    {
        if( m_active->get() )
        {
            m_selectionRoi->setNodeMask( 0xFFFFFFFF );
        }
        else
        {
            m_selectionRoi->setNodeMask( 0x0 );
        }
    }

    WModule::activate();
}
