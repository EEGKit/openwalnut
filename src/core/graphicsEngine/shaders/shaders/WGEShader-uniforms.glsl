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

#ifndef WGESHADERUNIFORMS_GLSL
#define WGESHADERUNIFORMS_GLSL

// defines global attributes set by openwalnut and openscenegraph

uniform mat4 osg_ModelViewMatrix;
uniform mat4 osg_ProjectionMatrix;
uniform mat4 osg_ModelViewProjectionMatrix;
uniform mat3 osg_NormalMatrix;

uniform vec4 ow_lightsource;

uniform mat4 ow_TextureMatrix0;
uniform mat4 ow_TextureMatrix1;
uniform mat4 ow_TextureMatrix2;
uniform mat4 ow_TextureMatrix3;
uniform mat4 ow_TextureMatrix4;
uniform mat4 ow_TextureMatrix5;
uniform mat4 ow_TextureMatrix6;
uniform mat4 ow_TextureMatrix7;

#endif  // WGESHADERUNIFORMS_GLSL
