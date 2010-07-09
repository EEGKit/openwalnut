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

#ifndef WDATASETFIBERVECTOR_H
#define WDATASETFIBERVECTOR_H

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "../common/datastructures/WFiber.h"
#include "WDataSet.h"
#include "WDataSetFibers.h"

/**
 * Represents a simple set of WFibers.
 */
class WDataSetFiberVector : public WMixinVector< wmath::WFiber >, public WDataSet
{
public:
    /**
     * Default constructor for creating an empty fiber vector.
     */
    WDataSetFiberVector();

    /**
     * Constructs a new set of WFibers
     *
     * \param fibs Fiber vector to store in this data set
     */
    explicit WDataSetFiberVector( boost::shared_ptr< std::vector< wmath::WFiber > > fibs );

    /**
     * Convert a WDataSetFibers into a fiber vector dataset.
     *
     * \param fiberDS Dataset which has to be converted
     */
    explicit WDataSetFiberVector( boost::shared_ptr< WDataSetFibers > fiberDS );

    /**
     * Copy constructor for fibers
     *
     * \param other Instance to copy from
     */
    // defined since rule of three
    WDataSetFiberVector( const WDataSetFiberVector& other ); // NOLINT since cxxtest need it as unexcplicit!

    /**
     * Destructs WDataSetFiberVector instances
     */
    virtual ~WDataSetFiberVector(); // defined since rule of three

    /**
     * Operator for assigning instances of WDataSetFiberVector
     *
     * \param other Instance which should replace this
     *
     * \return Reference for further usage of the outcome of the assigment
     */
    WDataSetFiberVector& operator=( const WDataSetFiberVector& other ); // defined since rule of three

    /**
     * Sort fibers descending on their length.
     */
    void sortDescLength();

    /**
     * Generates new WDataSetFiberVector out of the used fibers from this dataset.
     *
     * \param unused If the i'th postion of this vector is true, then this fiber is considered as used.
     *
     * \return A reference to the new generate WDataSetFiberVector
     */
    boost::shared_ptr< WDataSetFiberVector > generateDataSetOutOfUsedFibers( const std::vector< bool > &unused ) const;

    /**
     * Determines whether this dataset can be used as a texture.
     *
     * \return true if usable as texture.
     */
    virtual bool isTexture() const;

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
     * Convert this dataset into WDataSetFibers format for other purposes if needed. (e.g. display)
     *
     * \return Reference to the dataset in WDataSetFibers format
     */
    boost::shared_ptr< WDataSetFibers > toWDataSetFibers() const;

protected:
    /**
     * The prototype as singleton.
     */
    static boost::shared_ptr< WPrototyped > m_prototype;
};

#endif  // WDATASETFIBERVECTOR_H
