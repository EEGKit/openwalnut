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
#include <utility>
#include <vector>

#include "WMFilterProtonData.h"

W_LOADABLE_MODULE( WMFilterProtonData )

WMFilterProtonData::WMFilterProtonData():
        WModuleContainer()
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

    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if(m_input->getData() == NULL)
        {
            continue;
        }

        if( m_protonData == NULL )
        {
            m_protonData = WMProtonData::SPtr( new WMProtonData(  m_input->getData()->getHeader(),  m_input->getData()->getData() ) );

            m_propertyStatus->setColumnPropertyHandler( WMColumnPropertyHandler::SPtr( new WMColumnPropertyHandler( m_protonData, m_properties,
                boost::bind( &WMFilterProtonData::setOutputFromCSV, this ) ) ) );

            m_propertyStatus->setFilterPropertyHandler( WMFilterPropertyHandler::SPtr( new WMFilterPropertyHandler( m_protonData, m_properties,
                boost::bind( &WMFilterProtonData::setOutputFromCSV, this ) ) ) );

            m_propertyStatus->setVisualizationPropertyHandler( WMVisualizationPropertyHandler::SPtr(
                new WMVisualizationPropertyHandler( m_protonData, m_properties, boost::bind( &WMFilterProtonData::setOutputFromCSV, this ) ) ) );

            m_propertyStatus->setEventIDLimitationPropertyHandler( WMEventIDLimitationPropertyHandler::SPtr(
                new WMEventIDLimitationPropertyHandler( m_protonData, m_properties, boost::bind( &WMFilterProtonData::setOutputFromCSV, this ) ) ) );

            m_propertyStatus->getColumnPropertyHandler()->setSelectionEventMethod( boost::bind( &WMFilterProtonData::updateProperty, this ) );
        }
        else
        {
            m_protonData->setCSVHeader(  m_input->getData()->getHeader() );
            m_protonData->setCSVData(  m_input->getData()->getData() );
        }
        m_propertyStatus->getFilterPropertyHandler()->createPDGMap(
            ( m_localPath / getMetaInformation()->query< std::string >( "common/pdgnames" , "NoFile" ) ).string() );

        clearProperties();
        m_propertyStatus->getColumnPropertyHandler()->createProperties();
        m_propertyStatus->getFilterPropertyHandler()->createProperties();
        m_propertyStatus->getVisualizationPropertyHandler()->createProperties();
        m_propertyStatus->getEventIDLimitationPropertyHandler()->createProperties();

        createColorBar();
        setToLoadedProperties();

        setOutputFromCSV( );
    }
}

void WMFilterProtonData::connectors()
{
    m_input = WModuleInputData< WDataSetCSV >::createAndAdd( shared_from_this(), "in", "CSV_input" );

    m_output_points = WModuleOutputData< WDataSetPoints >::createAndAdd( shared_from_this(), "output points", "Output CSV data as Point data" );
    m_output_fibers = WModuleOutputData< WDataSetFibers >::createAndAdd( shared_from_this(), "output fibers",  "Output CSV data as Fiber data" );
    m_output_transferFunction = WModuleOutputData< WDataSetSingle >::createAndAdd( shared_from_this(),
                                                                                   "output transferfunction", "Output transfer function" );

    WModule::connectors();
}

void WMFilterProtonData::clearProperties()
{
    m_properties->removeProperty( m_properties->getProperty( "Select columns" ) );
    m_properties->removeProperty( m_properties->getProperty( "Filtering" ) );
    m_properties->removeProperty( m_properties->getProperty( "Visualization" ) );
    m_properties->removeProperty( m_properties->getProperty( "Event ID Limitation" ) );
}

