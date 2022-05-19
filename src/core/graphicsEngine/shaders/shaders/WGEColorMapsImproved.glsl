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

#ifndef WGECOLORMAPSIMPROVED_GLSL
#define WGECOLORMAPSIMPROVED_GLSL

#version 150 core

#include "WGEShader-uniforms.glsl"

#include "WGEUtils.glsl"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions to ease clipping.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Clip pixel if it is the specified value. Use return-value as alpha.
 *
 * \param valueDescaled descaled value
 * \param clipValue value to use for clipping.
 *
 * \return 0.0 if clip, 1.0 if not.
 */
float clipIfValue( in float valueDescaled, in float clipValue )
{
    return clamp( sign( abs( valueDescaled - clipValue ) - 0.0001 ), 0.0, 1.0 );
}

/**
 * This function returns 0.0 if the pixel should be clipped. This could be used directly for alpha values. It therefore checks whether a descaled
 * value is 0.0 and clips it if it is.
 *
 * \param valueDescaled the descaled value
 * \param minV minimum
 *
 * \return 0.0 if clip, 1.0 if not.
 */
float clipZero( in float valueDescaled, in float minV )
{
    if( minV > 0.0 )
    {
        return clipIfValue( valueDescaled, minV );
    }
    return clipIfValue( valueDescaled, 0.0 );
}

/**
 * Works similiar to the other clipZero, but uses the vector lenght unstead.
 *
 * \param valueDescaled the descaled vector value
 *
 * \return 0.0 if clip, 1.0 if not.
 */
float clipZero( in vec3 valueDescaled )
{
    return clamp( sign( length( valueDescaled ) - 0.01 ), 0.0, 1.0 );
}

/**
 * The method decides whether the value should be clipped because of the threshold value. This is different for vector and scalar data, which is
 * why this method handles it transparently.
 *
 * \param valueDescaled the descaled data. Scalar or vector.
 * \param colormap if this is a vector colormap, thresholding is done using vector length.
 * \param thresholdVLower the descaled threshold value
 * \param thresholdVUpper the descaled threshold value
 * \param thresholdEnabled flag denoting whether to use thresholding or not
 *
 * \return 0.0 if clipped
 */
