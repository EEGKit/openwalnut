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
    m_columnSelectionGroup = m_properties->addPropertyGroup( "Select columns", "Select the columns which should be used" );

    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        m_dataset = m_input->getData();

        m_csvHeader = m_dataset->getHeader();
        m_csvData = m_dataset->getData();

        WMConverterCSV::searchPDGTypes();

        properties();

        setPointsOutOfCSVData( m_csvHeader, m_csvData );
        setFibersOutOfCSVData( m_csvHeader, m_csvData );
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

    if( property == m_singleSelectionForEventID )
    {
        WItemSelector selector = m_singleSelectionForEventID->get( true );
        std::string selectedValue = selector.at( 0 )->getAs< ItemType >()->getValue();
        eventIDIndex = getColumnNumberByName( selectedValue, m_csvHeader.at( 0 ) );
    }

    if( property == m_singleSelectionForTrackID )
    {
        WItemSelector selector = m_singleSelectionForTrackID->get( true );
        std::string selectedValue = selector.at( 0 )->getAs< ItemType >()->getValue();
        trackIDIndex = getColumnNumberByName( selectedValue, m_csvHeader.at( 0 ) );
    }

    if( property == m_singleSelectionForParentID )
    {
        WItemSelector selector = m_singleSelectionForParentID->get( true );
        std::string selectedValue = selector.at( 0 )->getAs< ItemType >()->getValue();
        parentIDIndex = getColumnNumberByName( selectedValue, m_csvHeader.at( 0 ) );
    }

    setPointsOutOfCSVData( m_csvHeader, m_csvData );
    setFibersOutOfCSVData( m_csvHeader, m_csvData );
}

