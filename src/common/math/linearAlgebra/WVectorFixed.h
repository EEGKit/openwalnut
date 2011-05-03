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

#ifndef WVECTORFIXED_H
#define WVECTORFIXED_H

// defined in WMatrixFixed
#include "WMatrixFixed.h"

// Here: only some typedefs for vectors get defined

// Double vectors
typedef WMatrixFixed< double, 2, 1 > WVector2d_2;
typedef WMatrixFixed< double, 3, 1 > WVector3d_2;
typedef WMatrixFixed< double, 4, 1 > WVector4d_2;
typedef WMatrixFixed< double, 1, 2 > WVector2dRow_2;
typedef WMatrixFixed< double, 1, 3 > WVector3dRow_2;
typedef WMatrixFixed< double, 1, 4 > WVector4dRow_2;

// Float vectors
typedef WMatrixFixed< float, 2, 1 > WVector2f_2;
typedef WMatrixFixed< float, 3, 1 > WVector3f_2;
typedef WMatrixFixed< float, 4, 1 > WVector4f_2;
typedef WMatrixFixed< float, 1, 2 > WVector2fRow_2;
typedef WMatrixFixed< float, 1, 3 > WVector3fRow_2;
typedef WMatrixFixed< float, 1, 4 > WVector4fRow_2;

#endif  // WVECTORFIXED_H
