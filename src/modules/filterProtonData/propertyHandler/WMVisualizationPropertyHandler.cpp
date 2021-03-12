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

    WPropertyBase::PropertyChangeNotifierType notifier = boost::bind(
        &WMVisualizationPropertyHandler::propertyCallback, this, boost::placeholders::_1 );

    m_sizesFromEdep = m_visualizationGroup->addProperty( "Size by energy deposition", "Scale track and point sizes based on energy deposition.",
                                                    true, notifier );
    m_colorFromEdep = m_visualizationGroup->addProperty( "Color by energy deposition", "Colorize tracks and points based on energy deposition.",
                                                    true, notifier );
    m_colorSelection = m_visualizationGroup->addProperty( "Point color", "Points colorized in the chosen color when "
                                                    "\"Color by energy deposition\" is disabled.",
        defaultColor::WHITE, notifier );

    m_gradient = m_visualizationGroup->addProperty( "Gradient color", "Colorize tracks and points based on energy deposition "
                                                    "with the configured gradient.", setColorGradient() , false );

    m_applyGradient = m_visualizationGroup->addProperty( "Set gradient", "Apply", WPVBaseTypes::PV_TRIGGER_READY, notifier );

    updateProperty();
}

void WMVisualizationPropertyHandler::updateProperty()
{
    if( m_protonData->isColumnAvailable( "X" ) &&
        m_protonData->isColumnAvailable( "Y" ) &&
        m_protonData->isColumnAvailable( "Z" ) &&
        m_protonData->isColumnAvailable( "Energy deposition" ) )
    {
        m_sizesFromEdep->setHidden( false );
        m_colorFromEdep->setHidden( false );
        toggleColorProperties();
    }
    else
    {
        m_sizesFromEdep->setHidden( true );
        m_colorFromEdep->setHidden( true );
        m_gradient->setHidden( true );
        m_applyGradient->setHidden( true );
    }
}

void WMVisualizationPropertyHandler::toggleColorProperties()
{
    if( m_colorFromEdep->get() )
    {
        m_gradient->setHidden( false );
        m_applyGradient->setHidden( false );
        m_colorSelection->setHidden( true );
    }
    else
    {
        m_gradient->setHidden( true );
        m_applyGradient->setHidden( true );
        m_colorSelection->setHidden( false );
    }
}

void WMVisualizationPropertyHandler::propertyCallback( WPropertyBase::SPtr property )
{
    m_dataUpdate();

    if( property == m_applyGradient && m_applyGradient->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
    {
        m_applyGradient->set( WPVBaseTypes::PV_TRIGGER_READY, false );
    }
    toggleColorProperties();
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
