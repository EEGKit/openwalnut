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
    m_aDouble          = m_properties->addProperty( "Shape radii", "Shape radii.", 20.0, m_propCondition );
    WModule::properties();
}

void WMAndroidExample::requirements()
{
    m_requirements.push_back( new WGERequirement() );
}

static const char vertexShader[] =
    "varying vec3 v_normal;                                                    \n"
    "varying vec3 v_vertex;               \n"
    "varying vec3 v_light;               \n"
    "const vec3 lightPos      =vec3(0.0, 100.0, 0.0);                        \n"
    "void main() {                                                          \n"
    "    gl_Position   = gl_ModelViewProjectionMatrix * gl_Vertex;          \n"
    "    v_vertex = ( gl_ModelViewMatrix * gl_Vertex).xyz; \n"
    "    v_normal   = normalize(gl_NormalMatrix * -gl_Normal);            \n"
    "    v_light = normalize(gl_ModelViewMatrix*vec4(lightPos, 0.0)).xyz ;\n"
    "}                                                                      \n";

static const char fragmentShader[] =
    "precision mediump float;                  \n"
    "varying mediump vec3 v_normal;               \n"
    "varying mediump vec3 v_vertex;               \n"
    "varying mediump vec3 v_light;               \n"
    "const vec4 cessnaColor   =vec4(1.0, 0.5, 0.0, 1.0);                    \n"
    "const vec4 lightAmbient  =vec4(0.1, 0.1, 0.1, 1.0);                    \n"
    "const vec4 lightDiffuse  =vec4(0.4, 0.4, 0.4, 1.0);                    \n"
    "const vec4 lightSpecular =vec4(0.8, 0.8, 0.8, 1.0);                    \n"
    "void DirectionalLight(in vec3 normal,                                  \n"
    "                      in vec3 ecPos,                                   \n"
    "                      inout vec4 ambient,                              \n"
    "                      inout vec4 diffuse,                              \n"
    "                      inout vec4 specular)                             \n"
    "{                                                                      \n"
    "     float nDotVP;                                                     \n"
    "     vec3 L = v_light;   \n"
    "     nDotVP = max(0.0, dot(normal, L));                                \n"
    "                                                                       \n"
    "     if (nDotVP > 0.0) {                                               \n"
    "       vec3 E = normalize(-ecPos);                                     \n"
    "       vec3 R = normalize(reflect( L, normal ));                       \n"
    "       specular = pow(max(dot(R, E), 0.0), 16.0) * lightSpecular;      \n"
    "     }                                                                 \n"
    "     ambient  = lightAmbient;                                          \n"
    "     diffuse  = lightDiffuse * nDotVP;                                 \n"
    "}                                                                      \n"
    "void main() {                             \n"
    "    vec4 ambiCol = vec4(0.0);                                          \n"
    "    vec4 diffCol = vec4(0.0);                                          \n"
    "    vec4 specCol = vec4(0.0);                                          \n"
    "    vec4 ecPos    = vec4( v_vertex, 1.0 );        \n"
    "    DirectionalLight( v_normal, ecPos.xyz, ambiCol, diffCol, specCol);    \n"
    "    vec4 color = cessnaColor * (ambiCol + diffCol + specCol);               \n"
    "  gl_FragColor = color;                   \n"
    "}                                         \n";

void WMAndroidExample::moduleMain()
{
    debugLog() << "Entering moduleMain()";

    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );

    // Signal ready state. Now your module can be connected by the container, which owns the module.
    ready();
    debugLog() << "Module is now ready.";

    m_rootNode = new WGEManagedGroupNode( m_active );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );

    // Normally, you will have a loop which runs as long as the module should not shutdown. In this loop you can react on changing data on input
    // connectors or on changed in your properties.
    debugLog() << "Entering main loop";
    while( !m_shutdownFlag() )
    {
        // woke up since the module is requested to finish
        if( m_shutdownFlag() )
        {
            break;
        }

        debugLog() << "Creating new OSG node";

        // You should grab your values at the beginning of such calculation blocks, since the property might change at any time!
        int rows = 10;
        double radii = 10.0;

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

        // for GLES, we need a shader
        osg::Shader* vshader = new osg::Shader( osg::Shader::VERTEX, vertexShader );
        osg::Shader* fshader = new osg::Shader( osg::Shader::FRAGMENT, fragmentShader );

        osg::Program* prog = new osg::Program;
        prog->addShader( vshader );
        prog->addShader( fshader );
        newGeode->getOrCreateStateSet()->setAttribute( prog );

        // remove old node
        m_rootNode->remove( m_geode );
        m_geode = newGeode;
        // And insert the new node
        m_rootNode->insert( m_geode );

        // done. wait for events.
        debugLog() << "Waiting ...";
        m_moduleState.wait();
    }
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
}
