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
// these lines where created by a python script, do not edit
#include "_OWmodule_clusterDisplay_PCH.h"

#include <string>

#include "WOSGButtonLabel.h"

#include "WOSGButton.h"

WOSGButton::WOSGButton( std::string name, osgWidget::Box::BoxType type, bool resize_hint, bool pushable ) :
    osgWidget::Box( name, type, resize_hint )
{
    getBackground()->setColor( 0.8f, 0.8f, 0.8f, 0.8f );

    m_label = new WOSGButtonLabel( pushable );
    m_label->setLabel( name );
    m_label->setName( std::string( "Button_" ) + name );
    addWidget( m_label );
}

WOSGButton::~WOSGButton()
{
}

void WOSGButton::setLabel( std::string label )
{
    m_label->setLabel( label );
}

void WOSGButton::setId( size_t id )
{
    m_id = id;
}

void WOSGButton::setBackgroundColor( WColor color )
{
    m_label->setColor( color.getRed(), color.getGreen(), color.getBlue(), 1.0f );
}
