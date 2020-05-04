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

#ifndef OPENVRTEXTUREBUFFER_H_
#define OPENVRTEXTUREBUFFER_H_

#include <openvr.h>

#include <osg/Geode>
#include <osg/Texture2D>
#include <osg/Version>
#include <osg/FrameBufferObject>

#if(OSG_VERSION_GREATER_OR_EQUAL(3, 4, 0))
    typedef osg::GLExtensions OSG_GLExtensions;
    typedef osg::GLExtensions OSG_Texture_Extensions;
#else
    typedef osg::FBOExtensions OSG_GLExtensions;
    typedef osg::Texture::Extensions OSG_Texture_Extensions;
#endif

class OpenVRTextureBuffer : public osg::Referenced
{
public:
    OpenVRTextureBuffer(osg::ref_ptr<osg::State> state, int width, int height, int msaaSamples);
    void destroy(osg::GraphicsContext* gc);
    GLuint getTexture() { return m_Resolve_ColorTex; }
    int textureWidth() const { return m_width; }
    int textureHeight() const { return m_height; }
    int samples() const { return m_samples; }
    void onPreRender(osg::RenderInfo& renderInfo);
    void onPostRender(osg::RenderInfo& renderInfo);

protected:
    ~OpenVRTextureBuffer() {}

    friend class OpenVRMirrorTexture;
    GLuint m_Resolve_FBO; // MSAA FBO is copied to this FBO after render.
    GLuint m_Resolve_ColorTex; // color texture for above FBO.
    GLuint m_MSAA_FBO; // framebuffer for MSAA RTT
    GLuint m_MSAA_ColorTex; // color texture for MSAA RTT 
    GLuint m_MSAA_DepthTex; // depth texture for MSAA RTT
    GLint m_width; // width of texture in pixels
    GLint m_height; // height of texture in pixels
    int m_samples;  // sample width for MSAA

};

#endif /* OPENVRTEXTUREBUFFER_H_ */
