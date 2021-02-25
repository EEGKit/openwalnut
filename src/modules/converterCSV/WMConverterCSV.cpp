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

#include "WMConverterCSV.h"

W_LOADABLE_MODULE( WMConverterCSV )

WMConverterCSV::WMConverterCSV():
        WModule()
{
}

WMConverterCSV::~WMConverterCSV()
{
}

const std::string WMConverterCSV::getName() const
{
    return "FilterProtonData";
}

const std::string WMConverterCSV::getDescription() const
{
    return "Converter CSV-data To WDataSetPoints and WDataSetFibers";
}

boost::shared_ptr< WModule > WMConverterCSV::factory() const
{
    return boost::shared_ptr< WModule >( new WMConverterCSV() );
}

const char** WMConverterCSV::getXPMIcon() const
{
    return NULL;
}

void WMConverterCSV::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );

    ready();

    waitRestored();

    m_filteringGroup = m_properties->addPropertyGroup( "Filtering", "Filter primaries and secondaries" );
    m_visualizationGroup = m_properties->addPropertyGroup( "Visualization", "Visualization options" );

    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        m_dataset = m_input->getData();

        WDataSetCSV::ContentSPtr csvHeader = m_dataset->getHeader();
        WDataSetCSV::ContentSPtr csvData = m_dataset->getData();

        if( m_protonData == NULL )
        {
            m_protonData = WMProtonData::SPtr( new WMProtonData( csvHeader, csvData ) );
            m_columnPropertyHandler = WMColumnPropertyHandler::SPtr(
                new WMColumnPropertyHandler(
                    m_protonData,
                    m_properties,
                    boost::bind(
                        &WMConverterCSV::setPointsOutOfCSVData,
                        this,
                        boost::placeholders::_1,
                        boost::placeholders::_2
                    )
                )
            );
        }
        else
        {
            m_protonData->setCSVHeader( csvHeader );
            m_protonData->setCSVData( csvData );
        }

        m_columnPropertyHandler->searchPDGTypes();

        properties();
        m_columnPropertyHandler->createProperties();

        setPointsOutOfCSVData( m_protonData->getCSVHeader(), m_protonData->getCSVData() );
        setFibersOutOfCSVData( m_protonData->getCSVHeader(), m_protonData->getCSVData() );
    }
}

void WMConverterCSV::connectors()
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

void WMConverterCSV::properties()
{

    WPropertyBase::PropertyChangeNotifierType notifierCheckBox = boost::bind( &WMConverterCSV::updateCheckboxProperty,
                                                                this, boost::placeholders::_1 );

    WPropertyBase::PropertyChangeNotifierType eventIDNotifier = boost::bind( &WMConverterCSV::updateMesh,
                                                                this, boost::placeholders::_1 );

    

    if( m_dataset != NULL )
    {
        m_showPrimaries = m_filteringGroup->addProperty( "Show primaries", "Show/hide primaries", true, notifierCheckBox );
        m_showSecondaries = m_filteringGroup->addProperty( "Show secondaries", "Show/hide secondaries", true, notifierCheckBox );

        m_sizesFromEdep = m_visualizationGroup->addProperty( "Scale point size", "Scale point size with energy deposition", true, notifierCheckBox );
        m_colorFromEdep = m_visualizationGroup->addProperty( "Color by edep", "Color points based on energy deposition", true, notifierCheckBox );

        m_colorSelection = m_visualizationGroup->addProperty( "Plain color", "Choose how to color the points when not coloring by edep.",
                                                              defaultColor::WHITE, notifierCheckBox );

        WPropGroup m_eventIDGroup = m_properties->addPropertyGroup( "Event ID Limitation", "Adjust the range of eventIDs to show.", 0 );
        m_minCap = m_eventIDGroup->addProperty( "Min Cap", "Set your minium border of your range.", 1, eventIDNotifier );
        m_maxCap = m_eventIDGroup->addProperty( "Max Cap", "Set your maximum border of your range.", 2000, eventIDNotifier );
    
        WMConverterCSV::determineMinMaxEventID();
    }

    WModule::properties();
}

