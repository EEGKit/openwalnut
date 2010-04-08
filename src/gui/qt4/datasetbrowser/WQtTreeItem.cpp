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
#include <string>

#include "../../../common/WProgressCombiner.h"
#include "../../../common/WProgress.h"

#include "../../../kernel/WModuleInputConnector.h"
#include "../../../kernel/WModuleOutputConnector.h"

#include "WTreeItemTypes.h"
#include "WQtTreeItem.h"

WQtTreeItem::WQtTreeItem( QTreeWidgetItem * parent, WTreeItemType type, boost::shared_ptr< WModule > module ) :
    QTreeWidgetItem( parent, type )
{
    m_module = module;
    m_name = module->getName();

    if ( module->getProperties()->getProperty( "active" )->toPropBool()->get() )
    {
        this->setCheckState( 0, Qt::Checked );
    }
    else
    {
        this->setCheckState( 0, Qt::Unchecked );
    }

    this->setFlags( Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled );

    m_updateTimer = boost::shared_ptr< QTimer >( new QTimer() );
    connect( m_updateTimer.get(), SIGNAL( timeout() ), this, SLOT( update() ) );
    m_updateTimer->start( 50 );
}

WQtTreeItem::~WQtTreeItem()
{
}

boost::shared_ptr< WModule > WQtTreeItem::getModule()
{
    return m_module;
}

std::string WQtTreeItem::getName()
{
    return m_name;
}

void WQtTreeItem::updateTooltip( std::string progress )
{
    std::string tooltip = "";
    if ( m_module->isCrashed()() )
    {
        tooltip += "<b>A problem occured. The module has been stopped. </b><br/><br/>";
    }
    tooltip += "<b>Module: </b>" + m_module->getName() + "<br/>";
    tooltip += "<b>Progress: </b>" + progress + "<br/>";
    tooltip += "<b>Connectors: </b>";

    // also list the connectors
    std::string conList = "";
    std::set< boost::shared_ptr< WModuleInputConnector > > cons = m_module->getInputConnectors();
    std::set< boost::shared_ptr< WModuleOutputConnector > > consOut = m_module->getOutputConnectors();
    conList += "<table><tr><th>Name</th><th>Description</th><th>Connected</th></tr>";
    int conCount = 0;
    for ( std::set< boost::shared_ptr< WModuleInputConnector > >::const_iterator it = cons.begin(); it != cons.end(); ++it )
    {
        ++conCount;
        conList += "<tr><td><b>" + ( *it )->getName() + "&nbsp;</b></td><td>&nbsp;" + ( *it )->getDescription() + "&nbsp;</td>";
        conList += ( *it )->isConnected() ? "<td>&nbsp;yes&nbsp;</td>" : "<td>&nbsp;no&nbsp;</td>";
        conList += "</tr>";
    }
    for ( std::set< boost::shared_ptr< WModuleOutputConnector > >::const_iterator it = consOut.begin(); it != consOut.end(); ++it )
    {
        ++conCount;
        conList += "<tr><td><b>" + ( *it )->getName() + "&nbsp;</b></td><td>&nbsp;" + ( *it )->getDescription() + "&nbsp;</td>";
        conList += ( *it )->isConnected() ? "<td>&nbsp;yes&nbsp;</td>" : "<td>&nbsp;no&nbsp;</td>";
        conList += "</tr>";
    }
    conList += "</table>";

    tooltip += conCount ? "yes" + conList + "<br/><br/>" : "none<br/>";
    tooltip += "<b>Module Description: </b><br/>" + m_module->getDescription();

    setToolTip( 0, tooltip.c_str() );
}

void WQtTreeItem::update()
{
    updateState();
}

void WQtTreeItem::updateState()
{
    boost::shared_ptr< WProgressCombiner> p = m_module->getRootProgressCombiner();

    // update the progress combiners internal state
    p->update();

    // is it pending?
    std::string progress = "waiting";
    if ( m_module->isCrashed()() )
    {
        setText( 0, ( m_name + " (problem occurred)" ).c_str() );
    }
    else if ( p->isPending() )
    {
        std::ostringstream title;
        if ( p->isDetermined() )
        {
            title.setf( std::ios::fixed );
            title.precision( 0 );
            title << p->getProgress() << "%";
        }
        else
        {
            title << "Pending";
        }

        setText( 0, ( m_name + " - " + title.str() ).c_str() );
    }
    else
    {
        setText( 0, m_name.c_str() );
    }

    // update tooltip
    updateTooltip( progress );
}

