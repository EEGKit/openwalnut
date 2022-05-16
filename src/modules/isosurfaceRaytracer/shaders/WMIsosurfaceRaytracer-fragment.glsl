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

#include "WGEColormapping-fragment.glsl"

#include "WGEShadingTools.glsl"
#include "WGETextureTools.glsl"
#include "WGEPostprocessing.glsl"

/////////////////////////////////////////////////////////////////////////////
// Varyings
/////////////////////////////////////////////////////////////////////////////

// The ray's starting point in texture space
in vec3 v_rayStart;

// The ray direction in texture space, normalized
in vec3 v_ray;

// The sampling distance
in float v_sampleDistance;

// The steps in relation to a default number of steps of 128.
in float v_relativeSampleDistance;

// the Surface normal at this point
in vec3 v_normal;

// The isovalue scaled using texture scaling information to [0,1]
in float v_isovalue;

// The scaling component of the modelview matrix.
in float v_worldScale;

// The position of the camera
flat in vec3 v_eyePos;

in vec4 v_color;

/////////////////////////////////////////////////////////////////////////////
// Uniforms
/////////////////////////////////////////////////////////////////////////////

// texture containing the data
uniform sampler3D u_texture0Sampler;

uniform int u_texture0SizeX;
uniform int u_texture0SizeY;
uniform int u_texture0SizeZ;

#ifdef STOCHASTICJITTER_ENABLED
// texture containing the stochastic jitter texture
uniform sampler2D u_texture1Sampler;

// Size in X direction in pixels
uniform int u_texture1SizeX;
#endif

#ifdef GRADIENTTEXTURE_ENABLED
uniform sampler3D u_gradientsSampler;
#endif

#ifdef BORDERCLIP_ENABLED
/**
 * The distance before the entry/exit point that should be clipped.
 */
uniform float u_borderClipDistance = 0.05;
#endif

// The number of steps to use.
uniform int u_steps;

// The alpha value to set
uniform float u_alpha;

// the ratio between normal color and the colormapping color.
uniform float u_colormapRatio;


/////////////////////////////////////////////////////////////////////////////
// Attributes
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Variables
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Functions
/////////////////////////////////////////////////////////////////////////////

/**
 * Method uses the ray direction (varying) and the ray entry point in the cube (varying) to calculate the exit point. This is lateron needed to
 * get a proper maximum distance along the ray.
 *
 * \param d out - this value will contain the maximum distance along the ray untill the end of the cube
 * \param rayStart in - the start point of the ray in the volume
 *
 * \return the end point
 */
vec3 findRayEnd( in vec3 rayStart, in vec3 ray, out float d )
{
    // we need to ensure the vector components are not exactly 0.0 since they are used for division
    vec3 r = ray + vec3( 0.000000001 );
    vec3 p = rayStart;

    // v_ray in cube coordinates is used to check against the unit cube borders
    // when will v_ray reach the front face?
    float tFront     = - p.z / r.z;                  // (x,x,0) = v_rayStart + t * v_ray
    float tBack      = ( 1.0 - p.z ) / r.z;          // (x,x,1) = v_rayStart + t * v_ray

    float tLeft      = - p.x / r.x;                  // (0,x,x) = v_rayStart + t * v_ray
    float tRight     = ( 1.0 - p.x ) / r.x;          // (1,x,x) = v_rayStart + t * v_ray

    float tBottom    = - p.y / r.y;                  // (x,0,x) = v_rayStart + t * v_ray
    float tTop       = ( 1.0 - p.y ) / r.y;          // (x,1,x) = v_rayStart + t * v_ray

    // get the nearest hit
    d = min( min( max( tFront, tBack ), max( tLeft, tRight ) ), max( tBottom, tTop ) );
    return p + ( r * d );
}

/**
 * Returns the gradient vector at the given position.
 *
 * \param position the voxel for which to get the gradient
 *
 * \return the gradient, NOT normalized
 */
vec3 getNormal( in vec3 position, in vec3 ray )
{
    vec3 grad;
#ifdef GRADIENTTEXTURE_ENABLED
    grad = ( 2.0 * texture( u_gradientsSampler, position ).rgb ) + vec3( -1.0 );
#else
    grad = getGradient( u_texture0Sampler, position );
#endif
    return sign( dot( grad, -ray ) ) * grad;
}

