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
#extension GL_EXT_gpu_shader4 : enable

#include "WGEUtils.glsl"

/**
 * The texture Unit for the projected vectors
 */
uniform sampler2D u_texture0Sampler;

/**
 * The texture Unit for the edges and noise
 */
uniform sampler2D u_texture1Sampler;

/**
 * Size of advection texture in pixels
 */
uniform int u_texture0SizeX;

/**
 * Size of advection texture in pixels
 */
uniform int u_texture0SizeY;

/**
 * Size of advection texture in pixels
 */
uniform int u_texture0SizeZ;

/**
 * The blending ratio between noise and advected noise
 */
uniform float u_noiseRatio;

/**
 * Number of iterations per frame.
 */
uniform int u_numIter;

/**
 * Main. Calculates the Laplace Filter for each pixel.
 */
void main()
{
    vec2 texCoord = gl_TexCoord[0].st;
    
    // get some needed values
    float edge  = texture2D( u_texture1Sampler, texCoord ).r;
    float depth = texture2D( u_texture1Sampler, texCoord ).g;
    float noise = texture2D( u_texture1Sampler, texCoord ).b;
    vec2 vec    = ( 2.0 * ( texture2D( u_texture0Sampler, texCoord ).rg - vec2( 0.5, 0.5 ) ) );

    // simply iterate along the line using the vector at each point
    vec2 lastVec = vec;
    vec2 lastPos = gl_TexCoord[0].st;
    float sum = 0.0;
    for ( int i = 0; i < u_numIter; ++i )
    {
        vec2 newPos = lastPos - vec2( lastVec.x / ( 2.0 * u_texture0SizeX), lastVec.y / ( 2.0 * u_texture0SizeY) );
        vec2 newVec = ( 2.0 * ( texture2D( u_texture0Sampler, newPos ).rg - vec2( 0.5, 0.5 ) ) );
        if ( length( newVec ) < 0.01 )
            break;

        // it is also possible to scale using a Geometric progression: float( u_numIter - i ) / u_numIter * texture2D
        sum += texture2D( u_texture1Sampler, newPos ).b;

        lastPos = newPos;
        lastVec = newVec;
    }

    // the sum needs to be scaled to [0,1] again
    float n = ( sum ) / float( u_numIter );
    if ( depth > 0.99 )
    {
        n = noise;
    }

    // finally, blend noise and old noise
    gl_FragColor = vec4( ( n * ( 1.0 - u_noiseRatio ) ) + ( noise * u_noiseRatio ) );
}

