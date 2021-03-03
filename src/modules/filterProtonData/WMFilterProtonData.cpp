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
                m_propertyStatus->getColumnPropertyHandler(), boost::bind( &WMFilterProtonData::setOutputFromCSV, this ) ) ) );

            m_propertyStatus->setVisualizationPropertyHandler( WMVisualizationPropertyHandler::SPtr(
                new WMVisualizationPropertyHandler( m_protonData, m_properties, boost::bind( &WMFilterProtonData::setOutputFromCSV, this ) ) ) );

            m_propertyStatus->setEventIDLimitationPropertyHandler( WMEventIDLimitationPropertyHandler::SPtr(
                new WMEventIDLimitationPropertyHandler( m_protonData, m_properties, boost::bind( &WMFilterProtonData::setOutputFromCSV, this ) ) ) );
        }
        else
        {
            m_protonData->setCSVHeader(  m_input->getData()->getHeader() );
            m_protonData->setCSVData(  m_input->getData()->getData() );
        }
        m_propertyStatus->getColumnPropertyHandler()->createProperties();
        m_propertyStatus->getFilterPropertyHandler()->createProperties();
        m_propertyStatus->getVisualizationPropertyHandler()->createProperties();
        m_propertyStatus->getEventIDLimitationPropertyHandler()->createProperties();
        setOutputFromCSV( );
    }
}

void WMFilterProtonData::connectors()
{
    m_input = WModuleInputData< WDataSetCSV >::createAndAdd( shared_from_this(), "in", "CSV_input" );

    m_output_points = WModuleOutputData< WDataSetPoints >::createAndAdd( shared_from_this(), "output points", "Output CSV data as Point data" );
    m_output_fibers = WModuleOutputData< WDataSetFibers >::createAndAdd( shared_from_this(), "output fibers",  "Output CSV data as Fiber data" );
    WModule::connectors();
}

void WMFilterProtonData::properties()
{
    WModule::properties();
}

void WMFilterProtonData::setOutputFromCSV( )
{
    m_converter = boost::shared_ptr< WMCsvConverter >( new WMCsvConverter( m_protonData, m_propertyStatus ) );

    m_output_points->updateData( m_converter->getPoints() );
    m_output_fibers->updateData( m_converter->getFibers() );
}
