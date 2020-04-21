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

#include <osg/StateAttribute>
#include <osg/Texture2D>
#include <osg/TexMat>

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

    // show hud?
    m_showHUD = m_properties->addProperty( "Show HUD", "Check to enable the debugging texture HUD.", true );

    WModule::properties();
}

void WMVRCamera::requirements()
{
    // NOTE: Refer to WMTemplate.cpp if you do not understand these commands.

    // We need graphics to draw anything:
    m_requirements.push_back( new WGERequirement() );
}

void WMVRCamera::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );

    // Now, we can mark the module ready.
    ready();
    debugLog() << "Starting...";

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Eye Setup
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    // create the root node for all the geometry
    m_output = osg::ref_ptr< WGEManagedGroupNode > ( new WGEManagedGroupNode( m_active ) );

    // create the roots for the Eyes
    m_leftEye = osg::ref_ptr< WGEGroupNode > ( new WGEGroupNode() );
    m_rightEye = osg::ref_ptr< WGEGroupNode > ( new WGEGroupNode() );

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_output );

    // add for side-views
    boost::shared_ptr< WGEViewer > v = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Left Eye View" );
    if( v )
    {
        v->getScene()->insert( m_leftEye );
    }
    v = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Right Eye View" );
    if( v )
    {
        v->getScene()->insert( m_rightEye );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Main loop
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        // woke up since the module is requested to finish
        if( m_shutdownFlag() )
        {
            break;
        }

        // create the Eyes. This loop is only entered if WGEColormapper was fired or shutdown.
        // initOSG();

        debugLog() << "Waiting...";

        m_moduleState.wait();
    }

    // Never miss to clean up. Especially remove your OSG nodes. Everything else you add to these nodes will be removed automatically.
    debugLog() << "Shutting down VRCamera";

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_output );

    v = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Left Eye View" );
    if( v )
    {
        v->getScene()->remove( m_leftEye );
    }

    v = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Right Eye View" );
    if( v )
    {
        v->getScene()->remove( m_rightEye );
    }
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
