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

#ifndef WREADER2_H
#define WREADER2_H

#include <string>
#include <vector>

#include <boost/filesystem.hpp>

#include "../WDataSet2.h"

/**
 * A base class for file readers that can be used by the data loading modules.
 */
class WReader2 : public WPrototyped
{
public:

    //! Conveniance typedef for a boost::shared_ptr.
    typedef boost::shared_ptr< WReader2 > SPtr;

    //! Conveniance typedef for a boost::shared_ptr to a const object.
    typedef boost::shared_ptr< WReader2 const > ConstSPtr;

    /**
     * Creates a read object for file reading.
     */
    WReader2();

    /**
     * Destructor.
     */
    virtual ~WReader2();

    /**
     * Check if this file can be loaded by this reader.
     *
     * \param filename The name or path to the file to be loaded.
     * \return true if a dataset can be contructed from the content of the file by this reader.
     */
    virtual bool acceptFile( boost::filesystem::path const& filename ) const = 0;

    /**
     * Suggest the type of the datasets that could be constructed from the content of the file.
     * It is assumed that you checked the file with acceptFile first.
     *
     * \param filename The name or path to the file to be loaded.
     * \return A vector of pointers to the different dataset types that can be constructed from the file.
     */
    virtual std::vector< WDataSet2Base::SPtr > suggestDataSets( boost::filesystem::path const& filename ) const = 0;

    /**
     * Actually constructs the dataset.
     *
     * \param filename The name or path to the file to be loaded.
     * \param dataSetSample A pointer from which the type of the dataset to be constructed can be inferred.
     * \return The loaded dataset.
     */
    virtual WDataSet2Base::SPtr load( boost::filesystem::path const& filename,
                                     WDataSet2Base::ConstSPtr const& dataSetSample ) const = 0;

protected:
private:
};

#endif  // WREADER2_H