float clipThreshold( in vec3 valueDescaled, in int colormap, in float thresholdVLower, in float thresholdVUpper, in bool thresholdEnabled )
{
    float isVec = float( colormap == 6 );

    return max( 1.0 - float( thresholdEnabled ),
            isVec * clamp( sign( length( valueDescaled ) - thresholdVLower ) +
                           sign( - ( length( valueDescaled ) - thresholdVUpper ) ),
                           0.0, 1.0 )
                +
          ( 1.0 - isVec ) * clamp( sign( valueDescaled.r - ( thresholdVLower  ) ) + // or
                                   sign( - ( valueDescaled.r - ( thresholdVUpper ) ) ),
                                   0.0, 1.0 ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Colormaps.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Grayscale colormap. Maps value directly to gray values.
 *
 * \param value the <b>scaled</b> value
 *
 * \return color.
 */
vec4 grayscale( in vec3 value )
{
    return vec4( value, 1.0 );
}

/**
 * Maps red and blue to the positive and negative part of the interval linearly. It handles the zero-point according to specified min and scale
 * values.
 *
 * \note it clips on the mid-point of the scala or 0 if minV < 0
 *
 * \param valueDescaled <b>descaled</b> value
 * \param minV min value
 * \param scaleV scaling factor
 *
 * \return pos2neg colormap
 */
vec4 negative2positive( in float valueDescaled, in float minV, in float scaleV )
{
    // NOTE: this version should also respect positive-only values.
    const vec3 zeroColor = vec3( 1.0, 1.0, 1.0 );
    const vec3 negColor = vec3( 1.0, 1.0, 0.0 ); // non-blue because it will be subtracted from white -> blue
    const vec3 posColor= vec3( 0.0, 1.0, 1.0 ); // non-red because it will be subtracted from white -> red

    // The descaled value can be in interval [minV,minV+Scale]. But as we want linear scaling where the pos and neg colors are scaled linearly
    // against each other, and we want to handle real negative values correctly.
    float valIsPositive = abs( clamp( sign( valueDescaled ), 0.0, 1.0 ) );
    float valIsNegative = abs( clamp( sign( valueDescaled ), -1.0, 0.0 ) );
    float minIsNegative = abs( clamp( sign( minV ), -1.0, 0.0 ) );
    // not needed: float minIsPositive = abs( clamp( sign( minV ), 0.0, 1.0 ) );

    float extremeNeg = min( 0, minIsNegative * minV ) + 0.0001; // Add to avoid zero
    float extremePos = max( 0, minV + scaleV ) + 0.0001; // Add to avoid zero
    float normalizedNeg = abs( valueDescaled / extremeNeg );
    float normalizedPos = abs( valueDescaled / extremePos );


    // use neg color for shares < 0.0 and pos color for the others
    return vec4( zeroColor - ( vec3( valIsNegative ) * normalizedNeg * negColor + vec3( valIsPositive ) * normalizedPos * posColor ), 1.0 );

    // There have been a couple of iteration in this colormap. Please see version control for old code.
}

/**
 * Vector colormap. This basically is a grayscale colormap for each channel. It additionally clips according to vector length.
 *
 * \param valueDescaled the <b>descaled</b> vector data
 * \param minV minimum value
 * \param scaleV scaling value
 *
 * \return colormap.
 */
vec4 vector( in vec3 valueDescaled, in float minV, in float scaleV )
{
    // similar to negative2positive, we need the width of the interval.
    float m = max( abs( minV ), abs( minV + scaleV ) );
    return vec4( abs( valueDescaled / m ), 1.0 );
}

/**
 * Hot Iron colormap. It fades between red and yellowish colors.
 *
 * \param value the scaled value
 *
 * \return color.
 */
vec4 hotIron( in float value )
{
    vec4 color8  = vec4( 255.0 / 255.0, 255.0 / 255.0, 204.0 / 255.0, 1.0 );
    vec4 color7  = vec4( 255.0 / 255.0, 237.0 / 255.0, 160.0 / 255.0, 1.0 );
    vec4 color6  = vec4( 254.0 / 255.0, 217.0 / 255.0, 118.0 / 255.0, 1.0 );
    vec4 color5  = vec4( 254.0 / 255.0, 178.0 / 255.0,  76.0 / 255.0, 1.0 );
    vec4 color4  = vec4( 253.0 / 255.0, 141.0 / 255.0,  60.0 / 255.0, 1.0 );
    vec4 color3  = vec4( 252.0 / 255.0,  78.0 / 255.0,  42.0 / 255.0, 1.0 );
    vec4 color2  = vec4( 227.0 / 255.0,  26.0 / 255.0,  28.0 / 255.0, 1.0 );
    vec4 color1  = vec4( 189.0 / 255.0,   0.0 / 255.0,  38.0 / 255.0, 1.0 );
    vec4 color0  = vec4( 128.0 / 255.0,   0.0 / 255.0,  38.0 / 255.0, 1.0 );

    float colorValue = value * 8.0;
    int sel = int( floor( colorValue ) );

    if( sel >= 8 )
    {
        return color0;
    }
    else if( sel < 0 )
    {
        return color0;
    }
    else
    {
        colorValue -= float( sel );

        if( sel < 1 )
        {
            return ( color1 * colorValue + color0 * ( 1.0 - colorValue ) );
        }
        else if( sel < 2 )
        {
            return ( color2 * colorValue + color1 * ( 1.0 - colorValue ) );
        }
        else if( sel < 3 )
        {
            return ( color3 * colorValue + color2 * ( 1.0 - colorValue ) );
        }
        else if( sel < 4 )
        {
            return ( color4 * colorValue + color3 * ( 1.0 - colorValue ) );
        }
        else if( sel < 5 )
        {
            return ( color5 * colorValue + color4 * ( 1.0 - colorValue ) );
        }
        else if( sel < 6 )
        {
            return ( color6 * colorValue + color5 * ( 1.0 - colorValue ) );
        }
        else if( sel < 7 )
        {
            return ( color7 * colorValue + color6 * ( 1.0 - colorValue ) );
        }
        else if( sel < 8 )
        {
            return ( color8 * colorValue + color7 * ( 1.0 - colorValue ) );
        }
        else
        {
            return color0;
        }
    }
}

/**
 * Rainbow colormap. Fading through the rainbow colors.
 *
 * \param value the value in [0,1]
 *
 * \return color
 */
vec4 rainbow( in float value )
{
    float i = floor( 6.0 * value );
    float f = 6.0 * value - i;
    float q = 1.0 - f;

    int iq = int( mod( i, 6.0 ) );

    if( ( iq == 0 ) || ( iq == 6 ) )
    {
        return vec4( 1.0, f, 0.0, 1.0 );
    }
    else if( iq == 1 )
    {
        return vec4( q, 1.0, 0.0, 1.0 );
    }
    else if( iq == 2 )
    {
        return vec4( 0.0, 1.0, f, 1.0 );
    }
    else if( iq == 3 )
    {
        return vec4( 0.0, q, 1.0, 1.0 );
    }
    else if( iq == 4 )
    {
        return vec4( f, 0.0, 1.0, 1.0 );
    }
    else // iq == 5
    {
        return vec4( 1.0, 0.0, q, 1.0 );
    }
}

/**
 * Colormap fading between blue, green and purple.
 *
 * \param value the scaled value in [0,1]
 *
 * \return the color
 */
vec4 blueGreenPurple( in float value )
{
    value *= 5.0;
    vec4 color;
    if( value < 0.0 )
    {
        color = vec4( 0.0, 0.0, 0.0, 1.0 );
    }
    else if( value < 1.0 )
    {
        color = vec4( 0.0, value, 1.0, 1.0 );
    }
    else if( value < 2.0 )
    {
        color = vec4( 0.0, 1.0, 2.0 - value, 1.0 );
    }
    else if( value < 3.0 )
    {
        color =  vec4( value - 2.0, 1.0, 0.0, 1.0 );
    }
    else if( value < 4.0 )
    {
        color = vec4( 1.0, 4.0 - value, 0.0, 1.0 );
    }
    else if( value <= 5.0 )
    {
        color = vec4( 1.0, 0.0, value - 4.0, 1.0 );
    }
    else
    {
        color =  vec4( 1.0, 0.0, 1.0, 1.0 );
    }

    return color;
}



/**
 * "Viridis" colormap. It is optimized for perception. Default colormap in matplolib as of 2022 (since 2015).
 * Details:
 *   - https://bids.github.io/colormap/
 *   - https://github.com/matplotlib/matplotlib/blob/f6e0ee49c598f59c6e6cf4eefe473e4dc634a58a/lib/matplotlib/_cm_listed.py 
 *
 * \param value the scaled value in [0,1]
 *
 * \return the color
 */
vec4 viridis( in float value )
{
    const vec3 colorArray[] =
        vec3[256](
            vec3( 0.267004, 0.004874, 0.329415 ),
            vec3( 0.26851, 0.009605, 0.335427 ),
            vec3( 0.269944, 0.014625, 0.341379 ),
            vec3( 0.271305, 0.019942, 0.347269 ),
            vec3( 0.272594, 0.025563, 0.353093 ),
            vec3( 0.273809, 0.031497, 0.358853 ),
            vec3( 0.274952, 0.037752, 0.364543 ),
            vec3( 0.276022, 0.044167, 0.370164 ),
            vec3( 0.277018, 0.050344, 0.375715 ),
            vec3( 0.277941, 0.056324, 0.381191 ),
            vec3( 0.278791, 0.062145, 0.386592 ),
            vec3( 0.279566, 0.067836, 0.391917 ),
            vec3( 0.280267, 0.073417, 0.397163 ),
            vec3( 0.280894, 0.078907, 0.402329 ),
            vec3( 0.281446, 0.08432, 0.407414 ),
            vec3( 0.281924, 0.089666, 0.412415 ),
            vec3( 0.282327, 0.094955, 0.417331 ),
            vec3( 0.282656, 0.100196, 0.42216 ),
            vec3( 0.28291, 0.105393, 0.426902 ),
            vec3( 0.283091, 0.110553, 0.431554 ),
            vec3( 0.283197, 0.11568, 0.436115 ),
            vec3( 0.283229, 0.120777, 0.440584 ),
            vec3( 0.283187, 0.125848, 0.44496 ),
            vec3( 0.283072, 0.130895, 0.449241 ),
            vec3( 0.282884, 0.13592, 0.453427 ),
            vec3( 0.282623, 0.140926, 0.457517 ),
            vec3( 0.28229, 0.145912, 0.46151 ),
            vec3( 0.281887, 0.150881, 0.465405 ),
            vec3( 0.281412, 0.155834, 0.469201 ),
            vec3( 0.280868, 0.160771, 0.472899 ),
            vec3( 0.280255, 0.165693, 0.476498 ),
            vec3( 0.279574, 0.170599, 0.479997 ),
            vec3( 0.278826, 0.17549, 0.483397 ),
            vec3( 0.278012, 0.180367, 0.486697 ),
            vec3( 0.277134, 0.185228, 0.489898 ),
            vec3( 0.276194, 0.190074, 0.493001 ),
            vec3( 0.275191, 0.194905, 0.496005 ),
            vec3( 0.274128, 0.199721, 0.498911 ),
            vec3( 0.273006, 0.20452, 0.501721 ),
            vec3( 0.271828, 0.209303, 0.504434 ),
            vec3( 0.270595, 0.214069, 0.507052 ),
            vec3( 0.269308, 0.218818, 0.509577 ),
            vec3( 0.267968, 0.223549, 0.512008 ),
            vec3( 0.26658, 0.228262, 0.514349 ),
            vec3( 0.265145, 0.232956, 0.516599 ),
            vec3( 0.263663, 0.237631, 0.518762 ),
            vec3( 0.262138, 0.242286, 0.520837 ),
            vec3( 0.260571, 0.246922, 0.522828 ),
            vec3( 0.258965, 0.251537, 0.524736 ),
            vec3( 0.257322, 0.25613, 0.526563 ),
            vec3( 0.255645, 0.260703, 0.528312 ),
            vec3( 0.253935, 0.265254, 0.529983 ),
            vec3( 0.252194, 0.269783, 0.531579 ),
            vec3( 0.250425, 0.27429, 0.533103 ),
            vec3( 0.248629, 0.278775, 0.534556 ),
            vec3( 0.246811, 0.283237, 0.535941 ),
            vec3( 0.244972, 0.287675, 0.53726 ),
            vec3( 0.243113, 0.292092, 0.538516 ),
            vec3( 0.241237, 0.296485, 0.539709 ),
            vec3( 0.239346, 0.300855, 0.540844 ),
            vec3( 0.237441, 0.305202, 0.541921 ),
            vec3( 0.235526, 0.309527, 0.542944 ),
            vec3( 0.233603, 0.313828, 0.543914 ),
            vec3( 0.231674, 0.318106, 0.544834 ),
            vec3( 0.229739, 0.322361, 0.545706 ),
            vec3( 0.227802, 0.326594, 0.546532 ),
            vec3( 0.225863, 0.330805, 0.547314 ),
            vec3( 0.223925, 0.334994, 0.548053 ),
            vec3( 0.221989, 0.339161, 0.548752 ),
            vec3( 0.220057, 0.343307, 0.549413 ),
            vec3( 0.21813, 0.347432, 0.550038 ),
            vec3( 0.21621, 0.351535, 0.550627 ),
            vec3( 0.214298, 0.355619, 0.551184 ),
            vec3( 0.212395, 0.359683, 0.55171 ),
            vec3( 0.210503, 0.363727, 0.552206 ),
            vec3( 0.208623, 0.367752, 0.552675 ),
            vec3( 0.206756, 0.371758, 0.553117 ),
            vec3( 0.204903, 0.375746, 0.553533 ),
            vec3( 0.203063, 0.379716, 0.553925 ),
            vec3( 0.201239, 0.38367, 0.554294 ),
            vec3( 0.19943, 0.387607, 0.554642 ),
            vec3( 0.197636, 0.391528, 0.554969 ),
            vec3( 0.19586, 0.395433, 0.555276 ),
            vec3( 0.1941, 0.399323, 0.555565 ),
            vec3( 0.192357, 0.403199, 0.555836 ),
            vec3( 0.190631, 0.407061, 0.556089 ),
            vec3( 0.188923, 0.41091, 0.556326 ),
            vec3( 0.187231, 0.414746, 0.556547 ),
            vec3( 0.185556, 0.41857, 0.556753 ),
            vec3( 0.183898, 0.422383, 0.556944 ),
            vec3( 0.182256, 0.426184, 0.55712 ),
            vec3( 0.180629, 0.429975, 0.557282 ),
            vec3( 0.179019, 0.433756, 0.55743 ),
            vec3( 0.177423, 0.437527, 0.557565 ),
            vec3( 0.175841, 0.44129, 0.557685 ),
            vec3( 0.174274, 0.445044, 0.557792 ),
            vec3( 0.172719, 0.448791, 0.557885 ),
            vec3( 0.171176, 0.45253, 0.557965 ),
            vec3( 0.169646, 0.456262, 0.55803 ),
            vec3( 0.168126, 0.459988, 0.558082 ),
            vec3( 0.166617, 0.463708, 0.558119 ),
            vec3( 0.165117, 0.467423, 0.558141 ),
            vec3( 0.163625, 0.471133, 0.558148 ),
            vec3( 0.162142, 0.474838, 0.55814 ),
            vec3( 0.160665, 0.47854, 0.558115 ),
            vec3( 0.159194, 0.482237, 0.558073 ),
            vec3( 0.157729, 0.485932, 0.558013 ),
            vec3( 0.15627, 0.489624, 0.557936 ),
            vec3( 0.154815, 0.493313, 0.55784 ),
            vec3( 0.153364, 0.497, 0.557724 ),
            vec3( 0.151918, 0.500685, 0.557587 ),
            vec3( 0.150476, 0.504369, 0.55743 ),
            vec3( 0.149039, 0.508051, 0.55725 ),
            vec3( 0.147607, 0.511733, 0.557049 ),
            vec3( 0.14618, 0.515413, 0.556823 ),
            vec3( 0.144759, 0.519093, 0.556572 ),
            vec3( 0.143343, 0.522773, 0.556295 ),
            vec3( 0.141935, 0.526453, 0.555991 ),
            vec3( 0.140536, 0.530132, 0.555659 ),
            vec3( 0.139147, 0.533812, 0.555298 ),
            vec3( 0.13777, 0.537492, 0.554906 ),
            vec3( 0.136408, 0.541173, 0.554483 ),
            vec3( 0.135066, 0.544853, 0.554029 ),
            vec3( 0.133743, 0.548535, 0.553541 ),
            vec3( 0.132444, 0.552216, 0.553018 ),
            vec3( 0.131172, 0.555899, 0.552459 ),
            vec3( 0.129933, 0.559582, 0.551864 ),
            vec3( 0.128729, 0.563265, 0.551229 ),
            vec3( 0.127568, 0.566949, 0.550556 ),
            vec3( 0.126453, 0.570633, 0.549841 ),
            vec3( 0.125394, 0.574318, 0.549086 ),
            vec3( 0.124395, 0.578002, 0.548287 ),
            vec3( 0.123463, 0.581687, 0.547445 ),
            vec3( 0.122606, 0.585371, 0.546557 ),
            vec3( 0.121831, 0.589055, 0.545623 ),
            vec3( 0.121148, 0.592739, 0.544641 ),
            vec3( 0.120565, 0.596422, 0.543611 ),
            vec3( 0.120092, 0.600104, 0.54253 ),
            vec3( 0.119738, 0.603785, 0.5414 ),
            vec3( 0.119512, 0.607464, 0.540218 ),
            vec3( 0.119423, 0.611141, 0.538982 ),
            vec3( 0.119483, 0.614817, 0.537692 ),
            vec3( 0.119699, 0.61849, 0.536347 ),
            vec3( 0.120081, 0.622161, 0.534946 ),
            vec3( 0.120638, 0.625828, 0.533488 ),
            vec3( 0.12138, 0.629492, 0.531973 ),
            vec3( 0.122312, 0.633153, 0.530398 ),
            vec3( 0.123444, 0.636809, 0.528763 ),
            vec3( 0.12478, 0.640461, 0.527068 ),
            vec3( 0.126326, 0.644107, 0.525311 ),
            vec3( 0.128087, 0.647749, 0.523491 ),
            vec3( 0.130067, 0.651384, 0.521608 ),
            vec3( 0.132268, 0.655014, 0.519661 ),
            vec3( 0.134692, 0.658636, 0.517649 ),
            vec3( 0.137339, 0.662252, 0.515571 ),
            vec3( 0.14021, 0.665859, 0.513427 ),
            vec3( 0.143303, 0.669459, 0.511215 ),
            vec3( 0.146616, 0.67305, 0.508936 ),
            vec3( 0.150148, 0.676631, 0.506589 ),
            vec3( 0.153894, 0.680203, 0.504172 ),
            vec3( 0.157851, 0.683765, 0.501686 ),
            vec3( 0.162016, 0.687316, 0.499129 ),
            vec3( 0.166383, 0.690856, 0.496502 ),
            vec3( 0.170948, 0.694384, 0.493803 ),
            vec3( 0.175707, 0.6979, 0.491033 ),
            vec3( 0.180653, 0.701402, 0.488189 ),
            vec3( 0.185783, 0.704891, 0.485273 ),
            vec3( 0.19109, 0.708366, 0.482284 ),
            vec3( 0.196571, 0.711827, 0.479221 ),
            vec3( 0.202219, 0.715272, 0.476084 ),
            vec3( 0.20803, 0.718701, 0.472873 ),
            vec3( 0.214, 0.722114, 0.469588 ),
            vec3( 0.220124, 0.725509, 0.466226 ),
            vec3( 0.226397, 0.728888, 0.462789 ),
            vec3( 0.232815, 0.732247, 0.459277 ),
            vec3( 0.239374, 0.735588, 0.455688 ),
            vec3( 0.24607, 0.73891, 0.452024 ),
            vec3( 0.252899, 0.742211, 0.448284 ),
            vec3( 0.259857, 0.745492, 0.444467 ),
            vec3( 0.266941, 0.748751, 0.440573 ),
            vec3( 0.274149, 0.751988, 0.436601 ),
            vec3( 0.281477, 0.755203, 0.432552 ),
            vec3( 0.288921, 0.758394, 0.428426 ),
            vec3( 0.296479, 0.761561, 0.424223 ),
            vec3( 0.304148, 0.764704, 0.419943 ),
            vec3( 0.311925, 0.767822, 0.415586 ),
            vec3( 0.319809, 0.770914, 0.411152 ),
            vec3( 0.327796, 0.77398, 0.40664 ),
            vec3( 0.335885, 0.777018, 0.402049 ),
            vec3( 0.344074, 0.780029, 0.397381 ),
            vec3( 0.35236, 0.783011, 0.392636 ),
            vec3( 0.360741, 0.785964, 0.387814 ),
            vec3( 0.369214, 0.788888, 0.382914 ),
            vec3( 0.377779, 0.791781, 0.377939 ),
            vec3( 0.386433, 0.794644, 0.372886 ),
            vec3( 0.395174, 0.797475, 0.367757 ),
            vec3( 0.404001, 0.800275, 0.362552 ),
            vec3( 0.412913, 0.803041, 0.357269 ),
            vec3( 0.421908, 0.805774, 0.35191 ),
            vec3( 0.430983, 0.808473, 0.346476 ),
            vec3( 0.440137, 0.811138, 0.340967 ),
            vec3( 0.449368, 0.813768, 0.335384 ),
            vec3( 0.458674, 0.816363, 0.329727 ),
            vec3( 0.468053, 0.818921, 0.323998 ),
            vec3( 0.477504, 0.821444, 0.318195 ),
            vec3( 0.487026, 0.823929, 0.312321 ),
            vec3( 0.496615, 0.826376, 0.306377 ),
            vec3( 0.506271, 0.828786, 0.300362 ),
            vec3( 0.515992, 0.831158, 0.294279 ),
            vec3( 0.525776, 0.833491, 0.288127 ),
            vec3( 0.535621, 0.835785, 0.281908 ),
            vec3( 0.545524, 0.838039, 0.275626 ),
            vec3( 0.555484, 0.840254, 0.269281 ),
            vec3( 0.565498, 0.84243, 0.262877 ),
            vec3( 0.575563, 0.844566, 0.256415 ),
            vec3( 0.585678, 0.846661, 0.249897 ),
            vec3( 0.595839, 0.848717, 0.243329 ),
            vec3( 0.606045, 0.850733, 0.236712 ),
            vec3( 0.616293, 0.852709, 0.230052 ),
            vec3( 0.626579, 0.854645, 0.223353 ),
            vec3( 0.636902, 0.856542, 0.21662 ),
            vec3( 0.647257, 0.8584, 0.209861 ),
            vec3( 0.657642, 0.860219, 0.203082 ),
            vec3( 0.668054, 0.861999, 0.196293 ),
            vec3( 0.678489, 0.863742, 0.189503 ),
            vec3( 0.688944, 0.865448, 0.182725 ),
            vec3( 0.699415, 0.867117, 0.175971 ),
            vec3( 0.709898, 0.868751, 0.169257 ),
            vec3( 0.720391, 0.87035, 0.162603 ),
            vec3( 0.730889, 0.871916, 0.156029 ),
            vec3( 0.741388, 0.873449, 0.149561 ),
            vec3( 0.751884, 0.874951, 0.143228 ),
            vec3( 0.762373, 0.876424, 0.137064 ),
            vec3( 0.772852, 0.877868, 0.131109 ),
            vec3( 0.783315, 0.879285, 0.125405 ),
            vec3( 0.79376, 0.880678, 0.120005 ),
            vec3( 0.804182, 0.882046, 0.114965 ),
            vec3( 0.814576, 0.883393, 0.110347 ),
            vec3( 0.82494, 0.88472, 0.106217 ),
            vec3( 0.83527, 0.886029, 0.102646 ),
            vec3( 0.845561, 0.887322, 0.099702 ),
            vec3( 0.85581, 0.888601, 0.097452 ),
            vec3( 0.866013, 0.889868, 0.095953 ),
            vec3( 0.876168, 0.891125, 0.09525 ),
            vec3( 0.886271, 0.892374, 0.095374 ),
            vec3( 0.89632, 0.893616, 0.096335 ),
            vec3( 0.906311, 0.894855, 0.098125 ),
            vec3( 0.916242, 0.896091, 0.100717 ),
            vec3( 0.926106, 0.89733, 0.104071 ),
            vec3( 0.935904, 0.89857, 0.108131 ),
            vec3( 0.945636, 0.899815, 0.112838 ),
            vec3( 0.9553, 0.901065, 0.118128 ),
            vec3( 0.964894, 0.902323, 0.123941 ),
            vec3( 0.974417, 0.90359, 0.130215 ),
            vec3( 0.983868, 0.904867, 0.136897 ),
            vec3( 0.993248, 0.906157, 0.143936 )
            );

    int id = int( value * 255.0 );

    return vec4( colorArray[id], 1.0 );
}

/**
 * Checks wether a bit is set. This is done in this function since the &-operator is not available in GLSL 1.20 and GLSL 1.30 is problematic on
 * Mac OS 10.6.*
 *
 * \note Remove this function and replace its calls with bitwise operations as soon as there on all platforms available.
 *
 * \param value the value in [0,1]
 * \param bitpos which bit
 *
 * \return
 */
bool isBitSet( in float value, in float bitpos )
{
    return ( abs( mod( floor( value / pow( 2.0, bitpos ) ), 2.0 ) - 1.0 ) ) < 0.001;
}

/**
 * Colormap especially suitable for mask data. It tries to find distinct colors for neighbouring values.
 *
 * \param value the value in [0,1]
 *
 * \return the volor
 */
vec4 atlas( in float value )
{
    float val = floor( value * 255.0 );
    float r = 0.0;
    float g = 0.0;
    float b = 0.0;
    float mult = 1.0;

    if( val == 0.0 )
    {
        return vec4( vec3( 0.0 ), 1.0 );
    }

    if( isBitSet( val, 0.0 ) )
    {
        b = 1.0;
    }
    if( isBitSet( val, 1.0 ) )
    {
        g = 1.0;
    }
    if( isBitSet( val, 2.0 ) )
    {
        r = 1.0;
    }
    if( isBitSet( val, 3.0 ) )
    {
        mult -= 0.15;
        if( r < 1.0 && g < 1.0 && b < 1.0 )
        {
            r = 1.0;
            g = 1.0;
        }
    }
    if( isBitSet( val, 4.0 ) )
    {
        mult -= 0.15;
        if( r < 1.0 && g < 1.0 && b < 1.0 )
        {
            b = 1.0;
            g = 1.0;
        }
    }
    if( isBitSet( val, 5.0 ) )
    {
        mult -= 0.15;
        if( r < 1.0 && g < 1.0 && b < 1.0 )
        {
            r = 1.0;
            b = 1.0;
        }
    }
    if( isBitSet( val, 6.0 ) )
    {
        mult -= 0.15;
        if( r < 1.0 && g < 1.0 && b < 1.0 )
        {
            g = 1.0;
        }
    }
    if( isBitSet( val, 7.0 ) )
    {
        mult -= 0.15;
        if( r < 1.0 && g < 1.0 && b < 1.0 )
        {
            r = 1.0;
        }
    }

    r *= mult;
    g *= mult;
    b *= mult;

    clamp( r, 0.0, 1.0 );
    clamp( g, 0.0, 1.0 );
    clamp( b, 0.0, 1.0 );

    return vec4( r, g, b, 1.0 );
}

/**
 * This method applies a colormap to the specified value an mixes it with the specified color. It uses the proper colormap and works on scaled
 * values.
 *
 * \return this color gets mixed using alpha value with the new colormap color
 * \param value the value to map, <b>scaled</b>
 * \param minV the minimum of the original value *
 * \param scaleV the scaler used to downscale the original value to [0-1]
 * \param clipZeroEnabled if true, zero values get clipped; based on descaled value.
 * \param thresholdVLower a threshold in original space (you need to downscale it to [0-1] if you want to use it to scaled values.
 * \param thresholdVUpper a threshold in original space (you need to downscale it to [0-1] if you want to use it to scaled values.
 * \param thresholdEnabled a flag denoting whether threshold-based clipping should be done or not
 * \param window a window level scaling in the descaled value
 * \param windowEnabled if true, the window level scaling is applied
 * \param alpha the alpha blending value
 * \param colormap the colormap index to use
 * \param cmactive allows disabling this colormap
 */
vec4 colormap( in vec4 value, float minV, float scaleV,
               bool clipZeroEnabled,
               float thresholdVLower, float thresholdVUpper, bool thresholdEnabled,
               vec2  window, bool windowEnabled,
               float alpha, int colormap, bool cmactive )
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Scale the input data to original space, and apply windowing
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // descale value
    vec3 valueDescaled = vec3( minV ) + ( value.rgb * scaleV );

    // apply window scaling. If windowing is disabled, the window is the same as the original data interval
    float winLo = ( window.x  * float( windowEnabled ) ) + // OR
                  ( minV * ( 1.0 - float( windowEnabled ) ) );
    float winUp = ( window.y  * float( windowEnabled ) ) + // OR
                  ( ( minV + scaleV ) * ( 1.0 - float( windowEnabled ) ) );
    float winLen = winUp - winLo;

    // this is the descaled value, scaled using the window interval to be mapped to [0,1]
    vec3 valueWindowedNormalized = ( valueDescaled - vec3( winLo ) ) / winLen;

    // clip values outside the window level
    valueWindowedNormalized = min( vec3( 1.0 ), valueWindowedNormalized );
    valueWindowedNormalized = max( vec3( 0.0 ), valueWindowedNormalized );

    // scale it back to original data space
    vec3 valueWindowedOriginal = vec3( minV ) + ( valueWindowedNormalized * scaleV );

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Do clippings
    //
    // It is important to note that the clip values actually define the pixels alpha value. This means a clip value of 1 means
    // the pixel is not clipped. A clip value of 0 means it gets fully transparent (=clipped in this context).
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // is this a border pixel marked by a 0 alpha value?
    float isNotBorder = float( value.a >= 0.75 );

    // make "zero" values transarent
    float clip = max( float( !clipZeroEnabled ), clipZero( valueWindowedOriginal.r, minV ) );

    // use threshold to clip away fragments.
    // NOTE: thresholding is applied to the original interval in valueDescaled, NOT the window interval
    float clipTh = clipThreshold( valueDescaled, colormap, thresholdVLower, thresholdVUpper, thresholdEnabled );

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Do colormapping
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // this is the final color returned by the colormapping algorithm. This is the correct value for the gray colormap
    vec4 cmapped = grayscale( valueWindowedNormalized.rgb );

    // negative to positive shading in red-blue
    if( colormap == 1 )
    {
        cmapped = rainbow( valueWindowedNormalized.r );
    }
    else if( colormap == 2 )
    {
        cmapped = hotIron( valueWindowedNormalized.r );
    }
    else if( colormap == 3 )
    {
        cmapped = negative2positive( valueWindowedOriginal.r, minV, scaleV );
    }
    else if( colormap == 4 )
    {
        cmapped = atlas( valueWindowedNormalized.r );
    }
    else if( colormap == 5 )
    {
        cmapped = blueGreenPurple( valueWindowedNormalized.r );
    }
    else if( colormap == 6 )
    {
        cmapped = vector( valueWindowedOriginal, minV, scaleV );
        clip = clipZero( valueDescaled );   // vectors get clipped by their length
    }
    else if( colormap == 7 )
    {
        cmapped = viridis( valueWindowedNormalized.r );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Compose
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // build final color
    return vec4( cmapped.rgb, cmapped.a *           // did the colormap use a alpha value?
                              isNotBorder *         // is this a border pixel?
                              alpha *               // did the user specified an alpha?
                              clip *                // value clip?
                              clipTh *              // clip due to threshold?
                              float( cmactive ) );    // is it active?
}

#endif // WGECOLORMAPSIMPROVED_GLSL