void WMFilterProtonData::properties()
{
    // Creating dummy properties for loading of projects
    // TODO(robin.eschbach) This is only a hotfix until property buffer is implemented
    // When changing property names they also have to be changed here, so the loading works.
    WPropertyGroup::SPtr groupColumn  = m_properties->addPropertyGroup( "Select columns", "Select columns", false );
    WPropertyGroup::SPtr groupFilter  = m_properties->addPropertyGroup( "Filtering", "Filtering", false );
    WPropertyGroup::SPtr groupVisual  = m_properties->addPropertyGroup( "Visualization", "Visualization", false );
    WPropertyGroup::SPtr groupEventID = m_properties->addPropertyGroup( "Event ID Limitation", "Event ID Limitation", false );
    WPropertyGroup::SPtr groupRename  = groupFilter->addPropertyGroup( "Rename or Delete Particle-Name",
                                                    "Filtering/Rename or Delete Particle-Name", false );

    WPropertyBase::PropertyChangeNotifierType columnNotifier = boost::bind( &WMFilterProtonData::loadNotifier,
                                                                this, groupColumn, boost::placeholders::_1 );
    WPropertyBase::PropertyChangeNotifierType filterNotifier = boost::bind( &WMFilterProtonData::loadNotifier,
                                                                this, groupFilter, boost::placeholders::_1 );
    WPropertyBase::PropertyChangeNotifierType visualNotifier = boost::bind( &WMFilterProtonData::loadNotifier,
                                                                this, groupVisual, boost::placeholders::_1 );
    WPropertyBase::PropertyChangeNotifierType eventIDNotifier = boost::bind( &WMFilterProtonData::loadNotifier,
                                                                this, groupEventID, boost::placeholders::_1 );
    WPropertyBase::PropertyChangeNotifierType renameNotifier = boost::bind( &WMFilterProtonData::loadNotifier,
                                                                this, groupRename, boost::placeholders::_1 );

    groupColumn->addProperty( "PDGEncoding", "PDGEncoding", std::string( "" ), columnNotifier, false );
    groupColumn->addProperty( "posX", "posX", std::string( "" ), columnNotifier, false );
    groupColumn->addProperty( "posY", "posY", std::string( "" ), columnNotifier, false );
    groupColumn->addProperty( "posZ", "posZ", std::string( "" ), columnNotifier, false );
    groupColumn->addProperty( "edep", "edep", std::string( "" ), columnNotifier, false );
    groupColumn->addProperty( "eventID", "eventID", std::string( "" ), columnNotifier, false );
    groupColumn->addProperty( "trackID", "trackID", std::string( "" ), columnNotifier, false );
    groupColumn->addProperty( "parentID", "parentID", std::string( "" ), columnNotifier, false );

    groupFilter->addProperty( "Show primaries", "Show primaries", std::string( "" ), filterNotifier, false );
    groupFilter->addProperty( "Show secondaries", "Show secondaries", std::string( "" ), filterNotifier, false );
    groupFilter->addProperty( "PDGEncoding", "PDGEncoding", std::string( "" ), filterNotifier, false );

    groupRename->addProperty( "New Name (press enter)", "New Name (press enter)", std::string( "" ), renameNotifier, false );
    groupRename->addProperty( "Select Particle", "Select Particle", std::string( "" ), renameNotifier, false );
    groupRename->addProperty( "Set Changes", "Set Changes", std::string( "" ), renameNotifier, false );

    groupVisual->addProperty( "Scale point size", "Scale point size", std::string( "" ), visualNotifier, false );
    groupVisual->addProperty( "Color by edep", "Color by edep", std::string( "" ), visualNotifier, false );
    groupVisual->addProperty( "Plain color", "Plain color", std::string( "" ), visualNotifier, false );
    groupVisual->addProperty( "Transfer Function", "Transfer Function", std::string( "" ), visualNotifier, false );
    groupVisual->addProperty( "Set gradient", "Set gradient", std::string( "" ), visualNotifier, false );

    groupEventID->addProperty( "Min Cap", "Min Cap", std::string( "" ), eventIDNotifier, false );
    groupEventID->addProperty( "Max Cap", "Max Cap", std::string( "" ), eventIDNotifier, false );
    groupEventID->addProperty( "Set selection", "Set selection", std::string( "" ), eventIDNotifier, false );

    WModule::properties();
}

void WMFilterProtonData::loadNotifier( WPropertyGroup::SPtr group, WPropertyBase::SPtr property )
{
    std::string name = group->getDescription() + "/" + property->getName();
    std::string value = property->getAsString();
    m_loadedProperties.push_back( std::pair< std::string, std::string >( name, value ) );
}

void WMFilterProtonData::setToLoadedProperties()
{
    for( auto iter = m_loadedProperties.begin(); iter != m_loadedProperties.end(); iter++ )
    {
        WPropertyBase::SPtr prop = m_properties->findProperty( ( *iter ).first );
        if( prop )
        {
            prop->setAsString( ( *iter ).second );
        }
    }
}

void WMFilterProtonData::setOutputFromCSV()
{
    m_converter = boost::shared_ptr< WMCsvConverter >( new WMCsvConverter( m_protonData, m_propertyStatus, m_colorBar ) );

    m_output_points->updateData( m_converter->getPoints() );
    m_output_fibers->updateData( m_converter->getFibers() );
    m_output_transferFunction->updateData( m_converter->getTransferFunction() );
}

void WMFilterProtonData::updateProperty()
{
    m_propertyStatus->getColumnPropertyHandler()->updateProperty();
    m_propertyStatus->getFilterPropertyHandler()->updateProperty();
    m_propertyStatus->getVisualizationPropertyHandler()->updateProperty();
    m_propertyStatus->getEventIDLimitationPropertyHandler()->updateProperty();
}

void WMFilterProtonData::createColorBar()
{
    m_colorBar = createAndAdd( "Transfer Function Color Bar" );
    m_colorBar->isReady().wait();
    m_output_transferFunction->connect( m_colorBar->getInputConnector( "transfer function" ) );
}

void WMFilterProtonData::activate()
{
    m_colorBar->getProperties()->getProperty( "active" )->toPropBool()->set( m_active->get() );
}
