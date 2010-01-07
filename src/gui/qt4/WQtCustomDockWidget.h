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

#ifndef WQTCUSTOMDOCKWIDGET_H
#define WQTCUSTOMDOCKWIDGET_H

#include <string>
#include <QtGui/QDockWidget>
#include "WQtGLWidget.h"

/**
 * Dock Widget which is created by a module to display custom information.
 */
class WQtCustomDockWidget : public QDockWidget
{
public:
    /**
     * constructor
     *
     * \param title the title of the widget
     * \param parent the parent of the widget
     * \param projectionMode The kind of projection which should be used
     */
    WQtCustomDockWidget( std::string title, QWidget* parent, WGECamera::ProjectionMode projectionMode );

protected:
    /**
     * Event handler for close events
     *
     * \param event the event description.
     */
    virtual void closeEvent( QCloseEvent* event );

private:
    /**
     * the included GL widget
     */
    boost::shared_ptr<WQtGLWidget> m_glWidget;
};

#endif  // WQTCUSTOMDOCKWIDGET_H
