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

/////////////////////////////////////////////////////////////////////////////
// Varyings
/////////////////////////////////////////////////////////////////////////////

#include "DVRRaycast.varyings"

/////////////////////////////////////////////////////////////////////////////
// Uniforms
/////////////////////////////////////////////////////////////////////////////

// texture containing the data
uniform sampler3D tex0;

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
    // please do not laugh, it is a very very very simple "isosurface" shader
    vec3 curPoint = v_rayStart;
    float value;
    int i = 0;
    while ( i < 300 )
    {
        i++;
        value = texture3D( tex0, curPoint ).r;

        if ( value >= 0.320 )
        {
            break;
        }
        else
        {
            curPoint += 0.001 * v_ray;
        }
    }

    if ( i >= 300 )
    {
        //discard;
    }

    gl_FragColor = 0.975* ( vec4( vec3( 1.0 - i/300.0 ), 1.0 ) + 0.2*vec4( abs( v_normal ), 1.0 ) );
    //gl_FragColor = vec4( v_rayStart, 1.0 );
    //vec3 v_ray2 = v_rayStart - ( gl_ModelViewMatrixInverse * vec4( 0.0, 1.0, 0.0, 1.0 ) ).xyz; 
    
    //gl_FragColor = vec4( abs(v_normal), 1.0 );
//    gl_FragColor = vec4( v_ray, 1.0 );
}

