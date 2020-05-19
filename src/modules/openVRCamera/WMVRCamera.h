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

#ifndef WMVRCAMERA_H
#define WMVRCAMERA_H

#include <string>
#include <vector>

#include <boost/thread.hpp>

#include "core/kernel/WModule.h"

#if (OSG_VERSION_GREATER_OR_EQUAL(3, 4, 0))
typedef osg::GLExtensions OSG_GLExtensions;
typedef osg::GLExtensions OSG_Texture_Extensions;
#else
typedef osg::FBOExtensions OSG_GLExtensions;
typedef osg::Texture::Extensions OSG_Texture_Extensions;
#endif

class OpenVRMirrorTexture : public osg::Referenced
{
public:
    OpenVRMirrorTexture(osg::ref_ptr<osg::State> state, GLint width, GLint height);
    void destroy(osg::GraphicsContext *gc);
    void blitTexture(osg::GraphicsContext *gc, OpenVRTextureBuffer *leftEye, OpenVRTextureBuffer *rightEye);

protected:
    ~OpenVRMirrorTexture() {}

    GLuint m_mirrorFBO;
    GLuint m_mirrorTex;
    GLint m_width;
    GLint m_height;
};

/**
 * Module starting and connecting to an OpenVR session
 * \ingroup modules
 */
class WMVRCamera : public WModule
{
    /**
 * Only UnitTests may be friends.
 */
    friend class WMVRCameraTest;

public:
    /**
     * Standard constructor.
     */
    WMVRCamera();

    /**
     * Destructor.
     */
    ~WMVRCamera();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description of module.
     */
    virtual const std::string getDescription() const;

    /**
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr<WModule> factory() const;

    /**
     * Get the icon for this module in XPM format.
     * \return the icon.
     */
    virtual const char **getXPMIcon() const;

    /**
     * Returns a Deviceproperty from m_vrSystem as a string
     */
    std::string GetDeviceProperty(vr::TrackedDeviceProperty prop);

    /**
     * Initialize the textureBuffer for submitting textures to OpenVR
     */
    void createRenderBuffers(osg::ref_ptr<osg::State> state);

    /**
     * Submit frame from texturebuffer to OpenVR
     */
    bool submitFrame();

    /**
     * TODO
     */
    void blitMirrorTexture(osg::GraphicsContext *gc);

    /**
     * Buffer for Submitting to OpenVR
     */
    osg::ref_ptr<OpenVRTextureBuffer> m_textureBuffer[2];

    /**
     * TODO
     */
    osg::ref_ptr<OpenVRMirrorTexture> m_mirrorTexture;

protected:
    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void moduleMain();

    /**
     * Initialize the connectors this module is using.
     */
    virtual void connectors();

    /**
     * Initialize the properties for this module.
     */
    virtual void properties();

    /**
     * Initialize requirements for this module.
     */
    virtual void requirements();

    /**
     * Callback for m_active. Overwrite this in your modules to handle m_active changes separately.
     */
    virtual void activate();

private:
    //! A condition for property updates.
    boost::shared_ptr<WCondition> m_propCondition;

    boost::shared_mutex m_updateLock; //!< Lock to prevent concurrent threads trying to update the osg node

    /**
     * The Geode containing all the cameras and the mesh
     */
    osg::ref_ptr<WGEManagedGroupNode> m_output;

    /**
     * The Object interfacing with OpenVR
     */
    //osg::ref_ptr< OpenVRDevice > m_HMD;

    /**
     * The OpenVR SDK Interface
     */
    vr::IVRSystem *m_vrSystem;

    /**
     * The OpenVR RenderModel Interface
     */
    vr::IVRRenderModels *m_vrRenderModels;

    /*
     * sample width for MSAA
     */
    int m_samples;
    /**
     * The geode with the the left Eye geometry
     */
    osg::ref_ptr<WGEGroupNode> m_leftEye;

    /**
     * The geode with the the right Eye geometry
     */
    osg::ref_ptr<WGEGroupNode> m_rightEye;

    WPropDouble m_leftEyePos; //!< x position of the slice

    WPropDouble m_rightEyePos; //!< x position of the slice
};

class OpenVRSwapCallback : public osg::GraphicsContext::SwapCallback
{
public:
    explicit OpenVRSwapCallback(WMVRCamera *device) : m_device(device), m_frameIndex(0) {}
    void swapBuffersImplementation(osg::GraphicsContext *gc);
    int frameIndex() const { return m_frameIndex; }

private:
    boost::shared_ptr<WMVRCamera> m_device;
    int m_frameIndex;
};

#endif // WMVRCAMERA_H