void WMConverterCSV::properties()
{
    WPropertyBase::PropertyChangeNotifierType notifier = boost::bind( &WMConverterCSV::updateProperty,
                                                                this, boost::placeholders::_1 );

    WPropertyBase::PropertyChangeNotifierType notifierCheckBox = boost::bind( &WMConverterCSV::updateCheckboxProperty,
                                                                this, boost::placeholders::_1 );

    WPropertyBase::PropertyChangeNotifierType eventIDNotifier = boost::bind( &WMConverterCSV::updateMesh,
                                                                this, boost::placeholders::_1 );

    WPropertyBase::PropertyChangeNotifierType pdgEncodingnotifier = boost::bind( &WMConverterCSV::updateSelectedPDGTypes,
                                                                 this, boost::placeholders::_1 );


    m_possibleSelectionsUsingTypes = WItemSelection::SPtr( new WItemSelection() );
    
    m_possibleSelection = WItemSelection::SPtr( new WItemSelection() );

    if( m_dataset != NULL )
    {
        std::vector< std::string > header = m_dataset->getHeader().at( 0 );

        for( std::vector<std::string>::iterator colName = header.begin(); colName != header.end(); colName++ )
        {
            m_possibleSelectionsUsingTypes->addItem( ItemType::create( *colName, *colName, "",  NULL ) );
        }

        for( auto pdgType : m_pdgTypes ) 
        {
            m_possibleSelection->addItem( std::to_string( pdgType ) );
        }

        m_multiSelection = m_properties->addProperty( "PDGEncoding", "Choose particle type(s) you want show", m_possibleSelection->getSelectorFirst(), pdgEncodingnotifier );
        WPropertyHelper::PC_NOTEMPTY::addTo( m_multiSelection );

        m_singleSelectionForPosX = m_properties->addProperty( "X", "Choose the xPos column from csv",
                                                                m_possibleSelectionsUsingTypes->getSelectorFirst(), notifier );
        xPosIndex = WMConverterCSV::getColumnNumberByName( "posX", m_csvHeader.at( 0 ) );
        yPosIndex = WMConverterCSV::getColumnNumberByName( "posY", m_csvHeader.at( 0 ) );
        zPosIndex = WMConverterCSV::getColumnNumberByName( "posZ", m_csvHeader.at( 0 ) );
        edepIndex = WMConverterCSV::getColumnNumberByName( "edep", m_csvHeader.at( 0 ) );
        eventIDIndex = WMConverterCSV::getColumnNumberByName( "eventID", m_csvHeader.at( 0 ) );
        trackIDIndex = WMConverterCSV::getColumnNumberByName( "trackID", m_csvHeader.at( 0 ) );
        parentIDIndex = WMConverterCSV::getColumnNumberByName( "parentID", m_csvHeader.at( 0 ) );

        if(xPosIndex < 0)
        {
            m_singleSelectionForPosX = m_columnSelectionGroup->addProperty( "X", "Choose the xPos column from csv",
                                                                m_possibleSelectionsUsingTypes->getSelectorNone(), notifier );
        }
        else
        {
            m_singleSelectionForPosX = m_columnSelectionGroup->addProperty( "X", "Choose the xPos column from csv",
                                                                m_possibleSelectionsUsingTypes->getSelector( xPosIndex ), notifier );
        }

        if(yPosIndex < 0)
        {
            m_singleSelectionForPosY = m_columnSelectionGroup->addProperty( "Y", "Choose the yPos column from csv",
                                                                m_possibleSelectionsUsingTypes->getSelectorNone(), notifier );
        }
        else
        {
            m_singleSelectionForPosY = m_columnSelectionGroup->addProperty( "Y", "Choose the yPos column from csv",
                                                                m_possibleSelectionsUsingTypes->getSelector( yPosIndex ), notifier );
        }

        if(zPosIndex < 0)
        {
            m_singleSelectionForPosZ = m_columnSelectionGroup->addProperty( "Z", "Choose the zPos column from csv",
                                                                m_possibleSelectionsUsingTypes->getSelectorNone(), notifier );
        }
        else
        {
            m_singleSelectionForPosZ = m_columnSelectionGroup->addProperty( "Z", "Choose the zPos column from csv",
                                                                m_possibleSelectionsUsingTypes->getSelector( zPosIndex ), notifier );
        }

        if(edepIndex < 0)
        {
            m_singleSelectionForPosEdep = m_columnSelectionGroup->addProperty( "edep", "Choose the edep column from csv",
                                                                m_possibleSelectionsUsingTypes->getSelectorNone(), notifier );
        }
        else
        {
            m_singleSelectionForPosEdep = m_columnSelectionGroup->addProperty( "edep", "Choose the edep column from csv",
                                                                m_possibleSelectionsUsingTypes->getSelector( edepIndex ), notifier );
        }

        if(eventIDIndex < 0)
        {
            m_singleSelectionForEventID = m_columnSelectionGroup->addProperty( "Event ID", "Choose the eventID column from csv",
                                                                m_possibleSelectionsUsingTypes->getSelectorNone(), notifier );
        }
        else
        {
            m_singleSelectionForEventID = m_columnSelectionGroup->addProperty( "Event ID", "Choose the eventID column from csv",
                                                                m_possibleSelectionsUsingTypes->getSelector( eventIDIndex ), notifier );
        }

        if(trackIDIndex < 0)
        {
            m_singleSelectionForTrackID = m_columnSelectionGroup->addProperty( "Track ID", "Choose the trackID column from csv",
                                                                m_possibleSelectionsUsingTypes->getSelectorNone(), notifier );
        }
        else
        {
            m_singleSelectionForTrackID = m_columnSelectionGroup->addProperty( "Track ID", "Choose the trackID column from csv",
                                                                m_possibleSelectionsUsingTypes->getSelector( trackIDIndex ), notifier );
        }

        if(parentIDIndex < 0)
        {
            m_singleSelectionForParentID = m_columnSelectionGroup->addProperty( "Parent ID", "Choose the ParentID column from csv",
                                                                m_possibleSelectionsUsingTypes->getSelectorNone(), notifier );
        }
        else
        {
            m_singleSelectionForParentID = m_columnSelectionGroup->addProperty( "Parent ID", "Choose the ParentID column from csv",
                                                                m_possibleSelectionsUsingTypes->getSelector( parentIDIndex ), notifier );
        }

        WPropertyHelper::PC_SELECTONLYONE::addTo( m_singleSelectionForPosX );
        WPropertyHelper::PC_NOTEMPTY::addTo( m_singleSelectionForPosX );

        WPropertyHelper::PC_SELECTONLYONE::addTo( m_singleSelectionForPosY );
        WPropertyHelper::PC_NOTEMPTY::addTo( m_singleSelectionForPosY );

        WPropertyHelper::PC_SELECTONLYONE::addTo( m_singleSelectionForPosZ );
        WPropertyHelper::PC_NOTEMPTY::addTo( m_singleSelectionForPosZ );

        WPropertyHelper::PC_SELECTONLYONE::addTo( m_singleSelectionForPosEdep );
        WPropertyHelper::PC_NOTEMPTY::addTo( m_singleSelectionForPosEdep );

        WPropertyHelper::PC_SELECTONLYONE::addTo( m_singleSelectionForEventID );
        WPropertyHelper::PC_NOTEMPTY::addTo( m_singleSelectionForEventID );

        WPropertyHelper::PC_SELECTONLYONE::addTo( m_singleSelectionForTrackID );
        WPropertyHelper::PC_NOTEMPTY::addTo( m_singleSelectionForTrackID );

        WPropertyHelper::PC_SELECTONLYONE::addTo( m_singleSelectionForParentID );
        WPropertyHelper::PC_NOTEMPTY::addTo( m_singleSelectionForParentID );

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

int WMConverterCSV::getColumnNumberByName( std::string columnNameToMatch, std::vector< std::string > headerToSearchIn )
{
    int pos = 0;
    for( std::vector< std::string >::iterator it = headerToSearchIn.begin(); it != headerToSearchIn.end(); it++ )
    {
        if( *it == columnNameToMatch ) return pos;
        pos++;
    }
    return -1;
}

void WMConverterCSV::setFibersOutOfCSVData( WDataSetCSV::Content header, WDataSetCSV::Content data )
{
    if( PDGEncodingIndex < 0|| xPosIndex < 0 || yPosIndex < 0 || zPosIndex < 0 || edepIndex < 0 || eventIDIndex < 0 || trackIDIndex < 0 || parentIDIndex < 0 )
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

    for( WDataSetCSV::Content::iterator dataRow = data.begin(); dataRow != data.end(); dataRow++ )
    {
        float posX, posY, posZ, edep;
        int eventID, pdgType;

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

        pdgType = std::stoi( dataRow->at( PDGEncodingIndex ) );
        posX = boost::lexical_cast< float >( dataRow->at( xPosIndex ) );
        posY = boost::lexical_cast< float >( dataRow->at( yPosIndex ) );
        posZ = boost::lexical_cast< float >( dataRow->at( zPosIndex ) );
        eventID = std::stoi( dataRow->at( eventIDIndex ) );
        edep = boost::lexical_cast< float >( dataRow->at( edepIndex ) );
        
        if ( !WMConverterCSV::isPDGTypeSelected( pdgType ) )
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

void WMConverterCSV::setPointsOutOfCSVData( WDataSetCSV::Content header, WDataSetCSV::Content data )
{
    if( xPosIndex < 0 || yPosIndex < 0 || zPosIndex < 0 || edepIndex < 0 || eventIDIndex < 0 || trackIDIndex < 0 || parentIDIndex < 0 )
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
    WMConverterCSV::setFibersOutOfCSVData( m_csvHeader, m_csvData );
}

void WMConverterCSV::determineMinMaxEventID()
{
    int eventIDIndex = WMConverterCSV::getColumnNumberByName( "eventID", m_csvHeader.at( 0 ) );

    WMConverterCSV::updateRangeOfEventIDSelection(
        std::stoi( m_csvData.front().at( eventIDIndex ) ),
        std::stoi( m_csvData.back().at( eventIDIndex ) ) );
}

void WMConverterCSV::searchPDGTypes() 
{
    int pdgColumnIndex = WMConverterCSV::getColumnNumberByName( "PDGEncoding", m_csvHeader.at( 0 ) );

    for( WDataSetCSV::Content::iterator dataRow = m_csvData.begin(); dataRow != m_csvData.end(); dataRow++ )
    {
        int currentParticleID = std::stoi( dataRow->at( pdgColumnIndex ) );
        
        if( std::find( m_pdgTypes.begin(), m_pdgTypes.end(), currentParticleID ) == m_pdgTypes.end() )
            m_pdgTypes.push_back( currentParticleID );
    }
}

void WMConverterCSV::updateSelectedPDGTypes( WPropertyBase::SPtr property ) 
{
    m_selectedPDGTypes.clear();
    
    if( m_multiSelection->changed() ) {
        WItemSelector selectedItems = m_multiSelection->get( true );
        PDGEncodingIndex = getColumnNumberByName( "PDGEncoding", m_csvHeader.at( 0 ) );

        for( int i = 0; i < selectedItems.size(); ++i ) 
        {
           m_selectedPDGTypes.push_back( selectedItems.at( i )->getName() );
        }
    }

    WMConverterCSV::setFibersOutOfCSVData( m_csvHeader, m_csvData );
}

bool WMConverterCSV::isPDGTypeSelected( int pdgType ) 
{
    for ( auto selectedPDGType : m_selectedPDGTypes ) 
    {
        if (pdgType == std::stoi( selectedPDGType ) ) 
        {
            return true;
        }   
    }
    return false;
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
