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

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <boost/thread.hpp>

#include <QtGui/QApplication>
#include <QtGui/QDockWidget>
#include <QtGui/QFileDialog>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QMessageBox>
#include <QtGui/QShortcut>
#include <QtGui/QSlider>
#include <QtGui/QVBoxLayout>

#include "../../common/WColor.h"
#include "../../common/WPreferences.h"
#include "../../dataHandler/WDataSetFibers.h"
#include "../../dataHandler/WDataSetSingle.h"
#include "../../dataHandler/WEEG2.h"
#include "../../graphicsEngine/WROIBox.h"
#include "../../graphicsEngine/WGEZoomTrackballManipulator.h"
#include "../../kernel/WKernel.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleCombiner.h"
#include "../../kernel/WModuleCombinerTypes.h"
#include "../../kernel/WProjectFile.h"
#include "../../kernel/modules/data/WMData.h"
#include "../../kernel/modules/navSlices/WMNavSlices.h"
#include "../icons/WIcons.h"
#include "controlPanel/WPropertyBoolWidget.h"
#include "events/WEventTypes.h"
#include "events/WModuleCrashEvent.h"
#include "events/WModuleReadyEvent.h"
#include "events/WModuleRemovedEvent.h"
#include "events/WOpenCustomDockWidgetEvent.h"
#include "guiElements/WQtPropertyBoolAction.h"
#include "WQtCustomDockWidget.h"
#include "WQtGLWidget.h"
#include "WQtNavGLWidget.h"

#include "WMainWindow.h"

WMainWindow::WMainWindow() :
    QMainWindow(),
    m_currentCompatiblesToolbar( NULL ),
    m_iconManager()
{
}

