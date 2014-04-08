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

#include <QtGui/QApplication>

#include "../WMainWindow.h"
#include "WQtViewWidget.h"
#include "WQtGridWidget.h"

#include "WQtWidgetFactory.h"

WQtWidgetFactory::WQtWidgetFactory( WMainWindow* mainWindow ):
    WUIWidgetFactory(),
    m_mainWindow( mainWindow )
{
    // initialize members
}

WQtWidgetFactory::~WQtWidgetFactory()
{
    // cleanup
}

WQtWidgetBase::SPtr getAsQtParent( WUIWidgetBase::SPtr parent )
{
    WQtWidgetBase::SPtr w = boost::dynamic_pointer_cast< WQtWidgetBase >( parent );
    return w;
}

WUIGridWidget::SPtr WQtWidgetFactory::createGridWidget( const std::string& title, WUIWidgetBase::SPtr parent ) const
{
    WQtGridWidget::SPtr widget( new WQtGridWidget( title, m_mainWindow, getAsQtParent( parent ) ) );
    WUIWidgetFactory::setParent( widget, parent );    // NOTE: this is the parent on the WUI side
    widget->realize();
    return widget;
}

WUIViewWidget::SPtr WQtWidgetFactory::createViewWidget(
    std::string title,
    WGECamera::ProjectionMode projectionMode,
    boost::shared_ptr< WCondition > abortCondition,
    WUIWidgetBase::SPtr parent ) const
{
    WQtViewWidget::SPtr widget( new WQtViewWidget( title, projectionMode, m_mainWindow, getAsQtParent( parent ) ) );
    WUIWidgetFactory::setParent( widget, parent );    // NOTE: this is the parent on the WUI side
    widget->realize( abortCondition );
    return widget;
}
