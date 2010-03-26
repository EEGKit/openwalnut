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

#include "TextureUtils.glsl"
#include "TransformationTools.glsl"

/////////////////////////////////////////////////////////////////////////////
// Varyings
/////////////////////////////////////////////////////////////////////////////

#include "DVRRaycast.varyings"

/////////////////////////////////////////////////////////////////////////////
// Uniforms
/////////////////////////////////////////////////////////////////////////////

// texture containing the data
uniform sampler3D tex0;

// The isovalue to use.
uniform float u_isovalue;

// Should the shader create some kind of isosurface instead of a volume rendering?
uniform bool u_isosurface;

// True if only the simple depth value should be used for coloring
uniform bool u_depthCueingOnly;

// The number of steps to use.
uniform int u_steps;

// The alpha value to set
uniform float u_alpha;

/////////////////////////////////////////////////////////////////////////////
// Attributes
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Variables
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Functions
/////////////////////////////////////////////////////////////////////////////

vec3 findRayEnd( out float d )
{
    vec3 r = v_ray + vec3( 0.0000001 );
    vec3 p = v_rayStart;

    // we need to ensure the vector components are not exactly 0.0

    // v_ray in cube coordinates is used to check against the unit cube borders
    // when will v_ray reach the front face?
    float tFront     = - p.z / r.z;                  // (x,x,0) = v_rayStart + t * v_ray
    float tBack      = ( 1.0 - p.z ) / r.z;          // (x,x,1) = v_rayStart + t * v_ray

    float tLeft      = - p.x / r.x;                  // (0,x,x) = v_rayStart + t * v_ray
    float tRight     = ( 1.0 - p.x ) / r.x;          // (1,x,x) = v_rayStart + t * v_ray

    float tBottom    = - p.y / r.y;                  // (x,0,x) = v_rayStart + t * v_ray
    float tTop       = ( 1.0 - p.y ) / r.y;          // (x,1,x) = v_rayStart + t * v_ray

    // get the nearest hit
    d = min( min( max( tFront, tBack ), max( tLeft, tRight ) ), max ( tBottom, tTop ) );
    return p + ( r * d );
}

float pointDistance( vec3 p1, vec3 p2 )
{
    return length( p1 - p2 );
}

/** 
 * @brief Function to calculate lighting intensitybased on "Real-Time Volume Graphics, p 119, chapter 5.4, Listing 5.1".
 * It is basically the same as blinnPhongIllumination function above. But it is faster if you just need 
 * the intensity.
 *
 * @param ambient   materials ambient intensity
 * @param diffuse   materials diffuse intensity
 * @param specular  materials specular intensity
 * @param shininess material shininess
 * @param lightIntensity  the light intensity
 * @param ambientIntensity the ambient light intensity
 * @param normalDir the normal
 * @param viewDir   viewing direction
 * @param lightDir  light direction
 * 
 * @return the light intensity.
 */
float blinnPhongIlluminationIntensity(float ambient, float diffuse, float specular, float shininess, 
							 float lightIntensity, float ambientIntensity, 
							 vec3 normalDir, vec3 viewDir, vec3 lightDir )
{
  vec3 H =  normalize( lightDir + viewDir );
  
  // compute ambient term
  float ambientV = ambient * ambientIntensity;

  // compute diffuse term
  float diffuseLight = max(dot(lightDir, normalDir), 0.);
  float diffuseV = diffuse * diffuseLight;

  // compute specular term
  float specularLight = pow(max(dot(H, normalDir), 0.), shininess);
  if( diffuseLight <= 0.) specularLight = 0.;
  float specularV = specular * specularLight;

  return ambientV + (diffuseV + specularV)*lightIntensity;
}

/**
 * Main entry point of the fragment shader.
 */
void main()
{
    // please do not laugh, it is a very very very simple "isosurface" shader
    gl_FragColor = vec4( 1.0, 0.0, 0.0, 1.0 );
    gl_FragDepth = gl_FragCoord.z; 

    // First, find the rayEnd point. We need to do it in the fragment shader as the ray end point may be interpolated wrong
    // when done for each vertex.
    float totalDistance = 0.0;
    vec3 rayEnd = findRayEnd( totalDistance );

    // Isosurface Mode
    if ( u_isosurface )
    {
        // the point along the ray in cube coordinates
        vec3 curPoint = v_rayStart;

        // the current value inside the data
        float value;

        // the step counter
        int i = 0;
        float stepDistance = totalDistance / float( u_steps );
        while ( i < u_steps ) // we do not need to ch
        {
            // get current value
            value = texture3D( tex0, curPoint ).r;

            // is it the isovalue?
            if ( abs( value - u_isovalue ) < 0.1 )
            {
                // we need to know the depth value of the current point inside the cube
                // Therefore, the complete standard pipeline is reproduced here:
                
                // 1: transfer to world space and right after it, to eye space
                vec3 curPointProjected = project( curPoint );
                
                // 2: set depth value
                gl_FragDepth = curPointProjected.z; 

                // 3: find a proper normal for a headlight
                float s = 0.005;
                float valueXP = texture3D( tex0, curPoint + vec3( s, 0.0, 0.0 ) ).r;
                float valueXM = texture3D( tex0, curPoint - vec3( s, 0.0, 0.0 ) ).r;
                float valueYP = texture3D( tex0, curPoint + vec3( 0.0, s, 0.0 ) ).r;
                float valueYM = texture3D( tex0, curPoint - vec3( 0.0, s, 0.0 ) ).r;
                float valueZP = texture3D( tex0, curPoint + vec3( 0.0, 0.0, s ) ).r;
                float valueZM = texture3D( tex0, curPoint - vec3( 0.0, 0.0, s ) ).r;

                vec3 dir = v_ray;
                dir += vec3( valueXP, 0.0, 0.0 );
                dir -= vec3( valueXM, 0.0, 0.0 );
                dir += vec3( 0.0, valueYP, 0.0 );
                dir -= vec3( 0.0, valueYM, 0.0 );
                dir += vec3( 0.0, 0.0, valueZP );
                dir -= vec3( 0.0, 0.0, valueZP );

                // Phong:
                float light = blinnPhongIlluminationIntensity( 
                        0.3,                                // material ambient
                        1.0,                                // material diffuse
                        1.3,                                // material specular
                        10.0,                               // shinines
                        1.0,                                // light diffuse
                        0.75,                               // light ambient
                        normalize( -dir ),                  // normal
                        normalize( v_ray ),                 // view direction
                        normalize( v_lightSource )          // light source position
                );

                // 4: set color
                vec4 color;
                if ( u_depthCueingOnly )
                {
                    float d = 1.0 - curPointProjected.z;
                    color = gl_Color * 1.5 * d * d;
                }
                else
                {
                    // NOTE: these are a lot of weird experiments ;-)
                    float d = 1.0 - curPointProjected.z;
                    d = 1.5*pointDistance( curPoint, vec3( 0.5 ) );
 
                    float w = dot( normalize( vec3( 0.5 ) - curPoint ), normalize( v_ray ) );
                    w = ( w + 0.5 );
                    if ( w > 0.8 ) w = 0.8;

                    float d2 = w*d*d*d*d*d;
                    color = gl_Color * 11.0 * d2;
                }

                color.a = u_alpha;
                gl_FragColor = color;

                break;
            }
            else
            {
                // no it is not the iso value
                // -> continue along the ray
                curPoint += stepDistance * v_ray;
            }

            // do not miss to count the steps already done
            i++;
        }

        // the ray did never hit the surface --> discard the pixel
        if ( i == u_steps )
        {
            discard;
        }
    }
}