void WMainWindow::setupGUI()
{
    m_iconManager.addIcon( std::string( "load" ), fileopen_xpm );
    m_iconManager.addIcon( std::string( "loadProject" ), projOpen_xpm );
    m_iconManager.addIcon( std::string( "saveProject" ), projSave_xpm );
    m_iconManager.addIcon( std::string( "logo" ), logoIcon_xpm );
    m_iconManager.addIcon( std::string( "help" ), question_xpm );
    m_iconManager.addIcon( std::string( "quit" ), quit_xpm );
    m_iconManager.addIcon( std::string( "moduleBusy" ), moduleBusy_xpm );
    m_iconManager.addIcon( std::string( "moduleCrashed" ), moduleCrashed_xpm );
    m_iconManager.addIcon( std::string( "remove" ), remove_xpm );
    m_iconManager.addIcon( std::string( "config" ), preferences_system_xpm );


    if( objectName().isEmpty() )
    {
        setObjectName( QString::fromUtf8( "MainWindow" ) );
    }
    // TODO(all): what is this?
    resize( 946, 632 );
    setWindowIcon( m_iconManager.getIcon( "logo" ) );
    setWindowTitle( QApplication::translate( "MainWindow", "OpenWalnut (development version)", 0, QApplication::UnicodeUTF8 ) );


    // the control panel instance is needed for the menu
    m_controlPanel = new WQtControlPanel( this );
    m_controlPanel->setFeatures( QDockWidget::AllDockWidgetFeatures );
    addDockWidget( Qt::RightDockWidgetArea, m_controlPanel );
    m_controlPanel->addSubject( "Default Subject" );

    setupPermanentToolBar();

    // we want the upper most tree item to be selected. This helps to make the always compatible modules
    // show up in the tool bar from the beginning. And ... it doesn't hurt.
    m_controlPanel->selectUpperMostEntry();

    // set the size of the control panel according to config file
    int controlPanelWidth = 250;
    if( WPreferences::getPreference( "qt4gui.dsbWidth", &controlPanelWidth ) )
    {
        m_controlPanel->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
        m_controlPanel->setMinimumWidth( controlPanelWidth );
    }

    // hide the control panel by default?
    bool controlPanelInvisibleByDefault = false;
    if( WPreferences::getPreference( "qt4gui.dsbInvisibleByDefault", &controlPanelInvisibleByDefault ) )
    {
        m_controlPanel->setVisible( !controlPanelInvisibleByDefault );
    }

    // undock the control panel by default?
    bool controlPanelFloatingByDefault = false;
    if( WPreferences::getPreference( "qt4gui.dsbFloatingByDefault", &controlPanelFloatingByDefault ) )
    {
        m_controlPanel->setFloating( controlPanelFloatingByDefault );
    }

    // NOTE: Please be aware that not every menu needs a shortcut key. If you add a shortcut, you should use one of the
    // QKeySequence::StandardKey defaults and avoid ambiguities like Ctrl-C for the configure dialog is not the best choice as Ctrl-C, for the
    // most users is the Copy shortcut.

    m_menuBar = new QMenuBar( this );

    // hide menu?
    bool hideMenu = false;
    WPreferences::getPreference( "qt4gui.hideMenuBar", &hideMenu );
    m_menuBar->setVisible( !hideMenu );

    QMenu* fileMenu = m_menuBar->addMenu( "File" );

    fileMenu->addAction( m_iconManager.getIcon( "load" ), "Load Dataset", this, SLOT( openLoadDialog() ), QKeySequence(  QKeySequence::Open ) );
    fileMenu->addSeparator();
    fileMenu->addAction( m_iconManager.getIcon( "loadProject" ), "Load Project", this, SLOT( projectLoad() ) );
    QMenu* saveMenu = fileMenu->addMenu( m_iconManager.getIcon( "saveProject" ), "Save Project" );
    saveMenu->addAction( "Save Project", this, SLOT( projectSaveAll() ), QKeySequence::Save );
    saveMenu->addAction( "Save Modules Only", this, SLOT( projectSaveModuleOnly() ) );
    saveMenu->addAction( "Save Camera Only", this, SLOT( projectSaveCameraOnly() ) );
    saveMenu->addAction( "Save ROIs Only", this, SLOT( projectSaveROIOnly() ) );
    fileMenu->addSeparator();
    fileMenu->addAction( m_iconManager.getIcon( "config" ), "Config", this, SLOT( openConfigDialog() ) );
    fileMenu->addSeparator();
    // TODO(all): If all distributions provide a newer QT version we should use QKeySequence::Quit here
    //fileMenu->addAction( m_iconManager.getIcon( "quit" ), "Quit", this, SLOT( close() ), QKeySequence( QKeySequence::Quit ) );
    fileMenu->addAction( m_iconManager.getIcon( "quit" ), "Quit", this, SLOT( close() ),  QKeySequence( Qt::CTRL + Qt::Key_Q ) );

    // This QAction stuff is quite ugly and complicated some times ... There is no nice constructor which takes name, slot keysequence and so on
    // directly -> set shortcuts, and some further properties using QAction's interface
    QMenu* viewMenu = m_menuBar->addMenu( "View" );

    QAction* controlPanelTrigger = m_controlPanel->toggleViewAction();
    QList< QKeySequence > controlPanelShortcut;
    controlPanelShortcut.append( QKeySequence( Qt::Key_F9 ) );
    controlPanelTrigger->setShortcuts( controlPanelShortcut );
    viewMenu->addAction( controlPanelTrigger );
    viewMenu->addSeparator();
    this->addAction( controlPanelTrigger );  // this enables the action even if the menu bar is invisible

    // NOTE: the shortcuts for these view presets should be chosen carefully. Most keysequences have another meaning in the most applications
    // so the user may get confused. It is also not a good idea to take letters as they might be used by OpenSceneGraph widget ( like "S" for
    // statistics ).
    // By additionally adding the action to the main window, we ensure the action can be triggered even if the menu bar is hidden.
    this->addAction( viewMenu->addAction( "Left", this, SLOT( setPresetViewLeft() ),           QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_L ) ) );
    this->addAction( viewMenu->addAction( "Right", this, SLOT( setPresetViewRight() ),         QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_R ) ) );
    this->addAction( viewMenu->addAction( "Superior", this, SLOT( setPresetViewSuperior() ),   QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_S ) ) );
    this->addAction( viewMenu->addAction( "Inferior", this, SLOT( setPresetViewInferior() ),   QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_I ) ) );
    this->addAction( viewMenu->addAction( "Anterior", this, SLOT( setPresetViewAnterior() ),   QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_A ) ) );
    this->addAction( viewMenu->addAction( "Posterior", this, SLOT( setPresetViewPosterior() ), QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_P ) ) );

    QMenu* helpMenu = m_menuBar->addMenu( "Help" );
    helpMenu->addAction( m_iconManager.getIcon( "help" ), "OpenWalnut Introduction", this, SLOT( openIntroductionDialog() ),
                         QKeySequence( QKeySequence::HelpContents ) );
    helpMenu->addSeparator();
    helpMenu->addAction( m_iconManager.getIcon( "logo" ), "About OpenWalnut", this, SLOT( openAboutDialog() ) );
    helpMenu->addAction( "About Qt", this, SLOT( openAboutQtDialog() ) );

    setMenuBar( m_menuBar );

    // initially 3 navigation views
    {
        bool hideWidget;
        if( !( WPreferences::getPreference( "qt4gui.hideAxial", &hideWidget ) && hideWidget) )
        {
#ifndef _MSC_VER
            m_navAxial = boost::shared_ptr< WQtNavGLWidget >( new WQtNavGLWidget( "Axial View", this, "Axial Slice", m_mainGLWidget.get() ) );
#else
            m_navAxial = boost::shared_ptr< WQtNavGLWidget >( new WQtNavGLWidget( "Axial View", this, "Axial Slice" ) );
#endif
            m_navAxial->setFeatures( QDockWidget::AllDockWidgetFeatures );
            addDockWidget( Qt::LeftDockWidgetArea, m_navAxial.get() );
        }
        if( !( WPreferences::getPreference( "qt4gui.hideCoronal", &hideWidget ) && hideWidget) )
        {
#ifndef _MSC_VER
            m_navCoronal = boost::shared_ptr< WQtNavGLWidget >( new WQtNavGLWidget( "Coronal View", this, "Coronal Slice", m_mainGLWidget.get() ) );
#else
            m_navCoronal = boost::shared_ptr< WQtNavGLWidget >( new WQtNavGLWidget( "Coronal View", this, "Coronal Slice" ) );
#endif
            m_navCoronal->setFeatures( QDockWidget::AllDockWidgetFeatures );
            addDockWidget( Qt::LeftDockWidgetArea, m_navCoronal.get() );
        }
        if( !( WPreferences::getPreference( "qt4gui.hideSagittal", &hideWidget ) && hideWidget) )
        {
#ifndef _MSC_VER
            m_navSagittal =
                boost::shared_ptr< WQtNavGLWidget >( new WQtNavGLWidget( "Sagittal View", this, "Sagittal Slice", m_mainGLWidget.get() ) );
#else
            m_navSagittal =
                boost::shared_ptr< WQtNavGLWidget >( new WQtNavGLWidget( "Sagittal View", this, "Sagittal Slice" ) );
#endif
            m_navSagittal->setFeatures( QDockWidget::AllDockWidgetFeatures );
            addDockWidget( Qt::LeftDockWidgetArea, m_navSagittal.get() );
        }
    }

    // we do not need the dummy widget if there are no other widgets.
    if( m_navAxial || m_navCoronal || m_navSagittal )
    {
        m_dummyWidget = new QDockWidget( "Spacer", this );
        QWidget* tmp = new QWidget( m_dummyWidget );
        m_dummyWidget->setTitleBarWidget( tmp );
        m_dummyWidget->setFeatures( QDockWidget::DockWidgetClosable );
        m_dummyWidget->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Ignored );
        addDockWidget( Qt::LeftDockWidgetArea, m_dummyWidget );
    }

    // Default background color from config file
    {
        WColor bgColor;
        double r;
        double g;
        double b;
        if( WPreferences::getPreference( "ge.bgColor.r", &r )
            && WPreferences::getPreference( "ge.bgColor.g", &g )
            && WPreferences::getPreference( "ge.bgColor.b", &b ) )
        {
            bgColor.setRGB( r, g, b );
            m_mainGLWidget->setBgColor( bgColor );

            if( m_navAxial )
            {
                m_navAxial->getGLWidget()->setBgColor( bgColor );
            }
            if( m_navCoronal )
            {
                m_navCoronal->getGLWidget()->setBgColor( bgColor );
            }
            if( m_navSagittal )
            {
                m_navSagittal->getGLWidget()->setBgColor( bgColor );
            }
        }
    }
}

