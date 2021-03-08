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

#include "core/common/WTransferFunction.h"

#include "WMVisualizationPropertyHandler.h"


WMVisualizationPropertyHandler::WMVisualizationPropertyHandler( WMProtonData::SPtr protonData,
                                            WPropertyGroup::SPtr properties,
                                            WMVisualizationPropertyHandler::CallbackPtr dataUpdate ):
    m_protonData( protonData ),
    m_properties( properties ),
    m_dataUpdate( dataUpdate )
{
}

void WMVisualizationPropertyHandler::createProperties()
{
    m_visualizationGroup = m_properties->addPropertyGroup( "Visualization", "Visualization options" );

    WPropertyBase::PropertyChangeNotifierType notifierCheckBox = boost::bind(
        &WMVisualizationPropertyHandler::updateCheckboxProperty, this, boost::placeholders::_1 );

    m_sizesFromEdep = m_visualizationGroup->addProperty( "Scale point size", "Scale point size with energy deposition", true, notifierCheckBox );
    m_colorFromEdep = m_visualizationGroup->addProperty( "Color by edep", "Color points based on energy deposition", true, notifierCheckBox );
    m_colorSelection = m_visualizationGroup->addProperty( "Plain color", "Choose how to color the points when not coloring by edep.",
        defaultColor::WHITE, notifierCheckBox );

    m_gradient = m_visualizationGroup->addProperty( "Transfer Function", "Transfer function that maps the energy deposition to a gradient",
                                                    setColorGradient() , false );

    m_applyGradient = m_visualizationGroup->addProperty( "Set gradient", "Apply", WPVBaseTypes::PV_TRIGGER_READY, notifierCheckBox );

    updateProperty();
}

void WMVisualizationPropertyHandler::updateProperty()
{
    if( m_protonData->isColumnAvailable( "posX" ) &&
        m_protonData->isColumnAvailable( "posY" ) &&
        m_protonData->isColumnAvailable( "posZ" ) &&
        m_protonData->isColumnAvailable( "edep" ) )
    {
        m_sizesFromEdep->setHidden( false );
        m_colorFromEdep->setHidden( false );
        m_colorSelection->setHidden( false );
        m_gradient->setHidden( false );
    }
    else
    {
        m_sizesFromEdep->setHidden( true );
        m_colorFromEdep->setHidden( true );
        m_colorSelection->setHidden( true );
        m_gradient->setHidden( true );
    }
}

void WMVisualizationPropertyHandler::updateCheckboxProperty( WPropertyBase::SPtr property )
{
    m_dataUpdate();

    if( property == m_applyGradient && m_applyGradient->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
    {
        m_applyGradient->set( WPVBaseTypes::PV_TRIGGER_READY, false );
    }
}

WPropBool WMVisualizationPropertyHandler::getColorFromEdep()
{
    return m_colorFromEdep;
}

WPropBool WMVisualizationPropertyHandler::getSizesFromEdep()
{
    return m_sizesFromEdep;
}

WPropColor WMVisualizationPropertyHandler::getColorSelection()
{
    return m_colorSelection;
}

WPropTrigger WMVisualizationPropertyHandler::getApplyGradientTrigger()
{
    return m_applyGradient;
}

WPropTransferFunction WMVisualizationPropertyHandler::getTransferFunction()
{
    return m_gradient;
}

WTransferFunction WMVisualizationPropertyHandler::setColorGradient()
{
    WTransferFunction tf;

    tf.addColor( 0.0, WColor( 0.0, 0.0, 1.0, 1.0 ) );
    tf.addColor( 0.25, WColor( 0.5, 0.0, 0.91, 1.0 ) );
    tf.addColor( 0.5, WColor( 0.75, 0.0, 0.73, 1.0 ) );
    tf.addColor( 0.75, WColor( 0.97, 0.0, 0.53, 1.0 ) );
    tf.addColor( 1.0, WColor( 1.0, 0.0, 0.0, 1.0 ) );

    tf.addAlpha( 0.0, 1.0 );
    tf.addAlpha( 0.25, 1.0 );
    tf.addAlpha( 0.5, 1.0 );
    tf.addAlpha( 0.75, 1.0 );
    tf.addAlpha( 1.0, 1.0 );

    return tf;
}
