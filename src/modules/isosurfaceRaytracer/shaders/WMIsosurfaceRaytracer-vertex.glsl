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

#include "WGEColormapping-vertex.glsl"

#include "WGETransformationTools.glsl"


/////////////////////////////////////////////////////////////////////////////
// Varyings
/////////////////////////////////////////////////////////////////////////////

// The ray's starting point in texture space
out vec3 v_rayStart;

// The ray direction in texture space, normalized
out vec3 v_ray;

// The sampling distance
out float v_sampleDistance;

// The steps in relation to a default number of steps of 128.
out float v_relativeSampleDistance;

// the Surface normal at this point
out vec3 v_normal;

// The isovalue scaled using texture scaling information to [0,1]
out float v_isovalue;

// The scaling component of the modelview matrix.
out float v_worldScale;

// The position of the camera
flat out vec3 v_eyePos;

out vec4 v_color;

/////////////////////////////////////////////////////////////////////////////
// Uniforms
/////////////////////////////////////////////////////////////////////////////
uniform int u_steps;

// scaling factor of the data in the texture. Needed for descaling.
uniform float u_texture0Scale;

// minimum value in texture. Needed for descaling.
uniform float u_texture0Min;

// The isovalue to use.
uniform float u_isovalue;

/**
 * Main entry point of the vertex shader.
 */
void main()
{
    colormapping();

    v_isovalue = ( u_isovalue - u_texture0Min ) / u_texture0Scale;

    v_normal = osg_Normal;

    // to have equidistant sampling for each side of the box, use a fixed step size
    v_sampleDistance = 1.0 / float( u_steps );
    v_relativeSampleDistance = 128.0 /  float( u_steps );

    // in texture space, the starting point simply is the current surface point in texture space
    v_rayStart = osg_Vertex.xyz;

    if( osg_ProjectionMatrix[3][3] == 1.0 )
    {
        // orthographic:
        // transform the ray direction to texture space, which equals object space
        // Therefore use two points, as we transform a vector
        vec4 camLookAt = vec4( 0.0, 0.0, -1.0, 0.0 );
        vec4 camPos    = vec4( 0.0, 0.0, 0.0, 0.0 );
        v_ray = normalize( worldToLocal( camLookAt, camPos ).xyz );
    }
    else
    {
        // perspective:
        // calculate object space coordinate for camera
        // create vector between camera and vertex
        v_eyePos = ( inverse( osg_ModelViewMatrix ) * vec4( 0.0, 0.0, 0.0, 1.0 ) ).xyz;
        v_ray = vec3( 0.0 );
    }

    #ifdef WGE_POSTPROCESSING_ENABLED
        // Keep track of scaling information. This is needed by some postprocessors.
        v_worldScale = getModelViewScale();
    #endif

    v_color = osg_Color;
    gl_Position = osg_ModelViewProjectionMatrix * osg_Vertex;
}
