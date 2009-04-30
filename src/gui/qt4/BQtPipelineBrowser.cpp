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

#include "BQtPipelineBrowser.h"

BQtPipelineBrowser::BQtPipelineBrowser()
    : QDockWidget( "Pipeline Browser" ),
      m_recommendedSize()
{
    setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
    m_recommendedSize.setWidth( 200 );
    m_recommendedSize.setHeight( 300 );
}

BQtPipelineBrowser::~BQtPipelineBrowser()
{
    // TODO(wiebel) Auto-generated destructor stub
}

QSize BQtPipelineBrowser::sizeHint() const
{
    return m_recommendedSize;
}