void WMainWindow::setupPermanentToolBar()
{
    m_mainGLWidget = boost::shared_ptr< WQtGLWidget >( new WQtGLWidget( "main", this, WGECamera::ORTHOGRAPHIC ) );
    setCentralWidget( m_mainGLWidget.get() );

    m_permanentToolBar = new WQtToolBar( "Permanent Toolbar", this );

    // Set the style of the toolbar
    // NOTE: this only works if the toolbar is used with QActions instead of buttons and other widgets
    m_permanentToolBar->setToolButtonStyle( getToolbarStyle() );

    m_iconManager.addIcon( std::string( "ROI icon" ), box_xpm );
    m_iconManager.addIcon( std::string( "Reset icon" ), o_xpm );
    m_iconManager.addIcon( std::string( "axial icon" ), axial_xpm );
    m_iconManager.addIcon( std::string( "coronal icon" ), cor_xpm );
    m_iconManager.addIcon( std::string( "sagittal icon" ), sag_xpm );

    // TODO(all): this should be QActions to allow the toolbar style to work properly
    m_loadButton = new WQtPushButton( m_iconManager.getIcon( "load" ), "load", m_permanentToolBar );
    WQtPushButton* roiButton = new WQtPushButton( m_iconManager.getIcon( "ROI icon" ), "ROI", m_permanentToolBar );
    WQtPushButton* resetButton = new WQtPushButton( m_iconManager.getIcon( "Reset icon" ), "Reset", m_permanentToolBar );
    resetButton->setShortcut( QKeySequence( Qt::Key_Escape ) );
    WQtPushButton* projectLoadButton = new WQtPushButton( m_iconManager.getIcon( "loadProject" ), "loadProject", m_permanentToolBar );
    WQtPushButton* projectSaveButton = new WQtPushButton( m_iconManager.getIcon( "saveProject" ), "saveProject", m_permanentToolBar );

    // setup save button
    QMenu* saveMenu = new QMenu( "Save Project", projectSaveButton );
    saveMenu->addAction( "Save Project", this, SLOT( projectSaveAll() ) );
    saveMenu->addAction( "Save Modules", this, SLOT( projectSaveModuleOnly() ) );
    saveMenu->addAction( "Save Camera", this, SLOT( projectSaveCameraOnly() ) );
    saveMenu->addAction( "Save ROIs", this, SLOT( projectSaveROIOnly() ) );
    projectSaveButton->setPopupMode( QToolButton::MenuButtonPopup );
    projectSaveButton->setMenu( saveMenu );

    connect( m_loadButton, SIGNAL( pressed() ), this, SLOT( openLoadDialog() ) );
    connect( resetButton, SIGNAL( pressed() ), m_mainGLWidget.get(), SLOT( reset() ) );
    connect( roiButton, SIGNAL( pressed() ), this, SLOT( newRoi() ) );
    connect( projectLoadButton, SIGNAL( pressed() ), this, SLOT( projectLoad() ) );
    connect( projectSaveButton, SIGNAL( pressed() ), this, SLOT( projectSaveAll() ) );

    m_loadButton->setToolTip( "Load Data" );
    resetButton->setToolTip( "Reset main view" );
    roiButton->setToolTip( "Create New ROI" );
    projectLoadButton->setToolTip( "Load a project from file" );
    projectSaveButton->setToolTip( "Save current project to file" );

    m_permanentToolBar->addWidget( m_loadButton );
    m_permanentToolBar->addSeparator();
    m_permanentToolBar->addWidget( projectLoadButton );
    m_permanentToolBar->addWidget( projectSaveButton );
    m_permanentToolBar->addSeparator();
    m_permanentToolBar->addWidget( resetButton );
    m_permanentToolBar->addWidget( roiButton );
    m_permanentToolBar->addSeparator();

    if( getToolbarPos() == InControlPanel )
    {
        m_controlPanel->addToolbar( m_permanentToolBar );
        //m_controlPanel->setTitleBarWidget( m_permanentToolBar );
    }
    else if( getToolbarPos() == Hide )
    {
        m_permanentToolBar->setVisible( false );
        addToolBar( Qt::TopToolBarArea, m_permanentToolBar );
    }
    else
    {
        addToolBar( toQtToolBarArea( getToolbarPos() ), m_permanentToolBar );
    }
}

