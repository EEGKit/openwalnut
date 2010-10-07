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
#include <vector>

#include "../graphicsEngine/WGraphicsEngine.h"

#include "WROIManager.h"
#include "WRMBranch.h"


WRMBranch::WRMBranch( boost::shared_ptr< WROIManager > roiManager ) :
    m_roiManager( roiManager )
{
    properties();
}

WRMBranch::~WRMBranch()
{
}

void WRMBranch::properties()
{
    m_properties = boost::shared_ptr< WProperties >( new WProperties( "Properties", "This branch's properties" ) );

    m_dirty = m_properties->addProperty( "Dirty", "", true, boost::bind( &WRMBranch::propertyChanged, this ) );
    m_dirty->setHidden( true );
    m_isNot = m_properties->addProperty( "NOT", "description", false, boost::bind( &WRMBranch::propertyChanged, this ) );
    m_bundleColor = m_properties->addProperty( "Bundle Color", "description", WColor( 1.0, 0.0, 0.0, 1.0 ),
            boost::bind( &WRMBranch::propertyChanged, this ) );
}

void WRMBranch::propertyChanged()
{
    setDirty();
}


void WRMBranch::addRoi( osg::ref_ptr< WROI > roi )
{
    m_rois.push_back( roi );
    setDirty();
}

bool WRMBranch::contains( osg::ref_ptr< WROI > roi )
{
    for( std::list< osg::ref_ptr< WROI > >::iterator iter = m_rois.begin(); iter != m_rois.end(); ++iter )
    {
        if ( ( *iter ) == roi )
        {
            return true;
        }
    }
    return false;
}

void WRMBranch::removeRoi( osg::ref_ptr< WROI > roi )
{
    for( std::list< osg::ref_ptr< WROI > >::iterator iter = m_rois.begin(); iter != m_rois.end(); ++iter )
    {
        if ( ( *iter ) == roi )
        {
            m_rois.erase( iter );
            setDirty();
            break;
        }
    }
}

void WRMBranch::getRois( std::vector< osg::ref_ptr< WROI > >& roiVec ) // NOLINT
{
    for( std::list< osg::ref_ptr< WROI > >::iterator iter = m_rois.begin(); iter != m_rois.end(); ++iter )
    {
        roiVec.push_back( ( *iter ) );
    }
}

void WRMBranch::removeAllRois()
{
    for( std::list< osg::ref_ptr< WROI > >::iterator iter = m_rois.begin(); iter != m_rois.end(); ++iter )
    {
        WGraphicsEngine::getGraphicsEngine()->getScene()->remove( ( *iter ) );
    }

    m_rois.clear();
}

void WRMBranch::setDirty()
{
    m_dirty->set( true );
    m_roiManager->setDirty();

    for ( std::list< boost::function< void() > >::iterator iter = m_changeNotifiers.begin();
                iter != m_changeNotifiers.end(); ++iter )
    {
        ( *iter )();
    }
}

osg::ref_ptr< WROI > WRMBranch::getFirstRoi()
{
    return m_rois.front();
}

boost::shared_ptr< WROIManager > WRMBranch::getRoiManager()
{
    return m_roiManager;
}

boost::shared_ptr< WProperties > WRMBranch::getProperties()
{
    return m_properties;
}

void WRMBranch::addChangeNotifier( boost::function< void() > notifier )
{
    boost::unique_lock< boost::shared_mutex > lock;
    lock = boost::unique_lock< boost::shared_mutex >( m_associatedNotifiersLock );
    m_changeNotifiers.push_back( notifier );
    lock.unlock();
}
