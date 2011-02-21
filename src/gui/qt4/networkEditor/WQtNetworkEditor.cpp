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

#include <string>
#include <iostream>

#include <boost/shared_ptr.hpp>

#include <QtGui/QDockWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QGraphicsView>
#include <QtGui/QGraphicsItem>
#include <QtGui/QGraphicsItemGroup>

#include "../WMainWindow.h"
#include "WQtNetworkEditor.h"
#include "WQtNetworkPort.h"

#include "../../../kernel/WModule.h"
#include "../../../kernel/WModuleFactory.h"
#include "../controlPanel/WQtControlPanel.h"
#include "../events/WEventTypes.h"
#include "../events/WModuleAssocEvent.h"
#include "../events/WModuleConnectEvent.h"
#include "../events/WModuleDeleteEvent.h"
#include "../events/WModuleDisconnectEvent.h"
#include "../events/WModuleReadyEvent.h"
#include "../events/WModuleRemovedEvent.h"

WQtNetworkEditor::WQtNetworkEditor( WMainWindow* parent )
    : QDockWidget( "NetworkEditor", parent ),
    timerId( 0 )
{
    m_mainWindow = parent;

    m_panel = new QWidget( this );

    m_view = new QGraphicsView();
    m_view->setDragMode( QGraphicsView::RubberBandDrag );
    m_view->setRenderHint( QPainter::Antialiasing );
    m_view->setMinimumSize( 20, 20 );

    m_scene = new WQtNetworkScene();
//    m_scene->setSceneRect( -200.0, -200.0, 400.0, 400.0 );
    m_scene->setSceneRect( m_scene->itemsBoundingRect() );

    m_view->setScene( m_scene );

    m_layout = new QVBoxLayout;
    m_layout->addWidget( m_view );

    m_panel->setLayout( m_layout );

    this->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
    this->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    setWidget( m_panel );
    connect( m_scene, SIGNAL( selectionChanged() ), this, SLOT( selectItem() ) );

    // this fakeitem is added to the scene to get a better behavior of the forced
    // based layout. ALL WQtNetworkItems in the scene are "connected" to this
    // object to avoid that conneceted groups push away each other.
    QGraphicsRectItem *fake = new QGraphicsRectItem();
    fake->setRect( 0, 0, 10, 10 );
    fake->setPos( 0, 0 );
    fake->setBrush( Qt::green );
    fake->setFlag( QGraphicsItem::ItemIsMovable, true );
    m_scene->addItem( fake );
    m_scene->setFakeItem( fake );
}

WQtNetworkEditor::~WQtNetworkEditor()
{
}

void WQtNetworkEditor::selectItem()
{
    boost::shared_ptr< WModule > module;

    //WQtCombinerToolbar* newToolbar = NULL;

    if ( m_scene->selectedItems().size() != 0 &&
         m_scene->selectedItems().at( 0 )->type() == WQtNetworkItem::Type )
    {
        if ( m_scene->selectedItems().at(0)->type() == WQtNetworkItem::Type )
        {
            module = ( static_cast< WQtNetworkItem* >( m_scene->selectedItems().at( 0 ) ) )->getModule();
        }

        // crashed modules should not provide any props
        if ( module->isCrashed()() )
        {
             return;
        }

        m_mainWindow->getControlPanel()->setNewActiveModule( module );
    }
    else
    {
        m_mainWindow->getControlPanel()->setNewActiveModule( module );
    }

    // set the new toolbar
    // NOTE: setCompatiblesToolbar removes the toolbar if NULL is specified.
    //m_mainWindow->setCompatiblesToolbar( newToolbar );
}

void WQtNetworkEditor::addModule( boost::shared_ptr< WModule > module )
{
    WQtNetworkItem *netItem = new WQtNetworkItem( this, module );
    m_items.push_back( netItem );

    // set the object at a random start position
    time( &m_time );
    netItem->setPos( rand_r( ( unsigned int * ) &m_time ) % 200,
                     rand_r( ( unsigned int * ) &m_time ) % 200 );

    m_scene->addItem( netItem );

    itemMoved();
}

