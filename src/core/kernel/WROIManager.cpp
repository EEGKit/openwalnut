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

#include <list>
#include <memory>
#include <vector>

#include "../common/WAssert.h"
#include "../graphicsEngine/WGraphicsEngine.h"
#include "WROIManager.h"

WROIManager::WROIManager()
{
    m_properties = std::shared_ptr< WProperties >( new WProperties( "Properties", "Module's properties" ) );
    m_dirty = m_properties->addProperty( "dirty", "dirty flag", true );
}

WROIManager::~WROIManager()
{
}

void WROIManager::addRoi( osg::ref_ptr< WROI > newRoi, std::shared_ptr< WRMBranch > toBranch )
{
    // add roi to branch
    toBranch->addRoi( newRoi );

    for( std::list< std::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > >::iterator iter = m_addNotifiers.begin();
            iter != m_addNotifiers.end(); ++iter )
    {
        ( **iter )( newRoi );
    }
}

void WROIManager::addRoi( osg::ref_ptr< WROI > newRoi )
{
    addRoi( newRoi, addBranch() );
}

std::shared_ptr< WRMBranch > WROIManager::addBranch()
{
    // create new branch
    std::shared_ptr< WRMBranch > newBranch( new WRMBranch( shared_from_this() ) );
    // add branch to list
    m_branches.push_back( newBranch );

    // return
    return newBranch;
}

void WROIManager::addRoi( osg::ref_ptr< WROI > newRoi, osg::ref_ptr< WROI > parentRoi )
{
    // find branch
    std::shared_ptr< WRMBranch > branch;
    for( std::list< std::shared_ptr< WRMBranch > >::iterator iter = m_branches.begin(); iter != m_branches.end(); ++iter )
    {
        if( ( *iter ).get()->contains( parentRoi ) )
        {
            branch = ( *iter );
        }
    }
    // add roi to branch
    branch->addRoi( newRoi );

    for( std::list< std::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > >::iterator iter = m_addNotifiers.begin();
            iter != m_addNotifiers.end(); ++iter )
    {
        ( **iter )( newRoi );
    }
}

void WROIManager::removeRoi( osg::ref_ptr< WROI > roi )
{
    WGraphicsEngine::getGraphicsEngine()->getScene()->remove( roi );

    for( std::list< std::shared_ptr< WRMBranch > >::iterator iter = m_branches.begin(); iter != m_branches.end(); ++iter )
    {
        ( *iter )->removeRoi( roi );

        if( ( *iter )->empty() )
        {
            for( std::list< std::shared_ptr< boost::function< void( std::shared_ptr< WRMBranch > ) > > >::iterator iter2
                      = m_removeBranchNotifiers.begin();
                  iter2 != m_removeBranchNotifiers.end();
                  ++iter2 )
            {
                ( **iter2 )( *iter );
            }
            m_branches.erase( iter );
            break;
        }
    }
    setDirty();

    for( std::list< std::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > >::iterator iter
              = m_removeNotifiers.begin();
          iter != m_removeNotifiers.end();
          ++iter )
    {
        ( **iter )( roi );
    }
}

void WROIManager::removeBranch( osg::ref_ptr< WROI > roi )
{
    for( std::list< std::shared_ptr< WRMBranch > >::iterator iter = m_branches.begin(); iter != m_branches.end(); ++iter )
    {
        if( roi == ( *iter )->getFirstRoi() )
        {
            ( *iter )->removeAllRois();
        }

        if( ( *iter )->empty() )
        {
            for( std::list< std::shared_ptr< boost::function< void( std::shared_ptr< WRMBranch > ) > > >::iterator iter2
                      = m_removeBranchNotifiers.begin();
                  iter2 != m_removeBranchNotifiers.end();
                  ++iter2 )
            {
                ( **iter2 )( *iter );
            }
            m_branches.erase( iter );
            break;
        }
    }
    setDirty();
}

