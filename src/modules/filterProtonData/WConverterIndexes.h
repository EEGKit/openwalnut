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

#ifndef WCONVERTERINDEXES_H
#define WCONVERTERINDEXES_H

#include <memory>


#include "WProtonData.h"


/**
 * Holds the indexes of the columns and is able to update them.
 */
class WConverterIndexes
{
public:
    /**
     * A shared_ptr to this class.
     */
    typedef std::shared_ptr< WConverterIndexes > SPtr;

    /**
     * Empty constructor for class creation.
     */
    WConverterIndexes();

    /**
     * Updates the indexes based on the data given.
     * \param protonData The data which contains the columns.
     */
    void update( WProtonData::SPtr protonData );

    /**
     * Gets the parentID index as integer.
     * \return int The parentID index.
     */
    int getParentID();

    /**
     * Gets the posX index as integer.
     * \return int The posX index.
     */
    int getPosX();


    /**
     * Gets the posY index as integer.
     * \return int The posY index.
     */
    int getPosY();

    /**
     * Gets the posZ index as integer.
     * \return int The posZ index.
     */
    int getPosZ();

    /**
     * Gets the edep index as integer.
     * \return int The edep index.
     */
    int getEdep();

    /**
     * Gets the PDGEncoding index as integer.
     * \return int The PDGEncoding index.
     */
    int getPDGEncoding();

    /**
     * Gets the eventID index as integer.
     * \return int The eventID index.
     */
    int getEventID();

private:
    int m_parentID = -1;    //!< The parentID index.

    int m_posX = -1;    //!< The posX index.

    int m_posY = -1;    //!< The posY index.

    int m_posZ = -1;    //!< The posZ index.

    int m_edep = -1;    //!< The edep index.

    int m_PDGEncoding = -1; //!< The PDGEncoding index.

    int m_eventID = -1; //!< The eventID index.
};

#endif  // WCONVERTERINDEXES_H
