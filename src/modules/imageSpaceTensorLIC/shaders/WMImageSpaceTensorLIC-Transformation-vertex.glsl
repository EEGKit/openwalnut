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


out vec4 ow_texCoord0;
out vec4 ow_texCoord1;
out vec4 ow_texCoord2;

/**
 * The normal interpolated
 */
out vec3 v_normal;

/**
 * The vertex
 */
out vec3 v_vertex;

/**
 * Normal in object space
 */
out vec3 v_normalObject;

/**
 * The normal interpolated and projected to screenspace
 */
out vec3 v_normalProjected;

/**
 * The light source in local coordinates
 */
out vec3 v_lightSource;

/**
 * The light source in local coordinates
 */
out vec3 v_viewDir;

/**
 * The factor which scales the 3d noise texture to a proper size according to screen size.
 */
out vec3 v_noiseScaleFactor;


/**
 * How much should the slice be moved along u_vertexShiftDirection?
 */
uniform int u_vertexShift = 0;

/**
 * The direction along which the slice gets moved
 */
uniform vec3 u_vertexShiftDirection = vec3( 0.0, 0.0, 0.0 );

/**
 * Size of input texture in pixels
 */
uniform int u_texture0SizeX = 255;

/**
 * Size of input texture in pixels
 */
uniform int u_texture0SizeY = 255;

/**
 * Size of input texture in pixels
 */
uniform int u_texture0SizeZ = 255;

/**
 * Size of input noise texture in pixels
 */
uniform int u_texture1SizeX = 255;

/**
 * Size of input noise texture in pixels
 */
uniform int u_texture1SizeY = 255;

/**
 * Size of input noise texture in pixels
 */
uniform int u_texture1SizeZ = 255;

/**
 * Vertex Main. Simply transforms the geometry. The work is done per fragment.
 */
void main()
{
    // Calculate the real vertex coordinate in openwalnut-scene-space
    vec4 vertex = osg_Vertex;
#ifdef VERTEXSHIFT_ENABLED
    // Calculate the real vertex coordinate in openwalnut-scene-space
    vertex = ( vec4( u_vertexShiftDirection.xyz, 0.0 ) * u_vertexShift ) + osg_Vertex;
#endif

    // Allow the colormapper to do some precalculations with the real vertex coordinate in ow-scene-space
    colormapping( vertex );

    // for easy access to texture coordinates
    // NOTE: The vertex is specified in ow-scene-space. The texture matrix was set by WGEDataTexture for the dataset and transforms the vertex in
    // ow-scene-space to the textures space.
    ow_texCoord0 = ow_TextureMatrix0 * vertex;
    ow_texCoord1 = ow_TextureMatrix1 * vertex;
    ow_texCoord2 = ow_TextureMatrix2 * vertex;
    //gl_TexCoord[3] = gl_TextureMatrix[3] * vertex;

    // some light precalculations
    v_normal = osg_NormalMatrix * osg_Normal;
    v_normalObject = osg_Normal;
    v_vertex = osg_Vertex.xyz;

    /*
    // NOTE: we normalize the vec here although we need to normalize it again in the fragment shader since the projected vector might be very
    // small and we want to avoid numerical issues when interpolating.
    v_normalProjected = normalize( projectVector( vec4( v_normal, 0.0 ) ) );

    // if we use 3d noise textures and auto resolution feature:
    vec4 vec1 = vec4( float( u_texture0SizeX ) / float( u_texture1SizeX ),
                      float( u_texture0SizeY ) / float( u_texture1SizeY ),
                      float( u_texture0SizeZ ) / float( u_texture1SizeZ ), 0.0 );
    v_noiseScaleFactor = vec1.xyz * u_noiseResoultuion;
#ifdef NOISE3DAUTORES_ENABLED
    vec4 vecMV = osg_ModelViewMatrix * vec4( vec3( 1.0 ), 0.0 ); // if the resolution should be modified, use the scaling info from the MV matrix
    v_noiseScaleFactor *= u_noiseResoultuion * length( vecMV.xyz );
#endif
*/

    // also get the coordinates of the light
    vec4 lpos = ow_lightsource; // this simply doesn't work well with OSG
    lpos = vec4( 0.0, 0.0, 1000.0, 1.0 );
    v_lightSource = worldToLocal( lpos ).xyz;

    // transform the view direction to texture space, which equals object space
    // Therefore use two points, as we transform a vector
    vec4 camPos    = vec4( 0.0, 0.0, -1.0, 0.0 );
    v_viewDir = worldToLocal( camPos ).xyz;

    // transform position
    gl_Position = osg_ModelViewProjectionMatrix * vertex;
}

