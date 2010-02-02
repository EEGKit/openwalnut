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

#ifndef WRMROIREPRESENTATION_H
#define WRMROIREPRESENTATION_H

#include <list>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/thread.hpp>

#include "WKdTree.h"
#include "./../../common/WProperties.h"
#include "../../graphicsEngine/WROI.h"
class WRMBranch;
/**
 * class encapsulates rois for the roi manager class
 */
class WRMROIRepresentation
{
public:
    /**
     * constructor
     *
     * \param roi
     * \param branch
     */
    WRMROIRepresentation( boost::shared_ptr< WROI > roi, boost::shared_ptr< WRMBranch > branch );

    /**
     * destructor
     */
    ~WRMROIRepresentation();

    /**
     * getter
     *
     * \return the roi
     */
    boost::shared_ptr< WROI > getROI();

    /**
     * getter for bit field for a selected fiber dataset
     *
     *\param index
     * \return the bit field
     */
    boost::shared_ptr< std::vector< bool > > getBitField( unsigned int index );

    /**
     * adds a bit field of a given size to the list of bit fields
     *
     * \param size
     */
    void addBitField( size_t size );

    /**
     * updates the bit fields with the fibers selected by the associated roi
     */
    void recalculate();

    /**
     * getter for dirty flag
     *
     * \return dirty flag
     */
    bool isDirty();

    /**
     * sets dirty flag true and notifies the branch
     */
    void setDirty();

    /**
     * getter
     *
     * \return the properties object
     */
    boost::shared_ptr< WProperties > getProperties();

protected:
    /**
     * slot gets called when a property has changed
     *
     * \param propertyName
     */
    void slotPropertyChanged( std::string propertyName );
private:
    /**
     * tests the kd tree for intersections with the roi
     * \param left
     * \param right
     * \param axis
     */
    void boxTest( int left, int right, int axis );

    /**
     * helper function return the line index of a given vertex
     *
     * \param point index of the point in question
     */
    unsigned int getLineForPoint( unsigned int point );

    bool m_dirty; //!< dirty flag, indicates the bit fields need updating

    boost::shared_ptr< WROI > m_roi; //!< stores a pointer to the associated roi

    boost::shared_ptr< WRMBranch > m_branch; //!< stores a pointer to the branch this roi belongs to

    std::list< boost::shared_ptr< std::vector<bool> > >m_bitFields; //!< list of bit fields, one for each loaded
                                                                    // fiber dataset

    /**
     * pointer to the bitfield, that is currently updated
     * this is used for the recursive update function, to reduce the amount of function parameters
     */
    boost::shared_ptr< std::vector<bool> >m_currentBitfield;

    /**
     * pointer to the array that is used for updating
     * this is used for the recurse update function, to reduce the amount of function parameters
     */
    boost::shared_ptr< std::vector< float > > m_currentArray;

    /**
     * pointer to the reverse array that is used for updating
     * this is used for the recurse update function, to reduce the amount of function parameters
     */
    boost::shared_ptr< std::vector< unsigned int > > m_currentReverse;

    boost::shared_ptr< WKdTree >m_kdTree; //!< stores a pointer to the kd tree

    std::vector<float> m_boxMin; //!< lower boundary of the box, used for boxtest
    std::vector<float> m_boxMax; //!< upper boundary of the box, used for boxtest

    /**
     * lock to prevent concurrent threads trying to update the osg node
     */
    boost::shared_mutex m_updateLock;

    /**
     * the property object for the module
     */
    boost::shared_ptr< WProperties > m_properties;
};

#endif  // WRMROIREPRESENTATION_H
