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

#ifndef WROIMANAGER_H
#define WROIMANAGER_H

#include <list>
#include <memory>
#include <shared_mutex>
#include <vector>

#include "WRMBranch.h"

/**
 * Class to store and manage different ROI's for fiber selection
 */
class  WROIManager: public std::enable_shared_from_this< WROIManager >
{
public:
    /**
     * standard constructor
     */
    WROIManager();

    /**
     * destructor
     */
    ~WROIManager();

    /**
     * Add a new branch.
     *
     * \return the new branch.
     */
    std::shared_ptr< WRMBranch > addBranch();

    /**
     * Adds a new main ROI
     *
     * \param newRoi
     */
    void addRoi( osg::ref_ptr< WROI > newRoi );

    /**
     * Adds a new ROI below a main ROI
     *
     * \param newRoi
     * \param parentRoi
     */
    void addRoi( osg::ref_ptr< WROI > newRoi, osg::ref_ptr< WROI > parentRoi );

    /**
     * Add a ROI to a branch.
     *
     * \param newRoi the new ROI to add
     * \param toBranch the branch to add the ROI to.
     */
    void addRoi( osg::ref_ptr< WROI > newRoi, std::shared_ptr< WRMBranch > toBranch );

    /**
     * removes a roi
     *
     * \param roi
     */
    void removeRoi( osg::ref_ptr< WROI > roi );

    /**
     * removes a branch
     *
     * \param roi the first roi in the branch
     */
    void removeBranch( osg::ref_ptr< WROI > roi );

    /**
     * getter
     * returns the branch item the roi is in
     * \param roi
     * \return branch
     */
    std::shared_ptr< WRMBranch> getBranch( osg::ref_ptr< WROI > roi );

    /**
     * sets the dirty flag which will cause recalculation of the bit field
     */
    void setDirty();

    /**
     *  getter
     *   \param reset if true the dirty flag will be set to false
     *  \return the dirty flag
     */
    bool dirty( bool reset = false );


    /// @cond Supress doxygen because it produces warning here becuase it does not correctly understand the C++ code.
    /**
     * Add a specified notifier to the list of default notifiers which get connected to each added roi.
     *
     * \param notifier  the notifier function
     */
    void addAddNotifier( std::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > notifier );

    /**
     * Remove a specified notifier from the list of default notifiers which get connected to each added roi.
     *
     * \param notifier  the notifier function
     */
    void removeAddNotifier( std::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > notifier );


    /**
     * Add a specified notifier to the list of default notifiers which get connected to each removed roi.
     *
     * \param notifier  the notifier function
     */
    void addRemoveNotifier( std::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > notifier );

    /**
     * Remove a specified notifier from the list of default notifiers which get connected to each removed roi.
     *
     * \param notifier  the notifier function
     */
    void removeRemoveNotifier( std::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > notifier );

    /**
     * Add a specified notifier to the list of default notifiers which get connected to each removed branch.
     *
     * \param notifier  the notifier function
     */
    void addRemoveBranchNotifier( std::shared_ptr< boost::function< void( std::shared_ptr< WRMBranch > ) > > notifier );

    /**
     * Remove a specified notifier from the list of default notifiers which get connected to each removed branch.
     *
     * \param notifier  the notifier function
     */
    void removeRemoveBranchNotifier(  std::shared_ptr< boost::function< void( std::shared_ptr< WRMBranch > ) > > notifier );
    /// @endcond

    /**
     * setter
     * \param roi
     */
    void setSelectedRoi( osg::ref_ptr< WROI > roi );

    /**
     * getter
     *
     * \return Pointer to the currently (in the ROI manager) selected ROI
     */
    osg::ref_ptr< WROI > getSelectedRoi();

    /**
     * getter for the properties object
     * \return the properties object
     */
    std::shared_ptr< WProperties > getProperties();

    /**
     * ROI list
     */
    typedef std::vector< osg::ref_ptr< WROI > > ROIs;

    /**
     * getter
     * \return all existing rois
     */
    ROIs getRois() const;

    /**
     * Branches list
     */
    typedef std::vector< std::shared_ptr< WRMBranch > > Branches;

    /**
     * Get a copy of the current branch list. Please note that after getting the list, it might already have been changed by another thread.
     *
     * \return the list of current branches
     */
    Branches getBranches() const;

protected:
private:
    std::list< std::shared_ptr< WRMBranch > > m_branches; //!< list of branches in the logical tree structure

    /**
     * Lock for associated notifiers set.
     */
    std::shared_mutex m_associatedNotifiersLock;

    /**
     * The notifiers connected to added rois by default.
     */
    std::list<  std::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > > m_addNotifiers;

    /**
     * The notifiers connected to removed rois by default.
     */
    std::list<  std::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > > m_removeNotifiers;

    /**
     * The notifiers connected to removed rois by default.
     */
    std::list<  std::shared_ptr< boost::function< void( std::shared_ptr< WRMBranch > ) > > > m_removeBranchNotifiers;


    osg::ref_ptr< WROI > m_selectedRoi; //!< stores a pointer to the currently selected roi

    /**
     * The property object for the module.
     */
    std::shared_ptr< WProperties > m_properties;

    /**
     * dirty flag
     */
    WPropBool m_dirty;
};

inline bool WROIManager::dirty( bool reset )
{
    bool ret = m_dirty->get();
    if( reset )
    {
        m_dirty->set( false );
    }
    return ret;
}

inline std::shared_ptr< WProperties > WROIManager::getProperties()
{
    return m_properties;
}

#endif  // WROIMANAGER_H
