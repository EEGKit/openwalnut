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

#include <osg/Vec3>
#include <osg/MatrixTransform>

#include "../../common/WPropertyHelper.h"
#include "../../common/math/WMath.h"
#include "../../graphicsEngine/WGEColormapping.h"
#include "../../graphicsEngine/WGEGeodeUtils.h"
#include "../../graphicsEngine/callbacks/WGELinearTranslationCallback.h"
#include "../../graphicsEngine/callbacks/WGENodeMaskCallback.h"
#include "../../graphicsEngine/shaders/WGEPropertyUniform.h"
#include "../../graphicsEngine/shaders/WGEShader.h"
#include "../../graphicsEngine/shaders/WGEShaderDefineOptions.h"
#include "../../graphicsEngine/shaders/WGEShaderPropertyDefineOptions.h"
#include "../../graphicsEngine/WGraphicsEngine.h"
#include "../../kernel/WKernel.h"

#include "WMNavigationSlices.h"
#include "WMNavigationSlices.xpm"

// This line is needed by the module loader to actually find your module. You need to add this to your module too. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMNavigationSlices )

WMNavigationSlices::WMNavigationSlices():
    WModule(),
    m_first( true )
{
}

WMNavigationSlices::~WMNavigationSlices()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMNavigationSlices::factory() const
{
    return boost::shared_ptr< WModule >( new WMNavigationSlices() );
}

const char** WMNavigationSlices::getXPMIcon() const
{
    return WMNavigationSlices_xpm;
}

const std::string WMNavigationSlices::getName() const
{
    return "Navigation Slices 2";
}

const std::string WMNavigationSlices::getDescription() const
{
    return "This module provides slices, orthogonally oriented in the OpenWalnut coordinate system. They allow the fast and easy exploration of "
           "three-dimensional datasets.";
}

void WMNavigationSlices::connectors()
{
    // call WModule's initialization
    WModule::connectors();
}

void WMNavigationSlices::properties()
{
    m_sliceGroup      = m_properties->addPropertyGroup( "Slices",  "Slice Options." );

    // enable slices
    // Flags denoting whether the glyphs should be shown on the specific slice
    m_showonX        = m_sliceGroup->addProperty( "Show Sagittal", "Show vectors on sagittal slice.", true );
    m_showonY        = m_sliceGroup->addProperty( "Show Coronal", "Show vectors on coronal slice.", true );
    m_showonZ        = m_sliceGroup->addProperty( "Show Axial", "Show vectors on axial slice.", true );

    // The slice positions.
    m_xPos           = m_sliceGroup->addProperty( "Sagittal Position", "Slice X position.", 0.0, true );
    m_yPos           = m_sliceGroup->addProperty( "Coronal Position", "Slice Y position.", 0.0, true );
    m_zPos           = m_sliceGroup->addProperty( "Axial Position", "Slice Z position.", 0.0, true );

    // call WModule's initialization
    WModule::properties();
}

