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

#include "WDataSet2.h"

boost::shared_ptr< WPrototyped > WDataSet2Base::m_prototype = boost::shared_ptr< WPrototyped >();

WDataSet2Base::WDataSet2Base()
    : WTransferable()
{
}

WDataSet2Base::~WDataSet2Base()
{
}

std::string const WDataSet2Base::getName() const
{
    return "Dataset base class";
}

std::string const WDataSet2Base::getDescription() const
{
    return "a dataset base class";
}

boost::shared_ptr< WPrototyped > WDataSet2Base::getPrototype()
{
    if ( !m_prototype )
    {
        m_prototype = boost::shared_ptr< WPrototyped >( new WDataSet2Base() );
    }

    return m_prototype;
}
