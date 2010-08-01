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

#ifndef WLOADEREEG_H
#define WLOADEREEG_H

#include <string>

#include "../WEEG.h"
#include "../exceptions/WDHIOFailure.h"
#include "WReader.h"


/**
 * Abstract base class for all Loaders who handle with EEG data
 * \ingroup dataHandler
 */
class WLoaderEEG : public WReader
{
public:
protected:
    /**
     * Constructs basic eeg Loader with a file name.
     *
     * \param fileName Path to be loaded
     * \throw WDHIOFailure in case of an error
     */
    explicit WLoaderEEG( std::string fileName ) throw( WDHIOFailure );

    /**
     * Load electrode positions from ELC file with same name
     *
     * \return electrode library containig the loaded positions
     */
    WEEGElectrodeLibrary extractElectrodePositions();
private:
};

#endif  // WLOADEREEG_H
