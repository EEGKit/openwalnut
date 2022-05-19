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

#ifndef WRTTCAMERA_H
#define WRTTCAMERA_H

#include <osg/Texture2D>

#include "core/graphicsEngine/WGECamera.h"
#include "core/graphicsEngine/WGEGroupNode.h"
#include "core/graphicsEngine/WGEUtils.h"
#include "core/graphicsEngine/offscreen/WGEOffscreenRenderNode.h"

/**
 * A camera that renders its view to a texture.
 */
class WRTTCamera : public WGECamera
{
public:
    /**
     * Creates and setups the RTT camera.
     * 
     * \param width The width of the viewport.
     * \param height The height of the viewport.
     * \param gc The GraphicsContext of the camera.
     * \param projectionMatrix The projection matrix of the camera.
     * \param scene The scene that the camera should render.
     * \param shaderPath The path to the shader files.
     */
    WRTTCamera( uint32_t width,
                uint32_t height,
                osg::GraphicsContext* gc,
                osg::Matrix projectionMatrix,
                osg::ref_ptr< osg::Node > scene,
                boost::filesystem::path shaderPath );

    /**
     * Gets the color texture of the texture pass.
     * \return osg::ref_ptr< osg::Texture2D > The texture color.
     */
    osg::ref_ptr< osg::Texture2D > getTextureColor();

    /**
     * Gets the geometry node.
     * \return osg::ref_ptr< WGEGroupNode > The geometry node.
     */
    osg::ref_ptr< WGEGroupNode > getGeometryNode();

private:
    /**
     * Sets the camera options, such as: FrameBufferObject, Renderbin, etc.
     */
    void setupCamera();

    /**
     * Creates the OffscreenRenderer and attaches the textures to it.
     */
    void setupRTT();

    boost::filesystem::path m_shaderPath; //!< The path to the shader files.

    osg::ref_ptr< WGEGroupNode > m_node; //!< The node containing every child of this camera.
    osg::ref_ptr< WGEGroupNode > m_geometry; //!< The node containing the scene that this camera renders.

    osg::ref_ptr< osg::Texture2D > m_geometryColor; //!< The color texture for the geometry pass.
    osg::ref_ptr< osg::Texture2D > m_geometryDepth; //!< The depth texture for the geometry pass.
    osg::ref_ptr< osg::Texture2D > m_textureColor; //!< The color texture for the texture pass.
    osg::ref_ptr< osg::Texture2D > m_finalColor; //!< The color texture for the final pass.
};

#endif  // WRTTCAMERA_H
