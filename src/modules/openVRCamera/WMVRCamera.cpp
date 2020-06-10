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

#include <algorithm>
#include <string>
#include <openvr.h>     // NOLINT: this is not a C system header as brainlint thinks

#include <osg/BufferObject>
#include <osg/StateAttribute>
#include <osg/Texture2D>
#include <osg/TexMat>
#include <osgDB/WriteFile>
//#include <QtCore/QDir>

#include "core/graphicsEngine/postprocessing/WGEPostprocessingNode.h"
#include "core/graphicsEngine/WGEManagedGroupNode.h"
#include "core/graphicsEngine/WGECamera.h"
#include "core/graphicsEngine/WGEGeodeUtils.h"
#include "core/graphicsEngine/WGERequirement.h"

#include "core/common/WPropertyHelper.h"
#include "core/graphicsEngine/WGEUtils.h"
#include "core/graphicsEngine/WGraphicsEngine.h"
#include "core/kernel/WKernel.h"

#include "WMVRCamera.h"
#include "WMVRCamera.xpm"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMVRCamera )

WMVRCamera::WMVRCamera():
    WModule()
{
    // WARNING: initializing connectors inside the constructor will lead to an exception.
    // Implement WModule::initializeConnectors instead.
}

WMVRCamera::~WMVRCamera()
{
    // cleanup
    // removeConnectors();
}

boost::shared_ptr< WModule > WMVRCamera::factory() const
{
    return boost::shared_ptr< WModule >( new WMVRCamera() );
}

const char** WMVRCamera::getXPMIcon() const
{
    return open_vr_camera_xpm;
}

const std::string WMVRCamera::getName() const
{
    return "OpenVRCamera";
}

const std::string WMVRCamera::getDescription() const
{
    return "This module will start an OpenVR session"
" right now it only adds an extra camera that renders to a texture";
}

void WMVRCamera::connectors()
{
    // call WModules initialization
    WModule::connectors();
}

void WMVRCamera::properties()
{
    // NOTE: Refer to WMTemplate.cpp if you do not understand these commands.
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_vrOn = m_properties->addProperty( "Submit Frames", "Toggle submitting frames to OpenVR",
                                                    false );
    m_debugTrigger1 = m_properties->addProperty( "Screenshot Eyes", "Speichern.",
                                                    WPVBaseTypes::PV_TRIGGER_READY );
    m_debugTrigger2 = m_properties->addProperty( "Screenshot Main", "Speichern.",
                                                    WPVBaseTypes::PV_TRIGGER_READY );

    WModule::properties();
}

void WMVRCamera::requirements()
{
    // NOTE: Refer to WMTemplate.cpp if you do not understand these commands.

    // We need graphics to draw anything:
    m_requirements.push_back( new WGERequirement() );
}

bool WMVRCamera::setupVRInterface()
{
    // Exit if we do not have an HMD present
    if(!vr::VR_IsHmdPresent())
    {
        errorLog() << "No valid HMD present!" << std::endl;
        return false;
        //requestStop();
    }

    // Loading the OpenVR Runtime
    vr::EVRInitError eError = vr::VRInitError_None;
    m_vrSystem = vr::VR_Init( &eError, vr::VRApplication_Scene );

    if(eError != vr::VRInitError_None )
    {
        m_vrSystem = nullptr;
        errorLog()
            << "Unable to initialize the OpenVR library." << std::endl
            << "Reason: " << vr::VR_GetVRInitErrorAsEnglishDescription( eError ) << std::endl;
        return false;
        //requestStop();
    }

    if( !vr::VRCompositor() )
    {
        m_vrSystem = nullptr;
        vr::VR_Shutdown();
        errorLog() << "Compositor initialization failed" << std::endl;
        return false;
        //requestStop();
    }

    m_vrRenderModels = ( vr::IVRRenderModels * ) vr::VR_GetGenericInterface( vr::IVRRenderModels_Version, &eError );

    if( m_vrRenderModels == nullptr )
    {
        m_vrSystem = nullptr;
        vr::VR_Shutdown();
        errorLog()
            << "Unable to get render model interface!" << std::endl
            << "Reason: " << vr::VR_GetVRInitErrorAsEnglishDescription( eError ) << std::endl;
        return false;
        //requestStop();
    }

    vr::VRCompositor()->SetTrackingSpace( vr::TrackingUniverseSeated );
    m_vrSystem->GetRecommendedRenderTargetSize( &m_vrRenderWidth, &m_vrRenderHeight );

    std::string driverName = GetDeviceProperty( vr::Prop_TrackingSystemName_String );
    std::string deviceSerialNumber = GetDeviceProperty( vr::Prop_SerialNumber_String );
    debugLog() << "HMD driver name: " << driverName;
    debugLog() << "HMD device serial number: " << deviceSerialNumber;

    return true;
}

