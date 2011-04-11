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
#include <iostream>

#include <QtGui/QDockWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QApplication>

#include "../../graphicsEngine/WGECamera.h"
#include "../../graphicsEngine/WGEViewer.h"
#include "../../graphicsEngine/WGEScene.h"
#include "events/WPropertyChangedEvent.h"
#include "events/WEventTypes.h"

#include "WQtNavGLWidget.h"

WQtNavGLWidget::WQtNavGLWidget( QString viewTitle, QString dockTitle, QWidget* parent, std::string sliderTitle, const QGLWidget * shareWidget )
    : WQtGLDockWidget( viewTitle, dockTitle, parent, WGECamera::ORTHOGRAPHIC, shareWidget )
{
    propertyWidgetMap.clear();
    m_sliderTitle = QString( sliderTitle.c_str() );

    m_scene = new WGEScene();
    m_scene->setDataVariance( osg::Object::DYNAMIC );
    getGLWidget()->getViewer()->getView()->requestContinuousUpdate( false );
    getGLWidget()->getViewer()->setScene( m_scene );
}

WQtNavGLWidget::~WQtNavGLWidget()
{
    for( std::map< boost::shared_ptr< WPropertyBase >, WPropertyIntWidget* >::iterator it = propertyWidgetMap.begin();
         it != propertyWidgetMap.end();
         ++it )
    {
        if( it->second != 0 )
        {
            delete it->second;
        }
    }
}

void WQtNavGLWidget::setSliderTitle( std::string title )
{
    m_sliderTitle = QString( title.c_str() );
}

void WQtNavGLWidget::setSliderProperty( boost::shared_ptr< WPropertyBase > prop )
{
    WPropertyIntWidget* propWidget;
    propWidget = new WPropertyIntWidget( prop->toPropInt(), NULL, parentWidget() );
    propertyWidgetMap[prop] = propWidget;
    m_layout->addWidget( propWidget );
    m_layout->setStretchFactor( getGLWidget().get(), 1 );
    m_layout->setStretchFactor( propWidget, 0 );
}

void WQtNavGLWidget::removeSliderProperty( boost::shared_ptr< WPropertyBase > prop )
{
    WPropertyIntWidget* propWidget = propertyWidgetMap[prop];
    m_layout->removeWidget( propWidget );
    delete propWidget;
    propertyWidgetMap.erase( prop );
}

