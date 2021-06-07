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

#include "WMLineDrawer.h"

W_LOADABLE_MODULE( WMLineDrawer )

WMLineDrawer::WMLineDrawer()
{
}

WMLineDrawer::~WMLineDrawer()
{
}

const std::string WMLineDrawer::getName() const
{
    return "Line Drawer";
}

const std::string WMLineDrawer::getDescription() const
{
    return "Allows user to draw lines.";
}

boost::shared_ptr< WModule > WMLineDrawer::factory() const
{
    return boost::shared_ptr< WModule >( new WMLineDrawer() );
}

const char** WMLineDrawer::getXPMIcon() const
{
    return NULL;
}

void WMLineDrawer::moduleMain()
{
    WSelectionManager manager;
    manager.setSelectionType( WSelectionManager::WSelectionType::BRUSH );

    m_moduleState.add( m_propCondition );

    ready();

    debugLog() << "READY...";

    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();
        if( m_shutdownFlag() )
        {
            break;
        }

        manager.setSelectionType( static_cast< WSelectionManager::WSelectionType >( m_selection->get().getItemIndexOfSelected( 0 ) ) );
    }
}

void WMLineDrawer::connectors()
{
    WModule::connectors();
}

void WMLineDrawer::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    WItemSelection::SPtr selections = WItemSelection::SPtr( new WItemSelection() );
    selections->addItem( ItemType::create( "Brush", "Brush", "", NULL ) );
    selections->addItem( ItemType::create( "Line Loop", "Line Loop", "", NULL ) );
    selections->addItem( ItemType::create( "Box", "Box", "", NULL ) );

    m_selection = m_properties->addProperty( "Selection Type", "The type of the selection", selections->getSelectorFirst(), m_propCondition );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_selection );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_selection );

    WModule::properties();
}
