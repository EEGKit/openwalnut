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

#include <sstream>
#include <cmath>
#include <string>

#include <boost/lexical_cast.hpp>

#include "../../../common/WLogger.h"
#include "../../../common/WPropertyVariable.h"

#include "WPropertyMatrix4X4Widget.h"

WPropertyMatrix4X4Widget::WPropertyMatrix4X4Widget( WPropMatrix4X4 property, QGridLayout* propertyGrid, QWidget* parent ):
    WPropertyWidget( property, propertyGrid, parent ),
    m_matrixProperty( property ),
    m_layout( &m_parameterWidgets ),
    m_asText( &m_informationWidgets ),
    m_infoLayout( &m_informationWidgets )
{
    // initialize members
    /*m_editX.resize( m_editX.minimumSizeHint().width() * 2.0, m_editX.size().height() );
    //m_editX.setMaximumWidth( m_editX.minimumSizeHint().width() * 5.0 );
    m_editY.resize( m_editY.minimumSizeHint().width() * 2.0, m_editY.size().height() );
    //m_editY.setMaximumWidth( m_editY.minimumSizeHint().width() * 5.0 );
    m_editZ.resize( m_editZ.minimumSizeHint().width() * 2.0, m_editZ.size().height() );
    //m_editZ.setMaximumWidth( m_editZ.minimumSizeHint().width() * 5.0 );

    // layout both against each other
    m_layout.addWidget( &m_editX );
    m_layout.addWidget( &m_editY );
    m_layout.addWidget( &m_editZ );

    m_parameterWidgets.setLayout( &m_layout );

    // Information Output ( Property Purpose = PV_PURPOSE_INFORMATION )
    m_infoLayout.addWidget( &m_asText );
    m_informationWidgets.setLayout( &m_infoLayout );

    update();

    // connect the modification signal of the edit and slider with our callback
    connect( &m_editX, SIGNAL( returnPressed() ), this, SLOT( editChanged() ) );
    connect( &m_editX, SIGNAL( textEdited( const QString& ) ), this, SLOT( textEdited( const QString& ) ) );
    connect( &m_editY, SIGNAL( returnPressed() ), this, SLOT( editChanged() ) );
    connect( &m_editY, SIGNAL( textEdited( const QString& ) ), this, SLOT( textEdited( const QString& ) ) );
    connect( &m_editZ, SIGNAL( returnPressed() ), this, SLOT( editChanged() ) );
    connect( &m_editZ, SIGNAL( textEdited( const QString& ) ), this, SLOT( textEdited( const QString& ) ) );*/
}

WPropertyMatrix4X4Widget::~WPropertyMatrix4X4Widget()
{
    // cleanup
}

void WPropertyMatrix4X4Widget::update()
{
    // set the values
    /*m_editX.setText( QString::fromStdString( toString( m_matProperty->get()[0] ) ) );
    m_editY.setText( QString::fromStdString( toString( m_matProperty->get()[1] ) ) );
    m_editZ.setText( QString::fromStdString( toString( m_matProperty->get()[2] ) ) );

    // do not forget to update the label
    m_asText.setText( QString::fromStdString( toString( m_matProperty->get() ) ) );*/
}

void WPropertyMatrix4X4Widget::editChanged()
{
    //setPropertyFromWidgets();
}

void WPropertyMatrix4X4Widget::textEdited( const QString& /*text*/ )
{
    //setPropertyFromWidgets( true );
}

void WPropertyMatrix4X4Widget::setPropertyFromWidgets( bool validateOnly )
{
/*    // grab all the values
    bool valid;
    double valueX = m_editX.text().toDouble( &valid );
    if ( !valid )
    {
        invalidate();
        return;
    }
    double valueY = m_editY.text().toDouble( &valid );
    if ( !valid )
    {
        invalidate();
        return;
    }
    double valueZ = m_editZ.text().toDouble( &valid );
    if ( !valid )
    {
        invalidate();
        return;
    }

    // create a new position
    wmath::WPosition p = wmath::WPosition( valueX, valueY, valueZ );

    // set/validate to the property
    if ( validateOnly )
    {
        invalidate( !m_posProperty->accept( p ) );
    }
    else
    {
        invalidate( !m_posProperty->set( p ) );    // NOTE: set automatically checks the validity of the value
    }*/
}

