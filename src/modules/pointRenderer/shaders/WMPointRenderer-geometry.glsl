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

#version 150

//#include "WGETransformationTools.glsl"

layout( points ) in;
layout( triangle_strip, max_vertices = 4 ) out;

uniform mat4 osg_ProjectionMatrix;
uniform mat4 osg_ModelViewMatrix;

in float v_pointSize[];
in vec3 v_normal[];
in float v_clip[];
in vec4 v_worldCenterVertex[];
in vec4 vColor[];

out vec3 v_vertex;
out vec3 v_texCoord;
out float v_worldSpaceRadius;
out vec3 v_centerPoint;
out vec4 v_worldVertex;
out float v_centerVertexDepth;
out float v_nearestVertexDepth;
out float v_vertexDepthDiff;
out float v_worldScale;
out vec4 fColor;

void main()
{
    if( v_clip[0] != 0.0 )
    {
        return;
    }

    vec3 vertex = gl_in[0].gl_Position.xyz;

    vec3 a = vec3( 0.0, 1.0, 0.0 );
    vec3 b = vec3( 1.0, 0.0, 0.0 );

    fColor = vColor[0];

    // World scale problem because of include
    v_worldScale = 1.0;
    v_worldSpaceRadius = v_worldScale * v_pointSize[0];
    v_centerPoint = vertex;
    v_worldVertex = v_worldCenterVertex[0];

    vec4 nearestVertex = vec4( v_centerPoint.xy, v_centerPoint.z - v_worldSpaceRadius, 1.0 );
    vec4 nearestVertexProjected = osg_ProjectionMatrix * nearestVertex;
    v_nearestVertexDepth = ( 0.5 * nearestVertexProjected.z / nearestVertexProjected.w ) + 0.5;

    vec4 centerVertex = vec4( v_centerPoint.xyz, 1.0 );
    vec4 centerVertexProjected = osg_ProjectionMatrix * centerVertex;
    v_centerVertexDepth = ( 0.5 * centerVertexProjected.z / centerVertexProjected.w ) * 0.5;

    v_vertexDepthDiff = v_centerVertexDepth - v_nearestVertexDepth;

    v_texCoord = ( -a ) + ( -b );
    v_vertex = v_centerPoint + v_worldSpaceRadius * v_texCoord;
    gl_Position = osg_ProjectionMatrix * vec4( v_vertex, 1.0 );
    EmitVertex();

    v_texCoord = ( -a ) + ( +b );
    v_vertex = v_centerPoint + v_worldSpaceRadius * v_texCoord;
    gl_Position = osg_ProjectionMatrix * vec4( v_vertex, 1.0 );
    EmitVertex();

    v_texCoord = ( +a ) + ( -b );
    v_vertex = v_centerPoint + v_worldSpaceRadius * v_texCoord;
    gl_Position = osg_ProjectionMatrix * vec4( v_vertex, 1.0 );
    EmitVertex();

    v_texCoord = ( +a ) + ( +b );
    v_vertex = v_centerPoint + v_worldSpaceRadius * v_texCoord;
    gl_Position = osg_ProjectionMatrix * vec4( v_vertex, 1.0 );
    EmitVertex();

    EndPrimitive();
}
