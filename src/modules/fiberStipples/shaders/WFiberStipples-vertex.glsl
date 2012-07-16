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

#include "WGETextureTools.glsl"

/**
 * These two uniforms are needed to transform the vectors out of their texture
 * back to their original form as they are stored in RBGA (for example allowing
 * only values between 0..1 for components but no negative ones).
 */
uniform float u_vectorsMin; uniform float u_vectorsScale;

/**
 * The matrix describes the transformation of gl_Vertex to OpenWalnut Scene Space
 */
uniform mat4 u_WorldTransform;

/**
 * Vector dataset as texture.
 */
uniform sampler3D u_vectorsSampler;

/**
 * Probabilistic tract as texture.
 */
uniform sampler3D u_probTractSampler;

/**
 * Number of voxels in X direction.
 */
uniform int u_probTractSizeX;

/**
 * Number of voxels in Y direction.
 */
uniform int u_probTractSizeY;

/**
 * Number of voxels in Z direction.
 */
uniform int u_probTractSizeZ;

// For correct transformation into texture space we also need the size of each voxel.
/**
 * Voxel size in X direction.
 */
uniform float u_pixelSizeX;

/**
 * Voxel size in Y direction.
 */
uniform float u_pixelSizeY;

/**
 * Voxel size in Z direction.
 */
uniform float u_pixelSizeZ;

// vectors spanning the plane of the quad
uniform vec3 u_aVec;
uniform vec3 u_bVec;

/**
 * First focal point, which is one of the endings of the projected diffusion direction.
 */
varying vec3 focalPoint1;

/**
 * Second focal point, which is one of the endings of the projected diffusion direction.
 */
varying vec3 focalPoint2;

// Scaled focal points, as otherwise the the stipple endings may not fit inside quad.
/**
 * Fixed factor for scaling.
 */
uniform float scale = 0.8;

/**
 * First focal point, scaled.
 */
varying vec3 scaledFocalPoint1;

/**
 * Second focal poin, scaled.
 */
varying vec3 scaledFocalPoint2;

/**
 * Maximum connectivity score withing the probabilistic tract dataset. This is
 * needed for scaling the connectivities between 0.0 and 1.0.
 */
uniform float u_maxConnectivityScore;

/**
 * Scaled connectivity score; now between 0.0...1.0.
 */
varying float probability;

/**
 * Probabilities below this threshold are ignored and discarded.
 */
uniform float u_threshold;

/**
 * Middle point of the quad in texture coordinates, needed for scaling the
 * projection of the principal diffusion direction to fit inside quad.
 */
uniform vec3 middlePoint_tex = vec3( 0.5, 0.5, 0.0 );

/**
 * Vertex Main. Simply transforms the geometry. The work is done per fragment.
 */
void main()
{
    gl_TexCoord[0] = gl_MultiTexCoord0; // for distinguishing the verties of the quad
    gl_TexCoord[1] = gl_MultiTexCoord1; // for coordinate system within fragment shader (enable unit quad coordinates)

    // compute texture coordinates from worldspace coordinates for texture access
    vec3 texturePosition = ( u_WorldTransform * gl_Vertex ).xyz;
    texturePosition.x /= u_pixelSizeX * u_probTractSizeX;
    texturePosition.y /= u_pixelSizeY * u_probTractSizeY;
    texturePosition.z /= u_pixelSizeZ * u_probTractSizeZ;

    // get connectivity score from probTract and with maximum value scale it between 0.0..1.0. from now on we call it probability
    probability = texture3D( u_probTractSampler, texturePosition ).r / float( u_maxConnectivityScore );

    // span quad incase of regions with high probablility
    if( probability > u_threshold ) // rgb are the same
    {
         // transform position, the 4th component must be explicitly set, as otherwise they would have been scaled
         gl_Position = gl_ModelViewProjectionMatrix * ( vec4( gl_TexCoord[0].xyz + gl_Vertex.xyz, 1.0 ) );
    }
    else
    {
         gl_Position = ftransform(); // discard those vertices
    }

    // get principal diffusion direction
    vec3 diffusionDirection = abs( texture3DUnscaled( u_vectorsSampler, texturePosition, u_vectorsMin, u_vectorsScale ).xyz );
    diffusionDirection = normalize( diffusionDirection );

    // project into plane (given by two vectors aVec and bVec)
    vec3 normal = normalize( cross( u_aVec, u_bVec ) );
    vec3 projectedDirection = diffusionDirection - dot( diffusionDirection, normal ) * normal;

    vec3 projectedDirectionTextCoords = 0.5 * vec3( dot( normalize( u_aVec ), projectedDirection ),
                                                    dot( normalize( u_bVec ), projectedDirection ),
                                                    0.0 );
    scaledFocalPoint1 = middlePoint_tex + scale * projectedDirectionTextCoords;
    scaledFocalPoint2 = middlePoint_tex - scale * projectedDirectionTextCoords;
    // but scale directions to fit 1x1 unit square, these 0.8 are just arbitrary choosen
    focalPoint1 = middlePoint_tex + projectedDirectionTextCoords;
    focalPoint2 = middlePoint_tex - projectedDirectionTextCoords;
}
