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


    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();
        m_dataset = m_input->getData();
        properties();
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

void WMConverterCSV::updateProperty( WPropertyBase::SPtr property )
{
    WDataSetCSV::Content m_csvHeader = m_dataset->getHeader();
    WDataSetCSV::Content m_csvData = m_dataset->getData();

    if( property == m_singleSelectionForPosX )
    {
        WItemSelector selector = m_singleSelectionForPosX->get( true );
        std::string selectedValue = selector.at( 0 )->getAs< ItemType >()->getValue();
        xPosIndex = getColumnNumberByName( selectedValue, m_csvHeader.at( 0 ) );
    }

    if( property == m_singleSelectionForPosY )
    {
        WItemSelector selector = m_singleSelectionForPosY->get( true );
        std::string selectedValue = selector.at( 0 )->getAs< ItemType >()->getValue();
        yPosIndex = getColumnNumberByName( selectedValue, m_csvHeader.at( 0 ) );
    }

    if( property == m_singleSelectionForPosZ )
    {
        WItemSelector selector = m_singleSelectionForPosZ->get( true );
        std::string selectedValue = selector.at( 0 )->getAs< ItemType >()->getValue();
        zPosIndex = getColumnNumberByName( selectedValue, m_csvHeader.at( 0 ) );
    }

    if( property == m_singleSelectionForPosEdep )
    {
        WItemSelector selector = m_singleSelectionForPosEdep->get( true );
        std::string selectedValue = selector.at( 0 )->getAs< ItemType >()->getValue();
        edepIndex = getColumnNumberByName( selectedValue, m_csvHeader.at( 0 ) );
    }

    setPointsOutOfCSVData( m_csvHeader, m_csvData );
    setFibersOutOfCSVData( m_csvHeader, m_csvData );
}

void WMConverterCSV::properties()
{
    WPropertyBase::PropertyChangeNotifierType notifier = boost::bind( &WMConverterCSV::updateProperty, this, boost::placeholders::_1 );
    WPropertyBase::PropertyChangeNotifierType notifierCheckBox = boost::bind( &WMConverterCSV::updateCheckboxProperty, this, boost::placeholders::_1 );

    m_possibleSelectionsUsingTypes = WItemSelection::SPtr( new WItemSelection() );

    if( m_dataset != NULL )
    {
        std::vector< std::string > header = m_dataset->getHeader().at( 0 );

        for( std::vector<std::string>::iterator colName = header.begin(); colName != header.end(); colName++ )
        {
            m_possibleSelectionsUsingTypes->addItem( ItemType::create( *colName, *colName, "",  NULL ) );
        }

        m_singleSelectionForPosX = m_properties->addProperty( "PosX", "Choose the xPos column from csv",
                                                            m_possibleSelectionsUsingTypes->getSelectorFirst(), notifier );

        m_singleSelectionForPosY = m_properties->addProperty( "PosY", "Choose the yPos column from csv",
                                                            m_possibleSelectionsUsingTypes->getSelectorFirst(), notifier );

        m_singleSelectionForPosZ = m_properties->addProperty( "PosZ", "Choose the zPos column from csv",
                                                            m_possibleSelectionsUsingTypes->getSelectorFirst(), notifier );

        m_singleSelectionForPosEdep = m_properties->addProperty( "edep", "Choose the edep column from csv",
                                                            m_possibleSelectionsUsingTypes->getSelectorFirst(), notifier );

        WPropertyHelper::PC_SELECTONLYONE::addTo( m_singleSelectionForPosX );
        WPropertyHelper::PC_NOTEMPTY::addTo( m_singleSelectionForPosX );

        WPropertyHelper::PC_SELECTONLYONE::addTo( m_singleSelectionForPosY );
        WPropertyHelper::PC_NOTEMPTY::addTo( m_singleSelectionForPosY );

        WPropertyHelper::PC_SELECTONLYONE::addTo( m_singleSelectionForPosZ );
        WPropertyHelper::PC_NOTEMPTY::addTo( m_singleSelectionForPosZ );

        WPropertyHelper::PC_SELECTONLYONE::addTo( m_singleSelectionForPosEdep );
        WPropertyHelper::PC_NOTEMPTY::addTo( m_singleSelectionForPosEdep );

        m_showPrimaries = m_properties->addProperty( "Show primaries", "Show/hide primaries", true, notifierCheckBox );
        m_showSecondaries = m_properties->addProperty( "Show secondaries", "Show/hide secondaries", true, notifierCheckBox );
    }

    WModule::properties();
}

