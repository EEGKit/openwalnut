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
#include <vector>

#include "WMFilterProtonData.h"

W_LOADABLE_MODULE( WMFilterProtonData )

WMFilterProtonData::WMFilterProtonData():
        WModule()
{
}

WMFilterProtonData::~WMFilterProtonData()
{
}

const std::string WMFilterProtonData::getName() const
{
    return "FilterProtonData";
}

const std::string WMFilterProtonData::getDescription() const
{
    return "Converter CSV-data To WDataSetPoints and WDataSetFibers";
}

boost::shared_ptr< WModule > WMFilterProtonData::factory() const
{
    return boost::shared_ptr< WModule >( new WMFilterProtonData() );
}

const char** WMFilterProtonData::getXPMIcon() const
{
    return NULL;
}

void WMFilterProtonData::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );

    ready();

    m_propertyStatus = boost::shared_ptr< WMPropertyStatus >( new WMPropertyStatus() );

    m_propertyStatus->setFilteringGroup( m_properties->addPropertyGroup( "Filtering", "Filter primaries and secondaries" ) );
    m_propertyStatus->setVisualizationGroup( m_properties->addPropertyGroup( "Visualization", "Visualization options" ) );
    
    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if( m_shutdownFlag() )
        {
            break;
        }

        m_dataset = m_input->getData();

        WDataSetCSV::ContentSPtr csvHeader = m_dataset->getHeader();
        WDataSetCSV::ContentSPtr csvData = m_dataset->getData();

        if( m_protonData == NULL )
        {
            m_protonData = WMProtonData::SPtr( new WMProtonData( csvHeader, csvData ) );
            m_propertyStatus->setColumnPropertyHandler(WMColumnPropertyHandler::SPtr( new WMColumnPropertyHandler( m_protonData, m_properties,
                boost::bind( &WMFilterProtonData::setOutputFromCSV, this ) ) ) );
        }
        else
        {
            m_protonData->setCSVHeader( csvHeader );
            m_protonData->setCSVData( csvData );
        }

        properties();

        m_propertyStatus->getColumnPropertyHandler()->createProperties();
        setOutputFromCSV( );
    }
}

void WMFilterProtonData::connectors()
{
    m_input = WModuleInputData< WDataSetCSV >::createAndAdd( shared_from_this(), "in", "CSV_input" );

    m_output_points = boost::shared_ptr< WModuleOutputData< WDataSet > >(
            new WModuleOutputData< WDataSet >(
                    shared_from_this(),
                    "output points",
                    "Output CSV data as Point data" )
    );

    m_output_fibers = boost::shared_ptr< WModuleOutputData< WDataSet > >(
            new WModuleOutputData< WDataSet >(
                    shared_from_this(),
                    "output fibers",
                    "Output CSV data as Fiber data" )
    );

    addConnector( m_output_points );
    addConnector( m_output_fibers );

    WModule::connectors();
}

void WMFilterProtonData::properties()
{
    

    if( m_dataset == NULL )
    {
        return;
    }

    WPropertyBase::PropertyChangeNotifierType notifierCheckBox = boost::bind( &WMFilterProtonData::updateCheckboxProperty,
                                                                this, boost::placeholders::_1 );

    WPropertyBase::PropertyChangeNotifierType eventIDNotifier = boost::bind( &WMFilterProtonData::updateMesh,
                                                                this);
    
    WPropGroup eventIDGroup = m_properties->addPropertyGroup( "Event ID Limitation", "Adjust the range of eventIDs to show.", 0 );
    
    m_propertyStatus->setShowPrimaries( m_propertyStatus->getFilteringGroup()->addProperty( "Show primaries", "Show/hide primaries", true, notifierCheckBox ) );
    m_propertyStatus->setShowSecondaries( m_propertyStatus->getFilteringGroup()->addProperty( "Show secondaries", "Show/hide secondaries", true, notifierCheckBox ) );
    m_propertyStatus->setSizesFromEdep( m_propertyStatus->getVisualizationGroup()->addProperty( "Scale point size", "Scale point size with energy deposition", true, notifierCheckBox ) );
    m_propertyStatus->setColorFromEdep( m_propertyStatus->getVisualizationGroup()->addProperty( "Color by edep", "Color points based on energy deposition", true, notifierCheckBox ) );
    m_propertyStatus->setColorSelection( m_propertyStatus->getVisualizationGroup()->addProperty( "Plain color", "Choose how to color the points when not coloring by edep.", defaultColor::WHITE, notifierCheckBox ) );
    m_propertyStatus->setMinCap( eventIDGroup->addProperty( "Min Cap", "Set your minium border of your range.", 1, eventIDNotifier ) );
    m_propertyStatus->setMaxCap( eventIDGroup->addProperty( "Max Cap", "Set your maximum border of your range.", 2000, eventIDNotifier ) );

    WMFilterProtonData::determineMinMaxEventID();

    WModule::properties();
}

void WMFilterProtonData::setOutputFromCSV( )
{
    m_converter = boost::shared_ptr< WMCsvConverter >( new WMCsvConverter() ); 
    m_converter->setOutputFromCSV(m_protonData, m_propertyStatus);

    m_output_points->updateData( m_converter->getPoints() );
    m_output_fibers->updateData( m_converter->getFibers() );
}

void WMFilterProtonData::updateRangeOfEventIDSelection( int minCap, int maxCap )
{
    m_propertyStatus->getMinCap()->setMin( minCap );
    m_propertyStatus->getMinCap()->setMax( maxCap );
    m_propertyStatus->getMaxCap()->setMin( minCap );
    m_propertyStatus->getMaxCap()->setMax( maxCap );

    int currentMinCap = m_propertyStatus->getMinCap()->get();
    int currentMaxCap = m_propertyStatus->getMaxCap()->get();

    if( currentMaxCap < currentMinCap )
        m_propertyStatus->getMaxCap()->set( currentMinCap );

    if( currentMinCap < 0 )
        m_propertyStatus->getMinCap()->set( 0 );
}

void WMFilterProtonData::updateMesh( )
{
    WMFilterProtonData::determineMinMaxEventID();
    WMFilterProtonData::setOutputFromCSV( );
}

void WMFilterProtonData::determineMinMaxEventID()
{
    int eventIDIndex = m_protonData->getColumnIndex( "eventID" );

    WMFilterProtonData::updateRangeOfEventIDSelection(
        std::stoi( m_protonData->getCSVData()->front().at( eventIDIndex ) ),
        std::stoi( m_protonData->getCSVData()->back().at( eventIDIndex ) ) );
}



void WMFilterProtonData::updateCheckboxProperty( WPropertyBase::SPtr property )
{
    if( m_propertyStatus->getShowPrimaries()->get() || m_propertyStatus->getShowSecondaries()->get() )
    {
        setOutputFromCSV( );
    }
    else
    {
        if( property == m_propertyStatus->getShowPrimaries() )
        {
            m_propertyStatus->getShowPrimaries()->set( true );
        }

        if( property == m_propertyStatus->getShowSecondaries() )
        {
            m_propertyStatus->getShowSecondaries()->set( true );
        }
    }
}
