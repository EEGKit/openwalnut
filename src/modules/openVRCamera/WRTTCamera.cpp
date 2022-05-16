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

#include "WRTTCamera.h"

WRTTCamera::WRTTCamera( uint32_t width,
                        uint32_t height,
                        osg::GraphicsContext* gc,
                        osg::Matrix projectionMatrix,
                        osg::ref_ptr< osg::Node > scene,
                        boost::filesystem::path shaderPath ):
    WGECamera(),
    m_shaderPath( shaderPath )
{
    setViewport( 0, 0, width, height );
    setGraphicsContext( gc );
    setProjectionMatrix( projectionMatrix );

    m_node = osg::ref_ptr<WGEGroupNode>( new WGEGroupNode() );
    m_geometry = osg::ref_ptr<WGEGroupNode>( new WGEGroupNode() );
    m_geometry->addChild( scene );
    m_node->insert( m_geometry );

    setupCamera();
    setupRTT();

    wge::enableTransparency( m_node );

    addChild( m_node );
}

void WRTTCamera::setupCamera()
{
    setClearColor( osg::Vec4( 1.0, 1.0, 1.0, 1.0 ) );
    setClearMask( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    setReferenceFrame( osg::Transform::ABSOLUTE_RF );
    setRenderTargetImplementation( osg::Camera::FRAME_BUFFER_OBJECT );
    setRenderOrder( osg::Camera::PRE_RENDER, 0 );
}

void WRTTCamera::setupRTT()
{
    // Setup final texture
    m_finalColor = new osg::Texture2D();
    m_finalColor->setTextureSize( _viewport->width(), _viewport->height() );
    m_finalColor->setInternalFormat( GL_RGBA8 );
    m_finalColor->setFilter( osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST );
    m_finalColor->setFilter( osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR );

    // Setup render passes
    osg::ref_ptr< WGEOffscreenRenderNode > offscreenRender( new WGEOffscreenRenderNode( this, _viewport->width(), _viewport->height(), true ) );
    osg::ref_ptr< WGEOffscreenRenderPass > geometryPass = offscreenRender->addGeometryRenderPass( m_geometry, "GeometryPass" );
    osg::ref_ptr< WGEOffscreenTexturePass > texturePass = offscreenRender->addTextureProcessingPass(
        new WGEShader( "WMVRCamera", m_shaderPath ), "TexturePass" );
    osg::ref_ptr< WGEOffscreenFinalPass > finalPass = offscreenRender->addFinalOnScreenPass( "FinalPass" );

    // Attach textures to passes
    m_geometryColor = geometryPass->attach( COLOR_BUFFER );
    m_geometryDepth = geometryPass->attach( DEPTH_BUFFER );
    m_textureColor = texturePass->attach( COLOR_BUFFER );
    m_finalColor = finalPass->attach( COLOR_BUFFER );

    // Bind textures to passes
    texturePass->bind( m_geometryColor, 0 );
    texturePass->bind( m_geometryDepth, 1 );
    finalPass->bind( m_textureColor, 0 );

    m_node->insert( offscreenRender );
}

osg::ref_ptr< osg::Texture2D > WRTTCamera::getTextureColor()
{
    return m_textureColor;
}

osg::ref_ptr< WGEGroupNode > WRTTCamera::getGeometryNode()
{
    return m_geometry;
}
