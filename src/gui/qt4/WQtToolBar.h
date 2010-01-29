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

#ifndef WQTTOOLBAR_H
#define WQTTOOLBAR_H

#include <QtGui/QToolBar>

#include "guiElements/WQtPushButton.h"

/**
 * This is a toolbar. Its main usage for now is the "compatible modules" toolbar
 */
class WQtToolBar : public QToolBar
{
public:
    /**
     * Constructs the toolbar.
     * \param title name of the toolbar.
     * \param parent the parent widget of this widget, i.e. the widget that manages it.
     */
    explicit WQtToolBar( const QString & title, QWidget* parent );

    /**
     * destructor
     */
    virtual ~WQtToolBar();

    /**
     * Adds a push button to the toolbar. The button is identifiable by its name.
     * \param name a name for the new button to identifie it
     * \param icon and icon for the button
     * \param label The optional text that is displayed besides the icon on the button.
     */
    WQtPushButton* addPushButton( QString name, QIcon icon, QString label = 0 );

    /**
     * Removes all buttons,
     */
    void clearButtons();

protected:
private:
};

#endif  // WQTTOOLBAR_H
