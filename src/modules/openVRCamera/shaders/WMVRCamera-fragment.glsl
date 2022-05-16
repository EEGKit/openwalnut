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

#include "WGEShadingTools.glsl"

/**
 * The texture Unit for the original color field
 */
uniform sampler2D u_texture0Sampler;
#define u_colorSampler u_texture0Sampler

/**
 * The depth texture
 */
uniform sampler2D u_texture1Sampler;
#define u_depthSampler u_texture1Sampler

vec4 getColor( in vec2 where )
{
    return texture2D( u_colorSampler, where );
}

float getDepth( in vec2 where )
{
    return texture2D( u_depthSampler, where ).r;
}


// The surface normal
in vec3 v_normal;
in vec4 v_texcoord;

vec2 pixelCoord = v_texcoord.st;

void main()
{
    // float light = blinnPhongIlluminationIntensity( normalize( viewAlign( v_normal ) ) );
    // finally set the color and depth
    // NOTE: you should remember that, when outputting to more than one texture (if you attached multiple color outputs), you need to use
    // gl_FragData instead of gl_FragColor!
    // gl_FragData[0] = vec4( light, 0.8, 0.8, 0.8 );
    // gl_FragData[0].rgb = light * vec3( 0.5, 1.0, 0.5 );
    // gl_FragData[0].a = 0.8;

    vec4 finalColor = getColor( pixelCoord );

    float depth = getDepth( pixelCoord );
    // if( finalColor.a == 0.0 )
    // {
    //    finalColor = vec4( 1.0, 1.0, 1.0, 1.0 );
    // }

    gl_FragColor = mix( vec4( 1.0 ), finalColor, finalColor.a );
    gl_FragDepth = depth;
}

