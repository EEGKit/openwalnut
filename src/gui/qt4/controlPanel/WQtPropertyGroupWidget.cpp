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

#include <QtGui/QGroupBox>
#include <QtGui/QPushButton>
#include <QtGui/QScrollArea>

#include "WQtPropertyGroupWidget.h"

WQtPropertyGroupWidget::WQtPropertyGroupWidget( std::string name, QWidget* parent  )
    : QWidget( parent ),
    m_name( name.c_str() ),
//    m_controlLayout(),
//    m_pageLayout(),
    m_numberOfWidgets( 0 )
{
    // note: never do layouts as none pointers
    // on destruction of a widget it will try to delete them which will cause crashes
    m_pageLayout = new QVBoxLayout();
    m_controlLayout = new QGridLayout();
    m_pageLayout->addLayout( m_controlLayout );
}

WQtPropertyGroupWidget::~WQtPropertyGroupWidget()
{
}

WPropertyBoolWidget* WQtPropertyGroupWidget::addProp( WPropBool property )
{
    ++m_numberOfWidgets;

    return new WPropertyBoolWidget( property, m_controlLayout, this );
}

WPropertyIntWidget* WQtPropertyGroupWidget::addProp( WPropInt property )
{
    ++m_numberOfWidgets;

    return new WPropertyIntWidget( property, m_controlLayout, this );
}

WPropertyDoubleWidget* WQtPropertyGroupWidget::addProp( WPropDouble property )
{
    ++m_numberOfWidgets;

    return new WPropertyDoubleWidget( property, m_controlLayout, this );
}

WPropertyStringWidget* WQtPropertyGroupWidget::addProp( WPropString property )
{
    ++m_numberOfWidgets;

    return new WPropertyStringWidget( property, m_controlLayout, this );
}

WPropertyColorWidget* WQtPropertyGroupWidget::addProp( WPropColor property )
{
    ++m_numberOfWidgets;

    return new WPropertyColorWidget( property, m_controlLayout, this );
}

WPropertyFilenameWidget* WQtPropertyGroupWidget::addProp( WPropFilename property )
{
    ++m_numberOfWidgets;

    return new WPropertyFilenameWidget( property, m_controlLayout, this );
}

WPropertyTriggerWidget* WQtPropertyGroupWidget::addProp( WPropTrigger property )
{
    ++m_numberOfWidgets;

    return new WPropertyTriggerWidget( property, m_controlLayout, this );
}

WPropertyPositionWidget* WQtPropertyGroupWidget::addProp( WPropPosition property )
{
    ++m_numberOfWidgets;

    return new WPropertyPositionWidget( property, m_controlLayout, this );
}

WPropertySelectionWidget* WQtPropertyGroupWidget::addProp( WPropSelection property )
{
    ++m_numberOfWidgets;

    return new WPropertySelectionWidget( property, m_controlLayout, this );
}

void WQtPropertyGroupWidget::addGroup( WQtPropertyGroupWidget* widget, bool asScrollArea )
{
    ++m_numberOfWidgets;

    // create a scrollbox and group box containing the widget
    QWidget* group = new QWidget( this );

    QScrollArea* scrollArea = 0;
    QGridLayout* grid = new QGridLayout();
    grid->addWidget( widget, 0, 0 );

    group->setLayout( grid );
    if ( asScrollArea )
    {
        scrollArea = new QScrollArea();
        scrollArea->setWidget( group );
        group->show();
    }

    // encapsulate it into an collapsable widget
    QFrame* box = new QFrame( this );
    box->setFrameShape( QFrame::StyledPanel );
    box->setFrameShadow( QFrame::Raised );
    QGridLayout* boxLayout = new QGridLayout( box );

    // create a button as title
    QPushButton* boxTitle = new QPushButton( this );
    boxLayout->addWidget( boxTitle, 0, 0 );

    // set the button up
    QSizePolicy sizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    sizePolicy.setHorizontalStretch( 0 );
    sizePolicy.setVerticalStretch( 0 );
    sizePolicy.setHeightForWidth( boxTitle->sizePolicy().hasHeightForWidth() );
    boxTitle->setSizePolicy( sizePolicy );
    boxTitle->setCheckable( true );
    boxTitle->setChecked( true );
    boxTitle->setFlat( true );
    QFont font;
    font.setBold( true );
    boxTitle->setFont( font );
    boxTitle->setText( widget->getName() );

    // toggle should cause the body widget to appear/disappear
    connect( boxTitle, SIGNAL( toggled( bool ) ), group, SLOT( setVisible( bool ) ) );

    // create a body widget
    if ( asScrollArea )
    {
        boxLayout->addWidget( scrollArea, 1, 0 );
    }
    else
    {
        boxLayout->addWidget( group, 1, 0 );
    }

    // insert into layout
    int row = m_controlLayout->rowCount();
    m_controlLayout->addWidget( box, row, 0, 1, 2 );
}

void WQtPropertyGroupWidget::addSpacer()
{
    m_pageLayout->addStretch();
    setLayout( m_pageLayout );
}

QString WQtPropertyGroupWidget::getName()
{
    return m_name;
}

void WQtPropertyGroupWidget::setName( QString name )
{
    m_name = name;
}

bool WQtPropertyGroupWidget::isEmpty() const
{
    return !m_numberOfWidgets;
}

