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