void WMainWindow::autoAdd( boost::shared_ptr< WModule > module, std::string proto )
{
    // get the prototype.
    if( !WKernel::getRunningKernel()->getRootContainer()->applyModule( module, proto, true ) )
    {
        WLogger::getLogger()->addLogMessage( "Auto Display active but module " + proto + " could not be added.",
                                             "GUI", LL_ERROR );
    }
}

void WMainWindow::moduleSpecificCleanup( boost::shared_ptr< WModule > module )
{
    // nav slices use separate buttons for slice on/off switching
    if( module->getName() == "Navigation Slices" )
    {
        boost::shared_ptr< WPropertyBase > prop;

        prop = module->getProperties()->findProperty( "showAxial" );
        m_permanentToolBar->removeAction( propertyActionMap[prop] );
        propertyActionMap.erase( prop );

        prop = module->getProperties()->findProperty( "showCoronal" );
        m_permanentToolBar->removeAction( propertyActionMap[prop] );
        propertyActionMap.erase( prop );

        prop = module->getProperties()->findProperty( "showSagittal" );
        m_permanentToolBar->removeAction( propertyActionMap[prop] );
        propertyActionMap.erase( prop );


        prop = module->getProperties()->findProperty( "Axial Slice" );
        if( m_navAxial )
        {
            m_navAxial->removeSliderProperty( prop );
        }

        prop = module->getProperties()->findProperty( "Coronal Slice" );
        if( m_navCoronal )
        {
            m_navCoronal->removeSliderProperty( prop );
        }

        prop = module->getProperties()->findProperty( "Sagittal Slice" );
        if( m_navSagittal )
        {
            m_navSagittal->removeSliderProperty( prop );
        }
    }
}

void WMainWindow::moduleSpecificSetup( boost::shared_ptr< WModule > module )
{
    // Add all special handlings here. This method is called whenever a module is marked "ready". You can set up the gui for special modules,
    // load certain modules for datasets and so on.

    // The Data Modules also play an special role. To have modules being activated when certain data got loaded, we need to hook it up here.
    bool useAutoDisplay = true;
    WPreferences::getPreference( "qt4gui.useAutoDisplay", &useAutoDisplay );
    if( useAutoDisplay && module->getType() == MODULE_DATA )
    {
        WLogger::getLogger()->addLogMessage( "Auto Display active and Data module added. The proper module will be added.",
                                             "GUI", LL_DEBUG );

        // data modules contain an member denoting the real data type. Currently we only have one data module and a not very modulated data
        // structures.
        boost::shared_ptr< WMData > dataModule = boost::shared_static_cast< WMData >( module );

        // grab data and identify type
        if( dataModule->getDataSet()->isA< WDataSetSingle >() && dataModule->getDataSet()->isTexture() )
        {
            // it is a dataset single
            // load a nav slice module if a WDataSetSingle is available!?

            // if it not already is running: add it
            if( !WMNavSlices::isRunning() )
            {
                autoAdd( module, "Navigation Slices" );
            }
        }
        else if( dataModule->getDataSet()->isA< WDataSetFibers >() )
        {
            // it is a fiber dataset -> add the FiberDisplay module
            autoAdd( module, "Fiber Display" );
        }
        else if( dataModule->getDataSet()->isA< WEEG2 >() )
        {
            // it is a eeg dataset -> add the eegView module
            autoAdd( module, "EEG View" );
        }
    }

    // nav slices use separate buttons for slice on/off switching
    if( module->getName() == "Navigation Slices" )
    {
        boost::shared_ptr< WPropertyBase > prop = module->getProperties()->findProperty( "showAxial" );
        if( !prop )
        {
               WLogger::getLogger()->
                   addLogMessage( "Navigation Slices module does not provide the property \"showAxial\", which is required by the GUI.", "GUI",
                                  LL_ERROR );
        }
        else
        {
            WQtPropertyBoolAction* a = new WQtPropertyBoolAction( prop->toPropBool(), m_permanentToolBar );
            a->setToolTip( "Toggle Axial Slice" );
            a->setText( "Toggle Axial Slice" );
            a->setIcon( m_iconManager.getIcon( "axial icon" ) );
            m_permanentToolBar->addAction( a );
            propertyActionMap[prop] = a;
        }

        prop = module->getProperties()->findProperty( "showCoronal" );
        if( !prop )
        {
               WLogger::getLogger()->
                   addLogMessage( "Navigation Slices module does not provide the property \"showCoronal\", which is required by the GUI.", "GUI",
                                  LL_ERROR );
        }
        else
        {
            WQtPropertyBoolAction* a = new WQtPropertyBoolAction( prop->toPropBool(), m_permanentToolBar );
            a->setToolTip( "Toggle Coronal Slice" );
            a->setText( "Toggle Coronal Slice" );
            a->setIcon( m_iconManager.getIcon( "coronal icon" ) );
            m_permanentToolBar->addAction( a );
            propertyActionMap[prop] = a;
        }

        prop = module->getProperties()->findProperty( "showSagittal" );
        if( !prop )
        {
               WLogger::getLogger()->
                   addLogMessage( "Navigation Slices module does not provide the property \"showSagittal\", which is required by the GUI.", "GUI",
                                  LL_ERROR );
        }
        else
        {
            WQtPropertyBoolAction* a = new WQtPropertyBoolAction( prop->toPropBool(), m_permanentToolBar );
            a->setToolTip( "Toggle Saggital Slice" );
            a->setText( "Toggle Saggital Slice" );
            a->setIcon( m_iconManager.getIcon( "sagittal icon" ) );
            m_permanentToolBar->addAction( a );
            propertyActionMap[prop] = a;
        }

        // now setup the nav widget sliders
        prop = module->getProperties()->findProperty( "Axial Slice" );
        if( !prop )
        {
               WLogger::getLogger()->
                   addLogMessage( "Navigation Slices module does not provide the property \"Axial Slice\", which is required by the GUI.", "GUI",
                                  LL_ERROR );
        }
        else
        {
            if( m_navAxial )
            {
                m_navAxial->setSliderProperty( prop );
            }
        }

        prop = module->getProperties()->findProperty( "Coronal Slice" );
        if( !prop )
        {
               WLogger::getLogger()->
                   addLogMessage( "Navigation Slices module does not provide the property \"Coronal Slice\", which is required by the GUI.", "GUI",
                                  LL_ERROR );
        }
        else
        {
            if( m_navCoronal )
            {
                m_navCoronal->setSliderProperty( prop );
            }
        }

        prop = module->getProperties()->findProperty( "Sagittal Slice" );
        if( !prop )
        {
               WLogger::getLogger()->
                   addLogMessage( "Navigation Slices module does not provide the property \"Sagittal Slice\", which is required by the GUI.", "GUI",
                                  LL_ERROR );
        }
        else
        {
            if( m_navSagittal )
            {
               m_navSagittal->setSliderProperty( prop );
            }
        }
    }
}

