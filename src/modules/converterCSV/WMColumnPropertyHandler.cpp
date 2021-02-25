#include "WMColumnPropertyHandler.h"

WMColumnPropertyHandler::WMColumnPropertyHandler( WMProtonData::SPtr protonData, WPropertyGroup::SPtr properties, WMColumnPropertyHandler::CallbackPtr dataUpdate ):
    m_protonData( protonData ),
    m_properties( properties ),
    m_dataUpdate( dataUpdate )
{
}

void WMColumnPropertyHandler::createProperties()
{
    WPropertyBase::PropertyChangeNotifierType notifier = boost::bind( &WMColumnPropertyHandler::propertyNotifier,
                                                                this, boost::placeholders::_1 );
    WPropertyBase::PropertyChangeNotifierType pdgEncodingnotifier = boost::bind( &WMColumnPropertyHandler::updateSelectedPDGTypes,
                                                                this, boost::placeholders::_1 );

    m_columnSelectionGroup = m_properties->addPropertyGroup( "Select columns", "Select the columns which should be used" );

    m_possibleSelectionsUsingTypes = WItemSelection::SPtr( new WItemSelection() );
    m_possibleSelection = WItemSelection::SPtr( new WItemSelection() );

    std::vector< std::string > header = m_protonData->getCSVHeader()->at( 0 );

    for( std::vector<std::string>::iterator colName = header.begin(); colName != header.end(); colName++ )
    {
        m_possibleSelectionsUsingTypes->addItem( ItemType::create( *colName, *colName, "",  NULL ) );
    }

    for( auto pdgType : m_pdgTypes ) 
    {
        m_possibleSelection->addItem( std::to_string( pdgType ) );
    }

    m_selectedPDGTypes.push_back( std::to_string( m_pdgTypes[0] ) );

    m_multiSelection = m_columnSelectionGroup->addProperty( "PDGEncoding", "Choose particle type(s) you want show", m_possibleSelection->getSelectorFirst(), pdgEncodingnotifier );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_multiSelection );

    m_singleSelectionForPosX = addHeaderProperty( "posX", notifier );
    m_singleSelectionForPosY = addHeaderProperty( "posY", notifier );
    m_singleSelectionForPosZ = addHeaderProperty( "posZ", notifier );
    m_singleSelectionForEdep = addHeaderProperty( "edep", notifier );
    m_singleSelectionForEventID = addHeaderProperty( "eventID", notifier );
    m_singleSelectionForTrackID = addHeaderProperty( "trackID", notifier );
    m_singleSelectionForParentID = addHeaderProperty( "parentID", notifier );
}

void WMColumnPropertyHandler::updateProperty()
{
    // TODO
}

WPropSelection WMColumnPropertyHandler::addHeaderProperty( std::string headerName, WPropertyBase::PropertyChangeNotifierType notifier )
{
    int index = m_protonData->getColumnIndex( headerName );
    WItemSelector selector = index < 0 ? m_possibleSelectionsUsingTypes->getSelectorNone() : m_possibleSelectionsUsingTypes->getSelector( index );
    WPropSelection selection = m_columnSelectionGroup->addProperty(
                                                headerName,
                                                "Choose the " + headerName + " column from csv",
                                                selector,
                                                notifier );

    WPropertyHelper::PC_SELECTONLYONE::addTo( selection );
    WPropertyHelper::PC_NOTEMPTY::addTo( selection );

    return selection;
}


void WMColumnPropertyHandler::propertyNotifier( WPropertyBase::SPtr property )
{
    const WItemSelector* selector;
    std::string columnName;

    if( property == m_singleSelectionForPosX )
    {
        selector = &m_singleSelectionForPosX->get( true );
        columnName = "posX";
    }
    else if( property == m_singleSelectionForPosY )
    {
        selector = &m_singleSelectionForPosY->get( true );
        columnName = "posY";
    }
    else if( property == m_singleSelectionForPosZ )
    {
        selector = &m_singleSelectionForPosZ->get( true );
        columnName = "posZ";
    }
    else if( property == m_singleSelectionForEdep )
    {
        selector = &m_singleSelectionForEdep->get( true );
        columnName = "edep";
    }
    else if( property == m_singleSelectionForEventID )
    {
        selector = &m_singleSelectionForEventID->get( true );
        columnName = "eventID";
    }
    else if( property == m_singleSelectionForTrackID )
    {
        selector = &m_singleSelectionForTrackID->get( true );
        columnName = "trackID";
    }
    else if( property == m_singleSelectionForParentID )
    {
        selector = &m_singleSelectionForParentID->get( true );
        columnName = "parentID";
    }

    if( selector != NULL )
    {
        std::string selectedValue = selector->at( 0 )->getAs< ItemType >()->getValue();
        m_protonData->setColumnIndex( columnName, getColumnNumberByName( selectedValue, m_protonData->getCSVHeader()->at( 0 ) ) );

        m_dataUpdate( m_protonData->getCSVHeader(), m_protonData->getCSVData() );
    }
}

int WMColumnPropertyHandler::getColumnNumberByName( std::string columnNameToMatch, std::vector< std::string > headerToSearchIn )
{
    int pos = 0;
    for( std::vector< std::string >::iterator it = headerToSearchIn.begin(); it != headerToSearchIn.end(); it++ )
    {
        if( *it == columnNameToMatch ) return pos;
        pos++;
    }
    return -1;
}

void WMColumnPropertyHandler::searchPDGTypes() 
{
    int pdgColumnIndex = m_protonData->getColumnIndex( "PDGEncoding" );


    //for( WDataSetCSV::Content::iterator dataRow = m_protonData->getCSVData().begin(); dataRow < m_protonData->getCSVData().end(); dataRow++ )
    for( auto idx = 0; idx < m_protonData->getCSVData()->size(); idx++)
    {
        std::vector< std::string > row = m_protonData->getCSVData()->at( idx );
        int currentParticleID = std::stoi( row.at( pdgColumnIndex ) );
        
        if( std::find( m_pdgTypes.begin(), m_pdgTypes.end(), currentParticleID ) == m_pdgTypes.end() ) 
        {
            m_pdgTypes.push_back( currentParticleID );
        }
    }
}

void WMColumnPropertyHandler::updateSelectedPDGTypes( WPropertyBase::SPtr property ) 
{
    m_selectedPDGTypes.clear();
    
    if( m_multiSelection->changed() ) {
        WItemSelector selectedItems = m_multiSelection->get( true );
        //PDGEncodingIndex = getColumnNumberByName( "PDGEncoding", m_csvHeader.at( 0 ) );

        for( int i = 0; i < selectedItems.size(); ++i ) 
        {
           m_selectedPDGTypes.push_back( selectedItems.at( i )->getName() );
        }
    }

    m_dataUpdate( m_protonData->getCSVHeader(), m_protonData->getCSVData() );
}

bool WMColumnPropertyHandler::isPDGTypeSelected( int pdgType ) 
{
    for( auto idx = 0; idx < m_selectedPDGTypes.size(); idx++ )
    {
        if (pdgType == std::stoi( m_selectedPDGTypes[idx] ) ) 
        {
            return true;
        }
    }
    return false;

    /*for ( auto selectedPDGType : m_selectedPDGTypes ) 
    {
        debugLog() << "STOI 9";
        if (pdgType == std::stoi( selectedPDGType ) ) 
        {
            return true;
        }   
    }
    return false;*/
}