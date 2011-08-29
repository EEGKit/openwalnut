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

#include "WGEColormapping-fragment.glsl"

#include "WGEShadingTools.glsl" // wge_LightIntensityParameter wge_DefaultLightIntensity
#include "WGETextureTools.glsl"
#include "WGEPostprocessing.glsl"

/////////////////////////////////////////////////////////////////////////////
// Varyings
// Used in vertex and fragment shader
/////////////////////////////////////////////////////////////////////////////

#include "WMProbTractVis-varyings.glsl"

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

// The number of surfaces being drawn
uniform int u_surfCount;

// The amount of deviation tolerated for the isovalue
uniform float u_isovalTolerance;

// The number of steps to use.
uniform int u_steps;

// The alpha values to set
uniform float u_alpha;
uniform vec4 u_isoalphas;

// the ratio between normal color and the colormapping color.
uniform float u_colormapRatio;

// The isocolors to use.
uniform mat4 u_isocolors;

// The weighting parameter for the depth-dependent saturation.
uniform float u_saturation;

// member variables to avoid handing them over as function parameters
float maxDistance;
float stepDistance;

/////////////////////////////////////////////////////////////////////////////
// Attributes
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Variables
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Functions
/////////////////////////////////////////////////////////////////////////////

// returns closest point of the cube
vec3 findRayEnd( out float d )
{
    // (0,0,-1,0) in world coordinates + avoid (0,0,0,0)
    vec3 r = v_ray + vec3( 0.0000001 );
    // current surface point in texture space
    vec3 p = v_rayStart;

    // we need to ensure the vector components are not exactly 0.0

    // v_ray in cube coordinates is used to check against the unit cube borders
    // when will v_ray reach the front face? -> solve equations
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
vec3 getNormal( in vec3 position )
{
    vec3 grad;
#ifdef GRADIENTTEXTURE_ENABLED
    grad = ( 2.0 * texture3D( u_gradientsSampler, position ).rgb ) + vec3( -1.0 );
#else
    grad = getGradient( u_texture0Sampler, position );
#endif
    return sign( dot( grad, -v_ray ) ) * grad;
}

// raycaster for probabilistic tractograms which stops after first hit
void rayCast( in vec3 curPoint, in float isovalue, in vec4 isocolor )
{
    for( int j = 1; j < u_steps; j++ )
    {
        // get current value
        float curValue = texture3D( u_texture0Sampler, curPoint ).r;

        // is it the isovalue?
        if( abs( curValue - isovalue ) < u_isovalTolerance )
        {
            // we need the depth value of the current point inside the cube -> standard pipeline

            // 1. transfer to world space and then to eye space
            vec4 curPointProjected = gl_ModelViewProjectionMatrix * vec4( curPoint, 1.0 );

            // 2. scale to screen space and [0,1]
            // don't need x and y
            curPointProjected.z /= curPointProjected.w;
            curPointProjected.z  = curPointProjected.z * 0.5 + 0.5;

            // 3. set depth value
            gl_FragDepth = curPointProjected.z;

            // 4. shading
            // find normal for a headlight in world coordinates
            vec3 normal = ( gl_ModelViewMatrix * vec4( getNormal( curPoint ), 0.0 ) ).xyz;
            #ifdef WGE_POSTPROCESSING_ENABLED
            wge_FragNormal = textureNormalize( normal );
            #endif

            // full brightness
            float light = 1.0;
            #ifdef PHONGSHADING_ENABLED
            // only calculate the phong illumination only if needed
            light = blinnPhongIlluminationIntensity( normalize( normal ) );
            #endif

            // 5. set color
            // get color from colormap (alpha is set to one since we use the isovalue's alpha value here)
            vec4 mapcolor = colormapping( vec4( curPoint.x * u_texture0SizeX,
                                                curPoint.y * u_texture0SizeY,
                                                curPoint.z * u_texture0SizeZ,
                                                isocolor.a ) );

            // mix color with colormap
            vec4 color = mix( mapcolor, isocolor, 1 - u_colormapRatio );
            color.a = isocolor.a;

            // change saturation depending on depth:
            // compute relative distance with respect to the maximum distance
            float d = stepDistance * j / maxDistance;
            // approximate desaturated color (cf. HSI color space)
            vec3 gray = ( color.r + color.g + color.b ) / 3 * vec3( 1.0, 1.0, 1.0 );
            // desaturate color depending on the exponentiated distance d^k (k = u_saturation)
            color.rgb = mix( color.rgb, gray, pow( d, u_saturation ) );

            // 6: the final color construction
            // alpha blending of background (old FragColor) and foreground (new color)
            // (1-alpha)*fragcol + alpha*light*col
            wge_FragColor.rgb = mix( wge_FragColor.rgb, light * color.rgb, color.a );
            // (1-alpha)*fragalpha + alpha*1
            wge_FragColor.a = mix( wge_FragColor.a, 1, color.a );

            break;
        }

        else
        {
            // no it is not the iso value
            // -> continue along the ray
            curPoint += stepDistance * v_ray;
        }
    }
}

/**
 * Main entry point of the fragment shader.
 */
void main()
{
    // 1.0 = back, 0.0 = front
    // fragment depth needed for postprocessing
    gl_FragDepth = 1.0;
    // need initial FragColor for color construction later (step 6 in rayTrace())
    wge_FragColor = vec4( 1.0, 1.0, 1.0, 0.0 );

    // want to find out the maximal distance we have to evaluate and the end of our ray
    maxDistance = 0.0;
    // findRayEnd also sets the maxDistance
    vec3 rayEnd = findRayEnd( maxDistance );
    stepDistance = maxDistance / float( u_steps );

    #ifdef STOCHASTICJITTER_ENABLED
    // stochastic jittering can help to void these ugly wood-grain artifacts with larger sampling distances but might
    // introduce some noise artifacts.
    float jitter = 0.5 - texture2D( u_texture1Sampler, gl_FragCoord.xy / u_texture1SizeX ).r;
    // the point along the ray in cube coordinates
    vec3 curPoint = v_rayStart + v_ray + ( v_ray * stepDistance * jitter );
    vec3 rayStart = curPoint;
    #else
    // current point in texture space + v_ray
    vec3 curPoint = v_rayStart + v_ray;
    #endif

    float isovalue;
    vec4 isocolor;

    // for each isosurface, set the isovalue + isocolor and call the raytracer
    for( int j = 0; j < u_surfCount; j++ )
    {
        isovalue = v_isovalues[j];
        #ifdef MANUALALPHA_ENABLED
        // pick individual alpha values for all surfaces
        isocolor = u_isocolors[j];
        #else
        // use value-dependent alpha
        isocolor = vec4( u_isocolors[j].rgb, u_isoalphas[j] );
        #endif
        rayCast( curPoint, isovalue, isocolor );
    }
}

