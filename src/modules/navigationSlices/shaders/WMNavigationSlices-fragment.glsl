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

// this color is used if currently picking the slice
const vec4 pickColor = vec4( 0.25, 0.0, 0.75, 1.0 );

// if this is 1, the slice is currently in pick mode
uniform float u_picked;

// animation time. Used for pick-animation
uniform int u_timer;

/**
 * Applies colormapping.
 */
void main()
{
    // get the colormap
    vec4 cmap = colormapping();

    // this creates a sinuidal blending in [0,1] with one complete 2pi cycle per second
    // NOTE: the 100.0 depends on the timer tick. We have a timer that counts one per a hundreth of a second.
    float pickColorIntensity = 0.5 * ( 1.0 + sin( 3.0 * 3.1472 * float( u_timer ) / 100.0 ) );
    // as we use this for blending, we want it to swing in [0.5-0.75]
    pickColorIntensity = 0.5 + ( pickColorIntensity * 0.25 );

    // mix colormapping and pick color
    gl_FragColor = mix( pickColor, cmap, ( ( 1.0 - u_picked ) * 1.0 ) + ( u_picked * pickColorIntensity ) );
}

