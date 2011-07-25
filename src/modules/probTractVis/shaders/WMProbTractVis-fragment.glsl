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

float maxDistance;
float stepDistance;

mat4x3 toYCbCr;
mat4x3 toRGB;

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

void rayTrace( in vec3 curPoint, in float isovalue, in vec4 isocolor )
{
    for( int i = 1; i < u_steps; i++ )
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
            vec4 mapcolor = colormapping( vec4( curPoint.x * u_texture0SizeX, curPoint.y * u_texture0SizeY, curPoint.z * u_texture0SizeZ, isocolor.a ) );
            // mix color with colormap
            vec4 color = mix( mapcolor, isocolor, 1 - u_colormapRatio );
//            // to hsv
//            float d = max( color.rgb ) - min( color.rgb );
////            vec3 hsv;
//            float v = max( color.rgb );
//            float s = sign( v ) * d / v;
//            float h = ( 1 - d )                   * 0                                      + // r = g = b
//                      ( 1 - sign( v - color.r ) ) * 60 *       ( color.g - color.b ) / d   + // max = r
//                      ( 1 - sign( v - color.g ) ) * 60 * ( 2 + ( color.b - color.r ) / d ) + // max = g
//                      ( 1 - sign( v - color.b ) ) * 60 * ( 4 + ( color.r - color.g ) / d );  // max = b
//            // scale saturation
//            s = log( i * stepDistance / maxDistance );
//            // to rgb
//            float hi = floor( h / 60 );
//            float f = h / 60 - hi;
//            float p = v * ( 1 - s );
//            float q = v * ( 1 - s * f );
//            float t = v * ( 1 - s * ( 1 - f ) );

//            color.rgb = ( 1 - sign( abs( 0 - hi ) )   * vec3( v, t, p ) + // hi = 0
//                        ( 1 - sign( abs( 1 - hi ) ) ) * vec3( q, v, p ) + // hi = 1
//                        ( 1 - sign( abs( 2 - hi ) ) ) * vec3( p, v, t ) + // hi = 2
//                        ( 1 - sign( abs( 3 - hi ) ) ) * vec3( p, q, v ) + // hi = 3
//                        ( 1 - sign( abs( 4 - hi ) ) ) * vec3( t, p, v ) + // hi = 4
//                        ( 1 - sign( abs( 5 - hi ) ) ) * vec3( v, p, q ) + // hi = 5
//                        ( 1 - sign( abs( 6 - hi ) )   * vec3( v, t, p );  // hi = 6

            float t = stepDistance * i / maxDistance;
//            float t = stepDistance * ( u_steps - i ) / maxDistance;
//            vec3 col;
//            col.x = toYCbCr[0] * color.r;
//            col.y = toYCbCr[1] * color.g;
//            col.z = toYCbCr[2] * color.b;

//            col.yz = col.yz * log ( t );

//            color.r = toRGB[0]
            color.rgb = exp( t * 1.1 ) * color.rgb;
            color.a = isocolor.a;

            // 6: the final color construction
            // alpha blending of background (old FragColor) and foreground (new color)
            // (1-alpha)*fragcol*fragalpha + alpha*light*col
            wge_FragColor.rgb = mix( wge_FragColor.rgb * wge_FragColor.a, light * color.rgb, color.a );
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
    gl_FragDepth = 1.0; //TODO(aberres): adapt?
    // need initial FragColor for color construction later (step 6 in rayTrace()
    wge_FragColor = vec4( 1.0, 1.0, 1.0, 1.0 );

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

    toYCbCr = mat4x3( 1.00000000e+00,  -1.21889419e-06,   1.40199959e+00,  -1.79455791e+02,
                      1.00000000e+00,  -3.44135678e-01,  -7.14136156e-01,   1.35458795e+02,
                      1.00000000e+00,   1.77200007e+00,   4.06298063e-07,  -2.26816060e+02 );

    toRGB = mat4x3(  0.299,     0.587,     0.114,     0.0,
                    -0.168736, -0.331264,  0.5,       128.0,
                     0.5,      -0.418688, -0.081312,  128.0 );

    // for each isosurface, set the isovalue + isocolor and call the raytracer
    for( int j = 0; j < u_surfCount; j++ )
    {
        isovalue = v_isovalues[j];
        #ifdef MANUALALPHA_ENABLED
        // use slider to define global alpha for all surfaces
        isocolor = u_isocolors[j];
        #else
        // use value-dependent alpha
        isocolor = vec4( u_isocolors[j].rgb, u_isoalphas[j] );
        #endif
        rayTrace( curPoint, isovalue, isocolor, stepDistance );
    }

    // debug by using graphical output
    // want to know current depth value
    // need return to display this
    // gl_FragData[0] = vec4(curPointProjected.z,0,0,1);
    // return;
}