Qt::ToolButtonStyle WMainWindow::getToolbarStyle() const
{
    // this sets the toolbar style
    int toolBarStyle = 0;
    WPreferences::getPreference( "qt4gui.toolBarStyle", &toolBarStyle );
    if( ( toolBarStyle < 0 ) || ( toolBarStyle > 3 ) ) // ensure a valid value
    {
        toolBarStyle = 0;
    }

    // cast and return
    return static_cast< Qt::ToolButtonStyle >( toolBarStyle );
}

WMainWindow::ToolBarPosition WMainWindow::getToolbarPos()
{
    int toolbarPos = 0;
    WPreferences::getPreference( "qt4gui.toolBarPos", &toolbarPos );
    return static_cast< ToolBarPosition >( toolbarPos );
}

WMainWindow::ToolBarPosition WMainWindow::getCompatiblesToolbarPos()
{
    int compatiblesToolbarPos = 0;
    if( !WPreferences::getPreference( "qt4gui.compatiblesToolBarPos", &compatiblesToolbarPos ) )
    {
        return getToolbarPos();
    }

    return static_cast< ToolBarPosition >( compatiblesToolbarPos );
}

Qt::ToolBarArea WMainWindow::toQtToolBarArea( ToolBarPosition pos )
{
    switch ( pos )
    {
        case Top:
            return Qt::TopToolBarArea;
            break;
        case Bottom:
            return Qt::BottomToolBarArea;
            break;
        case Left:
            return Qt::LeftToolBarArea;
            break;
        case Right:
            return Qt::RightToolBarArea;
            break;
        case InControlPanel:
            return Qt::RightToolBarArea;
        default:
            return Qt::NoToolBarArea;
            break;
      }
}

void WMainWindow::setCompatiblesToolbar( WQtCombinerToolbar* toolbar )
{
    if( m_currentCompatiblesToolbar )
    {
        delete m_currentCompatiblesToolbar;
    }
    m_currentCompatiblesToolbar = toolbar;

    // hide it?
    if( getCompatiblesToolbarPos() == Hide )
    {
        if( toolbar )
        {
            toolbar->setVisible( false );
        }
        return;
    }

    if( !toolbar )
    {
        // ok, reset the toolbar
        // So create a dummy to permanently reserve the space
        m_currentCompatiblesToolbar = new WQtCombinerToolbar( this );
    }

    // optional toolbar break
    {
        bool useToolBarBreak = true;
        WPreferences::getPreference( "qt4gui.useToolBarBreak", &useToolBarBreak );
        if( useToolBarBreak )
        {
            addToolBarBreak( toQtToolBarArea( getCompatiblesToolbarPos() ) );
        }
    }

    // and the position of the toolbar
    addToolBar( toQtToolBarArea( getCompatiblesToolbarPos() ), m_currentCompatiblesToolbar );
}

WQtCombinerToolbar* WMainWindow::getCompatiblesToolbar()
{
    return m_currentCompatiblesToolbar;
}

WQtControlPanel* WMainWindow::getControlPanel()
{
    return m_controlPanel;
}

