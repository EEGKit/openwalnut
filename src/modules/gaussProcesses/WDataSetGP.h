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

#ifndef WDATASETGP_H
#define WDATASETGP_H

#include <boost/shared_ptr.hpp>

#include "../../common/WCondition.h"
#include "../../common/WFlag.h"
#include "../../common/WMixinVector.h"
#include "../../common/WProgress.h"
#include "../../dataHandler/WDataSet.h"
#include "../../dataHandler/WDataSetDTI.h"
#include "../../dataHandler/WDataSetFibers.h"
#include "WGaussProcess.h"

/**
 * Stores many Gaussian processes.
 */
class WDataSetGP : public WMixinVector< WGaussProcess >, public WDataSet
{
public:
    /**
     * Assembles a dataset of gaussian processes out of the deterministic tracts as well the
     * underlying tensor field.
     *
     * \param tracts The dataset containing the deterministic tracts
     * \param tensors The underlying tensor field
     * \param shutdownFlag In case the constructor should abort this flag can be used to transport
     * the condition when to stop.  The object is left in an empty state then.
     * \param progress You may use this to indicate progress of dataset creation in terms of
     * processed tracts
     */
    WDataSetGP( boost::shared_ptr< const WDataSetFibers > tracts,
                boost::shared_ptr< const WDataSetDTI > tensors,
                const WBoolFlag& shutdownFlag,
                boost::shared_ptr< WProgress > progress );

    /**
     * Destructs this dataset.
     */
    virtual ~WDataSetGP();

    /**
     * Determines whether this dataset can be used as a texture.
     *
     * \return true if usable as texture.
     */
    virtual bool isTexture() const;

protected:
private:
};

inline bool WDataSetGP::isTexture() const
{
    return false;
}

#endif  // WDATASETGP_H
