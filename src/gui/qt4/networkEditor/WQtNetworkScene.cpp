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
#include <iostream>

#include <boost/shared_ptr.hpp>

#include <QtGui/QDockWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QGraphicsView>
#include <QtGui/QGraphicsItemGroup>
#include <QtGui/QGraphicsSceneMouseEvent>

#include "WQtNetworkScene.h"
#include "WQtNetworkItem.h"
#include "WQtNetworkPort.h"

WQtNetworkScene::WQtNetworkScene()
    : QGraphicsScene()
{
}

WQtNetworkScene::~WQtNetworkScene()
{
}

void WQtNetworkScene::mousePressEvent( QGraphicsSceneMouseEvent *mouseEvent )
{
    QList<QGraphicsItem *> item = this->items( mouseEvent->scenePos() );
    if( item.isEmpty() )
    {
        emit selectionChanged();
    }

    QGraphicsScene::mousePressEvent( mouseEvent );
}

void WQtNetworkScene::setFakeItem( QGraphicsItem *fake )
{
    m_fakeItem = fake;
}

QGraphicsItem* WQtNetworkScene::getFakeItem()
{
    return m_fakeItem;
}
