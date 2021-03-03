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

#include "WConnectorData.h"

#include "action/WFiberActionAddVertex.h"
#include "action/WFiberActionRemoveVertex.h"
#include "WFiberHandler.h"

WFiberHandler::WFiberHandler( WMPointConnector* pointConnector )
{
    m_pointConnector = pointConnector;

    m_actionHandler = WActionHandler::SPtr( new WActionHandler() );
    m_fibers = PCFiberListSPtr( new PCFiberList() );

    m_fibers->push_back( PCFiber() );

    m_selectedFiber = 0;
    m_fiberCount = 1;
}

void WFiberHandler::addVertexToFiber( osg::Vec3 vertex, bool silent )
{
    m_fibers->at( m_selectedFiber ).push_back( vertex );

    if( !silent )
    {
        m_actionHandler->pushAction( WFiberActionAddVertex::SPtr( new WFiberActionAddVertex( vertex, this ) ) );
    }
}

void WFiberHandler::addVertexToFiberAt( osg::Vec3 vertex, size_t position, bool silent )
{
    auto fiber = m_fibers->begin() + m_selectedFiber;
    fiber->emplace( fiber->begin() + position, vertex );

    if( !silent )
    {
        m_actionHandler->pushAction( WFiberActionAddVertex::SPtr( new WFiberActionAddVertex( vertex, this ) ) );
    }
}

void WFiberHandler::removeVertexFromFiber( osg::Vec3 vertex, bool silent )
{
    auto fiber = m_fibers->begin() + m_selectedFiber;
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
        m_actionHandler->pushAction( WFiberActionRemoveVertex::SPtr( new WFiberActionRemoveVertex( vertex, position, this ) ) );
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

    m_possibleFiberSelections = WItemSelection::SPtr( new WItemSelection() );
    m_possibleFiberSelections->addItem( ItemType::create( "Line 1", "Line 1", "", NULL ) );

    m_fiberSelection = properties->addProperty(
        "Selected Line", "The line to which the points are added", m_possibleFiberSelections->getSelectorFirst(), notifier );

    WPropertyHelper::PC_SELECTONLYONE::addTo( m_fiberSelection );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_fiberSelection );

    m_addFiber = properties->addProperty( "Add Line", "Add Line", WPVBaseTypes::PV_TRIGGER_READY, notifier );

    m_undoTrigger = properties->addProperty( "Undo", "Undo", WPVBaseTypes::PV_TRIGGER_READY, notifier );
    m_redoTrigger = properties->addProperty( "Redo", "Redo", WPVBaseTypes::PV_TRIGGER_READY, notifier );
}

void WFiberHandler::updateProperty( WPropertyBase::SPtr property )
{
    if( property == m_addFiber && m_addFiber->get(true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
    {
        m_addFiber->set( WPVBaseTypes::PV_TRIGGER_READY, false );

        m_fiberCount++;
        std::string name = "Line " + boost::lexical_cast< std::string >( m_fiberCount );

        m_fibers->push_back( PCFiber() );
        m_possibleFiberSelections->addItem( ItemType::create( name, name, "", NULL ) );
        m_fiberSelection->set( m_possibleFiberSelections->getSelectorLast() );
    }
    else if( property == m_fiberSelection )
    {
        m_selectedFiber = m_fiberSelection->get().getItemIndexOfSelected( 0 );
        m_pointConnector->getConnectorData()->deselectPoint();

        selectLastPoint();

        m_pointConnector->updatePoints();
    }
    else if( property == m_undoTrigger && m_undoTrigger->get(true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
    {
        m_undoTrigger->set( WPVBaseTypes::PV_TRIGGER_READY, false );
        m_actionHandler->undo();
    }
    else if( property == m_redoTrigger && m_redoTrigger->get(true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
    {
        m_redoTrigger->set( WPVBaseTypes::PV_TRIGGER_READY, false );
        m_actionHandler->redo();
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
