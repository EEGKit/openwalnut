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

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// Use filesystem version 2 for compatibility with newer boost versions.
#ifndef BOOST_FILESYSTEM_VERSION
    #define BOOST_FILESYSTEM_VERSION 2
#endif
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>

#include <QtGui/QApplication>
#include <QtGui/QFileDialog>
#include <QtCore/QSettings>

#include "WMainWindow.h" // this has to be included before any other includes
#include "core/common/WConditionOneShot.h"
#include "core/common/WIOTools.h"
#include "core/common/WPathHelper.h"
#include "core/dataHandler/WDataHandler.h"
#include "core/dataHandler/WSubject.h"
#include "core/graphicsEngine/WGraphicsEngine.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WModuleContainer.h"
#include "core/kernel/WProjectFile.h"
#include "core/kernel/WROIManager.h"
#include "controlPanel/WQtControlPanel.h"
#include "events/WModuleAssocEvent.h"
#include "events/WModuleConnectEvent.h"
#include "events/WModuleCrashEvent.h"
#include "events/WModuleDisconnectEvent.h"
#include "events/WModuleReadyEvent.h"
#include "events/WModuleRemovedEvent.h"
#include "events/WOpenCustomDockWidgetEvent.h"
#include "events/WRoiAssocEvent.h"
#include "events/WRoiRemoveEvent.h"
#include "events/WUpdateTextureSorterEvent.h"
#include "WQt4Gui.h"

WMainWindow* WQt4Gui::m_mainWindow = NULL;

QSettings* WQt4Gui::m_settings = NULL;

WQt4Gui::WQt4Gui( const boost::program_options::variables_map& options, int argc, char** argv )
    : WGUI( argc, argv ),
    m_optionsMap( options )
{
    m_settings = new QSettings( "OpenWalnut.org", "OpenWalnut" );
}

WQt4Gui::~WQt4Gui()
{
    delete m_settings;
}

void WQt4Gui::moduleError( boost::shared_ptr< WModule > module, const WException& exception )
{
    QCoreApplication::postEvent( m_mainWindow, new WModuleCrashEvent( module, exception.what() ) );
}

WMainWindow* WQt4Gui::getMainWindow()
{
    return m_mainWindow;
}

