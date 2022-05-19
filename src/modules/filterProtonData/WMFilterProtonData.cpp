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

#include <memory>
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
    return "Filter Proton Data";
}

const std::string WMFilterProtonData::getDescription() const
{
    return "";
}

std::shared_ptr< WModule > WMFilterProtonData::factory() const
{
    return std::shared_ptr< WModule >( new WMFilterProtonData() );
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

    m_propertyStatus = std::shared_ptr< WPropertyStatus >( new WPropertyStatus() );

    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if( m_input->updated() )
        {
            if( m_input->getData() == NULL )
            {
                continue;
            }

            std::shared_ptr< WProgress > progressBar( new WProgress( "Preparing..." ) );
            m_progress->addSubProgress( progressBar );

            if( m_protonData == NULL )
            {
                m_protonData = WProtonData::SPtr( new WProtonData(  m_input->getData()->getHeader(),  m_input->getData()->getData() ) );

                m_propertyStatus->setColumnPropertyHandler( WColumnPropertyHandler::SPtr( new WColumnPropertyHandler( m_protonData, m_properties,
                    boost::bind( &WConditionSet::notify, &m_moduleState ) ) ) );

                m_propertyStatus->setFilterPropertyHandler( WFilterPropertyHandler::SPtr( new WFilterPropertyHandler( m_protonData, m_properties,
                    boost::bind( &WConditionSet::notify, &m_moduleState ) ) ) );

                m_propertyStatus->setVisualizationPropertyHandler( WVisualizationPropertyHandler::SPtr(
                    new WVisualizationPropertyHandler( m_protonData, m_properties, boost::bind( &WConditionSet::notify, &m_moduleState ) ) ) );

                m_propertyStatus->setEventIDLimitationPropertyHandler( WEventIDLimitationPropertyHandler::SPtr(
                    new WEventIDLimitationPropertyHandler( m_protonData, m_properties, boost::bind( &WConditionSet::notify, &m_moduleState ) ) ) );

                m_propertyStatus->getColumnPropertyHandler()->setSelectionEventMethod( boost::bind( &WMFilterProtonData::updateProperty, this ) );

                m_propertyStatus->setOutputPropertyHandler( WOutputPropertyHandler::SPtr( new WOutputPropertyHandler( m_protonData, m_properties,
                    boost::bind( &WConditionSet::notify, &m_moduleState ) ) ) );
            }
            else
            {
                m_protonData->setCSVHeader( m_input->getData()->getHeader() );
                m_protonData->setCSVData( m_input->getData()->getData() );
            }
            m_propertyStatus->getFilterPropertyHandler()->createPDGMap(
                ( m_localPath / getMetaInformation()->query< std::string >( "common/pdgnames" , "NoFile" ) ).string() );

            clearProperties();
            m_propertyStatus->getColumnPropertyHandler()->createProperties();
            m_propertyStatus->getFilterPropertyHandler()->createProperties();
            m_propertyStatus->getVisualizationPropertyHandler()->createProperties();
            m_propertyStatus->getEventIDLimitationPropertyHandler()->createProperties();
            m_propertyStatus->getOutputPropertyHandler()->createProperties();

            if( m_colorBar == NULL )
            {
                createColorBar();
            }
            setToLoadedProperties();

            setOutputFromCSV();

            progressBar->finish();
        }
        else
        {
            if( m_input->getData() == NULL )
            {
                continue;
            }

            std::shared_ptr< WProgress > progressBar( new WProgress( "Updating output..." ) );
            m_progress->addSubProgress( progressBar );

            setOutputFromCSV();

            progressBar->finish();
        }
    }
    stop();
}

void WMFilterProtonData::connectors()
{
    m_input = WModuleInputData< WDataSetCSV >::createAndAdd( shared_from_this(), "in", "CSV_input" );

    m_output_points = WModuleOutputData< WDataSetPoints >::createAndAdd( shared_from_this(), "output points", "Output CSV data as Point data" );
    m_output_fibers = WModuleOutputData< WDataSetFibers >::createAndAdd( shared_from_this(), "output fibers",  "Output CSV data as Fiber data" );
    m_output_transferFunction = WModuleOutputData< WDataSetSingle >::create( shared_from_this(),
                                                                             "output transferfunction", "Output transfer function" );
    m_output_points_eventIds = WModuleOutputData< WDataSetPoints >::createAndAdd( shared_from_this(),
                                                    "output points and data",  "Output CSV data as points and additional data for PointConnector" );

    WModule::connectors();
}

void WMFilterProtonData::clearProperties()
{
    m_properties->removeProperty( m_properties->getProperty( "Select columns" ) );
    m_properties->removeProperty( m_properties->getProperty( "Filtering" ) );
    m_properties->removeProperty( m_properties->getProperty( "Visualization" ) );
    m_properties->removeProperty( m_properties->getProperty( "Event Id Limitation" ) );
    m_properties->removeProperty( m_properties->getProperty( "Output" ) );
}

