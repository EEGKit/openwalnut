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

#include "WGEShadingTools.glsl"

/**
 * The normal.
 */
in vec3 v_normal;

in vec4 v_color;
in vec4 v_texcoord;
out vec4 fragColor;


void main()
{
    vec4 col = v_color;
    vec4 white = vec4( 1, 1, 1, 1 );

    // calculate lighting
    float light = blinnPhongIlluminationIntensity( normalize( v_normal ) );
    col *= light;

    // use texture coordinate to mix along the cylinder
    fragColor = mix( white, col, v_texcoord.t + 0.2 );
}
