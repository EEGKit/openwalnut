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

#include "WGEShadingTools.glsl"
#include "WGETextureTools.glsl"
#include "WGETransformationTools.glsl"
#include "WGEUtils.glsl"

#include "WGEColorMaps.glsl"

#include "WMImageSpaceLIC-Transformation-varyings.glsl"

/**
 * The texture unit sampler
 */
uniform sampler3D u_texture0Sampler;

/**
 * Scaling factor to unscale the texture
 */
uniform float u_texture0Scale;

/**
 * Smallest possible value in the texture
 */
uniform float u_texture0Min;

/**
 * Size of input texture in pixels
 */
uniform int u_texture0SizeX;

/**
 * Size of input texture in pixels
 */
uniform int u_texture0SizeY;

/**
 * Size of input texture in pixels
 */
uniform int u_texture0SizeZ;

/**
 * Transforms each vector on each pixel to image space.
 */
void main()
{
    vec2 vecProjected;    // contains the final vector at each fragment

#ifdef VECTORDATA
    // get the current vector at this position
    vec3 vec = texture3DUnscaled( u_texture0Sampler, gl_TexCoord[0].xyz, u_texture0Min, u_texture0Scale ).rgb;

    // zero length vectors are uninteresting. discard them
    if ( isZero( length( vec ), 0.01 ) )
    {
        discard;
    }

    // project the vectors to image space
    vecProjected = projectVector( vec ).xy;

#endif
#ifdef SCALARDATA

    // do central differences to get the vector
    // project the vectors to image space

    float sx = 1.0 / u_texture0SizeX;
    float sy = 1.0 / u_texture0SizeY;
    float sz = 1.0 / u_texture0SizeZ;
    float valueXP = texture3DUnscaled( u_texture0Sampler, gl_TexCoord[0].xyz + vec3( sx, 0.0, 0.0 ), u_texture0Min, u_texture0Scale ).r;
    float valueXM = texture3DUnscaled( u_texture0Sampler, gl_TexCoord[0].xyz - vec3( sx, 0.0, 0.0 ), u_texture0Min, u_texture0Scale ).r;
    float valueYP = texture3DUnscaled( u_texture0Sampler, gl_TexCoord[0].xyz + vec3( 0.0, sy, 0.0 ), u_texture0Min, u_texture0Scale ).r;
    float valueYM = texture3DUnscaled( u_texture0Sampler, gl_TexCoord[0].xyz - vec3( 0.0, sy, 0.0 ), u_texture0Min, u_texture0Scale ).r;
    float valueZP = texture3DUnscaled( u_texture0Sampler, gl_TexCoord[0].xyz + vec3( 0.0, 0.0, sz ), u_texture0Min, u_texture0Scale ).r;
    float valueZM = texture3DUnscaled( u_texture0Sampler, gl_TexCoord[0].xyz - vec3( 0.0, 0.0, sz ), u_texture0Min, u_texture0Scale ).r;

    vec3 dir = vec3( valueXP - valueXM, valueYP - valueYM, valueZP - valueZM );

    // zero length vectors are uninteresting. discard them
    vecProjected = projectVector( dir ).xy;
    if ( isZero( length( dir ), 0.01 ) )
    {
        discard;
    }

#endif

    // calculate lighting for the surface
    // TODO(ebaum): material properties should be used instead
    float light = blinnPhongIlluminationIntensity(
            0.2,
            1.0,
            1.3,
            10.0,
            1.0,
            0.3,
            -normalize( v_normal ),
            normalize( v_viewDir ),
            normalize( v_lightSource ) );

    gl_FragData[0] = vec4( textureNormalize( vecProjected ), light, 1.0 );
    gl_FragData[1] = vec4( texture3D( u_texture0Sampler, gl_TexCoord[0].xyz ).rgb , 1.0 );
}