void WMVRCamera::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // View Setup
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    // create a root node for this module
    m_rootnode = osg::ref_ptr< WGEManagedGroupNode > ( new WGEManagedGroupNode( m_active ) );

    // create the roots for the Eyes
    m_leftEyeNode = osg::ref_ptr< WGEGroupNode > ( new WGEGroupNode() );
    m_rightEyeNode = osg::ref_ptr< WGEGroupNode > ( new WGEGroupNode() );

    // insert the created nodes
    m_rootnode->insert( m_leftEyeNode );
    m_rootnode->insert( m_rightEyeNode );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootnode );

    // get side-views
    boost::shared_ptr< WGEViewer > leftEyeView = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Left Eye View" );
    boost::shared_ptr< WGEViewer > rightEyeView = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Right Eye View" );
    leftEyeView->reset();
    rightEyeView->reset();

    // Now, we can mark the module ready.
    ready();
    debugLog() << "Starting...";

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // HMD Setup
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    if( setupVRInterface() )
    {
        debugLog() << "OpenVR initialized successfully";
        m_vrIsInitialized = true;
    }
    else
    {
        errorLog() << "OpenVR failed to initialize";
        m_vrRenderWidth = 2160;
        m_vrRenderHeight = 1200;
        m_vrIsInitialized = false;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Render to Texture Setup
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    leftEyeView->getCamera()->setClearMask( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    leftEyeView->getCamera()->setRenderTargetImplementation( osg::Camera::FRAME_BUFFER_OBJECT );
    leftEyeView->getCamera()->setRenderOrder( osg::Camera::PRE_RENDER, vr::Eye_Left );
    leftEyeView->getCamera()->setComputeNearFarMode( osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );

    rightEyeView->getCamera()->setClearMask( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    rightEyeView->getCamera()->setRenderTargetImplementation( osg::Camera::FRAME_BUFFER_OBJECT );
    rightEyeView->getCamera()->setRenderOrder( osg::Camera::PRE_RENDER, vr::Eye_Right );
    rightEyeView->getCamera()->setComputeNearFarMode( osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );

    m_leftTexture = new osg::Texture2D;
    m_leftTexture->setTextureSize( m_vrRenderWidth, m_vrRenderHeight );
    m_leftTexture->setInternalFormat( GL_RGBA8 );
    m_leftTexture->setFilter( osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST );
    m_leftTexture->setFilter( osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR );

    m_rightTexture = new osg::Texture2D;
    m_rightTexture->setTextureSize( m_vrRenderWidth, m_vrRenderHeight );
    m_rightTexture->setInternalFormat( GL_RGBA8 );
    m_rightTexture->setFilter( osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST );
    m_rightTexture->setFilter( osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR );

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Offscreen render pipeline setup
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    osg::ref_ptr< WGEOffscreenRenderNode > offscreenRenderLeft(
        new WGEOffscreenRenderNode(
            leftEyeView->getCamera(),
            m_vrRenderWidth,
            m_vrRenderHeight,
            true
        )
    );
    osg::ref_ptr< WGEOffscreenRenderNode > offscreenRenderRight(
        new WGEOffscreenRenderNode(
            rightEyeView->getCamera(),
            m_vrRenderWidth,
            m_vrRenderHeight,
            true
        )
    );

    //Add Geometry to render pipeline
    osg::ref_ptr< WGEOffscreenRenderPass > geometryPassLeft = offscreenRenderLeft->addGeometryRenderPass( m_leftEyeNode, "GeometryPassLeft" );
    osg::ref_ptr< WGEOffscreenRenderPass> geometryPassRight = offscreenRenderRight->addGeometryRenderPass( m_rightEyeNode, "GeometryPassRight" );

    //Add a texture processing step to render pipeline
    osg::ref_ptr< WGEOffscreenTexturePass > texturePassLeft = offscreenRenderLeft->addTextureProcessingPass(
         new WGEShader( "WMVRCameraTestShader", m_localPath ), "TexturePassLeft" );
    osg::ref_ptr< WGEOffscreenTexturePass> texturePassRight = offscreenRenderRight->addTextureProcessingPass(
         new WGEShader( "WMVRCameraTestShader", m_localPath ), "TexturePassRight" );

    //And a final pass
    osg::ref_ptr< WGEOffscreenFinalPass > finalPassLeft = offscreenRenderLeft->addFinalOnScreenPass( "FinalPassLeft" );
    osg::ref_ptr< WGEOffscreenFinalPass > finalPassRight = offscreenRenderRight->addFinalOnScreenPass( "FinalPassRight" );

    //attach geometry to color and depth buffer
    osg::ref_ptr< osg::Texture2D > geometryColorLeft  = geometryPassLeft->attach( WGECamera::COLOR_BUFFER );
    osg::ref_ptr< osg::Texture2D > geometryDepthLeft = geometryPassLeft->attach( WGECamera::DEPTH_BUFFER );
    osg::ref_ptr< osg::Texture2D > geometryColorRight  = geometryPassRight->attach( WGECamera::COLOR_BUFFER );
    osg::ref_ptr< osg::Texture2D > geometryDepthRight = geometryPassRight->attach( WGECamera::DEPTH_BUFFER );

    //attach texture pass to color buffer
    m_textureColorLeft  = texturePassLeft->attach( WGECamera::COLOR_BUFFER );
    m_textureColorRight  = texturePassRight->attach( WGECamera::COLOR_BUFFER );

    //use geometry pass as input for texture pass
    texturePassLeft->bind( geometryColorLeft, 0 );
    texturePassLeft->bind( geometryDepthLeft, 1 );
    texturePassRight->bind( geometryColorRight, 0 );
    texturePassRight->bind( geometryDepthRight, 1 );

    //bind images to texture pass output
    osg::ref_ptr< osg::Image > limage = new osg::Image();
    limage->allocateImage( m_vrRenderWidth,
                      m_vrRenderHeight,
                      1,   // 2D texture is 1 pixel deep
                      GL_RGBA,
                      GL_UNSIGNED_BYTE );
    limage->setInternalTextureFormat( GL_RGBA8 );
    m_textureColorLeft->setImage( limage );

    osg::ref_ptr< osg::Image > rimage = new osg::Image();
    rimage->allocateImage( m_vrRenderWidth,
                      m_vrRenderHeight,
                      1,   // 2D texture is 1 pixel deep
                      GL_RGBA,
                      GL_UNSIGNED_BYTE );
    rimage->setInternalTextureFormat( GL_RGBA8 );
    m_textureColorRight->setImage( rimage );

    // attach final pass to color output
    finalPassLeft->attach( WGECamera::COLOR_BUFFER, m_leftTexture );
    finalPassRight->attach( WGECamera::COLOR_BUFFER, m_rightTexture );
    finalPassLeft->bind( m_textureColorLeft, 0 );
    finalPassRight->bind( m_textureColorRight, 0 );

    // The final pass should also blend properly:
    finalPassLeft->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
    finalPassRight->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );

    m_rootnode->insert( offscreenRenderLeft );
    m_rootnode->insert( offscreenRenderRight );

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Main loop
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    // updatecallback for submitting frames
    m_rootnode->addUpdateCallback( new SafeUpdateCallback( this ) );

    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        // woke up since the module is requested to finish
        if( m_shutdownFlag() )
        {
            break;
        }

        debugLog() << "Waiting...";
        m_moduleState.wait();
    }

    // Shut down OpenVR
    m_vrSystem = nullptr;
    vr::VR_Shutdown();

    // Never miss to clean up. Especially remove your OSG nodes. Everything else you add to these nodes will be removed automatically.
    debugLog() << "Shutting down openVRCamera";

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootnode );
}