bool WQtNetworkEditor::event( QEvent* event )
{
    // a module got associated with the root container -> add it to the list
    if ( event->type() == WQT_ASSOC_EVENT )
    {
        // convert event to assoc event
        WModuleAssocEvent* e1 = dynamic_cast< WModuleAssocEvent* >( event );     // NOLINT
        if ( e1 )
        {
            WLogger::getLogger()->addLogMessage( "Inserting module \"" + e1->getModule()->getName() +
                                                "\" to network editor.", "NetworkEditor", LL_DEBUG );
            addModule( e1->getModule() );
        }

        //TODO(skiunke): disablen des moduls solange nicht rdy!
        return true;
    }

    // a module changed its state to "ready" -> activate it in dataset browser
    if ( event->type() == WQT_READY_EVENT )
    {
        // convert event to ready event
        WModuleReadyEvent* e = dynamic_cast< WModuleReadyEvent* >( event );     // NOLINT
        if ( !e )
        {
            // this should never happen, since the type is set to WQT_READY_EVENT.
            WLogger::getLogger()->addLogMessage( "Event is not an WModueReadyEvent although its type claims it. Ignoring event.",
                                                 "NetworkEditor", LL_WARNING );

            return true;
        }

        WLogger::getLogger()->addLogMessage( "Activating module \"" + e->getModule()->getName() + "\" in network editor.",
                                             "NetworkEditor", LL_DEBUG );

        // search all the item matching the module
        WQtNetworkItem *item = findItemByModule( e->getModule() );
        if( item != 0 )
        {
            item->activate( true );
        }

        return true;
    }

    // a module tree item was connected to another one
    if ( event->type() == WQT_MODULE_CONNECT_EVENT )
    {
        WModuleConnectEvent* e = dynamic_cast< WModuleConnectEvent* >( event );     // NOLINT
        if ( !e )
        {
            // this should never happen, since the type is set to WQT_MODULE_CONNECT_EVENT.
            WLogger::getLogger()->addLogMessage( "Event is not an WModuleConnectEvent although its type claims it. Ignoring event.",
                                                 "NetworkEditor", LL_WARNING );
            return true;
        }

        WLogger::getLogger()->addLogMessage( "Connecting \"" + e->getInput()->getModule()->getName() + "\" and \"" +
                                                             e->getOutput()->getModule()->getName() + "\".", "NetworkEditor", LL_DEBUG );

        boost::shared_ptr< WModule > mIn;
        boost::shared_ptr< WModule > mOut;

        if( e->getInput()->isInputConnector() == true &&
            e->getOutput()->isOutputConnector() == true )
        {
            mIn = e->getInput()->getModule();
            mOut = e->getOutput()->getModule();
        }
        else if ( e->getInput()->isOutputConnector() == true &&
                    e->getOutput()->isInputConnector() == true )
        {
            mIn = e->getOutput()->getModule();
            mOut = e->getInput()->getModule();
        }
        else
        {
            return true;
            //TODO(skiunke): warning
        }

        WQtNetworkItem *inItem = findItemByModule( mIn );
        WQtNetworkItem *outItem = findItemByModule( mOut );

        WQtNetworkInputPort *ip = NULL;
        WQtNetworkOutputPort *op = NULL;

        for ( QList< WQtNetworkInputPort* >::const_iterator iter = inItem->getInPorts().begin();
                iter != inItem->getInPorts().end();
                ++iter )
        {
            WQtNetworkInputPort *inP = dynamic_cast< WQtNetworkInputPort* >( *iter );
            if( e->getInput() == inP->getConnector() )
            {
                ip = inP;
            }
        }

        for ( QList< WQtNetworkOutputPort* >::const_iterator iter = outItem->getOutPorts().begin();
                iter != outItem->getOutPorts().end();
                ++iter )
        {
            WQtNetworkOutputPort *outP = dynamic_cast< WQtNetworkOutputPort* >( *iter );
            if( e->getOutput() == outP->getConnector() )
            {
                op = outP;
            }
        }

        if( ip != NULL &&
            op != NULL )
        {
            WQtNetworkArrow *arrow = new WQtNetworkArrow( op, ip );

            arrow->setZValue( -1000.0 );
            op->addArrow( arrow );
            ip->addArrow( arrow );
            arrow->updatePosition();

            m_scene->addItem( arrow );
        }
    }


    // a module tree item was disconnected from another one
    if ( event->type() == WQT_MODULE_DISCONNECT_EVENT )
    {
        WLogger::getLogger()->addLogMessage( "DISCONNECT.", "NetworkEditor", LL_ERROR );

        WModuleDisconnectEvent* e = dynamic_cast< WModuleDisconnectEvent* >( event );     // NOLINT
        if ( !e )
        {
            // this should never happen, since the type is set to WQT_MODULE_DISCONNECT_EVENT.
            WLogger::getLogger()->addLogMessage( "Event is not an WModuleDisconnectEvent although its type claims it. Ignoring event.",
                                                 "NetworkEditor", LL_WARNING );
            return true;
        }

        WLogger::getLogger()->addLogMessage( "Disonnecting \"" + e->getInput()->getCanonicalName() +
                                             "\" and \"" + e->getOutput()->getCanonicalName() +
                                             "\"." , "NetworkEditor", LL_DEBUG );

        boost::shared_ptr< WModule > mIn;
        boost::shared_ptr< WModule > mOut;

        if( e->getInput()->isInputConnector() == true &&
            e->getOutput()->isOutputConnector() == true )
        {
            mIn = e->getInput()->getModule();
            mOut = e->getOutput()->getModule();
        }
        else if( e->getInput()->isOutputConnector() == true &&
                    e->getOutput()->isInputConnector() == true )
        {
            mIn = e->getOutput()->getModule();
            mOut = e->getInput()->getModule();
        }
        else
        {
            return true;
            //TODO(skiunke): warning
        }


        WQtNetworkItem *inItem = findItemByModule( mIn );
        WQtNetworkItem *outItem = findItemByModule( mOut );

        WQtNetworkInputPort *ip = NULL;
        WQtNetworkOutputPort *op = NULL;

        WLogger::getLogger()->addLogMessage( "1.", "NetworkEditor", LL_ERROR );

        for ( QList< WQtNetworkInputPort* >::const_iterator iter = inItem->getInPorts().begin();
            iter != inItem->getInPorts().end();
            ++iter )
        {
           WQtNetworkInputPort *inP = dynamic_cast< WQtNetworkInputPort* >( *iter );
           if( e->getInput() == inP->getConnector() )
           {
               ip = inP;
           }
        }
            WLogger::getLogger()->addLogMessage( "2", "NetworkEditor", LL_ERROR );

            for ( QList< WQtNetworkOutputPort* >::const_iterator iter = outItem->getOutPorts().begin();
                    iter != outItem->getOutPorts().end();
                    ++iter )
            {
                WQtNetworkOutputPort *outP = dynamic_cast< WQtNetworkOutputPort* >( *iter );
                if( e->getOutput() == outP->getConnector() )
                {
                    op = outP;
                }
            }

        WLogger::getLogger()->addLogMessage( "3", "NetworkEditor", LL_ERROR );

        WQtNetworkArrow *ar = NULL;

        wlog::error( "networkeditor" ) <<  "gehe durch " << m_scene->items().size() << " items";

        for ( QList< QGraphicsItem * >::const_iterator iter = m_scene->items().begin();
                iter != m_scene->items().end();
                ++iter )
        {
            wlog::error( "netw" ) <<  *iter;
            ar = dynamic_cast< WQtNetworkArrow* >( *iter );
            if( ar &&
                ar->getStartPort() == op &&
                ar->getEndPort() == ip )
            {
                break;
            }
        }
        if ( ar )
        {
            op->removeArrow( ar );
            ip->removeArrow( ar );
            m_scene->removeItem( ar );
        }
        else
        {
            WLogger::getLogger()->addLogMessage( "Arrow not found!.", "NetworkEditor", LL_ERROR );
        }

        return true;
    }

    // a module was removed from the container
    if ( event->type() == WQT_MODULE_REMOVE_EVENT )
    {
        WLogger::getLogger()->addLogMessage( "REMOVE.", "NetworkEditor", LL_ERROR );

        WModuleRemovedEvent* e = dynamic_cast< WModuleRemovedEvent* >( event );
        if ( !e )
        {
            // this should never happen, since the type is set to WQT_MODULE_REMOVE_EVENT.
            WLogger::getLogger()->addLogMessage( "Event is not an WModuleRemovedEvent although its type claims it. Ignoring event.",
                                                 "NetworkEditor", LL_WARNING );
            return true;
        }

        WLogger::getLogger()->addLogMessage( "Removing \"" + e->getModule()->getName() +
                                             "\"from network editor.", "NetworkEditor", LL_DEBUG );


        WQtNetworkItem *item = findItemByModule( e->getModule() );
        if( item != 0 )
        {
            item->activate( false );
            //e->getModule()->requestStop(); // TODO(rfrohl): do we need this ?
        }

        return true;
    }

    // a module tree item should be deleted
    if ( event->type() == WQT_MODULE_DELETE_EVENT )
    {
        WLogger::getLogger()->addLogMessage( "DELETE.", "NetworkEditor", LL_ERROR );

        WModuleDeleteEvent* e = dynamic_cast< WModuleDeleteEvent* >( event );
        if ( !e )
        {
            // this should never happen, since the type is set to WQT_MODULE_REMOVE_EVENT.
            WLogger::getLogger()->addLogMessage( "Event is not an WModuleRemovedEvent although"
                                                 "its type claims it. Ignoring event.",
                                                 "NetworkEditor", LL_WARNING );
            return true;
        }

        WLogger::getLogger()->addLogMessage( "Delete \"" + e->getTreeItem()->getModule()->getName() +
                                             "\" from network editor", "NetworkEditor", LL_DEBUG );

        WQtNetworkItem *item = findItemByModule( e->getTreeItem()->getModule() );

        if( item != 0 )
        {
            m_scene->removeItem( item );
            m_items.removeAll( item );
            delete item;
        }

        return true;
    }

    return QDockWidget::event( event );
}