std::shared_ptr< WRMBranch> WROIManager::getBranch( osg::ref_ptr< WROI > roi )
{
    std::shared_ptr< WRMBranch> branch;

    for( std::list< std::shared_ptr< WRMBranch > >::iterator iter = m_branches.begin(); iter != m_branches.end(); ++iter )
    {
        if( ( *iter )->contains( roi ) )
        {
            branch = ( *iter );
        }
    }
    return branch;
}

void WROIManager::setDirty()
{
    m_dirty->set( true );
}

/// @cond Supress doxygen because it produces warning here becuase it does not correctly understand the C++ code.
void WROIManager::addAddNotifier( std::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > notifier )
{
    std::unique_lock< std::shared_mutex > lock;
    lock = std::unique_lock< std::shared_mutex >( m_associatedNotifiersLock );
    m_addNotifiers.push_back( notifier );
    lock.unlock();
}

void WROIManager::removeAddNotifier( std::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > notifier )
{
    std::unique_lock< std::shared_mutex > lock;
    lock = std::unique_lock< std::shared_mutex >( m_associatedNotifiersLock );
    std::list<  std::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > >::iterator it;
    it = std::find( m_addNotifiers.begin(), m_addNotifiers.end(), notifier );
    if( it != m_addNotifiers.end() )
    {
        m_addNotifiers.erase( it );
    }
    lock.unlock();
}

void WROIManager::addRemoveNotifier( std::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > notifier )
{
    std::unique_lock< std::shared_mutex > lock;
    lock = std::unique_lock< std::shared_mutex >( m_associatedNotifiersLock );
    m_removeNotifiers.push_back( notifier );
    lock.unlock();
}

void WROIManager::removeRemoveNotifier( std::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > notifier )
{
    std::unique_lock< std::shared_mutex > lock;
    lock = std::unique_lock< std::shared_mutex >( m_associatedNotifiersLock );
    std::list<  std::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > >::iterator it;
    it = std::find( m_removeNotifiers.begin(), m_removeNotifiers.end(), notifier );
    if( it != m_removeNotifiers.end() )
    {
        m_removeNotifiers.erase( it );
    }
    lock.unlock();
}

void WROIManager::addRemoveBranchNotifier(  std::shared_ptr< boost::function< void( std::shared_ptr< WRMBranch > ) > > notifier )
{
    std::unique_lock< std::shared_mutex > lock;
    lock = std::unique_lock< std::shared_mutex >( m_associatedNotifiersLock );
    m_removeBranchNotifiers.push_back( notifier );
    lock.unlock();
}

void WROIManager::removeRemoveBranchNotifier(  std::shared_ptr< boost::function< void( std::shared_ptr< WRMBranch > ) > > notifier )
{
    std::unique_lock< std::shared_mutex > lock;
    lock = std::unique_lock< std::shared_mutex >( m_associatedNotifiersLock );
    std::list<  std::shared_ptr< boost::function< void( std::shared_ptr< WRMBranch > ) > > >::iterator it;
    it = std::find( m_removeBranchNotifiers.begin(), m_removeBranchNotifiers.end(), notifier );
    if( it != m_removeBranchNotifiers.end() )
    {
        m_removeBranchNotifiers.erase( it );
    }
    lock.unlock();
}
/// @endcond

void WROIManager::setSelectedRoi( osg::ref_ptr< WROI > roi )
{
    m_selectedRoi = roi;
}

osg::ref_ptr< WROI > WROIManager::getSelectedRoi()
{
    return m_selectedRoi;
}

WROIManager::ROIs WROIManager::getRois() const
{
    ROIs returnVec;

    for( std::list< std::shared_ptr< WRMBranch > >::const_iterator iter = m_branches.begin(); iter != m_branches.end(); ++iter )
    {
        ( *iter )->getRois( returnVec );
    }
    return returnVec;
}

WROIManager::Branches WROIManager::getBranches() const
{
    // copy to this vec
    Branches returnVec;

    // copy
    for( std::list< std::shared_ptr< WRMBranch > >::const_iterator iter = m_branches.begin(); iter != m_branches.end(); ++iter )
    {
        returnVec.push_back( *iter );
    }
    return returnVec;
}