void WMVRCamera::activate()
{
    // This method gets called, whenever the m_active property changes. Your module should always handle this if you do not use the
    // WGEManagedGroupNode for your scene. The user can (de-)activate modules in his GUI and you can handle this case here:
    if( m_active->get() )
    {
        debugLog() << "Activate.";
    }
    else
    {
        debugLog() << "Deactivate.";
    }

    // The simpler way is by using WGEManagedGroupNode which deactivates itself according to m_active. See moduleMain for details.

    // Always call WModule's activate!
    WModule::activate();
}

std::string WMVRCamera::GetDeviceProperty( vr::TrackedDeviceProperty prop )
{
    uint32_t bufferLen = m_vrSystem->GetStringTrackedDeviceProperty( vr::k_unTrackedDeviceIndex_Hmd, prop, NULL, 0 );
    if(bufferLen == 0)
    {
        return "";
    }

    char* buffer = new char[ bufferLen ];
    bufferLen = m_vrSystem->GetStringTrackedDeviceProperty( vr::k_unTrackedDeviceIndex_Hmd, prop, buffer, bufferLen );
    std::string result = buffer;
    delete [] buffer;
    return result;
}

void WMVRCamera::SafeUpdateCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    if( m_module->m_debugTrigger1->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
    {
        m_module->debugLog() << " Links Breite:" << m_module->m_textureColorLeft->getImage()->s() <<
                            " Höhe:" << m_module->m_textureColorLeft->getImage()->t() <<
                            " Tiefe:" << m_module->m_textureColorLeft->getImage()->r();
        m_module->debugLog() << "Pointer:" << m_module->m_textureColorLeft->getImage()->getDataPointer();
        m_module->debugLog() << "DatenMenge:" << m_module->m_textureColorLeft->getImage()->getTotalDataSize();

        m_module->debugLog() << "Rechts Breite:" << m_module->m_textureColorRight->getImage()->s() <<
                            " Höhe:" << m_module->m_textureColorRight->getImage()->t() <<
                            " Tiefe:" << m_module->m_textureColorRight->getImage()->r();
        m_module->debugLog() << "Pointer:" << m_module->m_textureColorRight->getImage()->getDataPointer();
        m_module->debugLog() << "DatenMenge:" << m_module->m_textureColorRight->getImage()->getTotalDataSize();

        //std::string leftFilename = "C:/Users/Jonas/OpenWalnut - leftEye.png";
        //std::string rightFilename = "C:/Users/Jonas/OpenWalnut - rightEye.png";
        std::string leftFilename = "./OpenWalnut - leftEye.png";
        std::string rightFilename = "./OpenWalnut - rightEye.png";
        //std::string leftFilename = ( QDir::homePath() + QDir::separator() + "OpenWalnut - leftEye.png" ).toStdString();
        //std::string rightFilename = ( QDir::homePath() + QDir::separator() + "OpenWalnut - rightEye.png" ).toStdString();

        osgDB::writeImageFile( *m_module->m_textureColorLeft->getImage(), leftFilename );
        osgDB::writeImageFile( *m_module->m_textureColorRight->getImage(), rightFilename );

        m_module->m_debugTrigger1->set( WPVBaseTypes::PV_TRIGGER_READY, false );
    }
    if( m_module->m_debugTrigger2->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
    {
        osg::ref_ptr< osg::Image > image = new osg::Image();
        image->readPixels( 0, 0, m_module->m_vrRenderWidth, m_module->m_vrRenderHeight, GL_RGBA, GL_UNSIGNED_BYTE );

        m_module->debugLog() << "Breite:" << image->s();
        m_module->debugLog()  << "Pointer:" << image->getDataPointer();
        m_module->debugLog() << "DatenMenge:" << image->getTotalDataSize();

        //std::string filename = "C:/Users/Jonas/OpenWalnut - framebuffer.png";
        std::string filename = "./OpenWalnut - framebuffer.png";
        //std::string filename = ( QDir::homePath() + QDir::separator() + "OpenWalnut - framebuffer.png" ).toStdString();

        osgDB::writeImageFile( *image, filename );

        m_module->m_debugTrigger2->set( WPVBaseTypes::PV_TRIGGER_READY, false );
    }
    if( m_module->m_vrOn->changed(true) )
    {
        if( m_module->m_vrOn->get() )
        {
            m_module->debugLog() << "Start submitting frames to OpenVR";
        }
        else
        {
            m_module->debugLog() << "Stop submitting frames to OpenVR";
        }
    }
    if( !m_initialUpdate && m_module->m_vrOn->get() && m_module->m_vrIsInitialized )
    {
        //get OpenVR tracking information
        vr::TrackedDevicePose_t poses[vr::k_unMaxTrackedDeviceCount];           // NOLINT: the size is constant
        for(uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i) poses[i].bPoseIsValid = false;
        vr::VRCompositor()->WaitGetPoses( poses, vr::k_unMaxTrackedDeviceCount, NULL, 0 );

        unsigned int leftContextID = WKernel::getRunningKernel()->getGraphicsEngine()
                                        ->getViewerByName( "Left Eye View" )->getCamera()
                                        ->getGraphicsContext()->getState()->getContextID();
        unsigned int rightContextID = WKernel::getRunningKernel()->getGraphicsEngine()
                                        ->getViewerByName( "Right Eye View" )->getCamera()
                                        ->getGraphicsContext()->getState()->getContextID();

        vr::Texture_t leftEyeTexture = {
            ( void* )( uintptr_t )m_module->m_textureColorLeft->getTextureObject( leftContextID )->id(),
            vr::TextureType_OpenGL,
            vr::ColorSpace_Gamma
            };
        vr::Texture_t rightEyeTexture = {
            ( void* )( uintptr_t )m_module->m_textureColorRight->getTextureObject( rightContextID )->id(),
            vr::TextureType_OpenGL,
            vr::ColorSpace_Gamma
            };

        vr::EVRCompositorError lError = vr::VRCompositor()->Submit( vr::Eye_Left, &leftEyeTexture );
        vr::EVRCompositorError rError = vr::VRCompositor()->Submit( vr::Eye_Right, &rightEyeTexture );

        if( lError != vr::VRCompositorError_None || rError !=vr::VRCompositorError_None )
        {
            m_module->errorLog() << "Links:" << lError << "|Rechts:" << rError << std::endl;
        }
    }
    m_initialUpdate = false;

    traverse( node, nv );
}
