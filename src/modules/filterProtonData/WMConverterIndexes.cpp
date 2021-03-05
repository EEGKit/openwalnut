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

#include "WMConverterIndexes.h"

WMConverterIndexes::WMConverterIndexes()
{
}

void WMConverterIndexes::update( WMProtonData::SPtr protonData )
{
    m_parentID = protonData->getColumnIndex( "parentID" );
    m_posX = protonData->getColumnIndex( "posX" );
    m_posY = protonData->getColumnIndex( "posY" );
    m_posZ = protonData->getColumnIndex( "posZ" );
    m_edep = protonData->getColumnIndex( "edep" );
    m_PDGEncoding = protonData->getColumnIndex( "PDGEncoding" );
    m_eventID = protonData->getColumnIndex( "eventID" );
}

int WMConverterIndexes::getParentID()
{
    return m_parentID;
}

int WMConverterIndexes::getPosX()
{
    return m_posX;
}

int WMConverterIndexes::getPosY()
{
    return m_posY;
}

int WMConverterIndexes::getPosZ()
{
    return m_posZ;
}

int WMConverterIndexes::getEdep()
{
    return m_edep;
}

int WMConverterIndexes::getPDGEncoding()
{
    return m_PDGEncoding;
}

int WMConverterIndexes::getEventID()
{
    return m_eventID;
}