void WMainWindow::projectSave( const std::vector< boost::shared_ptr< WProjectFileIO > >& writer )
{
    QFileDialog fd;
    fd.setWindowTitle( "Save Project as" );
    fd.setFileMode( QFileDialog::AnyFile );
    fd.setAcceptMode( QFileDialog::AcceptSave );

    QStringList filters;
    filters << "Project File (*.owproj *.owp)"
            << "Any files (*)";
    fd.setNameFilters( filters );
    fd.setViewMode( QFileDialog::Detail );
    QStringList fileNames;
    if( fd.exec() )
    {
        fileNames = fd.selectedFiles();
    }

    QStringList::const_iterator constIterator;
    for( constIterator = fileNames.constBegin(); constIterator != fileNames.constEnd(); ++constIterator )
    {
        boost::shared_ptr< WProjectFile > proj = boost::shared_ptr< WProjectFile >(
                new WProjectFile( ( *constIterator ).toStdString() )
        );

        try
        {
            // This call is synchronous.
            if( writer.empty() )
            {
                proj->save();
            }
            else
            {
                proj->save( writer );
            }
        }
        catch( const std::exception& e )
        {
            QString title = "Problem while saving project file.";
            QString message = "<b>Problem while saving project file.</b><br/><br/><b>File:  </b>" + ( *constIterator ) +
                              "<br/><b>Message:  </b>" + QString::fromStdString( e.what() );
            QMessageBox::critical( this, title, message );
        }
    }
}

void WMainWindow::projectSaveAll()
{
    std::vector< boost::shared_ptr< WProjectFileIO > > w;
    // an empty list equals "all"
    projectSave( w );
}

void WMainWindow::projectSaveCameraOnly()
{
    std::vector< boost::shared_ptr< WProjectFileIO > > w;
    w.push_back( WProjectFile::getCameraWriter() );
    projectSave( w );
}

void WMainWindow::projectSaveROIOnly()
{
    std::vector< boost::shared_ptr< WProjectFileIO > > w;
    w.push_back( WProjectFile::getROIWriter() );
    projectSave( w );
}

void WMainWindow::projectSaveModuleOnly()
{
    std::vector< boost::shared_ptr< WProjectFileIO > > w;
    w.push_back( WProjectFile::getModuleWriter() );
    projectSave( w );
}

void WMainWindow::projectLoad()
{
    QFileDialog fd;
    fd.setFileMode( QFileDialog::ExistingFiles );

    QStringList filters;
    filters << "Simple Project File (*.owproj *.owp)"
            << "Any files (*)";
    fd.setNameFilters( filters );
    fd.setViewMode( QFileDialog::Detail );
    QStringList fileNames;
    if( fd.exec() )
    {
        fileNames = fd.selectedFiles();
    }

    QStringList::const_iterator constIterator;
    for( constIterator = fileNames.constBegin(); constIterator != fileNames.constEnd(); ++constIterator )
    {
        boost::shared_ptr< WProjectFile > proj = boost::shared_ptr< WProjectFile >(
                new WProjectFile( ( *constIterator ).toStdString() )
        );

        // This call is asynchronous. It parses the file and the starts a thread to actually do all the stuff
        proj->load();
    }
}

void WMainWindow::openLoadDialog()
{
#ifdef _MSC_VER
    QStringList fileNames;
    QString filters;
    filters = QString::fromStdString( std::string( "Known file types (*.cnt *.edf *.asc *.nii *.nii.gz *.fib);;" )
        + std::string( "EEG files (*.cnt *.edf *.asc);;" )
        + std::string( "NIfTI (*.nii *.nii.gz);;" )
        + std::string( "Fibers (*.fib);;" )
        + std::string( "Any files (*)" ) );

    fileNames = QFileDialog::getOpenFileNames( this, "Open", "", filters  );
#else
    QFileDialog fd;
    fd.setFileMode( QFileDialog::ExistingFiles );

    QStringList filters;
    filters << "Known file types (*.cnt *.edf *.asc *.nii *.nii.gz *.fib)"
            << "EEG files (*.cnt *.edf *.asc)"
            << "NIfTI (*.nii *.nii.gz)"
            << "Fibers (*.fib)"
            << "Any files (*)";
    fd.setNameFilters( filters );
    fd.setViewMode( QFileDialog::Detail );
    QStringList fileNames;
    if( fd.exec() )
    {
        fileNames = fd.selectedFiles();
    }
#endif

    std::vector< std::string > stdFileNames;

    QStringList::const_iterator constIterator;
    for( constIterator = fileNames.constBegin(); constIterator != fileNames.constEnd(); ++constIterator )
    {
        stdFileNames.push_back( ( *constIterator ).toLocal8Bit().constData() );
    }

    m_loaderSignal( stdFileNames );

    // walkaround that a button keeps his down state after invoking a dialog
    m_loadButton->setDown( false );
}

void WMainWindow::openAboutQtDialog()
{
    QMessageBox::aboutQt( this, "About Qt" );
}
void WMainWindow::openAboutDialog()
{
    QMessageBox::about( this, "About OpenWalnut",
                        "OpenWalnut ( http://www.openwalnut.org )\n\n"
                        "Copyright 2009-2010 OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS. "
                        "For more information see http://www.openwalnut.org/copying.\n\n"
                        "This program comes with ABSOLUTELY NO WARRANTY. "
                        "This is free software, and you are welcome to redistribute it "
                        "under the terms of the GNU Lesser General Public License. "
                        "You should have received a copy of the GNU Lesser General Public License "
                        "along with OpenWalnut. If not, see <http://www.gnu.org/licenses/>.\n"
                        "\n"
                        "Thank you for using OpenWalnut." );
}

