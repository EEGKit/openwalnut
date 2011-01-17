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

#include <QtGui/QApplication>

#include "../../../kernel/WKernel.h"

#include "../../../common/WProgressCombiner.h"
#include "../../../common/WProgress.h"

#include "../../../kernel/WModuleInputConnector.h"
#include "../../../kernel/WModuleOutputConnector.h"

#include "../events/WModuleDeleteEvent.h"
#include "../events/WEventTypes.h"
#include "../events/WPropertyChangedEvent.h"

#include "../WQt4Gui.h"
#include "../WMainWindow.h"

#include "WTreeItemTypes.h"
#include "WQtTreeItem.h"

WQtTreeItem::WQtTreeItem( QTreeWidgetItem * parent, WTreeItemType type, boost::shared_ptr< WModule > module ) :
    QTreeWidgetItem( parent, type ),
    m_deleteInProgress( false ),
    m_needPostDeleteEvent( true ),
    m_handledInput( "" )
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

    // grab the runtime name property
    // replace the name by the filename
    boost::shared_ptr< WPropertyBase > p = module->getProperties()->findProperty( "Name" );

    // always ensure that findProperty really found something
    if ( p )
    {
        m_nameProp = p->toPropString();
    }

    // was it a string prop?
    if ( m_nameProp )
    {
        m_name = m_nameProp->get( true );
        m_nameProp->getUpdateCondition()->subscribeSignal( boost::bind( &WQtTreeItem::nameChanged, this ) );
    }

    m_updateTimer = boost::shared_ptr< QTimer >( new QTimer() );
    connect( m_updateTimer.get(), SIGNAL( timeout() ), this, SLOT( update() ) );
    m_updateTimer->start( 500 );
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
    WModule::InputConnectorList consIn = m_module->getInputConnectors();
    WModule::OutputConnectorList consOut = m_module->getOutputConnectors();
    conList += "<table><tr><th>Name</th><th>Description</th><th>Type (I/O)</th><th>Connected</th></tr>";
    int conCount = 0;
    for ( WModule::InputConnectorList::const_iterator it = consIn.begin(); it != consIn.end(); ++it )
    {
        ++conCount;
        conList += "<tr><td><b>" + ( *it )->getName() + "&nbsp;</b></td><td>" + ( *it )->getDescription() + "&nbsp;</td>";
        conList += "<td><center>In</center></td>";
        conList += ( *it )->isConnected() ? "<td><center>Yes</center></td>" : "<td><center>No</center></td>";
        conList += "</tr>";
    }
    for ( WModule::OutputConnectorList::const_iterator it = consOut.begin(); it != consOut.end(); ++it )
    {
        ++conCount;
        conList += "<tr><td><b>" + ( *it )->getName() + "&nbsp;</b></td><td>" + ( *it )->getDescription() + "&nbsp;</td>";
        conList += "<td><center>Out</center></td>";
        conList += ( *it )->isConnected() ? "<td></center>Yes</center></td>" : "<td><center>No</center></td>";
        conList += "</tr>";
    }
    conList += "</table>";

    tooltip += conCount ? "Yes" + conList + "<br/><br/>" : "None<br/>";
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

    std::string connInfo = "";
    if ( ( m_handledOutput != "" ) && ( m_handledInput != "" ) )
    {
        connInfo = " (" + m_handledOutput + "->" + m_handledInput + ") ";
    }

    // is it pending?
    std::string progress = "Waiting";
    if ( m_module->isCrashed()() )
    {
        progress = "Problem occurred";
        setText( 0, ( m_name + " (problem occurred)"  + connInfo ).c_str() );

        // strike out the name of the module to show the crash visually.
        QFont curFont = font( 0 );
        curFont.setStrikeOut( true );
        setFont( 0, curFont );
        setIcon( 0, WQt4Gui::getMainWindow()->getIconManager()->getIcon( "moduleCrashed" ) );

        // this ensures that crashed modules can be deleted
        setDisabled( false );
    }
    else if ( p->isPending() )
    {
        progress = "Busy " + p->getCombinedNames();
        setIcon( 0, WQt4Gui::getMainWindow()->getIconManager()->getIcon( "moduleBusy" ) );
        std::ostringstream title;

        // construct a name for the progress indicator
        std::string name = p->getName();
        if ( !name.empty() )
        {
            name = " [" + name + "]";
        }

        if ( p->isDetermined() )
        {
            title.setf( std::ios::fixed );
            title.precision( 0 );
            title << p->getProgress() << "%" << name;
        }
        else
        {
            title << "Pending" << name;
        }

        setText( 0, ( m_name + " - " + title.str() + connInfo ).c_str() );
    }
    else
    {
        setIcon( 0, QIcon() );
        setText( 0, ( m_name + connInfo ).c_str() );
    }

    // if the user requested it to be deleted: disable and color it
    if ( m_deleteInProgress )
    {
        setForeground( 0, QBrush( QColor::fromRgb( 255, 0, 0 ) ) );
        setDisabled( true );
    }

    // is finished?
    if ( m_deleteInProgress && !m_module->isRunning().get() && m_needPostDeleteEvent )
    {
        m_needPostDeleteEvent = false;  // this ensures the event is only posted once
        QCoreApplication::postEvent( WQt4Gui::getMainWindow()->getControlPanel(), new WModuleDeleteEvent( this ) );
    }

    // active ?
    if ( m_module->getProperties()->getProperty( "active" )->toPropBool()->get() )
    {
        setCheckState( 0, Qt::Checked );
    }
    else
    {
        setCheckState( 0, Qt::Unchecked );
    }

    // update tooltip
    updateTooltip( progress );
}

void WQtTreeItem::gotRemoved()
{
    // this ensures a visual feedback to the user while waiting for the module to finish.

    // update tree item state
    m_deleteInProgress = true;
}

void WQtTreeItem::nameChanged()
{
    // luckily, the update mechanism of WQtTreeItem regularly sets the name using m_name. So we do not even need to post some kind of event.
    m_name = m_nameProp->get( true );
}

std::string WQtTreeItem::getHandledInput() const
{
    return m_handledInput;
}

void WQtTreeItem::setHandledInput( std::string in )
{
    m_handledInput = in;
}

std::string WQtTreeItem::getHandledOutput() const
{
    return m_handledOutput;
}

void WQtTreeItem::setHandledOutput( std::string out )
{
    m_handledOutput = out;
}

void WQtTreeItem::handleCheckStateChange()
{
    // active ?
    if ( checkState( 0 ) == Qt::Checked )
    {
        m_module->getProperties()->getProperty( "active" )->toPropBool()->set( true );
    }
    else
    {
        m_module->getProperties()->getProperty( "active" )->toPropBool()->set( false );
    }
}

