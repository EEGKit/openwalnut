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

#include "WDataSetCSV.h"


WDataSetCSV::WDataSetCSV( WDataSetCSV::ContentSPtr header, WDataSetCSV::ContentSPtr data )
    : m_header( header ), m_data( data )
{
}

WDataSetCSV::WDataSetCSV()
{
}

WDataSetCSV::~WDataSetCSV()
{
}

WDataSetCSV::ContentSPtr WDataSetCSV::getHeader()
{
    return m_header;
}

WDataSetCSV::ContentSPtr WDataSetCSV::getData()
{
    return m_data;
}

WDataSetCSV::SeperatedRowSPtr WDataSetCSV::getRawDataSet()
{
    return rawDataSet;
}

void WDataSetCSV::setRawDataSet( WDataSetCSV::SeperatedRowSPtr rawDataSetIn )
{
    rawDataSet = rawDataSetIn;
}
