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
#include <sstream>
#include <string>
// #include <iostream>


#include "core/common/WLogger.h"
#include "core/common/WPropertyVariable.h"
#include "core/common/WTransferFunction2D.h"
#include "../WGuiConsts.h"

#include "WPropertyTransferFunction2DWidget.h"

WPropertyTransferFunction2DWidget::WPropertyTransferFunction2DWidget( WPropTransferFunction2D property, QGridLayout* propertyGrid, QWidget* parent ):
    WPropertyWidget( property, propertyGrid, parent ),
    m_transferFunctionProperty( property ),
    m_layout( &m_parameterWidgets ),
    m_infoLayout( &m_informationWidgets ),
    m_transferFunction( &m_parameterWidgets, this ),
    modifying( false )
{
    setAttribute( Qt::WA_DeleteOnClose );
    m_layout.addWidget( &m_transferFunction );
    m_layout.setMargin( WGLOBAL_MARGIN );
    m_layout.setSpacing( WGLOBAL_SPACING );

    m_parameterWidgets.setLayout( &m_layout );

    // Information Output ( Property Purpose = PV_PURPOSE_INFORMATION )
    m_infoLayout.setMargin( WGLOBAL_MARGIN );
    m_infoLayout.setSpacing( WGLOBAL_SPACING );
    m_informationWidgets.setLayout( &m_infoLayout );


    //connect the modification signal of the edit and slider with our callback
    //connect( &m_slider, SIGNAL( valueChanged( int ) ), this, SLOT( sliderChanged( int ) ) );
    //connect( &m_edit, SIGNAL( editingFinished() ), this, SLOT( editChanged() ) );
    //connect( &m_edit, SIGNAL( textEdited( const QString& ) ), this, SLOT( textEdited( const QString& ) ) );
}

WPropertyTransferFunction2DWidget::~WPropertyTransferFunction2DWidget()
{
    // cleanup
    // std::cout << "cleanup." << std::endl;
}

namespace
{
    QColor toQColor( const WColor &color )
    {
        QColor tmp;
        tmp.setRgbF( color[0],
                     color[1],
                     color[2],
                     color[3] );

        return tmp;
    }
}

// this function is more complex than it should be because of the way
// the widget handles mouse input which interferes with adding points the same way
// because points are updated immediately and I don't know whether and how I can
// block the update routine while still inserting points into the scene
void WPropertyTransferFunction2DWidget::update()
{
}

void WPropertyTransferFunction2DWidget::guiUpdate( const WTransferFunction2D& tf )
{
}