void WMainWindow::openIntroductionDialog()
{
    QMessageBox::information( this, "OpenWalnut Introduction",
                              "<h3>Navigation in Main View</h3>"
                              "<table>"
                              "<tr><td><b><i>Mouse Button&nbsp;&nbsp;</i></b></td><td><b><i>Action</i></b></td></tr>"
                              "<tr><td>Left</td><td>Rotate</td></tr>"
                              "<tr><td>Middle</td><td>Pan</td></tr>"
                              "<tr><td>Right</td><td>Pick, move ROI box or move slice</td></tr>"
                              "<tr><td>Right + Shift</td><td>Resize ROI box</td></tr>"
                              "<tr><td>Wheel</td><td>Zoom</td></tr>"
                              "<tr><td><b><i>Key</i></b></td><td><b><i>Action</i></b></td></tr>"
                              "<tr><td>+</td><td>Zoom in</td></tr>"
                              "<tr><td>-</td><td>Zoom out</td></tr>"
                              "<tr><td>[space]</td><td>Reset view</td></tr>"
                              "</table>"
                              "<h3>Navigation in EEG View</h3>"
                              "<table>"
                              "<tr><td><b><i>Mouse Button</i></b></td><td><b><i>Action</i></b></td></tr>"
                              "<tr><td>Left</td><td>Mark event position</td></tr>"
                              "<tr><td>Middle</td><td>Pan</td></tr>"
                              "<tr><td>Right</td><td>Zoom in time</td></tr>"
                              "<tr><td>Wheel</td><td>Scale voltage</td></tr>"
                              "<tr><td>Wheel + Right</td><td>Change spacing between graphs</td></tr>"
                              "<tr><td>Wheel + Left</td><td>Change sensitivity of the coloring of the head surface</td></tr>"
                              "</table>"
                              "<h3>Program-wide Keyboard Shortcuts</h3>"
                              "<table>"
                              "<tr><td><b><i>Key</i></b></td><td><b><i>Action</i></b></td></tr>"
                              "<tr><td>Ctrl + q</td><td>Quit</td></tr>"
                              "<tr><td>Esc</td><td>Resets main view</td></tr>"
                              "<tr><td>F1</td><td>Opens this help window</td></tr>"
                              "</table>" );
}

void WMainWindow::setPresetViewLeft()
{
    boost::shared_ptr< WGEViewer > viewer;
    viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "main" );
    osg::ref_ptr<osgGA::TrackballManipulator>  cm = osg::dynamic_pointer_cast<osgGA::TrackballManipulator>( viewer->getCameraManipulator() );
    osg::Quat q( 0.5, -0.5, -0.5, 0.5 );
    cm->setRotation( q );
}

void WMainWindow::setPresetViewRight()
{
    boost::shared_ptr< WGEViewer > viewer;
    viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "main" );
    osg::ref_ptr<osgGA::TrackballManipulator>  cm = osg::dynamic_pointer_cast<osgGA::TrackballManipulator>( viewer->getCameraManipulator() );
    osg::Quat q( -0.5, -0.5, -0.5, -0.5 );
    cm->setRotation( q );
}

void WMainWindow::setPresetViewSuperior()
{
    boost::shared_ptr< WGEViewer > viewer;
    viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "main" );
    osg::ref_ptr<osgGA::TrackballManipulator>  cm = osg::dynamic_pointer_cast<osgGA::TrackballManipulator>( viewer->getCameraManipulator() );
    osg::Quat q( 0., 0., 0., 1 );
    cm->setRotation( q );
}

void WMainWindow::setPresetViewInferior()
{
    boost::shared_ptr< WGEViewer > viewer;
    viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "main" );
    osg::ref_ptr<osgGA::TrackballManipulator>  cm = osg::dynamic_pointer_cast<osgGA::TrackballManipulator>( viewer->getCameraManipulator() );
    osg::Quat q( 0., -1., 0., 0. );
    cm->setRotation( q );
}

void WMainWindow::setPresetViewAnterior()
{
    boost::shared_ptr< WGEViewer > viewer;
    viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "main" );
    osg::ref_ptr<osgGA::TrackballManipulator>  cm = osg::dynamic_pointer_cast<osgGA::TrackballManipulator>( viewer->getCameraManipulator() );
    osg::Quat q( 0., -0.707107, -0.707107, 0. );
    cm->setRotation( q );
}

void WMainWindow::setPresetViewPosterior()
{
    boost::shared_ptr< WGEViewer > viewer;
    viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "main" );
    osg::ref_ptr<osgGA::TrackballManipulator>  cm = osg::dynamic_pointer_cast<osgGA::TrackballManipulator>( viewer->getCameraManipulator() );
    osg::Quat q( 0.707107, 0., 0., 0.707107 );
    cm->setRotation( q );
}

void WMainWindow::openNotImplementedDialog()
{
    QMessageBox::information( this, "Not yet implemented!",
                              "This functionality is planned for future versions of OpenWalnut. "
                              "It is not yet implemented." );
}

boost::signals2::signal1< void, std::vector< std::string > >* WMainWindow::getLoaderSignal()
{
    return &m_loaderSignal;
}

WIconManager*  WMainWindow::getIconManager()
{
    return &m_iconManager;
}

void WMainWindow::closeEvent( QCloseEvent* e )
{
    // use some "Really Close?" Dialog here
    bool reallyClose = true;


    // handle close event
    if( reallyClose )
    {
        // signal everybody to shut down properly.
        WKernel::getRunningKernel()->finalize();

        // now nobody acesses the osg anymore
        // clean up gl widgets
        m_mainGLWidget->close();
        if( m_navAxial )
        {
            m_navAxial->close();
        }
        if( m_navCoronal )
        {
            m_navCoronal->close();
        }
        if( m_navSagittal )
        {
            m_navSagittal->close();
        }

        // delete CustomDockWidgets
        boost::mutex::scoped_lock lock( m_customDockWidgetsLock );
        for( std::map< std::string, boost::shared_ptr< WQtCustomDockWidget > >::iterator it = m_customDockWidgets.begin();
             it != m_customDockWidgets.end(); ++it )
        {
            it->second->close();
        }
        //m_customDockWidgetsLock.unlock();

        // finally close
        e->accept();
    }
    else
    {
        e->ignore();
    }
}

