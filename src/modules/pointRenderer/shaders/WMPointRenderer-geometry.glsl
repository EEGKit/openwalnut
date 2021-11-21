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

#include "WGEShader-uniforms.glsl"

#include "WGETransformationTools.glsl"

layout( points ) in;
layout( triangle_strip, max_vertices = 4 ) out;

/////////////////////////////////////////////////////////////////////////////
// Uniforms
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Attributes
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Varyings
/////////////////////////////////////////////////////////////////////////////

/**
 * The point size
 */
in float v_pointSize[];

/**
 * The normal/tangent in scene-space
 */
in vec3 v_normal[];

/**
 * Clip vertex if not 0.0
 */
in float v_clip[];

/**
 * Vertex in world space.
 */
in vec4 v_worldCenterVertex[];

/**
 * The incoming color from the vertex shader.
 */
in vec4 v_colorIn[];

/**
 * The vertex coordinates in world-space
 */
out vec3 v_vertex;

/**
 * The texture coordinates in [-1,1]
 */
out vec3 v_texCoord;

/**
 * The radius of a sphere in world-space
 */
out float v_worldSpaceRadius;

/**
 * The center point of the sphere
 */
out vec3 v_centerPoint;

/**
 * Vertex in world space.
 */
out vec4 v_worldVertex;

/**
 * Depth of the center point
 */
out float v_centerVertexDepth;

/**
 * Depth of the neareast point on the sphere towards the camera
 */
out float v_nearestVertexDepth;

/**
 * Difference between nearest depth and center point depth
 */
out float v_vertexDepthDiff;

/**
 * The world-space scaling factor
 */
out float v_worldScale;

/**
 * The output color of this primitive.
 */
out vec4 v_colorOut;

/**
 * Main entry point of the geometry shader.
 */
void main()
{
    // clip whole sprite
    if( v_clip[0] != 0.0 )
    {
        return;
    }

    // grab the info we got from the vertex array:
    vec3 vertex = gl_in[0].gl_Position.xyz;

    // the plane of the sprite is determined by the tangent ond two orthogonal vectors a and b
    // NOTE: the 1.0 corresponds to a radius of 1 (unit-sphere)
    vec3 a = vec3( 0.0, 1.0, 0.0 );
    vec3 b = vec3( 1.0, 0.0, 0.0 );

    v_colorOut = v_colorIn[0];

    v_worldScale = getModelViewScale();
    v_worldSpaceRadius = v_worldScale * v_pointSize[0];
    v_centerPoint = vertex;
    v_worldVertex = v_worldCenterVertex[0];

    // calculate the depth using the point on the sphere most near to the cam
    vec4 nearestVertex = vec4( v_centerPoint.xy, v_centerPoint.z - v_worldSpaceRadius, 1.0 );
    vec4 nearestVertexProjected = osg_ProjectionMatrix * nearestVertex;
    v_nearestVertexDepth = ( 0.5 * nearestVertexProjected.z / nearestVertexProjected.w ) + 0.5;

    vec4 centerVertex = vec4( v_centerPoint.xyz, 1.0 );
    vec4 centerVertexProjected = osg_ProjectionMatrix * centerVertex;
    v_centerVertexDepth = ( 0.5 * centerVertexProjected.z / centerVertexProjected.w ) + 0.5;

    v_vertexDepthDiff = v_centerVertexDepth - v_nearestVertexDepth;

    // vertex 1
    v_texCoord = ( -a ) + ( -b );
    v_vertex = v_centerPoint + v_worldSpaceRadius * v_texCoord;
    gl_Position = osg_ProjectionMatrix * vec4( v_vertex, 1.0 );
    EmitVertex();

    // vertex 2
    v_texCoord = ( -a ) + ( +b );
    v_vertex = v_centerPoint + v_worldSpaceRadius * v_texCoord;
    gl_Position = osg_ProjectionMatrix * vec4( v_vertex, 1.0 );
    EmitVertex();

    // vertex 3
    v_texCoord = ( +a ) + ( -b );
    v_vertex = v_centerPoint + v_worldSpaceRadius * v_texCoord;
    gl_Position = osg_ProjectionMatrix * vec4( v_vertex, 1.0 );
    EmitVertex();

    // vertex 4
    v_texCoord = ( +a ) + ( +b );
    v_vertex = v_centerPoint + v_worldSpaceRadius * v_texCoord;
    gl_Position = osg_ProjectionMatrix * vec4( v_vertex, 1.0 );
    EmitVertex();

    EndPrimitive();
}