int WMConverterCSV::getColumnNumberByName( std::string columnNameToMatch, std::vector< std::string > headerToSearchIn )
{
    int pos = 0;
    for( std::vector< std::string >::iterator it = headerToSearchIn.begin(); it != headerToSearchIn.end(); it++ )
    {
        if( *it == columnNameToMatch ) return pos;
        pos++;
    }
    return pos;
}


void WMConverterCSV::setFibersOutOfCSVData( WDataSetCSV::Content header, WDataSetCSV::Content data )
{
    if( xPosIndex < 0 || yPosIndex < 0 || zPosIndex < 0 || edepIndex < 0 )
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


    int eventIDIndex = getColumnNumberByName( "eventID", header.at( 0 ) );
    int parentIDIndex = getColumnNumberByName( "parentID", header.at( 0 ) );

    float maxEdep = 0.0;

    for( WDataSetCSV::Content::iterator dataRow = data.begin(); dataRow != data.end(); dataRow++ )
    {
        float posX, posY, posZ, edep;
        int eventID;

        if( dataRow->empty() )
        {
            continue;
        }

        if( !m_showPrimaries->get() && std::stoi( dataRow->at(parentIDIndex ) ) == 0 )
        {
            continue;
        }

        if( !m_showSecondaries->get() && std::stoi( dataRow->at(parentIDIndex ) ) != 0 )
        {
            continue;
        }

        posX = boost::lexical_cast< float >( dataRow->at( xPosIndex ) );
        posY = boost::lexical_cast< float >( dataRow->at( yPosIndex ) );
        posZ = boost::lexical_cast< float >( dataRow->at( zPosIndex ) );
        eventID = std::stoi( dataRow->at( eventIDIndex ) );
        edep = boost::lexical_cast< float >( dataRow->at( edepIndex ) );

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
        if(currentEventID != *eID )
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

void WMConverterCSV::setPointsOutOfCSVData( WDataSetCSV::Content header, WDataSetCSV::Content data )
{
    if( xPosIndex < 0 || yPosIndex < 0 || zPosIndex < 0 || edepIndex < 0 )
    {
        return;
    }

    SPFloatVector m_vertices = SPFloatVector( new std::vector< float >() );
    SPFloatVector m_colors = SPFloatVector( new std::vector< float >() );
    SPFloatVector m_sizes = SPFloatVector( new std::vector< float >() );

    std::vector< float > edeps;
    int parentIDIndex = getColumnNumberByName( "parentID", header.at( 0 ) );

    float maxEdep = 0.0;
    float posX, posY, posZ, edep;
    for(WDataSetCSV::Content::iterator dataRow = data.begin(); dataRow != data.end(); dataRow++ )
    {
        if( dataRow->empty() )
        {
            continue;
        }

        if( !m_showPrimaries->get() && std::stoi( dataRow->at(parentIDIndex ) ) == 0 )
        {
            continue;
        }

        if( !m_showSecondaries->get() && std::stoi( dataRow->at(parentIDIndex ) ) != 0 )
        {
            continue;
        }

        posX = boost::lexical_cast< float >( dataRow->at( xPosIndex ) );
        posY = boost::lexical_cast< float >( dataRow->at( yPosIndex ) );
        posZ = boost::lexical_cast< float >( dataRow->at( zPosIndex ) );
        edep = boost::lexical_cast< float >( dataRow->at( edepIndex ) );

        if( edep > maxEdep )
        {
            maxEdep = edep;
        }

        m_vertices->push_back( posX );
        m_vertices->push_back( posY );
        m_vertices->push_back( posZ );

        m_sizes->push_back( edep );
        edeps.push_back( edep );
    }

    normalizeEdeps( edeps, m_colors, maxEdep );

    m_points = boost::shared_ptr< WDataSetPointsAndSizes >(
            new WDataSetPointsAndSizes(
                    m_vertices,
                    m_colors,
                    m_sizes
            )
    );

    m_output_points->updateData( m_points );
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
        WDataSetCSV::Content m_csvHeader = m_dataset->getHeader();
        WDataSetCSV::Content m_csvData = m_dataset->getData();

        setPointsOutOfCSVData( m_csvHeader, m_csvData );
        setFibersOutOfCSVData( m_csvHeader, m_csvData );
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

