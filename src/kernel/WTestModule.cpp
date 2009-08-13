//---------------------------------------------------------------------------
//
// Project: OpenWalnut
//
// Copyright 2009 SomeCopyrightowner
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

#include "WTestModule.h"

WTestModule::WTestModule(): WModule()
{
    // initialize members
}

WTestModule::~WTestModule()
{
    // cleanup
}

WTestModule::WTestModule( const WTestModule& other )
{
    *this = other;
}

std::string WTestModule::getName() const
{
    return "Test Module";
}

std::string WTestModule::getDescription() const
{
    return "This module is for testing and development";
}