int WQt4Gui::run()
{
    // init logger
    m_loggerConnection = WLogger::getLogger()->subscribeSignal( WLogger::AddLog, boost::bind( &WQt4Gui::slotAddLog, this, _1 ) );

    // make qapp instance before using the applicationDirPath() function
    QApplication appl( m_argc, m_argv, true );

    // the call path of the application, this uses QApplication which needs to be instantiated.
    boost::filesystem::path walnutBin = boost::filesystem::path( QApplication::applicationDirPath().toStdString() );

    // setup path helper which provides several paths to others^
    WPathHelper::getPathHelper()->setAppPath( walnutBin );

    // get the minimum log level from preferences
    LogLevel logLevel = static_cast< LogLevel >( WQt4Gui::getSettings().value( "qt4gui/logLevel", LL_INFO ).toInt() );
    WLogger::getLogger()->setDefaultLogLevel( logLevel );

    // print the first output
    wlog::debug( "OpenWalnut" ) << "OpenWalnut binary path: " << walnutBin;
    wlog::info( "GUI" ) << "Bringing up GUI";

    // startup graphics engine
    m_ge = WGraphicsEngine::getGraphicsEngine();

    // and startup kernel
    m_kernel = boost::shared_ptr< WKernel >( WKernel::instance( m_ge, shared_from_this() ) );
    m_kernel->run();
    t_ModuleErrorSignalHandlerType func = boost::bind( &WQt4Gui::moduleError, this, _1, _2 );
    m_kernel->getRootContainer()->addDefaultNotifier( WM_ERROR, func );

    // create the window
    m_mainWindow = new WMainWindow();
    m_mainWindow->setupGUI();
    m_mainWindow->show();

    // connect out loader signal with kernel
#ifdef _WIN32
    getLoadButtonSignal()->connect( boost::bind( &WKernel::loadDataSetsSynchronously, m_kernel, _1 ) );
#else
    getLoadButtonSignal()->connect( boost::bind( &WKernel::loadDataSets, m_kernel, _1 ) );
#endif

    WCondition::t_ConditionNotifierType newDatasetSignal = boost::bind( &WQt4Gui::slotUpdateTextureSorter, this );
    WDataHandler::getDefaultSubject()->getListChangeCondition()->subscribeSignal( newDatasetSignal );

    // bind the GUI's slot with the ready signal

    // Assoc Event
    t_ModuleGenericSignalHandlerType assocSignal = boost::bind( &WQt4Gui::slotAddDatasetOrModuleToTree, this, _1 );
    m_kernel->getRootContainer()->addDefaultNotifier( WM_ASSOCIATED, assocSignal );

    // Ready Event
    t_ModuleGenericSignalHandlerType readySignal = boost::bind( &WQt4Gui::slotActivateDatasetOrModuleInTree, this, _1 );
    m_kernel->getRootContainer()->addDefaultNotifier( WM_READY, readySignal );

    // Remove Event
    t_ModuleGenericSignalHandlerType removedSignal = boost::bind( &WQt4Gui::slotRemoveDatasetOrModuleInTree, this, _1 );
    m_kernel->getRootContainer()->addDefaultNotifier( WM_REMOVED, removedSignal );

    // Connect Event
    t_GenericSignalHandlerType connectionEstablishedSignal = boost::bind( &WQt4Gui::slotConnectionEstablished, this, _1, _2 );
    m_kernel->getRootContainer()->addDefaultNotifier( CONNECTION_ESTABLISHED, connectionEstablishedSignal );

    // Disconnect Event
    t_GenericSignalHandlerType connectionClosedSignal = boost::bind( &WQt4Gui::slotConnectionClosed, this, _1, _2 );
    m_kernel->getRootContainer()->addDefaultNotifier( CONNECTION_CLOSED, connectionClosedSignal );

    boost::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > assocRoiSignal;
    assocRoiSignal =
        boost::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > >(
            new boost::function< void( osg::ref_ptr< WROI > ) > ( boost::bind( &WQt4Gui::slotAddRoiToTree, this, _1 ) ) );
    m_kernel->getRoiManager()->addAddNotifier( assocRoiSignal );


    boost::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > removeRoiSignal;
    removeRoiSignal =
        boost::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > >(
            new boost::function< void( osg::ref_ptr< WROI > ) > ( boost::bind( &WQt4Gui::slotRemoveRoiFromTree, this, _1 ) ) );
    m_kernel->getRoiManager()->addRemoveNotifier( removeRoiSignal );

    // now we are initialized
    m_isInitialized( true );

    // check if we want to load data due to command line and call the respective function
    if( m_optionsMap.count("input") )
    {
        m_kernel->loadDataSets( m_optionsMap["input"].as< std::vector< std::string > >() );
    }

    // Load project file
    if( m_optionsMap.count("project") )
    {
        try
        {
            boost::shared_ptr< WProjectFile > proj = boost::shared_ptr< WProjectFile >(
                    new WProjectFile( m_optionsMap["project"].as< std::string >() )
            );

            // This call is asynchronous. It parses the file and the starts a thread to actually do all the stuff
            proj->load();
        }
        catch( const WException& e )
        {
            wlog::error( "GUI" ) << "Project file \"" << m_optionsMap["project"].as< std::string >() << "\" could not be loaded. Message: " <<
                e.what();
        }
    }

    // run
    int qtRetCode = appl.exec();

    delete m_mainWindow;

    // signal everybody to shut down properly.
    WKernel::getRunningKernel()->wait( true );
    WKernel::getRunningKernel()->getGraphicsEngine()->wait( true );

    m_loggerConnection.disconnect();

    return qtRetCode;
}

void WQt4Gui::slotUpdateTextureSorter()
{
    // create a new event for this and insert it into event queue
    QCoreApplication::postEvent( m_mainWindow->getControlPanel(), new WUpdateTextureSorterEvent() );
}

void WQt4Gui::slotAddLog( const WLogEntry& /*entry*/ )
{
    // TODO(rfrohl): create a new event for this and insert it into event queue
}

void WQt4Gui::slotAddDatasetOrModuleToTree( boost::shared_ptr< WModule > module )
{
    // create a new event for this and insert it into event queue
    QCoreApplication::postEvent( m_mainWindow->getControlPanel(), new WModuleAssocEvent( module ) );
    QCoreApplication::postEvent( m_mainWindow->getNetworkEditor(), new WModuleAssocEvent( module ) );
}

