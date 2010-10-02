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

#include <cmath>
#include <string>

#include <boost/lexical_cast.hpp>

#include "../../../common/WLogger.h"
#include "../../../common/WPropertyVariable.h"

#include "WPropertyStringWidget.h"

WPropertyStringWidget::WPropertyStringWidget( WPropString property, QGridLayout* propertyGrid, QWidget* parent ):
    WPropertyWidget( property, propertyGrid, parent ),
    m_stringProperty( property ),
    m_edit( &m_parameterWidgets ),
    m_layout( &m_parameterWidgets ),
    m_asText( &m_informationWidgets ),
    m_infoLayout( &m_informationWidgets )
{
    // initialize members
    m_parameterWidgets.setLayout( &m_layout );

    // layout
    m_layout.addWidget( &m_edit );

    // Information Output ( Property Purpose = PV_PURPOSE_INFORMATION )
    m_infoLayout.addWidget( &m_asText );
    m_informationWidgets.setLayout( &m_infoLayout );
    m_asText.setWordWrap( true );
    // To have word warp work correctly -> set size policy
    m_asText.setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );

    // set the initial values
    update();

    // connect the modification signal of the edit and slider with our callback
    connect( &m_edit, SIGNAL( returnPressed() ), this, SLOT( editChanged() ) );
    connect( &m_edit, SIGNAL( textEdited( const QString& ) ), this, SLOT( textEdited( const QString& ) ) );
}

WPropertyStringWidget::~WPropertyStringWidget()
{
    // cleanup
}

void WPropertyStringWidget::update()
{
    QString val = QString::fromStdString( m_stringProperty->get() );
    m_edit.setText( val );
    m_asText.setText( val );
}

void WPropertyStringWidget::editChanged()
{
    std::string value = m_edit.text().toStdString();
    // now: is the value acceptable by the property?
    invalidate( !m_stringProperty->set( value ) );     // NOTE: set automatically checks the validity of the value
}

void WPropertyStringWidget::textEdited( const QString& text )
{
    // this method does NOT set the property actually, but tries to validate it
    std::string value = text.toStdString();
    invalidate( !m_stringProperty->accept( value ) );
}


