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

#ifndef WSCALARSTRUCTURAL_H
#define WSCALARSTRUCTURAL_H

#include <boost/mpl/vector.hpp>

#include "WParameterTypes.h"

/**
 * Structural type, representing a single scalar value. The only parameter of a scalar type is its integral type.
 */
class WScalarStructural
{
    /**
     * A type-vector containing all the needed parameters for this structural type. Extend this to your needs.
     */
    typedef boost::mpl::vector< IntegralVariant > Parameters;   // NOLINT - no this is not a std::vector and we do not need to include <vector>.
};

#endif  // WSCALARSTRUCTURAL_H

