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

#ifndef WGEPOSTPROCESSORUTILS_VERTEX_GLSL
#define WGEPOSTPROCESSORUTILS_VERTEX_GLSL

#version 150 core

#include "WGEShader-attributes.glsl"
#include "WGEShader-uniforms.glsl"


out vec4 v_TexCoord;
out vec4 v_color;

/**
 * Does transfer the quad to image space.
 */
void main()
{
    // pass the color to the fragment shader
    v_color = osg_Color;

    // pass tex coordinates
    v_TexCoord = ow_TextureMatrix0 * osg_MultiTexCoord0;

    // transform position
    gl_Position = osg_ModelViewProjectionMatrix * osg_Vertex;
}

#endif // WGEPOSTPROCESSORUTILS_VERTEX_GLSL


