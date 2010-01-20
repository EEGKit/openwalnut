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

#include <set>

#include <osg/ShapeDrawable>
#include <osg/Group>
#include <osg/Geode>

#include "../common/WLogger.h"

#include "WGEScene.h"

WGEScene::WGEScene():
    osg::Group(),
    m_insertionQueueDirty( false ),
    m_removalQueueDirty( false )
{
    WLogger::getLogger()->addLogMessage( "Initializing OpenSceneGraph Root Node", "GE", LL_INFO );

    setDataVariance( osg::Object::DYNAMIC );

    // setup an update callback
    m_nodeUpdater = osg::ref_ptr< SafeUpdaterCallback >( new SafeUpdaterCallback() );
    setUpdateCallback( m_nodeUpdater );
}

WGEScene::~WGEScene()
{
    WLogger::getLogger()->addLogMessage( "Shutting down OpenSceneGraph Root Node", "GE", LL_INFO );

    // cleanup
}

void WGEScene::insert( osg::ref_ptr< osg::Node > node )
{
    boost::unique_lock<boost::shared_mutex> lock = boost::unique_lock<boost::shared_mutex>( m_childInsertionQueueLock );
    m_childInsertionQueue.insert( node );
    m_insertionQueueDirty = true;
    lock.unlock();
}

void WGEScene::remove( osg::ref_ptr< osg::Node > node )
{
    boost::unique_lock<boost::shared_mutex> lock = boost::unique_lock<boost::shared_mutex>( m_childRemovalQueueLock );
    m_childRemovalQueue.insert( node );
    m_removalQueueDirty = true;
    lock.unlock();
}

void WGEScene::SafeUpdaterCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    // the node also is a WGEScene
    WGEScene* rootNode = static_cast< WGEScene* >( node );

    // write lock the insertion list
    boost::unique_lock<boost::shared_mutex> lock;

    if ( rootNode->m_insertionQueueDirty )
    {
        lock = boost::unique_lock<boost::shared_mutex>( rootNode->m_childInsertionQueueLock );

        // insert all children which requested it
        for ( std::set< osg::ref_ptr< osg::Node > >::iterator iter = rootNode->m_childInsertionQueue.begin();
              iter != rootNode->m_childInsertionQueue.end();
              ++iter )
        {
            rootNode->addChild( ( *iter ) );
        }
        // all children added -> clear
        rootNode->m_insertionQueueDirty = false;
        rootNode->m_childInsertionQueue.clear();
        lock.unlock();
    }

    // same game for removal request list

    // write lock the removal list
    if ( rootNode->m_removalQueueDirty )
    {
        lock = boost::unique_lock<boost::shared_mutex>( rootNode->m_childRemovalQueueLock );

        // insert all children which requested it
        for ( std::set< osg::ref_ptr< osg::Node > >::iterator iter = rootNode->m_childRemovalQueue.begin();
              iter != rootNode->m_childRemovalQueue.end();
              ++iter )
        {
            rootNode->removeChild( ( *iter ) );
        }
        // all children added -> clear
        rootNode->m_removalQueueDirty = false;
        rootNode->m_childRemovalQueue.clear();
        lock.unlock();
    }

    // forward the call
    traverse( node, nv );
}

