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
#include <vector>

#include "WConnectorData.h"

#include "action/WFiberActionAddVertex.h"
#include "action/WFiberActionAddFiber.h"
#include "action/WFiberActionRemoveVertex.h"
#include "action/WFiberActionRemoveFiber.h"
#include "action/WFiberActionToggle.h"

#include "core/common/WPathHelper.h"

#include "WFiberHandler.h"

WFiberHandler::WFiberHandler( WMPointConnector* pointConnector )
{
    m_pointConnector = pointConnector;
    m_actionHandler = WActionHandler::SPtr( new WActionHandler() );
    m_fibers = PCFiberListSPtr( new PCFiberList() );
    m_hidden = boost::shared_ptr< std::vector< char > >( new std::vector< char >() );

    m_selectedFiber = 0;
    m_fiberCount = 1;
}

void WFiberHandler::addVertexToFiber( osg::Vec3 vertex, size_t fiberIdx, bool silent )
{
    m_fibers->at( fiberIdx ).push_back( vertex );

    if( !silent )
    {
        m_actionHandler->pushAction( WFiberActionAddVertex::SPtr( new WFiberActionAddVertex( vertex, fiberIdx, this ) ) );
    }
}

void WFiberHandler::addVertexToFiberAt( osg::Vec3 vertex, size_t fiberIdx, size_t position, bool silent )
{
    auto fiber = m_fibers->begin() + fiberIdx;
    fiber->emplace( fiber->begin() + position, vertex );

    if( !silent )
    {
        m_actionHandler->pushAction( WFiberActionAddVertex::SPtr( new WFiberActionAddVertex( vertex, fiberIdx, this ) ) );
    }
}

void WFiberHandler::removeVertexFromFiber( osg::Vec3 vertex, size_t fiberIdx, bool silent )
{
    auto fiber = m_fibers->begin() + fiberIdx;
    size_t position = 0;

    for( auto it = fiber->begin(); it != fiber->end(); it++)
    {
        if( *it == vertex )
        {
            fiber->erase( it );
            break;
        }
        position++;
    }

    if( !silent )
    {
        m_actionHandler->pushAction( WFiberActionRemoveVertex::SPtr( new WFiberActionRemoveVertex( vertex, fiberIdx, position, this ) ) );
    }
}

void WFiberHandler::selectLastPoint()
{
    PCFiber fiber = m_fibers->at( m_selectedFiber );
    if( !fiber.empty() )
    {
        m_pointConnector->getConnectorData()->selectPoint( fiber.back() );
    }
}

void WFiberHandler::createProperties( WPropertyGroup::SPtr properties )
{
    WPropertyBase::PropertyChangeNotifierType notifier = boost::bind( &WFiberHandler::updateProperty, this, boost::placeholders::_1 );

    WPropertyGroup::SPtr fiberGroup = properties->addPropertyGroup( "Fibers", "Property group for fiber selection, adding and deleting." );
    WPropertyGroup::SPtr undoGroup = properties->addPropertyGroup( "Undo | Redo", "Property group for undo and redo." );

    m_possibleFiberSelections = WItemSelection::SPtr( new WItemSelection() );

    m_fiberSelection = fiberGroup->addProperty(
        "Selected Line", "The line to which the points are added", m_possibleFiberSelections->getSelectorNone(), notifier );

    addFiber( "Fiber 1", true );

    WPropertyHelper::PC_SELECTONLYONE::addTo( m_fiberSelection );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_fiberSelection );

    m_addFiber = fiberGroup->addProperty( "Add Fiber", "Add Fiber", WPVBaseTypes::PV_TRIGGER_READY, notifier );
    m_removeFiber = fiberGroup->addProperty( "Remove Fiber", "Remove Fiber", WPVBaseTypes::PV_TRIGGER_READY, notifier );
    m_toggleFiber = fiberGroup->addProperty( "Toggle Fiber", "Toggle Fiber", WPVBaseTypes::PV_TRIGGER_READY, notifier );

    m_undoTrigger = undoGroup->addProperty( "Undo", "Undo", WPVBaseTypes::PV_TRIGGER_READY, notifier );
    m_redoTrigger = undoGroup->addProperty( "Redo", "Redo", WPVBaseTypes::PV_TRIGGER_READY, notifier );
}

static bool sortComparator( boost::shared_ptr< WItemSelectionItem > a, boost::shared_ptr< WItemSelectionItem > b )
{
    return a->getName().compare( b->getName() ) < 0;
}

void WFiberHandler::addFiber( std::string name, bool silent )
{
    m_fibers->push_back( PCFiber() );
    m_hidden->push_back( false );

    m_possibleFiberSelections->addItem( ItemType::create( name, name, "", NULL ) );
    m_fiberSelection->set( m_possibleFiberSelections->getSelectorLast() );

    if( !silent )
    {
        m_actionHandler->pushAction( WFiberActionAddFiber::SPtr( new WFiberActionAddFiber( name, m_fibers->size() - 1, this ) ) );
    }
}

