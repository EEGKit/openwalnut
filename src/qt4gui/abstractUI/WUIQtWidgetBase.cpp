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

#include <boost/function.hpp>

#include "core/common/WLogger.h"

#include "../WQt4Gui.h"

#include "WUIQtWidgetBase.h"

WUIQtWidgetBase::WUIQtWidgetBase( WMainWindow* mainWindow, WUIQtWidgetBase::SPtr parent ):
    m_mainWindow( mainWindow ),
    m_widget( NULL ),
    m_parent( parent )
{
    // initialize members
}

WUIQtWidgetBase::~WUIQtWidgetBase()
{
    // cleanup
    if( m_widget )
    {
        delete m_widget;
    }
}

void WUIQtWidgetBase::realize( boost::shared_ptr< WCondition > abortCondition )
{
    WConditionSet conditionSet;
    conditionSet.setResetable( true, false );
    // there are several events we want to wait for:
    // 1) the caller of this function does not want to wait anymore:
    if( abortCondition )
    {
        conditionSet.add( abortCondition );
    }

    // 2) the execution call was done:
    WCondition::SPtr doneNotify( new WConditionOneShot() );
    conditionSet.add( doneNotify );

    // use the UI to do the GUI thread call
    WQt4Gui::execInGUIThreadAsync( boost::bind( &WUIQtWidgetBase::realizeGT, this ), doneNotify );

    // wait ...
    conditionSet.wait();
}

void WUIQtWidgetBase::realizeGT()
{
    // only top-level widgets will be registered
    if( !m_parent )
    {
        m_mainWindow->registerCustomWidget( this );
    }
    realizeImpl();
}

bool WUIQtWidgetBase::isReal()
{
    return ( m_widget );
}

void WUIQtWidgetBase::show()
{
    if( m_widget )
    {
        WQt4Gui::execInGUIThread( boost::bind( &WUIQtWidgetBase::showGT, this ) );
    }
}

void WUIQtWidgetBase::setVisible( bool visible )
{
    if( m_widget )
    {
        WQt4Gui::execInGUIThread( boost::bind( &WUIQtWidgetBase::setVisibleGT, this, visible ) );
    }
}

/**
 * Simple function to query the visibility in GUI thread.
 */
struct IsVisibleFunctor
{
    /**
     * Functor queries the visibility in GUI thread
     */
    void operator()()
    {
        m_return = m_widget->isVisible();
    }

    /**
     * The widget to query.
     */
    QWidget* m_widget;

    /**
     * The return value.
     */
    bool m_return;
};

bool WUIQtWidgetBase::isVisible() const
{
    if( m_widget )
    {
        IsVisibleFunctor f;
        f.m_widget = m_widget;
        WQt4Gui::execInGUIThread( f );
        return f.m_return;
    }
    return false;
}

void WUIQtWidgetBase::closeImpl()
{
    wlog::debug( "WUIQtWidgetBase" ) << "Close: \"" << getTitleQString().toStdString() << "\"";

    // move this to the GUI thread.
    if( m_widget )
    {
        WQt4Gui::execInGUIThread( boost::bind( &WUIQtWidgetBase::closeGT, this ) );
    }
}

void WUIQtWidgetBase::showGT()
{
    m_widget->show();
}

void WUIQtWidgetBase::setVisibleGT( bool visible )
{
    m_widget->setVisible( visible );
}

bool WUIQtWidgetBase::isVisibleGT() const
{
    return m_widget->isVisible();
}

void WUIQtWidgetBase::closeGT()
{
    m_mainWindow->deregisterCustomWidget( this );
    cleanUpGT();    // we do not delete the widget ourselfs ... let the bridge implementor do it.
    m_widget = NULL;
}

QWidget* WUIQtWidgetBase::getWidget() const
{
    return m_widget;
}

QWidget* WUIQtWidgetBase::getParentAsQtWidget() const
{
    if( getQtParent() )
    {
        return getQtParent()->getWidget();
    }
    else
    {
        return NULL;
    }
}

WUIQtWidgetBase::SPtr WUIQtWidgetBase::getQtParent() const
{
    return m_parent;
}