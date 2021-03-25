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

#include "WConverterIndexes.h"


WConverterIndexes::WConverterIndexes()
{
}

void WConverterIndexes::update( WProtonData::SPtr protonData )
{
    m_parentID = protonData->getColumnIndexBySelection( "Parent id" );
    m_posX = protonData->getColumnIndexBySelection( "X" );
    m_posY = protonData->getColumnIndexBySelection( "Y" );
    m_posZ = protonData->getColumnIndexBySelection( "Z" );
    m_edep = protonData->getColumnIndexBySelection( "Energy deposition" );
    m_PDGEncoding = protonData->getColumnIndexBySelection( "Particle Data Group" );
    m_eventID = protonData->getColumnIndexBySelection( "Event id" );
}

int WConverterIndexes::getParentID()
{
    return m_parentID;
}

int WConverterIndexes::getPosX()
{
    return m_posX;
}

int WConverterIndexes::getPosY()
{
    return m_posY;
}

int WConverterIndexes::getPosZ()
{
    return m_posZ;
}

int WConverterIndexes::getEdep()
{
    return m_edep;
}

int WConverterIndexes::getPDGEncoding()
{
    return m_PDGEncoding;
}

int WConverterIndexes::getEventID()
{
    return m_eventID;
}
