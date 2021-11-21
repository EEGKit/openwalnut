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

layout( lines_adjacency ) in;
layout( triangle_strip, max_vertices = 4 ) out;

uniform vec2 u_viewport;
uniform float u_thickness;

vec4 calcPoint( vec4 position, vec4 miter, float len )
{
    position.x *= u_viewport.x;
    position.y *= u_viewport.y;
    position = position + miter * len;
    position.x /= u_viewport.x;
    position.y /= u_viewport.y;
    return position;
}

void main()
{
    // calc vectors
    vec4 v1 = normalize( gl_in[1].gl_Position - gl_in[0].gl_Position );
    vec4 v2 = normalize( gl_in[2].gl_Position - gl_in[1].gl_Position );
    vec4 v3 = normalize( gl_in[3].gl_Position - gl_in[2].gl_Position );

    // calc normals
    vec4 n1 = vec4( v1.y, -v1.x, v1.z, v1.w );
    vec4 n2 = vec4( v2.y, -v2.x, v2.z, v2.w );
    vec4 n3 = vec4( v3.y, -v3.x, v3.z, v3.w );

    // calc miter lines
    vec4 m1 = normalize( n1 + n2 );
    vec4 m2 = normalize( n2 + n3 );

    // project miter lines on on normals
    float proj1 = dot( m1, n2 );
    float proj2 = dot( m2, n2 );
    if( proj1 == 0 )
    {
        proj1 = 1;
    }
    if( proj2 == 0 )
    {
        proj2 = 1;
    }

    // calc miter length
    float len1 = u_thickness / proj1;
    float len2 = u_thickness / proj2;

    // prevent too long miters
    if( dot( v1, v2 ) < -0.75 )
    {
        m1 = n2;
        len1 = u_thickness;
    }
    if( dot( v2, v3 ) < -0.75 )
    {
        m2 = n2;
        len2 = u_thickness;
    }

    gl_Position = calcPoint( gl_in[1].gl_Position, -m1, len1 );
    EmitVertex();

    gl_Position = calcPoint( gl_in[1].gl_Position,  m1, len1 );
    EmitVertex();

    gl_Position = calcPoint( gl_in[2].gl_Position, -m2, len2 );
    EmitVertex();

    gl_Position = calcPoint( gl_in[2].gl_Position,  m2, len2 );
    EmitVertex();


    EndPrimitive();
}
