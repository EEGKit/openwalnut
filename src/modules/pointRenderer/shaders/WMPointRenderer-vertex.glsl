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

/////////////////////////////////////////////////////////////////////////////
// Uniforms
/////////////////////////////////////////////////////////////////////////////

/**
 * The size of the sprite
 */
uniform float u_pointSize = 1.0;

/////////////////////////////////////////////////////////////////////////////
// Attributes
/////////////////////////////////////////////////////////////////////////////
in float a_pointSize;

/////////////////////////////////////////////////////////////////////////////
// Varyings
/////////////////////////////////////////////////////////////////////////////

out float v_clip;

out float v_pointSize;

out vec4 v_colorIn;

/**
 * Vertex in world space.
 */
out vec4 v_worldCenterVertex;

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
    // use clipping planes to cut away parts
    v_clip = 0.0;
    // TODO(ebaum): implement this using proper clip-plane implementation
    // v_clip = float( ( osg_Vertex.x > 500.0 ) || ( osg_Vertex.y>500.0 ) );

#ifdef USE_ATTRIBUTE_ENABLED
    v_pointSize = a_pointSize * u_pointSize;
#endif
#ifdef USE_ATTRIBUTE_DISABLED
    v_pointSize = u_pointSize;
#endif

    // forward to geometry shader
    v_colorIn = osg_Color;
    gl_Position = osg_ModelViewMatrix * osg_Vertex;

    v_worldCenterVertex = osg_Vertex;
}

