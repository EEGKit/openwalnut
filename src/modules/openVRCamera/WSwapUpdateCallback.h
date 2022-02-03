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

#ifndef WSWAPUPDATECALLBACK_H
#define WSWAPUPDATECALLBACK_H

#include <chrono>

#include <osg/GraphicsContext>

#include "core/graphicsEngine/WGEZoomTrackballManipulator.h"
#include "core/kernel/WKernel.h"

#include "WMVRCamera.h"

class WMVRCamera; //!< Forward declaration of the module class.

/**
 * Callback which submits the textures to OpenVR SDK.
 */
class WSwapUpdateCallback : public osg::GraphicsContext::SwapCallback
{
public: // NOLINT
    /**
     * Constructor.
     *
     * \param module just set the creating module as pointer for later reference.
     */
    explicit WSwapUpdateCallback( WMVRCamera* module ): m_module( module ), m_initialUpdate( true )
    {
    };

    /**
     * This method is called whenever the buffers are swapped
     * \param gc The GraphicsContext of this callback
     */
    void swapBuffersImplementation( osg::GraphicsContext* gc );

    /**
     * Updates the frame counter, calculates the FPS and handles the FPS logging.
     */
    void handleFPSCounter();

    /**
     * Submits the current frame to the VR-Headset.
     */
    void submitFrame();

    /**
     * Updates the ViewMatrix of the eyes to the current pose of the VR-Headset.
     */
    void updateRTTTransformation();

    /**
     * Pointer used to access members of the module to modify the node.
     * Please do not use shared_ptr here as this would prevent deletion of the module as the callback contains
     * a reference to it. It is safe to use a simple pointer here as callback get deleted before the module.
     */
    WMVRCamera* m_module;

    /**
     * timestamp of the last frame
     */
    std::chrono::_V2::system_clock::time_point m_lastFrame;

    /**
     * timestamps of the last 120 frames
     */
    double m_lastFrames[120];

    /**
     * current position in m_lastFrames for saving the next timestamp
     */
    unsigned int m_frameCounter = 0;

    /**
     * Denotes whether the update callback is called the first time. It is especially useful
     * to set some initial value even if the property has not yet changed.
     */
    bool m_initialUpdate;
};

#endif  // WSWAPUPDATECALLBACK_H