/**
 * Main entry point of the fragment shader.
 */
void main()
{
    wgeInitGBuffer();
    wge_FragColor = vec4( 1.0, 0.0, 0.0, 1.0 );
    gl_FragDepth = 1.0;

    vec3 ray = v_ray;
    if( osg_ProjectionMatrix[3][3] == 0.0 )
    {
        ray = v_rayStart - v_eyePos;
    }

#ifdef WGE_POSTPROCESSING_ENABLED
    wge_FragZoom = v_worldScale;
#endif

    // First, find the rayEnd point. We need to do it in the fragment shader as the ray end point may be interpolated wrong
    // when done for each vertex.
    float totalDistance = 0.0;      // the maximal distance along the ray until the BBox ends
    float currentDistance = 0.02;   // accumulated distance along the ray

#ifdef STOCHASTICJITTER_ENABLED
    // stochastic jittering can help to void these ugly wood-grain artifacts with larger sampling distances but might
    // introduce some noise artifacts.
    float jitter = 0.5 - texture( u_texture1Sampler, gl_FragCoord.xy / float( u_texture1SizeX ) ).r;
    vec3 rayStart = v_rayStart + ( ray * v_sampleDistance * jitter );
#else
    vec3 rayStart = v_rayStart;
#endif

    vec3 rayEnd = findRayEnd( rayStart, ray, totalDistance );

    // walk along the ray
    while( currentDistance <= ( totalDistance - 0.02 )  )
    {
        // get current value
        vec3 rayPoint = rayStart + ( currentDistance * ray );

        float value = texture( u_texture0Sampler, rayPoint ).r;

        if( ( abs( value - v_isovalue ) < ISO_EPSILON )
#ifdef BORDERCLIP_ENABLED
                &&
            !( length( rayPoint - rayStart ) < u_borderClipDistance )
                &&
            !( length( rayPoint - rayEnd ) < u_borderClipDistance )
#endif
        )
        {
            // we need to know the depth value of the current point inside the cube
            // Therefore, the complete standard pipeline is reproduced here:

            // 1: transfer to world space and right after it, to eye space
            vec4 rayPointProjected = osg_ModelViewProjectionMatrix * vec4( rayPoint, 1.0 );

            // 2: scale to screen space and [0,1]
            // -> x and y is not needed
            // curPointProjected.x /= curPointProjected.w;
            // curPointProjected.x  = curPointProjected.x * 0.5 + 0.5 ;
            // curPointProjected.y /= curPointProjected.w;
            // curPointProjected.y  = curPointProjected.y * 0.5 + 0.5 ;
            rayPointProjected.z /= rayPointProjected.w;
            rayPointProjected.z  = rayPointProjected.z * 0.5 + 0.5;

            // 3: set depth value
            gl_FragDepth = rayPointProjected.z;

            vec3 normal = ( osg_ModelViewMatrix * vec4( getNormal( rayPoint, ray ), 0.0 ) ).xyz;
#ifdef WGE_POSTPROCESSING_ENABLED
            wge_FragNormal = textureNormalize( normal );
#endif

            float light = 1.0;
#ifdef PHONGSHADING_ENABLED
            // only calculate the phong illumination only if needed
            wge_LightIntensityParameter lightParams = wge_DefaultLightIntensity;
    #ifdef PHONGSHADING_NOSPECULAR_ENABLED
            lightParams.materialSpecular = 0.0;
    #endif
            light = blinnPhongIlluminationIntensity( lightParams, normalize( normal ) );
#endif
            // 4: set color
            // mix color with colormap
            vec4 color = mix(
                colormapping( vec4( rayPoint.x * u_texture0SizeX, rayPoint.y * u_texture0SizeY, rayPoint.z * u_texture0SizeZ, 1.0 ) ),
                vec4( v_color.rgb, u_alpha ),
                1.0 - u_colormapRatio );
            // 5: the final color construction
            wge_FragColor = vec4( light * color.rgb, color.a );
            break;
        }

        // go to next value
        currentDistance += v_sampleDistance;
    }
}