void WMFilterProtonData::properties()
{
    // Creating dummy properties for loading of projects
    // TODO(eschbach) This is only a hotfix until property buffer is implemented
    // When changing property names they also have to be changed here, so the loading works.
    WPropertyGroup::SPtr groupColumn  = m_properties->addPropertyGroup( "Select columns", "Select the columns which should be used", false );
    WPropertyGroup::SPtr groupFilter  = m_properties->addPropertyGroup( "Filtering", "Filter primaries, secondaries and particle types", false );
    WPropertyGroup::SPtr groupVisual  = m_properties->addPropertyGroup( "Visualization", "Visualization options", false );
    WPropertyGroup::SPtr groupEventID = m_properties->addPropertyGroup( "Event Id Limitation", "Adjust the range of eventIDs to be shown.", false );
    WPropertyGroup::SPtr groupRename  = groupFilter->addPropertyGroup( "Rename Particle Types", "Filtering/Rename Particle Types", false );
    WPropertyGroup::SPtr groupOutput  = m_properties->addPropertyGroup( "Output", "Configures the third output", false );

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
    WPropertyBase::PropertyChangeNotifierType outputNotifier = boost::bind( &WMFilterProtonData::loadNotifier,
                                                                this, groupOutput, boost::placeholders::_1 );

    groupColumn->addProperty( "X", "Choose the column which should be used to determine the x coordinate.",
                            std::string( "" ), columnNotifier, false );
    groupColumn->addProperty( "Y", "Choose the column which should be used to determine the y coordinate.",
                            std::string( "" ), columnNotifier, false );
    groupColumn->addProperty( "Z", "Choose the column which should be used to determine the z coordinate.",
                            std::string( "" ), columnNotifier, false );
    groupColumn->addProperty( "Particle Data Group", "Choose the column which should be used to determine the particle data group.",
                            std::string( "" ), columnNotifier, false );
    groupColumn->addProperty( "Energy deposition", "Choose the column which should be used to determine the energy deposition.",
                            std::string( "" ), columnNotifier, false );
    groupColumn->addProperty( "Event id",
                            "Choose the column which should be used to determine the event id. Tracks will be drawn based on the the event id, all "
                            "particles with the same event id will be connected.", std::string( "" ), columnNotifier, false );
    groupColumn->addProperty( "Parent id", "Choose the column which should be used to determine the parent id."
                            "Primaries and secondaries filtering is based on that id,"
                            " if a particle has the parent id 0 it is a primary otherwise it is a secondary.",
                            std::string( "" ), columnNotifier, false );
    groupFilter->addProperty( "Show primaries", "Show or hide primaries. One can either hide primaries or secondaries,"
                            "but not both at the same time.", std::string( "" ), filterNotifier, false );
    groupFilter->addProperty( "Show secondaries", "Show or hide secondaries. One can either hide primaries or secondaries,"
                            "but not both at the same time.", std::string( "" ), filterNotifier, false );
    groupFilter->addProperty( "Show particles", "Choose particle type(s) to be shown.", std::string( "" ), filterNotifier, false );

    groupRename->addProperty( "New name (press enter)", "Type in a new name for the selected particle type."
                            "To submit your entry press enter while you are in the textbox.", std::string( "" ), renameNotifier, false );
    groupRename->addProperty( "Select particle", "Select the particle type to be renamed.", std::string( "" ), renameNotifier, false );
    groupRename->addProperty( "Apply Changes", "Save", std::string( "" ), renameNotifier, false );

    groupVisual->addProperty( "Enable cluster size", "Enables the edep to clustersize conversion", std::string( "" ), visualNotifier, false );
    groupVisual->addProperty( "Size by energy deposition", "Scale track and point sizes based on energy deposition.", std::string( "" ),
                            visualNotifier, false );
    groupVisual->addProperty( "Color by energy deposition", "Colorize tracks and points based on energy deposition.", std::string( "" ),
                            visualNotifier, false );
    groupVisual->addProperty( "Point color", "Points colorized in the chosen color when \"Color by energy deposition\" is disabled.",
                            std::string( "" ), visualNotifier, false );
    groupVisual->addProperty( "Gradient color", "Colorize tracks and points based on energy deposition with the configured gradient.",
                            std::string( "" ), visualNotifier, false );
    groupVisual->addProperty( "Set gradient", "Apply", std::string( "" ), visualNotifier, false );

    groupEventID->addProperty( "Minimum event id", "Filters out every event id which is lower than the set value.", std::string( "" ),
                            eventIDNotifier, false );
    groupEventID->addProperty( "Maximum event id", "Filters out every event id which is higher than the set value.", std::string( "" ),
                            eventIDNotifier, false );
    groupEventID->addProperty( "Set selection", "Apply", std::string( "" ), eventIDNotifier, false );

    groupOutput->addProperty( "Energy deposition", "Enables the energy deposition output", std::string( "" ), outputNotifier, false );
    groupOutput->addProperty( "Event id", "Enables the event id output", std::string( "" ), outputNotifier, false );

    WModule::properties();
}

void WMFilterProtonData::loadNotifier( WPropertyGroup::SPtr group, WPropertyBase::SPtr property )
{
    std::string name = group->getName() + "/" + property->getName();
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
    m_converter = std::shared_ptr< WCsvConverter >( new WCsvConverter( m_protonData, m_propertyStatus, m_colorBar ) );

    m_output_points->updateData( m_converter->getPoints() );
    m_output_fibers->updateData( m_converter->getFibers() );
    m_output_points_eventIds->updateData( m_converter->getPointsAndData() );
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
