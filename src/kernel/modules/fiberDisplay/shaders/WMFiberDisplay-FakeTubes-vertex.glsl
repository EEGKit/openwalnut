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

varying float tangent_dot_view;
varying vec3 tangentR3;
varying float s_param;
varying vec4 tubeColor;
uniform float u_thickness;
uniform int u_viewportHeight;
uniform int u_viewportWidth;
varying vec2 imageTangent;
varying float usePointSprite;
varying vec3 view;
varying vec3 lightDir;
varying float NdotL;
varying vec3 normal;
varying float endPoint;
uniform gl_DepthRangeParameters gl_DepthRange;
varying float z;
uniform vec3 nearPos;
uniform vec3 farPos;
varying float zNear;
varying float zFar;

void main()
{

    VaryingTexCoord0 = gl_Vertex;

    gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex; //< make clipping planes work



    vec3 tangent;



    float thickness = 0.01 * u_thickness/ 100.;



    tangentR3 = gl_Normal;
    tangent = ( gl_ModelViewProjectionMatrix * vec4( gl_Normal, 0. ) ).xyz; //< transform our tangent vector
    s_param = gl_MultiTexCoord0.x; //< store texture coordinate for shader



    vec4 pos = ftransform(); //< transform position to eye space

    offset.x *= thickness;
    offset.y *= thickness;



    pos.xyz += offset * ( s_param ); //< add offset in y-direction (eye-space)

    myColor = gl_Color;

	gl_Position = pos; //< store final position
}
