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

in vec4 v_color;

/////////////////////////////////////////////////////////////////////////////
// Varyings
/////////////////////////////////////////////////////////////////////////////

#ifdef CLIPPLANE_ENABLED
/**
 * The distance to the plane
 */
in float v_dist;
#endif

/**
 * The surface normal. Needed for nice lighting.
 */
in vec3 v_normal;

/**
 * The line tangent.
 */
in vec3 v_tangent;

/**
 * The normal parameterizing the surface in orthogonal tangent direction.
 */
in vec3 v_biNormal;

/**
 * The actual, corrected vertex.
 */
in vec4 v_vertex;

/**
 * The diameter of the tube in world-space.
 */
in float v_diameter;

/**
 * This is the interpolated surface parameter describing the surface orthogonal to the tangent. 0 is the center of the strip and -1 and 1 the
 * borders.
 */
in float v_surfaceParam;

/**
 * The scaling component of the modelview matrix.
 */
in float v_worldScale;

/**
 * This varying carries the current cluster color.
 */
in vec4 v_clusterColor;

/**
 * Varying defines wether to discard the fragments or not.
 */
in float v_discard;

/////////////////////////////////////////////////////////////////////////////
// Uniforms
/////////////////////////////////////////////////////////////////////////////

/**
 * The max distance allowed
 */
uniform float u_distance = 1.0;

/**
 * Ratio between colormap and fiber color.
 */
uniform float u_colormapRatio = 1.0;

/**
 * Ratio between original dataset color and ROI colors.
 */
uniform float u_roiFilterColorOverride = 0.0;

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
 * Main entry point of the fragment shader.
 */
void main()
{
#ifdef BITFIELD_ENABLED
    if( v_discard > 0.0 )
    {
        discard;
    }
#endif

#ifdef CLIPPLANE_ENABLED
    // discard fragment if too far from plane
    if( abs( v_dist ) >= u_distance )
    {
        discard;
    }
#endif

#ifdef CLUSTER_FILTER_ENABLED
    // is this a cluster color?
    if( v_clusterColor.a < 0.0001 )
    {
        discard;
    }
#endif

    // the depth of the fragment. For lines and flattubes this is the fragments z value. Tubes need to recalculate this
    float depth = gl_FragCoord.z;

    // this allows modification of surface brightness corresponding to current surface parameter
    float colorScaler = 1.0;

    // the normal acutally used for lighting
    vec3 normal = normalize( v_normal );

    // We need to differentiate several cases here: Lines, FlatTubes and Tubes
#ifdef TUBE_ENABLED
#ifdef RIBBON_ENABLED
    // colorScaler = 1.0;  // surface parameter should have no influence
    // normal = v_normal   // apply lighting equally on the surface
    // depth = gl_FragCoord.z; // as it is a flat ribbon
#else
#ifndef ILLUMINATION_ENABLED
    // For tubes, we need to create some "tube-effect" with the surface parameter if lighting is disabled.
    // If light is enabled, the corrected normal ensures proper tube-effect.
    colorScaler = 1.0 - abs( v_surfaceParam );
#endif
    // The normal needs to be adopted too. Use the surface parameter to interpolate along the ortogonal tangent direction using the biNormal.
    normal = normalize( mix( normal, normalize( v_biNormal ), abs( v_surfaceParam ) ) );

    // Correct fragment depth
    // We use the known world-space diameter and move the vertex along the corrected normal using the surface parameter
    vec3 v = v_vertex.xyz + normal * v_diameter * ( 1.0 - abs( v_surfaceParam ) );

    // apply standard projection:
    vec4 vProj = osg_ProjectionMatrix * vec4( v.xyz, 1.0 );
    depth = ( 0.5 * vProj.z / vProj.w ) + 0.5;

#endif  // RIBBON_ENABLED
#else   // !TUBE_ENABLED
    // colorScaler = 1.0;  // surface parameter should have no influence as lines are 1px thick
    // normal = v_normal;  // for lines, the normal does not need to be modified
    // depth = gl_FragCoord.z; // the vertex is at the correct depth -> fragment too.
#endif  // TUBE_ENABLED

    // Calculate light finally
#ifdef ILLUMINATION_ENABLED
    float light = blinnPhongIlluminationIntensity( wge_DefaultLightIntensity, normal );
#else
    float light = 1.0;
#endif

    // apply colormapping to the input color
    vec4 finalColor = v_color;

    // use secondary color only if bitfield filtering is active
#ifdef BITFIELD_ENABLED
#ifdef SECONDARY_COLORING_ENABLED
    finalColor = mix( v_clusterColor, v_color, u_roiFilterColorOverride );
#endif
#endif

#ifdef CLUSTER_FILTER_ENABLED
    finalColor = vec4( v_clusterColor.rgb, 1.0 );
#endif
#ifdef COLORMAPPING_ENABLED
    finalColor = mix( finalColor, colormapping(), u_colormapRatio );
#endif

    // finally set the color and depth
    wgeInitGBuffer();
    wge_FragColor = vec4( vec3( light * finalColor.xyz * colorScaler ), finalColor.a );
    wge_FragNormal = textureNormalize( normal );
    wge_FragZoom = v_worldScale;
    wge_FragTangent = textureNormalize( v_tangent );
    gl_FragDepth = depth;
}

