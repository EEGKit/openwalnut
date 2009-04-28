//---------------------------------------------------------------------------
//
// Project: BrainCognize
//
// Copyright 2009 SomeCopyrightowner
//
// This file is part of BrainCognize.
//
// BrainCognize is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// BrainCognize is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with BrainCognize. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------

#ifndef BMAINWINDOW_H
#define BMAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

#include "../BGUI.h"

// forward declarations
class BQtGLWidget;
class BQtPipelineBrowser;

/**
 * \ingroup gui
 * This class contains the main window and the layout
 * of the widgets within the window.
 */
class BMainWindow : public BGUI
{
    public:

        /**
         * Set up all widgets menus an buttons in the main window.
         */
        void setupGUI( QMainWindow *MainWindow );

        /**
         * \brief
         * enable/disable the access to the GUI
         * \post
         * GUI enabled/disabeld
         * \param enable true iff gui should be enabled
         */
        void setEnabled( bool enable );

    private:
        QAction* m_actionLoad;
        QAction* m_actionSave;
        QAction* m_actionPreferences;
        QAction* m_actionExit;
        QAction* m_actionAbout_BrainCognize;
        QWidget* m_centralwidget;
        QMenuBar* m_menubar;
        QMenu* m_menuFile;
        QMenu* m_menuHelp;
        QStatusBar* m_statusBar;
        QToolBar* m_toolBar;

        BQtGLWidget* m_glWidget;
        BQtPipelineBrowser* m_pipelineBrowser;

        /**
         * Helper routine for adding new docks with GL content
         */
        void addDockableGLWidget( QMainWindow *MainWindow );
};

#endif  // BMAINWINDOW_H