void WMConverterCSV::setFibersOutOfCSVData( WDataSetCSV::ContentSPtr header, WDataSetCSV::ContentSPtr data )
{
    if( !m_protonData->columnsInitialized() )
    {
        return;
    }

    SPFloatVector m_vertices = SPFloatVector( new std::vector< float >() );
    SPSizeVector m_lineStartIndexes = SPSizeVector( new std::vector < size_t >() );
    SPSizeVector m_lineLengths  = SPSizeVector( new std::vector < size_t >() );
    SPSizeVector m_verticesReverse = SPSizeVector( new std::vector < size_t >() );

    SPFloatVector m_colors = SPFloatVector( new std::vector< float >() );

    std::vector< int > eventIDs;
    std::vector< float > edeps;

    float maxEdep = 0.0;
    int parentIDIndex = m_protonData->getColumnIndex( "parentID" );
    int posXIndex = m_protonData->getColumnIndex( "posX" );
    int posYIndex = m_protonData->getColumnIndex( "posY" );
    int posZIndex = m_protonData->getColumnIndex( "posZ" );
    int edepIndex = m_protonData->getColumnIndex( "edep" );
    int PDGEncodingIndex = m_protonData->getColumnIndex( "PDGEncoding" );
    int eventIDIndex = m_protonData->getColumnIndex( "eventID" );

    for( WDataSetCSV::Content::iterator dataRow = data->begin(); dataRow < data->end(); dataRow++ )
    {
        float posX, posY, posZ, edep;
        int eventID, pdgType;

        if( dataRow->empty() )
        {
            continue;
        }

        if( !m_showPrimaries->get() && std::stoi( dataRow->at( parentIDIndex ) ) == 0 )
        {
            continue;
        }

        if( !m_showSecondaries->get() && std::stoi( dataRow->at( parentIDIndex ) ) != 0 )
        {
            continue;
        }

        pdgType = std::stoi( dataRow->at( PDGEncodingIndex ) );
        posX = boost::lexical_cast< float >( dataRow->at( posXIndex ) );
        posY = boost::lexical_cast< float >( dataRow->at( posYIndex ) );
        posZ = boost::lexical_cast< float >( dataRow->at( posZIndex ) );

        eventID = std::stoi( dataRow->at( eventIDIndex ) );
        edep = boost::lexical_cast< float >( dataRow->at( edepIndex ) );
        
        if( !m_columnPropertyHandler->isPDGTypeSelected( pdgType ) )
        {
            continue;
        }

        if( eventID < m_minCap->get() || eventID > m_maxCap->get() ) 
        {
            continue;
        }

        if( edep > maxEdep )
        {
            maxEdep = edep;
        }

        m_vertices->push_back( posX );
        m_vertices->push_back( posY );
        m_vertices->push_back( posZ );
        eventIDs.push_back( eventID );
        edeps.push_back( edep );
    }

    normalizeEdeps( edeps, m_colors, maxEdep );

    int fiberLength = 0;
    int fiberStartIndex = 0;
    int reversePos = 0;
    int currentEventID = eventIDs.at( 0 );

    m_lineStartIndexes->push_back( fiberStartIndex );

    for(std::vector<int>::iterator eID = eventIDs.begin(); eID != eventIDs.end(); eID++ )
    {
        if( currentEventID != *eID )
        {
            currentEventID = *eID;
            m_lineStartIndexes->push_back( fiberStartIndex );
            m_lineLengths->push_back( fiberLength );
            fiberLength = 0;
            reversePos++;
        }
        fiberLength++;
        fiberStartIndex++;
        m_verticesReverse->push_back( reversePos );
    }

    m_lineLengths->push_back( fiberLength );

    m_fibers = boost::shared_ptr< WDataSetFibers >(
            new WDataSetFibers(
                    m_vertices,
                    m_lineStartIndexes,
                    m_lineLengths,
                    m_verticesReverse
            )
    );

    m_fibers->addColorScheme( m_colors, "Energy deposition", "Color fibers based on their energy." );

    m_output_fibers->updateData( m_fibers );
}

