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

#version 120

/**
 * The texture Unit for the advection texture
 */
uniform sampler2D u_texture0Sampler;

/**
 * The texture Unit for the edge, depht texture
 */
uniform sampler2D u_texture1Sampler;

/**
 * The texture Unit for the edge, depht texture
 */
uniform sampler2D u_texture2Sampler;

/**
 * Used to en-/disable lighting.
 */
uniform bool u_useLight;

/**
 * Used to en-/disable blending of edges.
 */
uniform bool u_useEdges;

/**
 * Ratio between colormap and advected noise
 */
uniform float u_cmapRatio;

/**
 * Main. Clips and Blends the final image space rendering with the previously acquired 3D information
 */
void main()
{
    vec2 texCoord = gl_TexCoord[0].st;
    float edge  = texture2D( u_texture1Sampler, texCoord ).r * ( u_useEdges ? 1.0 : 0.0 );
    float light  = texture2D( u_texture1Sampler, texCoord ).b * ( u_useLight ? 1.0 : 0.0 );
    float depth  = texture2D( u_texture1Sampler, texCoord ).g;
    float advected  = texture2D( u_texture0Sampler, texCoord ).r;
    vec4 cmap = texture2D( u_texture2Sampler, texCoord );

    gl_FragColor = u_cmapRatio * cmap +
                   ( 1.0 - u_cmapRatio ) * vec4( vec3( edge + 2.0 * advected * advected ), 1.0 );
    gl_FragDepth = depth;
}

