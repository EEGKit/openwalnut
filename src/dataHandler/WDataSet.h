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

#ifndef WDATASET_H
#define WDATASET_H

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <osg/ref_ptr>

#include "../common/WProperties.h"
#include "../common/WTransferable.h"
#include "WDataTexture3D_2.h"
#include "WExportDataHandler.h"

class WDataTexture3D;
class WCondition;
class WDataSetVector;

/**
 * Base class for all data set types. This class has a number of subclasses
 * specifying the different types of data sets. Two of the dataset types
 * represent single and time-dependent datasets (compound of several time
 * steps) respectively.
 * \ingroup dataHandler
 */
class OWDATAHANDLER_EXPORT WDataSet: public WTransferable, public boost::enable_shared_from_this< WDataSet > // NOLINT
{
public:
    /**
     * This constructor should be used if a dataSet does not stem from a file.
     * It presets the its correpsonding fileName as empty string.
     */
    WDataSet();

    /**
     * Since WDataSet is a base class and thus should be polymorphic we add
     * virtual destructor.
     */
    virtual ~WDataSet()
    {
    }

    /**
     * Set the name of the file that this data set stems from.
     * \param fileName the string representing the name
     */
    void setFileName( const std::string fileName );

    /**
     * Get the name of the file that this data set stems from.
     */
    std::string getFileName() const;

    /**
     * Determines whether this dataset can be used as a texture.
     *
     * \return true if usable as texture.
     */
    // TODO(schurade): pure virtual? Are WDataSet instances useful?
    virtual bool isTexture() const;

    /**
     * Checks if this dataset is a vector dataset.
     *
     * \return Returns a nonempty shared_ptr to it if it is a vector dataset, otherwise the pointer is empty!
     */
    virtual boost::shared_ptr< WDataSetVector > isVectorDataSet();

    /**
     * Returns the texture- representation of the dataset. May throw an exception if no texture is available.
     *
     * \return The texture.
     * \deprecated
     */
    virtual boost::shared_ptr< WDataTexture3D > getTexture();

    /**
     * Returns the texture- representation of the dataset. May throw an exception if no texture is available.
     *
     * \return The texture.
     * \deprecated
     */
    virtual osg::ref_ptr< WDataTexture3D_2 > getTexture2() const;

    /**
     * Gets the name of this prototype.
     *
     * \return the name.
     */
    virtual const std::string getName() const;

    /**
     * Gets the description for this prototype.
     *
     * \return the description
     */
    virtual const std::string getDescription() const;

    /**
     * Returns a prototype instantiated with the true type of the deriving class.
     *
     * \return the prototype.
     */
    static boost::shared_ptr< WPrototyped > getPrototype();

    /**
     * Gets the condition which is fired whenever the dataset gets some kind of dirty (threshold, opacity, ...)
     *
     * \return the condition, or NULL if the dataset has no texture.
     * \deprecated this is deprecated. Use dataset properties instead.
     */
    boost::shared_ptr< WCondition > getChangeCondition();

    /**
     * Return a pointer to the properties object of the dataset. Add all the modifiable settings here. This allows the user to modify several
     * properties of a dataset.
     *
     * \return the properties.
     */
    boost::shared_ptr< WProperties > getProperties() const;

    /**
     * Return a pointer to the information properties object of the dataset. The dataset intends these properties to not be modified.
     *
     * \return the properties.
     */
    boost::shared_ptr< WProperties > getInformationProperties() const;

protected:

    /**
     * The prototype as singleton.
     */
    static boost::shared_ptr< WPrototyped > m_prototype;

    /**
     * The property object for the dataset.
     */
    boost::shared_ptr< WProperties > m_properties;

    /**
     * The property object for the dataset containing only props whose purpose is "PV_PURPOSE_INFORMNATION". It is useful to define some property
     * to only be of informational nature. The GUI does not modify them. As it is a WProperties instance, you can use it the same way as
     * m_properties.
     */
    boost::shared_ptr< WProperties > m_infoProperties;

private:
    /**
     * Name of the file this data set was loaded from. This information
     * may allow hollowing data sets later. DataSets that were not loaded
     * from a file should have the empty string stored here.
     */
    std::string m_fileName;
};

#endif  // WDATASET_H

