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

#include <string>

#include <osg/MatrixTransform>
#include <osg/Projection>

#include "../../kernel/WKernel.h"
#include "../../graphicsEngine/WGEResourceManager.h"

#include "WMHud.h"

WMHud::WMHud()
{
}

WMHud::~WMHud()
{
}

boost::shared_ptr< WModule > WMHud::factory() const
{
    return boost::shared_ptr< WModule >( new WMHud() );
}

const std::string WMHud::getName() const
{
    return "HUD";
}

const std::string WMHud::getDescription() const
{
    return "This module provides several HUD's for status displays";
}

void WMHud::connectors()
{
}

void WMHud::properties()
{
    ( m_properties->addBool( "active", false, true ) )->connect( boost::bind( &WMHud::slotPropertyChanged, this, _1 ) );
    m_properties->addBool( "showHUD1", false );
}

void WMHud::moduleMain()
{
    // signal ready state
    ready();

    init();

    // Since the modules run in a separate thread: wait
    waitForStop();

    // clean up stuff
    // NOTE: ALLAWAYS remove your osg nodes!
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
}

void WMHud::init()
{
    m_rootNode = osg::ref_ptr< osg::Projection >( new osg::Projection );
    m_rootNode->setName( "HUDNode" );

    // Initialize the projection matrix for viewing everything we
    // will add as descendants of this node. Use screen coordinates
    // to define the horizontal and vertical extent of the projection
    // matrix. Positions described under this node will equate to
    // pixel coordinates.
    m_rootNode->setMatrix( osg::Matrix::ortho2D( 0, 1024, 0, 768 ) );

    // For the HUD model view matrix use an identity matrix
    osg::MatrixTransform* HUDModelViewMatrix = new osg::MatrixTransform;
    HUDModelViewMatrix->setMatrix( osg::Matrix::identity() );

    // Make sure the model view matrix is not affected by any transforms
    // above it in the scene graph
    HUDModelViewMatrix->setReferenceFrame( osg::Transform::ABSOLUTE_RF );

    // Add the HUD projection matrix as a child of the root node
    // and the HUD model view matrix as a child of the projection matrix
    // Anything under this node will be viewed using this projection matrix
    // and positioned with this model view matrix.
    //root->addChild(HUDProjectionMatrix);
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );
    m_rootNode->addChild( HUDModelViewMatrix );
    // Add the Geometry node to contain HUD geometry as a child of the
    // HUD model view matrix.

    m_HUDs = osg::ref_ptr<osg::Group>( new osg::Group() );

    // A geometry node for our HUD
    osg::ref_ptr<osg::Geode> HUDGeode = osg::ref_ptr<osg::Geode>( new osg::Geode() );

    HUDModelViewMatrix->addChild( m_HUDs );
    m_HUDs->addChild( HUDGeode );

    // Set up geometry for the HUD and add it to the HUD
    osg::Geometry* HUDBackgroundGeometry = new osg::Geometry();

    osg::Vec3Array* HUDBackgroundVertices = new osg::Vec3Array;
    HUDBackgroundVertices->push_back( osg::Vec3( 580, 0, -1 ) );
    HUDBackgroundVertices->push_back( osg::Vec3( 1024, 0, -1 ) );
    HUDBackgroundVertices->push_back( osg::Vec3( 1024, 100, -1 ) );
    HUDBackgroundVertices->push_back( osg::Vec3( 580, 100, -1 ) );

    osg::DrawElementsUInt* HUDBackgroundIndices = new osg::DrawElementsUInt( osg::PrimitiveSet::POLYGON, 0 );
    HUDBackgroundIndices->push_back( 0 );
    HUDBackgroundIndices->push_back( 1 );
    HUDBackgroundIndices->push_back( 2 );
    HUDBackgroundIndices->push_back( 3 );

    osg::Vec4Array* HUDcolors = new osg::Vec4Array;
    HUDcolors->push_back( osg::Vec4( 0.8f, 0.8f, 0.8f, 0.8f ) );

    osg::Vec3Array* HUDnormals = new osg::Vec3Array;
    HUDnormals->push_back( osg::Vec3( 0.0f, 0.0f, 1.0f ) );
    HUDBackgroundGeometry->setNormalArray( HUDnormals );
    HUDBackgroundGeometry->setNormalBinding( osg::Geometry::BIND_OVERALL );
    HUDBackgroundGeometry->addPrimitiveSet( HUDBackgroundIndices );
    HUDBackgroundGeometry->setVertexArray( HUDBackgroundVertices );
    HUDBackgroundGeometry->setColorArray( HUDcolors );
    HUDBackgroundGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    HUDGeode->addDrawable( HUDBackgroundGeometry );

    // Create and set up a state set using the texture from above
    osg::StateSet* HUDStateSet = new osg::StateSet();
    HUDGeode->setStateSet( HUDStateSet );

    // For this state set, turn blending on (so alpha texture looks right)
    HUDStateSet->setMode( GL_BLEND, osg::StateAttribute::ON );

    // Disable depth testing so geometry is draw regardless of depth values
    // of geometry already draw.
    HUDStateSet->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );
    HUDStateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

    // Need to make sure this geometry is draw last. RenderBins are handled
    // in numerical order so set bin number to 11
    HUDStateSet->setRenderBinDetails( 11, "RenderBin" );

    m_osgPickText = osg::ref_ptr< osgText::Text>( new osgText::Text() );

    HUDGeode->addDrawable( m_osgPickText );

    m_osgPickText->setCharacterSize( 14 );
    m_osgPickText->setFont( WGEResourceManager::getResourceManager()->getDefaultFont() );
    m_osgPickText->setText( "nothing picked" );
    m_osgPickText->setAxisAlignment( osgText::Text::SCREEN );
    m_osgPickText->setPosition( osg::Vec3( 600, 80, -1.5 ) );
    m_osgPickText->setColor( osg::Vec4( 0, 0, 0, 1 ) );

    m_rootNode->setUserData( this );
    m_rootNode->setUpdateCallback( new HUDNodeCallback );

    if ( m_properties->getValue<bool>( "active" ) )
    {
        m_rootNode->setNodeMask( 0xFFFFFFFF );
    }
    else
    {
        m_rootNode->setNodeMask( 0x0 );
    }

    // connect updateGFX with picking
    boost::shared_ptr< WGEViewer > viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "main" );
    assert( viewer );
    viewer->getPickHandler()->getPickSignal()->connect( boost::bind( &WMHud::updatePickText, this, _1 ) );
}

void WMHud::updatePickText( std::string text )
{
    m_pickText = text;
}

void WMHud::update()
{
    m_osgPickText->setText( m_pickText.c_str() );
}

void WMHud::slotPropertyChanged( std::string propertyName )
{
    if ( propertyName == "active" )
    {
        if ( m_properties->getValue<bool>( "active" ) )
        {
            m_rootNode->setNodeMask( 0xFFFFFFFF );
        }
        else
        {
            m_rootNode->setNodeMask( 0x0 );
        }
    }
}
