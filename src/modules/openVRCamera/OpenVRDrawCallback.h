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

#ifndef OPENVRDRAWCALLBACK_H_
#define OPENVRDRAWCALLBACK_H_

#include <osg/Geode>
#include <osg/Texture2D>
#include <osg/Version>
#include <osg/FrameBufferObject>

#include "OpenVRTextureBuffer.h"

class OpenVRPreDrawCallback : public osg::Camera::DrawCallback
{
public:
    OpenVRPreDrawCallback(osg::Camera* camera, OpenVRTextureBuffer* textureBuffer)
        : m_camera(camera)
        , m_textureBuffer(textureBuffer)
    {
    }

    virtual void operator()(osg::RenderInfo& renderInfo) const;
protected:
    osg::Camera* m_camera;
    OpenVRTextureBuffer* m_textureBuffer;

};

class OpenVRPostDrawCallback : public osg::Camera::DrawCallback
{
public:
    OpenVRPostDrawCallback(osg::Camera* camera, OpenVRTextureBuffer* textureBuffer)
        : m_camera(camera)
        , m_textureBuffer(textureBuffer)
    {
    }

    virtual void operator()(osg::RenderInfo& renderInfo) const;
protected:
    osg::Camera* m_camera;
    OpenVRTextureBuffer* m_textureBuffer;
};

class OpenVRInitialDrawCallback : public osg::Camera::DrawCallback
{
public:
    virtual void operator()(osg::RenderInfo& renderInfo) const;

};

#endif /* OPENVRDRAWCALLBACK_H_ */
