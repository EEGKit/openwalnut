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
#include <osg/MatrixTransform>
#include <osg/Geode>

#include "WGEGroupNode.h"

WGEGroupNode::WGEGroupNode():
    osg::MatrixTransform(),
    m_childOperationQueueDirty( false ),
    m_removeAll( false )
{
    setDataVariance( osg::Object::DYNAMIC );

    // setup an update callback
    m_nodeUpdater = osg::ref_ptr< SafeUpdaterCallback >( new SafeUpdaterCallback() );
    setUpdateCallback( m_nodeUpdater );

    osg::Matrix m;
    m.makeIdentity();
    setMatrix( m );
}

WGEGroupNode::~WGEGroupNode()
{
    // cleanup
}

void WGEGroupNode::insert( osg::ref_ptr< osg::Node > node )
{
    boost::unique_lock<boost::shared_mutex> lock = boost::unique_lock<boost::shared_mutex>( m_childOperationQueueLock );
    m_childOperationQueue.push( ChildOperation( true, node ) );
    m_childOperationQueueDirty = true;
    lock.unlock();
}

void WGEGroupNode::remove( osg::ref_ptr< osg::Node > node )
{
    boost::unique_lock<boost::shared_mutex> lock = boost::unique_lock<boost::shared_mutex>( m_childOperationQueueLock );
    m_childOperationQueue.push( ChildOperation( false, node ) );
    m_childOperationQueueDirty = true;
    lock.unlock();
}

void WGEGroupNode::clear()
{
    m_removeAll = true;
    m_childOperationQueueDirty = true;
}

void WGEGroupNode::SafeUpdaterCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    // the node also is a WGEGroupNode
    WGEGroupNode* rootNode = static_cast< WGEGroupNode* >( node );

    // write lock the insertion list
    boost::unique_lock<boost::shared_mutex> lock;

    // write lock the removal list
    if ( rootNode->m_childOperationQueueDirty )
    {
        lock = boost::unique_lock<boost::shared_mutex>( rootNode->m_childOperationQueueLock );
        if ( rootNode->m_removeAll )
        {
            rootNode->removeChild( 0, rootNode->getNumChildren() );
        }
        else
        {
            // insert all children which requested it
            /*for ( std::set< osg::ref_ptr< osg::Node > >::iterator iter = rootNode->m_childRemovalQueue.begin();
                  iter != rootNode->m_childRemovalQueue.end();
                  ++iter )
            {
                rootNode->removeChild( ( *iter ) );
            }*/
        }
        rootNode->dirtyBound();

        // all children added/removed -> clear
        rootNode->m_childOperationQueueDirty = false;
        rootNode->m_removeAll = false;
        //rootNode->m_childOperationQueue.clear();

        lock.unlock();
    }

    // forward the call
    traverse( node, nv );
}