void WFiberHandler::addFiberAt( std::string name, size_t position, bool hidden, bool silent, PCFiber fiber )
{
    m_fibers->emplace( m_fibers->begin() + position, fiber );
    m_hidden->emplace( m_hidden->begin() + position, hidden );

    m_possibleFiberSelections->addItem( ItemType::create( name, name, "", NULL ) );
    m_possibleFiberSelections->stableSort( &sortComparator );

    m_fiberSelection->set( m_possibleFiberSelections->getSelectorLast() );

    if( !fiber.empty() )
    {
        m_pointConnector->updateOutput();
    }

    if( !silent )
    {
        m_actionHandler->pushAction( WFiberActionAddFiber::SPtr( new WFiberActionAddFiber( name, position, this ) ) );
    }
}

void WFiberHandler::removeFiber( size_t idx, bool silent )
{
    if( idx == 0 )
    {
        return;
    }

    std::string name = m_possibleFiberSelections->at( idx )->getName();
    PCFiber fiber = m_fibers->at( idx );

    bool hidden = m_hidden->at( idx );

    m_fibers->erase( m_fibers->begin() + idx );
    m_hidden->erase( m_hidden->begin() + idx );

    m_possibleFiberSelections->remove( m_possibleFiberSelections->at( idx ) );
    m_fiberSelection->set( m_possibleFiberSelections->getSelectorLast() );

    m_pointConnector->updateOutput();

    if( !silent )
    {
        m_actionHandler->pushAction( WFiberActionRemoveFiber::SPtr( new WFiberActionRemoveFiber( name, idx, fiber, hidden, this ) ) );
    }
}

void WFiberHandler::toggleFiber( size_t idx, bool silent )
{
    WItemSelectionItem::SPtr selection = m_possibleFiberSelections->at( idx );
    std::string name = selection->getName();
    if( isHidden( idx ) )
    {
        name = name.substr( 0, name.size() - 2 );
    }
    else
    {
        name = name + " *";
    }
    m_hidden->at( idx ) = !( m_hidden->at( idx ) );

    m_possibleFiberSelections->replace( selection, ItemType::create( name, name, "", NULL ) );
    m_fiberSelection->set( m_possibleFiberSelections->getSelector( idx ) );

    m_pointConnector->updatePoints();
    m_pointConnector->updateOutput();

    if( !silent )
    {
        m_actionHandler->pushAction( WFiberActionToggle::SPtr( new WFiberActionToggle( idx , this ) ) );
    }
}

bool WFiberHandler::isHidden( size_t idx )
{
    return m_hidden->at( idx );
}

void WFiberHandler::selectFiber( size_t idx )
{
    m_selectedFiber = idx;
    m_pointConnector->getConnectorData()->deselectPoint();

    selectLastPoint();

    m_pointConnector->updatePoints();
    m_pointConnector->updateOutput();
}

bool WFiberHandler::getFiberOfPoint( osg::Vec3 vertex, size_t* idx )
{
    for( size_t fiberIdx = 0; fiberIdx < m_fibers->size(); fiberIdx++ )
    {
        PCFiber fiber = m_fibers->at( fiberIdx );
        if( std::find( fiber.begin(), fiber.end(), vertex ) != fiber.end() )
        {
            if(idx != NULL)
            {
                *idx = fiberIdx;
            }
            return true;
        }
    }
    return false;
}

bool WFiberHandler::isPointHidden( osg::Vec3 vertex )
{
    size_t idx = 0;
    return getFiberOfPoint( vertex, &idx ) && isHidden( idx );
}

void WFiberHandler::updateProperty( WPropertyBase::SPtr property )
{
    if( property == m_addFiber && m_addFiber->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
    {
        m_addFiber->set( WPVBaseTypes::PV_TRIGGER_READY, false );

        m_fiberCount++;
        std::string name = "Fiber " + boost::lexical_cast< std::string >( m_fiberCount );
        addFiber( name );
    }
    else if( property == m_removeFiber && m_removeFiber->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
    {
        m_removeFiber->set( WPVBaseTypes::PV_TRIGGER_READY, false );
        removeFiber( m_fiberSelection->get().getItemIndexOfSelected( 0 ) );
    }
    else if( property == m_toggleFiber && m_toggleFiber->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
    {
        m_toggleFiber->set( WPVBaseTypes::PV_TRIGGER_READY, false );
        toggleFiber( m_fiberSelection->get().getItemIndexOfSelected( 0 ) );
    }
    else if( property == m_undoTrigger && m_undoTrigger->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
    {
        m_undoTrigger->set( WPVBaseTypes::PV_TRIGGER_READY, false );
        m_actionHandler->undo();
    }
    else if( property == m_redoTrigger && m_redoTrigger->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
    {
        m_redoTrigger->set( WPVBaseTypes::PV_TRIGGER_READY, false );
        m_actionHandler->redo();
    }
    else if( property == m_fiberSelection )
    {
        selectFiber( m_fiberSelection->get().getItemIndexOfSelected( 0 ) );
    }
}

WFiberHandler::PCFiberListSPtr WFiberHandler::getFibers()
{
    return m_fibers;
}


WMPointConnector* WFiberHandler::getPointConnector()
{
    return m_pointConnector;
}

size_t WFiberHandler::getSelectedFiber()
{
    return m_selectedFiber;
}

WActionHandler::SPtr WFiberHandler::getActionHandler()
{
    return m_actionHandler;
}
