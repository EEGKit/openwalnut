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

#ifndef WMCONVERTERINDEXES_H
#define WMCONVERTERINDEXES_H

#include <boost/shared_ptr.hpp>

#include "WMProtonData.h"

class WMConverterIndexes
{
public:
    typedef boost::shared_ptr< WMConverterIndexes > SPtr;

    WMConverterIndexes();

    void update( WMProtonData::SPtr protonData );

    int getParentID();

    int getPosX();

    int getPosY();

    int getPosZ();

    int getEdep();

    int getPDGEncoding();

    int getEventID();

private:
    int m_parentID = -1;

    int m_posX = -1;

    int m_posY = -1;

    int m_posZ = -1;

    int m_edep = -1;

    int m_PDGEncoding = -1;

    int m_eventID = -1;
};

#endif  // WMCONVERTERINDEXES_H
