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

in vec4 osg_Vertex;
in vec3 osg_Normal;
in vec4 osg_Color;
in vec4 osg_SecondaryColor;

/////////////////////////////////////////////////////////////////////////////
// Uniforms
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Attributes
/////////////////////////////////////////////////////////////////////////////

/**
 * This vertex attribute is the bitmask denoting whether to show the fiber or not
 */
#ifdef BITFIELD_ENABLED
    attribute float a_bitfield;
#endif

/////////////////////////////////////////////////////////////////////////////
// Varyings
/////////////////////////////////////////////////////////////////////////////

/**
 * The normal in scene-space
 */
out vec3 v_normal;

/**
 * Varying defines wether to discard the fragments or not.
 */
out float v_discard;

/**
 * Second color array
 */
out vec4 v_secondaryColor;


/**
 * The output normal for the fragment shader in world-space
 */
out vec3 v_normalWorld;

/**
 * The vertex coordinates in world-space
 */
out vec4 v_vertex;

/**
 * The scaling component of the modelview matrix.
 */
out float v_worldScale;

/**
 * This varying carries the current cluster color.
 */
out vec4 v_clusterColor;

/**
 * The color passed to the geometry shader.
 */
out vec4 v_colorIn;

/////////////////////////////////////////////////////////////////////////////
// Variables
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Functions
/////////////////////////////////////////////////////////////////////////////

/**
 * Main entry point of the vertex shader.
 */
void main()
{
#ifdef BITFIELD_ENABLED
    v_discard = 1.0 - a_bitfield;
#endif

    // Init with defaults
    v_normalWorld = vec3( 0.0 );
    v_vertex = vec4( 1.0 );
    v_worldScale = 1.0;
    v_clusterColor = vec4( 1.0 );

    v_secondaryColor = vec4( osg_SecondaryColor.rgb, 1.0 );

    // forward to geometry shader
    v_normal = osg_Normal;
    v_colorIn = osg_Color;
    gl_Position = osg_Vertex;
}

