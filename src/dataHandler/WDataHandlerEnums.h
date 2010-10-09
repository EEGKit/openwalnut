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

#ifndef WDATAHANDLERENUMS_H
#define WDATAHANDLERENUMS_H

#include <stdint.h>

/**
 *  Dataset types and number values taken from the nifti1.h, at this point it's unknown if it makes sense
 *  to keep the bit coding, but it doesn't hurt either
 * \ingroup dataHandler
 */
enum dataType
{
    W_DT_NONE            =        0,
    W_DT_UNKNOWN         =        0,     /* what it says, dude           */
    W_DT_BINARY          =        1,     /* binary (1 bit/voxel)         */
    W_DT_UNSIGNED_CHAR   =        2,     /* unsigned char (8 bits/voxel) */
    W_DT_SIGNED_SHORT    =        4,     /* signed short (16 bits/voxel) */
    W_DT_SIGNED_INT      =        8,     /* signed int (32 bits/voxel)   */
    W_DT_FLOAT           =       16,     /* float (32 bits/voxel)        */
    W_DT_COMPLEX         =       32,     /* complex (64 bits/voxel)      */
    W_DT_DOUBLE          =       64,     /* double (64 bits/voxel)       */
    W_DT_RGB             =      128,     /* RGB triple (24 bits/voxel)   */
    W_DT_ALL             =      255,     /* not very useful (?)          */
    W_DT_INT8            =      256,     /* signed char (8 bits)         */
    W_DT_UINT16          =      512,     /* unsigned short (16 bits)     */
    W_DT_UINT8           =        2,     /* alias for unsigned char (8 bits/voxel) */
    W_DT_INT16           =        4,     /* unsigned short (16 bits) alias name for W_DT_SIGNED_SHORT */
    W_DT_UINT32          =      768,     /* unsigned int (32 bits)       */
    W_DT_INT64           =     1024,     /* long long (64 bits)          */
    W_DT_UINT64          =     1280,     /* unsigned long long (64 bits) */
    W_DT_FLOAT128        =     1536,     /* long double (128 bits)       */
    W_DT_COMPLEX128      =     1792,     /* double pair (128 bits)       */
    W_DT_COMPLEX256      =     2048,     /* long double pair (256 bits)  */
    W_DT_RGBA32          =     2304     /* 4 byte RGBA (32 bits/voxel)  */
};

/**
 * An object that knows an appropriate dataType flag for the typename T.
 */
template< typename T >
struct DataType
{
};

/**
 * Specialization for a specific datatype.
 */
template<>
struct DataType< char >
{
    //! the dataType flag
    static dataType const type = W_DT_INT8;
};

/**
 * Specialization for a specific datatype.
 */
template<>
struct DataType< unsigned char >
{
    //! the dataType flag
    static dataType const type = W_DT_UINT8;
};

/**
 * Specialization for a specific datatype.
 */
template<>
struct DataType< int16_t >
{
    //! the dataType flag
    static dataType const type = W_DT_INT16;
};

/**
 * Specialization for a specific datatype.
 */
template<>
struct DataType< uint16_t >
{
    //! the dataType flag
    static dataType const type = W_DT_UINT16;
};

/**
 * Specialization for a specific datatype.
 */
template<>
struct DataType< int >
{
    //! the dataType flag
    static dataType const type = W_DT_SIGNED_INT;
};

/**
 * Specialization for a specific datatype.
 */
template<>
struct DataType< unsigned int >
{
    //! the dataType flag
    static dataType const type = W_DT_UINT32;
};

/**
 * Specialization for a specific datatype.
 */
template<>
struct DataType< int64_t >
{
    //! the dataType flag
    static dataType const type = W_DT_INT64;
};

/**
 * Specialization for a specific datatype.
 */
template<>
struct DataType< uint64_t >
{
    //! the dataType flag
    static dataType const type = W_DT_UINT64;
};

/**
 * Specialization for a specific datatype.
 */
template<>
struct DataType< float >
{
    //! the dataType flag
    static dataType const type = W_DT_FLOAT;
};

/**
 * Specialization for a specific datatype.
 */
template<>
struct DataType< double >
{
    //! the dataType flag
    static dataType const type = W_DT_DOUBLE;
};

enum qformOrientation
{
    Left_to_Right,
    Posterior_to_Anterior,
    Inferior_to_Superior
};

/**
 * \defgroup dataHandler Data Handler
 *
 * \brief
 * This library implements the data storage facility of OpenWalnut.
 */
#endif  // WDATAHANDLERENUMS_H
