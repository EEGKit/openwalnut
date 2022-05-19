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

#version 150 core

#include "WGEShader-attributes.glsl"
#include "WGEShader-uniforms.glsl"

#include "WGETransformationTools.glsl"

out vec4 v_color;

/////////////////////////////////////////////////////////////////////////////
// Varyings
/////////////////////////////////////////////////////////////////////////////

// The ray's starting point in texture space
out vec3 v_rayStart;

// The ray direction in texture space
out vec3 v_ray;

// the Surface normal at this point
out vec3 v_normal;

// The light source in local coordinates
out vec3 v_lightSource;


/////////////////////////////////////////////////////////////////////////////
// Uniforms
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Attributes
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Variables
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Functions
/////////////////////////////////////////////////////////////////////////////

/**
 * Main entry point of the vertex shader.
 */
void main()
{
    // for easy access to texture coordinates
    // gl_TexCoord[0] = osg_MultiTexCoord0;
    // gl_TexCoord[1] = osg_MultiTexCoord1;

    v_normal = osg_Normal;

    // in texture space, the starting point simply is the current surface point in texture space
    v_rayStart = osg_MultiTexCoord0.xyz; // this equals osg_Vertex!

    // transform the ray direction to texture space, which equals object space
    // Therefore use two points, as we transform a vector
    vec4 camLookAt = vec4( 0.0, 0.0, -1.0, 1.0 );
    vec4 camPos    = vec4( 0.0, 0.0, 0.0, 1.0 );
    v_ray = worldToLocal( camLookAt, camPos ).xyz;

    vec4 lpos = ow_lightsource;
    lpos = vec4( 0.0, 0.0, 1000.0, 1.0 );
    v_lightSource = worldToLocal( lpos ).xyz;

    // Simply project the vertex
    gl_Position = osg_ModelViewProjectionMatrix * osg_Vertex;
    v_color = osg_Color;
    // gl_FrontColor = gl_FrontMaterial.diffuse;
    // gl_BackColor = gl_BackMaterial.diffuse;
}

