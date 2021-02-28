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