WQtNetworkItem* WQtNetworkEditor::findItemByModule( boost::shared_ptr< WModule > module )
{
    for( QList< WQtNetworkItem* >::const_iterator iter = m_items.begin(); iter != m_items.end(); ++iter )
    {
       WQtNetworkItem *itemModule = dynamic_cast< WQtNetworkItem* >( *iter );
       if( itemModule &&
           itemModule->getModule() == module )
       {
           return itemModule;
       }
    }
    return 0;
}

void WQtNetworkEditor::itemMoved()
{
    if ( !timerId )
        timerId = startTimer( 1000 / 25 );
}

void WQtNetworkEditor::timerEvent( QTimerEvent *event )
{
    Q_UNUSED( event );

    QList< WQtNetworkItem *> items;
    foreach( QGraphicsItem *item, m_scene->items() )
    {
        if ( WQtNetworkItem *netItem = dynamic_cast< WQtNetworkItem  *>( item ) )
            items << netItem;
    }

    foreach( WQtNetworkItem *netItem, items )
        netItem->calculateForces();

    bool itemsMoved = false;
    foreach( WQtNetworkItem *netItem, items )
    {
        if ( netItem->advance() )
            itemsMoved = true;
    }

    if ( !itemsMoved )
    {
        killTimer( timerId );
        timerId = 0;
    }
}
