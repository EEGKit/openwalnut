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

#ifndef WQTWIDGETBASE_H
#define WQTWIDGETBASE_H

#include <QtGui/QWidget>

#include <boost/shared_ptr.hpp>

#include "core/common/WCondition.h"

#include "../WMainWindow.h"

/**
 * Implementation of \ref WUIWidgetBase. Not really. It is an base class for all WUIWidgetBase implementations to comfortably allow thread-save
 * GUI ops.
 */
class WQtWidgetBase
{
public:
    /**
     * Convenience typedef for a boost::shared_ptr< WQtWidgetBase >.
     */
    typedef boost::shared_ptr< WQtWidgetBase > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WQtWidgetBase >.
     */
    typedef boost::shared_ptr< const WQtWidgetBase > ConstSPtr;

    /**
     * Default constructor.
     *
     * \param mainWindow the main window instance
     */
    explicit WQtWidgetBase( WMainWindow* mainWindow );

    /**
     * Destructor.
     */
    virtual ~WQtWidgetBase();

    /**
     * The widget was created and can be used.
     *
     * \return true if the widget is valid.
     */
    virtual bool isReal();

    /**
     * Show this widget if not yet visible.
     */
    virtual void show();

    /**
     * Hide/show this widget. Unlike close(), you can show the widget again using show().
     *
     * \param visible false to hide widget
     */
    virtual void setVisible( bool visible = true );

    /**
     * Check if the widget is hidden or not.
     *
     * \return true if visible.
     */
    virtual bool isVisible() const;

    /**
     * Close the widget. When done, the widget can be safely deleted.
     */
    virtual void close();

    /**
     * Realize the widget. This method blocks until the GUI thread created the widget.
     *
     * \param abortCondition a condition enforcing abort of widget creation.
     */
    virtual void realize( boost::shared_ptr< WCondition > abortCondition );

    /**
     * Handle shutdown. This includes notification of the creating module and closing the widget. This method must be called from within the GUI
     * thread.
     */
    void guiShutDown();

protected:
    /**
     * Realize the widget. This method blocks until the GUI thread created the widget. Called from within the GUI thread! So you can safely do Qt
     * stuff.
     */
    virtual void realizeImpl() = 0;

    /**
     * Show this widget if not yet visible. Called in GUI Thread (GT).
     */
    virtual void showGT();

    /**
     * Hide/show this widget. Unlike close(), you can show the widget again using show(). Called in GUI Thread (GT).
     *
     * \param visible false to hide widget
     */
    virtual void setVisibleGT( bool visible = true );

    /**
     * Check if the widget is hidden or not. Called in GUI Thread (GT).
     *
     * \return true if visible.
     */
    virtual bool isVisibleGT() const;

    /**
     * Close the widget. When done, the widget can be safely deleted. Called in GUI Thread (GT).
     */
    virtual void closeGT();

    /**
     * Called directly before close in the GUI thread.
     */
    virtual void onClose();

    /**
     * Clean up all the memory in Gui Thread.
     */
    virtual void cleanUpGT() = 0;

    /**
     * The main window instance.
     */
    WMainWindow* m_mainWindow;

    /**
     * The widget representing this abstract UI element.
     */
    QWidget* m_widget;

private:
    /**
     * Forwards call from a boost function to the virtual realizeImpl method
     */
    void realizeGT();
};

#endif  // WQTWIDGETBASE_H

