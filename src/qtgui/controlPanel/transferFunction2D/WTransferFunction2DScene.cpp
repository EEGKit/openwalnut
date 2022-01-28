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

#include <vector>

#include <QtCore/QMimeData>
#include <QtCore/QUrl>
#include <QGraphicsSceneDragDropEvent>

#include "core/common/WTransferFunction.h"
#include "WTransferFunction2DScene.h"
#include "WTransferFunction2DWidget.h"

WTransferFunction2DScene::WTransferFunction2DScene( QObject*parent )
        : BaseClass( parent ),
          myparent( dynamic_cast<WTransferFunction2DWidget*>( parent ) )
{
    setBackgroundBrush( QBrush( Qt::white, Qt::CrossPattern ) ); //Qt::white );
}

WTransferFunction2DScene::~WTransferFunction2DScene()
{
}

void WTransferFunction2DScene::dragMoveEvent( QGraphicsSceneDragDropEvent* event )
{
}

namespace
{
    inline QColor toQColor( const WColor &color )
    {
        QColor tmp;
        tmp.setRgbF( color[0],
                     color[1],
                     color[2],
                     color[3] );

        return tmp;
    }
}


void do_update( const WTransferFunction& tf, WTransferFunction2DWidget* m_transferFunction )
{
}


void WTransferFunction2DScene::dropEvent( QGraphicsSceneDragDropEvent* event )
{
}

