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

#ifndef WQTCONTROLPANEL_H
#define WQTCONTROLPANEL_H

#include <list>
#include <string>
#include <vector>

#include <QtGui/QDockWidget>
#include <QtGui/QSplitter>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <QtGui/QListWidget>

#include "core/dataHandler/WDataSet.h"
#include "core/graphicsEngine/WROI.h"

#include "../WQtCombinerToolbar.h"
#include "WQtPropertyGroupWidget.h"
#include "WQtModuleHeaderTreeItem.h"
#include "WQtModuleTreeItem.h"
#include "WQtRoiHeaderTreeItem.h"
#include "WQtSubjectTreeItem.h"
#include "WQtTreeWidget.h"

class WMainWindow;
class WQtColormapper;

/**
 * container widget for a tree widget with context menu and some control widgets
 */
class WQtControlPanel : public QDockWidget
{
    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * \param parent Parent widget.
     */
    explicit WQtControlPanel( WMainWindow* parent = 0 );

    /**
     * Default Destructor.
     */
    virtual ~WQtControlPanel();

    /**
     * Adds a page to the context widget
     *
     * \param content A widget with controls
     *
     * \return the index of the new tab
     */
    int addTabWidgetContent( WQtPropertyGroupWidget* content );

    /**
     * Adds a subject entry to the tree widget
     *
     * \param name The entry name of the subjectin the tree widget
     * \return A pointer to the tree widget item
     */
    WQtSubjectTreeItem* addSubject( std::string name );

    /**
     * Adds a dataset entry to any given subject in the tree widget
     *
     * \param module shared pointer to the module associated with this tree widget entry
     * \param subjectId subject id this dataset belongs to
     *
     * \return A pointer to the tree widget item
     */
    WQtDatasetTreeItem* addDataset( boost::shared_ptr< WModule > module, int subjectId = 0 );

    /**
     * Adds a module to the control panel.
     *
     * \param module the module to add.
     *
     * \return the representation in control panel.
     */
    WQtModuleTreeItem* addModule( boost::shared_ptr< WModule > module );

    /**
     * Adds a roi entry to the control panel
     *
     * \param roi pointer to the roi representation object
     */
    void addRoi( osg::ref_ptr< WROI > roi );

    /**
     * Removes a roi entry from the control panel
     *
     * \param roi pointer to the roi representation object
     */
    void removeRoi( osg::ref_ptr< WROI > roi );

    /**
     * helper funtion to connect all qt widgets with their functions
     */
    void connectSlots();

    /**
     * Returns the module currently selected in control panel.
     *
     * \return the module.
     */
    boost::shared_ptr< WModule > getSelectedModule();

    /**
     * Returns the currently selected roi.
     *
     * \return pointer to roi representation
     */
    osg::ref_ptr< WROI > getSelectedRoi();

    /**
     * Returns the first roi in the currently selected branch.
     *
     * \return pointer to roi representation
     */
    osg::ref_ptr< WROI > getFirstRoiInSelectedBranch();

    /**
     * Returns a checkable action that can be used to show or close this dock widget.
     * The action's text is set to the dock widget's window title.
     *
     * This member function is overwritten to add a keyboard shortcut to this action.
     *
     * \return Modified QAction
     */
    QAction* toggleViewAction() const;

    /**
     * Add the specified toolbar to the top of the control panel.
     *
     * \param tb the toolbar to add
     */
    void addToolbar( QToolBar* tb );

    /**
     * Selects the uppermost entry in the module tree.
     */
    void selectUpperMostEntry();

    /**
     * allows other objects to build the property tab (probably only for WQtNetworkeditor)
     *
     * \param module the module from which the property tab is created
     **/
    void setNewActiveModule( boost::shared_ptr< WModule > module );

    /**
     * Gets the ROI dock widget.
     *
     * \return the ROI dock.
     */
    QDockWidget* getRoiDock() const;

    /**
     * Gets the module dock widget.
     *
     * \return the module dock
     */
    QDockWidget* getModuleDock() const;

    /**
     * Gets the colormapper dock
     *
     * \return the dock
     */
    QDockWidget* getColormapperDock() const;

protected:

    /**
     * Custom event dispatcher. Gets called by QT's Event system every time an event got sent to this widget. This event handler
     * processes several custom events, like WModuleAssocEvent.
     *
     * \param event the event that got transmitted.
     *
     * \return true if the event got handled properly.
     */
    virtual bool event( QEvent* event );

    /**
     * Helper function to return the first subject id in the tree widget
     */
    int getFirstSubject();

    /**
     * fills the modul toolbar with buttons for modules that are compatible with the currently
     * selected dataset
     *
     * \param module pointer to the currently selected module
     *
     */
    void createCompatibleButtons( boost::shared_ptr< WModule > module );

    /**
     * Reference to the main window of the application.
     */
    WMainWindow* m_mainWindow;

