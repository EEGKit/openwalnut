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

// Some rules to the inclusion of headers:
//  * Ordering:
//    * C Headers
//    * C++ Standard headers
//    * External Lib headers (like OSG or Boost headers)
//    * headers of other classes inside OpenWalnut
//    * your own header file

#include <osg/Geometry>
#include <osgViewer/Renderer>
#include <osgViewer/GraphicsWindow>

#include "core/kernel/WModule.h"

#include "OpenVRTextureBuffer.h"

static const OSG_GLExtensions* getGLExtensions(const osg::State& state)
{
#if(OSG_VERSION_GREATER_OR_EQUAL(3, 4, 0))
    return state.get<osg::GLExtensions>();
#else
    return osg::FBOExtensions::instance(state.getContextID(), true);
#endif
}

static const OSG_Texture_Extensions* getTextureExtensions(const osg::State& state)
{
#if(OSG_VERSION_GREATER_OR_EQUAL(3, 4, 0))
    return state.get<osg::GLExtensions>();
#else
    return osg::Texture::getExtensions(state.getContextID(), true);
#endif
}

/* Public functions */
OpenVRTextureBuffer::OpenVRTextureBuffer(osg::ref_ptr<osg::State> state, int width, int height, int samples) :
    m_Resolve_FBO(0),
    m_Resolve_ColorTex(0),
    m_MSAA_FBO(0),
    m_MSAA_ColorTex(0),
    m_MSAA_DepthTex(0),
    m_width(width),
    m_height(height),
    m_samples(samples)
{
    const OSG_GLExtensions* fbo_ext = getGLExtensions(*state);

    // We don't want to support MIPMAP so, ensure only level 0 is allowed.
    const int maxTextureLevel = 0;

    // Create an FBO for secondary render target ready for application of lens distortion shader.
    fbo_ext->glGenFramebuffers(1, &m_Resolve_FBO);

    glGenTextures(1, &m_Resolve_ColorTex);
    glBindTexture(GL_TEXTURE_2D, m_Resolve_ColorTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, maxTextureLevel);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    // Create an FBO for primary render target.
    fbo_ext->glGenFramebuffers(1, &m_MSAA_FBO);

    const OSG_Texture_Extensions* extensions = getTextureExtensions(*state);

    // Create MSAA colour buffer
    glGenTextures(1, &m_MSAA_ColorTex);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_MSAA_ColorTex);
    extensions->glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_samples, GL_RGBA, m_width, m_height, false);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER_ARB);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER_ARB);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAX_LEVEL, maxTextureLevel);

    // Create MSAA depth buffer
    glGenTextures(1, &m_MSAA_DepthTex);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_MSAA_DepthTex);
    extensions->glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_samples, GL_DEPTH_COMPONENT, m_width, m_height, false);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAX_LEVEL, maxTextureLevel);

    // check FBO status
    GLenum status = fbo_ext->glCheckFramebufferStatus(GL_FRAMEBUFFER_EXT);
    if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
    {
        osg::notify(osg::WARN) << "Error setting up frame buffer object." << std::endl;
    }

}

void OpenVRTextureBuffer::onPreRender(osg::RenderInfo& renderInfo)
{
    osg::State& state = *renderInfo.getState();
    const OSG_GLExtensions* fbo_ext = getGLExtensions(state);

    fbo_ext->glBindFramebuffer(GL_FRAMEBUFFER_EXT, m_MSAA_FBO);
    fbo_ext->glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D_MULTISAMPLE, m_MSAA_ColorTex, 0);
    fbo_ext->glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D_MULTISAMPLE, m_MSAA_DepthTex, 0);
}

void OpenVRTextureBuffer::onPostRender(osg::RenderInfo& renderInfo)
{
    osg::State& state = *renderInfo.getState();
    const OSG_GLExtensions* fbo_ext = getGLExtensions(state);

    fbo_ext->glBindFramebuffer(GL_READ_FRAMEBUFFER_EXT, m_MSAA_FBO);
    fbo_ext->glFramebufferTexture2D(GL_READ_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D_MULTISAMPLE, m_MSAA_ColorTex, 0);
    fbo_ext->glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, 0);

    fbo_ext->glBindFramebuffer(GL_DRAW_FRAMEBUFFER_EXT, m_Resolve_FBO);
    fbo_ext->glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_Resolve_ColorTex, 0);
    fbo_ext->glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, 0);

    // Copy MSAA_FBO texture to Resolve_FBO
    fbo_ext->glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    fbo_ext->glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
}

void OpenVRTextureBuffer::destroy(osg::GraphicsContext* gc)
{
    const OSG_GLExtensions* fbo_ext = getGLExtensions(*gc->getState());
    if (fbo_ext)
    {
        fbo_ext->glDeleteFramebuffers(1, &m_MSAA_FBO);
        fbo_ext->glDeleteFramebuffers(1, &m_Resolve_FBO);
    }
}
