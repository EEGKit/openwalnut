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

#include "WProperty.h"

WProperty::WProperty( std::string name, std::string value, std::string shortDesc, std::string longDesc )
    : m_name( name ),
    m_value( value ),
    m_shortDesc( shortDesc ),
    m_longDesc( longDesc )
{
    m_type = P_STRING;
}

WProperty::WProperty( std::string name, bool value, std::string shortDesc, std::string longDesc )
    : m_name( name ),
    m_shortDesc( shortDesc ),
    m_longDesc( longDesc )
{
    m_type = P_BOOL;
    setValue( value );
}

WProperty::WProperty( std::string name, char value, std::string shortDesc, std::string longDesc )
    : m_name( name ),
    m_shortDesc( shortDesc ),
    m_longDesc( longDesc )
{
    m_type = P_CHAR;
    setValue( value );
}

WProperty::WProperty( std::string name, int value, std::string shortDesc, std::string longDesc )
    : m_name( name ),
    m_shortDesc( shortDesc ),
    m_longDesc( longDesc )
{
    m_type = P_INT;
    setValue( value );
}

WProperty::WProperty( std::string name, float value, std::string shortDesc, std::string longDesc )
    : m_name( name ),
    m_shortDesc( shortDesc ),
    m_longDesc( longDesc )
{
    m_type = P_FLOAT;
    setValue( value );
}

WProperty::WProperty( std::string name, double value, std::string shortDesc, std::string longDesc )
    : m_name( name ),
    m_shortDesc( shortDesc ),
    m_longDesc( longDesc )
{
    m_type = P_DOUBLE;
    setValue( value );
}


WProperty::~WProperty()
{
}


PropertyType WProperty::getType()
{
    return m_type;
}

std::string WProperty::getName()
{
    return m_name;
}

void WProperty::setShortDesc( const std::string desc )
{
    m_shortDesc = desc;
}


void WProperty::setLongDesc( const std::string desc )
{
    m_longDesc = desc;
}


std::string WProperty::getShortDesc()
{
    return m_shortDesc;
}


std::string WProperty::getLongDesc()
{
    return m_longDesc;
}


std::string WProperty::getValueString()
{
    return m_value;
}
