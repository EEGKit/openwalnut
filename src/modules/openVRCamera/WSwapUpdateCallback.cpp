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

#include "WSwapUpdateCallback.h"

void WSwapUpdateCallback::swapBuffersImplementation( osg::GraphicsContext* gc )
{
    handleFPSCounter();

    if( m_module->m_VR_logCameraViewMatrix->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
    {
        m_module->debugLog() << "Left Eye Camera view matrix: " << m_module->m_leftEyeCamera->getViewMatrix();
        m_module->debugLog() << "Right Eye Camera view matrix: " << m_module->m_rightEyeCamera->getViewMatrix();
        m_module->m_VR_logCameraViewMatrix->set( WPVBaseTypes::PV_TRIGGER_READY, false );
    }

    if( m_module->m_VR_resetHMDPosition->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
    {
        m_module->ResetHMDPosition();
        m_module->m_VR_resetHMDPosition->set( WPVBaseTypes::PV_TRIGGER_READY, false );
    }

    if( m_module->m_vrOn->changed( true ) )
    {
        if( m_module->m_vrOn->get() )
        {
            m_module->debugLog() << "Start submitting frames to OpenVR";
            vr::VRChaperone()->ResetZeroPose( vr::TrackingUniverseSeated );
        }
        else
        {
            m_module->debugLog() << "Stop submitting frames to OpenVR";
        }
    }

    if( m_module->m_vrOn->get() && m_module->m_vrIsInitialized )
    {
        if( !m_initialUpdate )
        {
            submitFrame();
        }

        updateRTTTransformation();
        // m_module->updateControllerPoses();
    }

    m_initialUpdate = false;

    gc->swapBuffersImplementation();
}

void WSwapUpdateCallback::handleFPSCounter()
{
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    double elapsedSeconds = ( now - m_lastFrame ).count() / 1.0e9;
    m_lastFrame = now;
    m_lastFrames[m_frameCounter++] = elapsedSeconds;
    if( m_frameCounter >= 120 )
    {
        m_frameCounter = 0;
    }
    double elapsedSecondsSum = 0;
    for( int i = 0; i <= 119; i++ )
    {
        elapsedSecondsSum += m_lastFrames[i];
    }
    double averageElapsedSeconds = elapsedSecondsSum / 120.0;

    if( m_module->m_VR_fpsTrigger->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
    {
        m_module->debugLog() << elapsedSeconds << " sec since last Frame | " << 1 / elapsedSeconds << " fps";
        m_module->debugLog() << averageElapsedSeconds << " sec average Frametiming | " << 1 / averageElapsedSeconds << " fps";

        m_module->m_VR_fpsTrigger->set( WPVBaseTypes::PV_TRIGGER_READY, false );
    }
}

void WSwapUpdateCallback::submitFrame()
{
    m_module->updateDeviceIDs();

    // We only need to get the context from the left eye camera as the right eye camera is in the same context
    unsigned int ctxID = m_module->m_leftEyeCamera->getGraphicsContext()->getState()->getContextID();

    vr::Texture_t leftEyeTexture = {
        ( void * )( uintptr_t ) m_module->m_leftEyeCamera->getTextureColor()->getTextureObject( ctxID )->id(),
        vr::TextureType_OpenGL,
        vr::ColorSpace_Gamma
    };
    vr::Texture_t rightEyeTexture = {
        ( void * )( uintptr_t ) m_module->m_rightEyeCamera->getTextureColor()->getTextureObject( ctxID )->id(),
        vr::TextureType_OpenGL,
        vr::ColorSpace_Gamma
    };

    vr::EVRCompositorError lError = vr::VRCompositor()->Submit( vr::Eye_Left, &leftEyeTexture );
    vr::EVRCompositorError rError = vr::VRCompositor()->Submit( vr::Eye_Right, &rightEyeTexture );

    if( lError != vr::VRCompositorError_None || rError != vr::VRCompositorError_None )
    {
        m_module->errorLog() << "Links:" << lError << "|Rechts:" << rError << std::endl;
    }
}

void WSwapUpdateCallback::updateRTTTransformation()
{
    m_module->updateHMDPose();

    osg::Quat rot = m_module->m_cameraRotation * m_module->m_HMD_rotation;
    osg::Vec3d trans = m_module->m_cameraPosition + m_module->m_HMD_position;

    osg::Matrixd leftEyeOffsetMatrix = m_module->convertHmdMatrixToOSG( m_module->m_vrSystem->GetEyeToHeadTransform( vr::Eye_Left ) );
    osg::Matrixd rightEyeOffsetMatrix = m_module->convertHmdMatrixToOSG( m_module->m_vrSystem->GetEyeToHeadTransform( vr::Eye_Right ) );

    osg::Matrixd headMatrix = osg::Matrixd::translate( -trans ) * osg::Matrixd::rotate( rot );
    osg::Matrixd leftEyeMatrix = headMatrix * osg::Matrixd::translate( leftEyeOffsetMatrix.getTrans() * -100 );
    osg::Matrixd rightEyeMatrix = headMatrix * osg::Matrixd::translate( rightEyeOffsetMatrix.getTrans() * -100 );

    m_module->m_leftEyeCamera->setViewMatrix( leftEyeMatrix );
    m_module->m_rightEyeCamera->setViewMatrix( rightEyeMatrix );
}
