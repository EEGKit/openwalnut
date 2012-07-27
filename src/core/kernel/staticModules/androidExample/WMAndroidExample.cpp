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


#include <string>

#include <osg/ShapeDrawable>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Material>
#include <osg/StateAttribute>

#include "core/kernel/WKernel.h"
#include "core/common/WPathHelper.h"
#include "core/common/WPropertyHelper.h"
#include "core/graphicsEngine/WGEUtils.h"
#include "core/graphicsEngine/WGERequirement.h"

#include "WMAndroidExample.xpm"
#include "WMAndroidExample.h"

WMAndroidExample::WMAndroidExample():
    WModule()
{
}

WMAndroidExample::~WMAndroidExample()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMAndroidExample::factory() const
{
    return boost::shared_ptr< WModule >( new WMAndroidExample() );
}

const char** WMAndroidExample::getXPMIcon() const
{
    return template_xpm;
}

const std::string WMAndroidExample::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "Android Example";
}

const std::string WMAndroidExample::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    return "This module is intended to be a module template and an example for writing modules.";
}

void WMAndroidExample::connectors()
{
    // call WModule's initialization
    WModule::connectors();
}

void WMAndroidExample::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );
    m_aDouble          = m_properties->addProperty( "Shape radii",              "Shape radii.", 20.0, m_propCondition );
    WModule::properties();
}

void WMAndroidExample::requirements()
{
    m_requirements.push_back( new WGERequirement() );
}

void WMAndroidExample::moduleMain()
{
   // debugLog() << "Entering moduleMain()";

    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );

    // Signal ready state. Now your module can be connected by the container, which owns the module.
    //ready();
   // debugLog() << "Module is now ready.";

    m_rootNode = new WGEManagedGroupNode( m_active );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );

    // Normally, you will have a loop which runs as long as the module should not shutdown. In this loop you can react on changing data on input
    // connectors or on changed in your properties.
   //debugLog() << "Entering main loop";
 //   while( !m_shutdownFlag() )
   // {
        //debugLog() << "Waiting ...";
        //m_moduleState.wait();

        // woke up since the module is requested to finish
        //if( m_shutdownFlag() )
        //{
           // break;
       // }

        //debugLog() << "Creating new OSG node";

        // You should grab your values at the beginning of such calculation blocks, since the property might change at any time!
        int rows = 5;
        double radii = 5.0;

        osg::ref_ptr< osg::Geode > newGeode = new osg::Geode();
        for( int32_t i = 0; i < rows; ++i )
        {
            newGeode->addDrawable(
                    new osg::ShapeDrawable( new osg::Box(             osg::Vec3(  25, 128, i * 15 ), radii ) ) );
            newGeode->addDrawable(
                    new osg::ShapeDrawable( new osg::Sphere(          osg::Vec3(  75, 128, i * 15 ), radii ) ) );
            newGeode->addDrawable(
                    new osg::ShapeDrawable( new osg::Cone(            osg::Vec3( 125, 128, i * 15 ), radii, radii ) ) );
            newGeode->addDrawable(
                    new osg::ShapeDrawable( new osg::Cylinder(        osg::Vec3( 175, 128, i * 15 ), radii, radii ) ) );
            newGeode->addDrawable(
                    new osg::ShapeDrawable( new osg::Capsule(         osg::Vec3( 225, 128, i * 15 ), radii, radii ) ) );
        }

        //m_rootNode->remove( m_geode );
        m_geode = newGeode;
	std::cout << "Teste cndskf vlfndslkgv ldsnfdsngfkldsngflkodshfkldsnfglkds" << std::endl; 
        // And insert the new node
       // m_rootNode->insert( m_geode );
	m_rootNode->insert( newGeode );
   // }
    // WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
}