void WQt4Gui::slotAddRoiToTree( osg::ref_ptr< WROI > roi )
{
    QCoreApplication::postEvent( m_mainWindow->getControlPanel(), new WRoiAssocEvent( roi ) );
}

void WQt4Gui::slotRemoveRoiFromTree( osg::ref_ptr< WROI > roi )
{
    QCoreApplication::postEvent( m_mainWindow->getControlPanel(), new WRoiRemoveEvent( roi ) );
}

void WQt4Gui::slotActivateDatasetOrModuleInTree( boost::shared_ptr< WModule > module )
{
    // create a new event for this and insert it into event queue
    QCoreApplication::postEvent( m_mainWindow->getControlPanel(), new WModuleReadyEvent( module ) );
    QCoreApplication::postEvent( m_mainWindow, new WModuleReadyEvent( module ) );
    QCoreApplication::postEvent( m_mainWindow->getNetworkEditor(), new WModuleReadyEvent( module ) );
}

void WQt4Gui::slotRemoveDatasetOrModuleInTree( boost::shared_ptr< WModule > module )
{
    // create a new event for this and insert it into event queue
    QCoreApplication::postEvent( m_mainWindow->getNetworkEditor(), new WModuleRemovedEvent( module ) );
    QCoreApplication::postEvent( m_mainWindow->getControlPanel(), new WModuleRemovedEvent( module ) );
    QCoreApplication::postEvent( m_mainWindow, new WModuleRemovedEvent( module ) );
}

void WQt4Gui::slotConnectionEstablished( boost::shared_ptr<WModuleConnector> in, boost::shared_ptr<WModuleConnector> out )
{
    // create a new event for this and insert it into event queue
    if( in->isInputConnector() )
    {
        QCoreApplication::postEvent( m_mainWindow->getControlPanel(), new WModuleConnectEvent( in, out ) );
        QCoreApplication::postEvent( m_mainWindow->getNetworkEditor(), new WModuleConnectEvent( in, out ) );
    }
    else
    {
        QCoreApplication::postEvent( m_mainWindow->getControlPanel(), new WModuleConnectEvent( out, in ) );
        QCoreApplication::postEvent( m_mainWindow->getNetworkEditor(), new WModuleConnectEvent( out, in ) );
    }
}

void WQt4Gui::slotConnectionClosed( boost::shared_ptr<WModuleConnector> in, boost::shared_ptr<WModuleConnector> out )
{
    // create a new event for this and insert it into event queue
    if( in->isInputConnector() )
    {
        QCoreApplication::postEvent( m_mainWindow->getNetworkEditor(), new WModuleDisconnectEvent( in, out ) );
        QCoreApplication::postEvent( m_mainWindow->getControlPanel(), new WModuleDisconnectEvent( in, out ) );
    }
    else
    {
        QCoreApplication::postEvent( m_mainWindow->getNetworkEditor(), new WModuleDisconnectEvent( out, in ) );
        QCoreApplication::postEvent( m_mainWindow->getControlPanel(), new WModuleDisconnectEvent( out, in ) );
    }
}

boost::shared_ptr< WModule > WQt4Gui::getSelectedModule()
{
    return m_mainWindow->getControlPanel()->getSelectedModule();
}

boost::signals2::signal1< void, std::vector< std::string > >* WQt4Gui::getLoadButtonSignal()
{
    return m_mainWindow->getLoaderSignal();
}

boost::shared_ptr< WCustomWidget > WQt4Gui::openCustomWidget( std::string title, WGECamera::ProjectionMode projectionMode,
    boost::shared_ptr< WCondition > shutdownCondition )
{
    WConditionSet conditionSet;
    conditionSet.setResetable( true, false );
    conditionSet.add( shutdownCondition );

    boost::shared_ptr< WFlag< boost::shared_ptr< WCustomWidget > > > widgetFlag(
        new WFlag< boost::shared_ptr< WCustomWidget > >( new WConditionOneShot, boost::shared_ptr< WCustomWidget >() ) );
    conditionSet.add( widgetFlag->getCondition() );

    QCoreApplication::postEvent( m_mainWindow, new WOpenCustomDockWidgetEvent( title, projectionMode, widgetFlag ) );

    conditionSet.wait();
    return widgetFlag->get();
}

void WQt4Gui::closeCustomWidget( std::string title )
{
    m_mainWindow->closeCustomDockWidget( title );
}

QSettings& WQt4Gui::getSettings()
{
    return *m_settings;
}

