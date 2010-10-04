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

#ifndef WRMBRANCH_H
#define WRMBRANCH_H

#include <list>
#include <string>
#include <vector>

#include <boost/enable_shared_from_this.hpp>

#include "../common/WProperties.h"

#include "../graphicsEngine/WROI.h"

#include "WExportKernel.h"

class WROIManager;

/**
 * implements a branch in the tree like structure for rois
 */
class OWKERNEL_EXPORT WRMBranch : public boost::enable_shared_from_this< WRMBranch >
{
public:
    /**
     * construtor
     * \param roiManager
     */
    explicit WRMBranch( boost::shared_ptr< WROIManager > roiManager );

    /**
     * destructor
     */
    ~WRMBranch();

    /**
     * adds a roi to the branch
     *
     * \param roi
     */
    void addRoi( osg::ref_ptr< WROI > roi );

    /**
     * removes a roi from the branch
     *
     * \param roi
     */
    void removeRoi( osg::ref_ptr< WROI > roi );

    /**
     * removes all rois from the branch
     *
     */
    void removeAllRois();

    /**
     * getter for dirty flag
     *
     * \param reset when true the dirty flag will be set to false
     * \return the dirty flag
     */
    bool dirty( bool reset = false );

    /**
     * sets dirty flag true and notifies the branch
     */
    void setDirty();

    /**
     * returns whether the branch is empty
     */
    bool empty();

    /**
     * checks wether a roi is in this branch
     * \param roi
     * \return true if the roi is in the branch, false otherwise
     */
    bool contains( osg::ref_ptr< WROI > roi );

    /**
     * returns a pointer to the first roi in the branch
     *
     * \return the roi
     */
    osg::ref_ptr< WROI > getFirstRoi();

    /**
     * getter for roi manager pointer
     *
     * \return the roi manager
     */
    boost::shared_ptr< WROIManager > getRoiManager();

    /**
     * returns the properties object
     */
    boost::shared_ptr< WProperties > getProperties();

    /**
     * getter for the NOT flag
     * \return flag
     */
    bool isNot();


protected:
    /**
     * initializes properties
     */
    void properties();

    /**
     * slot gets called when a property has changed
     *
     */
    void propertyChanged();
private:
    boost::shared_ptr< WROIManager > m_roiManager; //!< stores a pointer to the roi manager

    std::list< osg::ref_ptr< WROI > > m_rois; //!< list of rois in this this branch,
                                                                  // first in the list is the master roi
    /**
     * the property object for the module
     */
    boost::shared_ptr< WProperties > m_properties;

    WPropBool m_dirty; //!< dirty flag to indicate if anything has changed within the branch

    /**
     * indicates if the branch is negated
     */
    WPropBool m_isNot;

    /**
     * The color used when in isosurface mode for blending.
     */
    WPropColor m_bundleColor;
};

inline bool WRMBranch::empty()
{
    return m_rois.empty();
}

inline bool WRMBranch::dirty( bool reset )
{
    bool ret = m_dirty->get();
    if ( reset )
    {
        m_dirty->set( false );
    }
    return ret;
}

inline bool WRMBranch::isNot()
{
    return m_isNot->get();
}
#endif  // WRMBRANCH_H