    /**
     * Searches the specified tree for all tree items matching the specified module.
     *
     * \param module the module uses as search criteria.
     * \param where in which subtree to search.
     *
     * \return a list of all matching items.
     */
    std::list< WQtTreeItem* > findItemsByModule( boost::shared_ptr< WModule > module, QTreeWidgetItem* where );

    /**
     * Searches the tree for all tree items matching the specified module.
     *
     * \param module the module uses as search criteria.
     *
     * \return a list of all matching items.
     */
    std::list< WQtTreeItem* > findItemsByModule( boost::shared_ptr< WModule > module );

private:
    WQtTreeWidget* m_moduleTreeWidget; //!< pointer to the tree widget

    WQtTreeWidget* m_roiTreeWidget; //!< pointer to the tree widget

    WQtColormapper* m_colormapper; //!< the colormapper control widget

    QTabWidget* m_tabWidget; //!< pointer to the tab widget

    QTabWidget* m_tabWidget2; //!< pointer to the tab widget

    QWidget* m_panel; //!< panel

    QVBoxLayout* m_layout; //!< layout

    QSplitter* m_splitter; //!< splitter to have resizable widgets in the control panel

    WQtModuleHeaderTreeItem* m_tiModules; //!< header for modules

    WQtRoiHeaderTreeItem* m_tiRois; //!< header for rois

    bool m_showToolBarText; //!< Show tool bar icons with text

    QDockWidget* m_roiDock;     //!< the dock widget with the roi tree
    QDockWidget* m_moduleDock;  //!< the dock widget with the module tree

    /**
     * The action to remove a module from the tree.
     */
    QAction* m_deleteModuleAction;

    /**
     * The action to remove a ROI from the tree.
     */
    QAction* m_deleteRoiAction;

    /**
     * Action which uses a compatibles list (submenu) to connect a selected item with other existing modules.
     */
    QAction* m_connectWithModuleAction;

    /**
     * Action which uses a compatibles list (submenu) to connect a selected item with other prototypes.
     */
    QAction* m_connectWithPrototypeAction;

    /**
     * Action which disconnects a connector from the module.
     */
    QAction* m_disconnectAction;

    /**
     * List all actions created for applying a prototype. Is needed for m_connectWithPrototypeAction.
     *
     * \note We need to store this action list here as Qt is not able to delete the actions if they get replaced. We need to handle this
     * manually.
     */
    WQtCombinerActionList m_connectWithPrototypeActionList;

    /**
     * List all actions created for applying a prototype. Is needed for m_connectWithModuleAction.
     *
     * \note We need to store this action list here as Qt is not able to delete the actions if they get replaced. We need to handle this
     * manually.
     */
    WQtCombinerActionList m_connectWithModuleActionList;

    /**
     * List all actions created for applying a prototype. Is needed for m_disconnectAction.
     *
     * \note We need to store this action list here as Qt is not able to delete the actions if they get replaced. We need to handle this
     * manually.
     */
    WQtCombinerActionList m_disconnectActionList;

    /**
     * If true, a selection change does not cause the property tab to rebuild. This is useful if multiple items get selected at once
     * programatically.
     */
    bool m_ignoreSelectionChange;

private slots:
    /**
     * function that gets called when a tree item is selected, on a new select that tab widget
     * is rebuilt with the controls provided by the tree item
     */
    void selectTreeItem();

    /**
     * function that gets called when a tree item is selected, on a new select that tab widget
     * is rebuilt with the controls provided by the tree item
     */
    void selectRoiTreeItem();

    /**
     * Will be called to select the data module for the given dataset.
     * \param dataSet the module for this dataset will be selected.
     */
    void selectDataModule( boost::shared_ptr< WDataSet > dataSet );

    /**
     * Will be called to select the data module for the given texture.
     * \param texture the texture currently selected.
     */
    void selectDataModule( osg::ref_ptr< WGETexture3D > texture );

    /**
     * function that builds the property tab
     *
     * \param props the properties.
     * \param infoProps the information properties shown on a separate tab
     */
    void buildPropTab( boost::shared_ptr< WProperties > props, boost::shared_ptr< WProperties > infoProps );

    /**
     * Method builds a widgets containing all properties in props. It recursively calls itself to build group widgets for WPropGroup properties.
     *
     * \param props the properties.
     */
    WQtPropertyGroupWidget* buildPropWidget( boost::shared_ptr< WProperties > props );

    /**
     * function gets called when a change to a tree item, eg. check box status, occurs
     */
    /**
     * Function gets change when a change to a tree item occurs.
     *
     * \param item the item
     * \param column column index
     */
    void changeTreeItem( QTreeWidgetItem* item, int column );

    /**
     * delete a ROI tree item
     */
    void deleteROITreeItem();

    /**
     * delete a module tree item
     */
    void deleteModuleTreeItem();

    /**
     * function to notify the roi manager of any drag&drop action in the roi tree
     */
    void handleDragDrop();
};

#endif  // WQTCONTROLPANEL_H
