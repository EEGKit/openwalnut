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

#include "WQtDSBWidget.h"

WQtDSBWidget::WQtDSBWidget( std::string name, QWidget* parent  )
    : QWidget( parent ),
    m_name( name.c_str() ),
    m_controlLayout(),
    m_pageLayout()
{
    m_pageLayout.addLayout( &m_controlLayout );
}

WQtDSBWidget::~WQtDSBWidget()
{
}

WPropertyBoolWidget* WQtDSBWidget::addPropBool( WPropBool property )
{
    return new WPropertyBoolWidget( property, &m_controlLayout, this );
}

WPropertyIntWidget* WQtDSBWidget::addPropInt( WPropInt property )
{
    return new WPropertyIntWidget( property, &m_controlLayout, this );
}

WPropertyIntWidget* WQtDSBWidget::addPropDouble( WPropDouble property )
{
    return NULL;
}

WPropertyIntWidget* WQtDSBWidget::addPropString( WPropString property )
{
    return NULL;
}

/*
QPushButton* WQtDSBWidget::addPushButton( QString label )
{
    int row = m_controlLayout.rowCount();

    QPushButton* button = new QPushButton();
    button->setText( label );

    m_controlLayout.addWidget( button, row, 0 );

    setLayout( &m_pageLayout );
    return button;
}
*/

void WQtDSBWidget::addSpacer()
{
    m_pageLayout.addStretch();
    setLayout( &m_pageLayout );
}

QString WQtDSBWidget::getName()
{
    return m_name;
}
