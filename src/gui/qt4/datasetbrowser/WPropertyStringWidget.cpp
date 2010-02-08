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

#include <iostream>
#include <cmath>

#include <boost/lexical_cast.hpp>

#include "../../../common/WLogger.h"
#include "../../../common/WPropertyVariable.h"

#include "WPropertyStringWidget.h"

WPropertyStringWidget::WPropertyStringWidget( WPropString property, QGridLayout* propertyGrid, QWidget* parent ):
    WPropertyWidget( property, propertyGrid, parent ),
    m_stringProperty( property ),
    m_edit( this ),
    m_layout()
{
    // initialize members
    setLayout( &m_layout );

    // layout both against each other
    m_layout.addWidget( &m_edit );

    // set the initial values
    m_edit.setText( QString( m_stringProperty->get().c_str() ) );

    // connect the modification signal of the edit and slider with our callback
    connect( &m_edit, SIGNAL( returnPressed() ), this, SLOT( editChanged() ) );
}

WPropertyStringWidget::~WPropertyStringWidget()
{
    // cleanup
}

void WPropertyStringWidget::editChanged()
{
    std::string value = m_edit.text().toStdString();
    // now: is the value acceptable by the property?
    if ( !m_stringProperty->accept( value ) )
    {
        invalidate();
    }
    else
    {
        invalidate( false );
        m_stringProperty->set( value );
    }
}