void WMNavigationSlices::initOSG()
{
    // remove the old slices
    m_output->clear();

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Property Setup
    ///////////////////////////////////////////////////////////////////////////////////////////////

    // no colormaps -> no slices
    bool empty = !WGEColormapping::instance()->size();
    if ( empty )
    {
        // hide the slider properties.
        m_xPos->setHidden();
        m_yPos->setHidden();
        m_zPos->setHidden();
        return;
    }

    // grab the current bounding box
    WBoundingBox bb = WGEColormapping::instance()->getBoundingBox();
    WVector3d minV = bb.getMin();
    WVector3d maxV = bb.getMax();

    // update the properties
    m_xPos->setMin( minV[0] );
    m_xPos->setMax( maxV[0] );
    m_yPos->setMin( minV[1] );
    m_yPos->setMax( maxV[1] );
    m_zPos->setMin( minV[2] );
    m_zPos->setMax( maxV[2] );
    // un-hide the slider properties.
    m_xPos->setHidden( false );
    m_yPos->setHidden( false );
    m_zPos->setHidden( false );

    // if this is done the first time, set the slices to the center of the dataset
    if ( m_first )
    {
        m_first = false;
        m_xPos->set( ( maxV[0] - minV[0] ) / 2.0 );
        m_yPos->set( ( maxV[1] - minV[1] ) / 2.0 );
        m_zPos->set( ( maxV[2] - minV[2] ) / 2.0 );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Slice Setup
    ///////////////////////////////////////////////////////////////////////////////////////////////

    // create a new geode containing the slices

    // X Slice
    osg::ref_ptr< osg::Node > xSlice = wge::genFinitePlane( minV, osg::Vec3( 0.0, maxV[1], 0.0 ),
                                                                  osg::Vec3( 0.0, 0.0, maxV[2] ) );
    xSlice->setName( "Sagittal Slice" );
    osg::Uniform* xSliceUniform = new osg::Uniform( "u_WorldTransform", osg::Matrix::identity() );
    xSlice->getOrCreateStateSet()->addUniform( xSliceUniform );

    // Y Slice
    osg::ref_ptr< osg::Node > ySlice = wge::genFinitePlane( minV, osg::Vec3( maxV[0], 0.0, 0.0 ),
                                                                  osg::Vec3( 0.0, 0.0, maxV[2] ) );
    ySlice->setName( "Coronal Slice" );
    osg::Uniform* ySliceUniform = new osg::Uniform( "u_WorldTransform", osg::Matrix::identity() );
    ySlice->getOrCreateStateSet()->addUniform( ySliceUniform );

    // Z Slice
    osg::ref_ptr< osg::Node > zSlice = wge::genFinitePlane( minV, osg::Vec3( maxV[0], 0.0, 0.0 ),
                                                                  osg::Vec3( 0.0, maxV[1], 0.0 ) );
    zSlice->setName( "Axial Slice" );
    osg::Uniform* zSliceUniform = new osg::Uniform( "u_WorldTransform", osg::Matrix::identity() );
    zSlice->getOrCreateStateSet()->addUniform( zSliceUniform );

    // disable culling.
    // NOTE: Somehow, this is ignore by OSG. If you know why: tell me please
    xSlice->setCullingActive( false );
    ySlice->setCullingActive( false );
    zSlice->setCullingActive( false );

    // each slice is child of an transformation node
    osg::ref_ptr< osg::MatrixTransform > mX = new osg::MatrixTransform();
    mX->addChild( xSlice );
    osg::ref_ptr< osg::MatrixTransform > mY = new osg::MatrixTransform();
    mY->addChild( ySlice );
    osg::ref_ptr< osg::MatrixTransform > mZ = new osg::MatrixTransform();
    mZ->addChild( zSlice );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Callback Setup
    ///////////////////////////////////////////////////////////////////////////////////////////////

    // Control transformation node by properties. We use an additional uniform here to provide the shader the transformation matrix used to
    // translate the slice.
    mX->addUpdateCallback( new WGELinearTranslationCallback< WPropDouble >( osg::Vec3( 1.0, 0.0, 0.0 ), m_xPos, xSliceUniform ) );
    mY->addUpdateCallback( new WGELinearTranslationCallback< WPropDouble >( osg::Vec3( 0.0, 1.0, 0.0 ), m_yPos, ySliceUniform ) );
    mZ->addUpdateCallback( new WGELinearTranslationCallback< WPropDouble >( osg::Vec3( 0.0, 0.0, 1.0 ), m_zPos, zSliceUniform ) );

    // set callbacks for en-/disabling the nodes
    xSlice->addUpdateCallback( new WGENodeMaskCallback( m_showonX ) );
    ySlice->addUpdateCallback( new WGENodeMaskCallback( m_showonY ) );
    zSlice->addUpdateCallback( new WGENodeMaskCallback( m_showonZ ) );

    // set the pick callbacks for each slice
    m_xSlicePicker = PickCallback::SPtr( new PickCallback( xSlice, m_xPos ) );
    m_ySlicePicker = PickCallback::SPtr( new PickCallback( ySlice, m_yPos, true ) );
    m_zSlicePicker = PickCallback::SPtr( new PickCallback( zSlice, m_zPos ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Done
    ///////////////////////////////////////////////////////////////////////////////////////////////

    // add the transformation nodes to the output group
    m_output->insert( mX );
    m_output->insert( mY );
    m_output->insert( mZ );
    m_output->dirtyBound();
}

WMNavigationSlices::PickCallback::PickCallback( osg::ref_ptr< osg::Node > node, WPropDouble property, bool negateDirection ):
    m_node( node ),
    m_property( property ),
    m_dir( negateDirection ? -1.0 : 1.0 )
{
    boost::shared_ptr< WGraphicsEngine > ge = WGraphicsEngine::getGraphicsEngine();
    boost::shared_ptr< WGEViewer > viewer = ge->getViewerByName( "main" );
    m_camera = viewer->getCamera();
    m_pickConnection = viewer->getPickHandler()->getPickSignal()->connect( boost::bind( &WMNavigationSlices::PickCallback::pick, this, _1 ) );
}

void WMNavigationSlices::PickCallback::pick( WPickInfo pickInfo )
{
    if ( pickInfo.getName() == m_node->getName() )
    {
        WVector3d normal = pickInfo.getPickNormal();
        WVector2d newPixelPos = pickInfo.getPickPixel();
        // dragging was initialized earlier
        if ( m_isPicked )
        {
            osg::Vec3 startPosScreen( m_oldPixelPosition[ 0 ], m_oldPixelPosition[ 1 ], 0.0 );
            osg::Vec3 endPosScreen( newPixelPos[ 0 ], newPixelPos[ 1 ], 0.0 );

            osg::Vec3 startPosWorld = wge::unprojectFromScreen( startPosScreen, m_camera );
            osg::Vec3 endPosWorld = wge::unprojectFromScreen( endPosScreen, m_camera );

            osg::Vec3 moveDirWorld = endPosWorld - startPosWorld;
            float diff = moveDirWorld * static_cast< osg::Vec3 >( normal );

            m_property->set( m_property->get() + m_dir * diff );
        }
        // this might have initialized dragging. Keep track of old position
        m_oldPixelPosition = newPixelPos;
        m_isPicked = true;
    }
    else    // someone else got picked.
    {
        m_isPicked = false;
    }
}

void WMNavigationSlices::moduleMain()
{
    // get notified about data changes
    m_moduleState.setResetable( true, true );

    // done.
    ready();

    // create the root node for all the geometry
    m_output = osg::ref_ptr< WGEManagedGroupNode > ( new WGEManagedGroupNode( m_active ) );
    m_output->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    // apply colormapping to transformation
    osg::ref_ptr< WGEShader > shader = new WGEShader( "WMNavigationSlices", m_localPath );
    WGEColormapping::apply( m_output, shader ); // this automatically applies the shader
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_output );

    // we need to be informed if the bounding box of the volume containing all the data changes.
    m_moduleState.add( WGEColormapping::instance()->getChangeCondition() );

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Main loop
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    // main loop
    while ( !m_shutdownFlag() )
    {
        // woke up since the module is requested to finish?
        if ( m_shutdownFlag() )
        {
            break;
        }

        // create the slices. This loop is only entered if WGEColormapper was fired or shutdown.
        initOSG();

        // Thats it. Nothing more to do. Slide movement and colormapping is done by the pick handler and WGEColormapper
        debugLog() << "Waiting ...";
        m_moduleState.wait();
    }

    // clean up
    m_xSlicePicker.reset();
    m_ySlicePicker.reset();
    m_zSlicePicker.reset();

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_output );
}