void WMConverterCSV::setPointsOutOfCSVData( WDataSetCSV::ContentSPtr header, WDataSetCSV::ContentSPtr data )
{
    if( !m_protonData->columnsInitialized() )
    {
        return;
    }

    SPFloatVector m_vertices = SPFloatVector( new std::vector< float >() );
    SPFloatVector m_colors = SPFloatVector( new std::vector< float >() );
    SPFloatVector m_sizes = SPFloatVector( new std::vector< float >() );

    std::vector< float > edeps;

    WColor plainColor = m_colorSelection->get( true );

    float maxEdep = 0.0;
    float posX, posY, posZ, edep;

    int parentIDIndex = m_protonData->getColumnIndex( "parentID" );
    int posXIndex = m_protonData->getColumnIndex( "posX" );
    int posYIndex = m_protonData->getColumnIndex( "posY" );
    int posZIndex = m_protonData->getColumnIndex( "posZ" );
    int edepIndex = m_protonData->getColumnIndex( "edep" );

    //for(WDataSetCSV::Content::iterator dataRow = data->begin(); dataRow != data->end(); dataRow++ )
    for( int idx = 0; idx < data->size(); idx++ )
    {
        std::vector< std::string > row = data->at( idx );

        if( row.empty() )
        {
            continue;
        }

        if( !m_showPrimaries->get() && std::stoi( row.at( parentIDIndex ) ) == 0 )
        {
            continue;
        }

        if( !m_showSecondaries->get() && std::stoi( row.at( parentIDIndex ) ) != 0 )
        {
            continue;
        }

        posX = boost::lexical_cast< float >( row.at( posXIndex ) );
        posY = boost::lexical_cast< float >( row.at( posYIndex ) );
        posZ = boost::lexical_cast< float >( row.at( posZIndex ) );
        edep = boost::lexical_cast< float >( row.at( edepIndex ) );

        if( edep > maxEdep )
        {
            maxEdep = edep;
        }

        m_vertices->push_back( posX );
        m_vertices->push_back( posY );
        m_vertices->push_back( posZ );

        if( !m_colorFromEdep->get() )
        {
            m_colors->push_back( plainColor[0] );
            m_colors->push_back( plainColor[1] );
            m_colors->push_back( plainColor[2] );
        }

        m_sizes->push_back( edep );
        edeps.push_back( edep );
    }

    if( m_colorFromEdep->get() )
    {
        normalizeEdeps( edeps, m_colors, maxEdep );
    }

    if( m_sizesFromEdep->get() )
    {
        m_points = boost::shared_ptr< WDataSetPointsAndSizes >(
                new WDataSetPointsAndSizes(
                        m_vertices,
                        m_colors,
                        m_sizes
                )
        );
    }
    else
    {
        m_points = boost::shared_ptr < WDataSetPoints >(
                new WDataSetPoints(
                        m_vertices,
                        m_colors
                        )
                );
    }

    m_output_points->updateData( m_points );
}

void WMConverterCSV::updateRangeOfEventIDSelection( int minCap, int maxCap )
{
    m_minCap->setMin( minCap );
    m_minCap->setMax( maxCap );
    m_maxCap->setMin( minCap );
    m_maxCap->setMax( maxCap );

    int currentMinCap = m_minCap->get();
    int currentMaxCap = m_maxCap->get();

    if( currentMaxCap < currentMinCap )
        m_maxCap->set( currentMinCap );

    if( currentMinCap < 0 )
        m_minCap->set( 0 );
}

void WMConverterCSV::updateMesh( WPropertyBase::SPtr property )
{
    WMConverterCSV::determineMinMaxEventID();
    WMConverterCSV::setFibersOutOfCSVData( m_protonData->getCSVHeader(), m_protonData->getCSVData() );
}

void WMConverterCSV::determineMinMaxEventID()
{
    int eventIDIndex = m_protonData->getColumnIndex( "eventID" );

    WMConverterCSV::updateRangeOfEventIDSelection(
        std::stoi( m_protonData->getCSVData()->front().at( eventIDIndex ) ),
        std::stoi( m_protonData->getCSVData()->back().at( eventIDIndex ) ) );
}

void WMConverterCSV::normalizeEdeps( std::vector< float > edeps, SPFloatVector colorArray, float maxEdep )
{
    for( std::vector< float >::iterator currentEdep = edeps.begin(); currentEdep != edeps.end(); currentEdep++ )
    {
        *currentEdep = *currentEdep / maxEdep;

        colorArray->push_back( *currentEdep * 10 );
        colorArray->push_back( *currentEdep * 100 );
        colorArray->push_back( *currentEdep * 1 );
    }
}

void WMConverterCSV::updateCheckboxProperty( WPropertyBase::SPtr property )
{
    if( m_showPrimaries->get() || m_showSecondaries->get() )
    {
        setPointsOutOfCSVData( m_protonData->getCSVHeader(), m_protonData->getCSVData() );
        setFibersOutOfCSVData( m_protonData->getCSVHeader(), m_protonData->getCSVData() );
    }
    else
    {
        if( property == m_showPrimaries )
        {
            m_showPrimaries->set( true );
        }

        if( property == m_showSecondaries )
        {
            m_showSecondaries->set( true );
        }
    }
}