void WMainWindow::customEvent( QEvent* event )
{
    if( event->type() == WOpenCustomDockWidgetEvent::CUSTOM_TYPE )
    {
        // OpenCustomDockWidgetEvent
        WOpenCustomDockWidgetEvent* ocdwEvent = static_cast< WOpenCustomDockWidgetEvent* >( event );
        std::string title = ocdwEvent->getTitle();

        boost::shared_ptr< WQtCustomDockWidget > widget;

        boost::mutex::scoped_lock lock( m_customDockWidgetsLock );
        if( m_customDockWidgets.count( title ) == 0 )
        {
            // create new custom dock widget
            widget = boost::shared_ptr< WQtCustomDockWidget >(
                new WQtCustomDockWidget( title, this, ocdwEvent->getProjectionMode() ) );
            addDockWidget( Qt::BottomDockWidgetArea, widget.get() );

            // store it in CustomDockWidget list
            m_customDockWidgets.insert( make_pair( title, widget ) );
        }
        else
        {
            widget = m_customDockWidgets[title];
            widget->increaseUseCount();
        }
        //m_customDockWidgetsLock.unlock();

        ocdwEvent->getFlag()->set( widget );
    }
    else
    {
        // other event
        QMainWindow::customEvent( event );
    }
}

bool WMainWindow::event( QEvent* event )
{
    // a module got associated with the root container -> add it to the list
    if( event->type() == WQT_READY_EVENT )
    {
        // convert event to ready event
        WModuleReadyEvent* e1 = dynamic_cast< WModuleReadyEvent* >( event );     // NOLINT
        if( e1 )
        {
            moduleSpecificSetup( e1->getModule() );
        }
    }

    if( event->type() == WQT_CRASH_EVENT )
    {
        // convert event to ready event
        WModuleCrashEvent* e1 = dynamic_cast< WModuleCrashEvent* >( event );     // NOLINT
        if( e1 )
        {
            QString title = "Problem in module: " + QString::fromStdString( e1->getModule()->getName() );
            QString description = "<b>Module Problem</b><br/><br/><b>Module:  </b>" + QString::fromStdString( e1->getModule()->getName() );

            QString message = QString::fromStdString( e1->getMessage() );
            QMessageBox msgBox;
            msgBox.setText( description );
            msgBox.setInformativeText( message  );
            msgBox.setStandardButtons( QMessageBox::Ok );
            msgBox.exec();
        }
    }

    if( event->type() == WQT_MODULE_REMOVE_EVENT )
    {
        // convert event to ready event
        WModuleRemovedEvent* e1 = dynamic_cast< WModuleRemovedEvent* >( event );     // NOLINT
        if( e1 )
        {
            moduleSpecificCleanup( e1->getModule() );
        }
    }

    return QMainWindow::event( event );
}

boost::shared_ptr< WQtCustomDockWidget > WMainWindow::getCustomDockWidget( std::string title )
{
    boost::mutex::scoped_lock lock( m_customDockWidgetsLock );
    boost::shared_ptr< WQtCustomDockWidget > out = m_customDockWidgets.count( title ) > 0 ?
        m_customDockWidgets[title] :
        boost::shared_ptr< WQtCustomDockWidget >();
    //m_customDockWidgetsLock.unlock();
    return out;
}


void WMainWindow::closeCustomDockWidget( std::string title )
{
    boost::mutex::scoped_lock lock( m_customDockWidgetsLock );
    if( m_customDockWidgets.count( title ) > 0 )
    {
        if( m_customDockWidgets[title]->decreaseUseCount() )
        {
            // custom dock widget should be deleted
            m_customDockWidgets.erase( title );
        }
    }
    //m_customDockWidgetsLock.unlock();
}

void WMainWindow::newRoi()
{
    // do nothing if we can not get
    wmath::WPosition crossHairPos = WKernel::getRunningKernel()->getSelectionManager()->getCrosshair()->getPosition();
    wmath::WPosition minROIPos = crossHairPos - wmath::WPosition( 10., 10., 10. );
    wmath::WPosition maxROIPos = crossHairPos + wmath::WPosition( 10., 10., 10. );

    if( m_controlPanel->getFirstRoiInSelectedBranch().get() == NULL )
    {
        osg::ref_ptr< WROIBox > newRoi = osg::ref_ptr< WROIBox >( new WROIBox( minROIPos, maxROIPos ) );
        WKernel::getRunningKernel()->getRoiManager()->addRoi( newRoi );
    }
    else
    {
        osg::ref_ptr< WROIBox > newRoi = osg::ref_ptr< WROIBox >( new WROIBox( minROIPos, maxROIPos ) );
        WKernel::getRunningKernel()->getRoiManager()->addRoi( newRoi, m_controlPanel->getFirstRoiInSelectedBranch() );
    }
}

void WMainWindow::openConfigDialog()
{
    if( m_configWidget.get() )
    {
        m_configWidget->wait( true );
    }

    m_configWidget = boost::shared_ptr< WQtConfigWidget >( new WQtConfigWidget );

    m_configWidget->initAndShow();
}

