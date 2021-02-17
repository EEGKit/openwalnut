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

        boost::shared_ptr< WDataSetCSV > dataset = m_input->getData();
        WDataSetCSV::Content m_csvHeader = dataset->getHeader();
        WDataSetCSV::Content m_csvData = dataset->getData();

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

void WMConverterCSV::properties()
{
}

int WMConverterCSV::getColumnNumberByName( std::string columnNameToMatch, std::vector<std::string> headerToSearchIn )
{
    int pos = 0;
    for( std::vector<std::string>::iterator it = headerToSearchIn.begin(); it != headerToSearchIn.end(); it++ )
    {
        if( *it == columnNameToMatch ) return pos;
        pos++;
    }
    return pos;
}


void WMConverterCSV::setFibersOutOfCSVData( WDataSetCSV::Content header, WDataSetCSV::Content data )
{
    debugLog() << "Jump IN";
    SPFloatVector m_vertices = SPFloatVector( new std::vector<float>() );
    SPSizeVector m_lineStartIndexes = SPSizeVector( new std::vector < size_t >() );
    SPSizeVector m_lineLengths  = SPSizeVector( new std::vector < size_t >() );
    SPSizeVector m_verticesReverse = SPSizeVector( new std::vector < size_t >() );

    std::vector< int > eventIDs;

    int xPosIndex = getColumnNumberByName( "posX", header.at( 0 ) );
    int yPosIndex = getColumnNumberByName( "posY", header.at( 0 ) );
    int zPosIndex = getColumnNumberByName( "posZ", header.at( 0 ) );
    int eventIDIndex = getColumnNumberByName( "currentEventID", header.at( 0 ) );
    int parentIDIndex = getColumnNumberByName( "parentID", header.at( 0 ) );

    for( WDataSetCSV::Content::iterator dataRow = data.begin(); dataRow != data.end(); dataRow++ )
    {
        float posX, posY, posZ;
        int eventID;

        if( dataRow->empty() )
        {
            continue;
        }

        if(std::stoi( dataRow->at(parentIDIndex ) ) != 0 )
        {
            continue;
        }

        posX = std::stof( dataRow->at( xPosIndex ) );
        posY = std::stof( dataRow->at( yPosIndex ) );
        posZ = std::stof( dataRow->at( zPosIndex ) );
        eventID = std::stoi( dataRow->at( eventIDIndex ) );

        m_vertices->push_back( posX );
        m_vertices->push_back( posY );
        m_vertices->push_back( posZ );
        eventIDs.push_back( eventID );
    }

    int fiberLength = 0;
    int fiberStartIndex = 0;
    int reversePos = 0;
    int currentEventID = eventIDs.at( 0 );

    m_lineStartIndexes->push_back( fiberStartIndex );

    for(std::vector<int>::iterator eID = eventIDs.begin(); eID != eventIDs.end(); eID++ )
    {
        debugLog() << "Hier " << *eID;
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

    m_output_fibers->updateData( m_fibers );
}

void WMConverterCSV::setPointsOutOfCSVData( WDataSetCSV::Content header, WDataSetCSV::Content data )
{
    SPFloatVector m_vertices = SPFloatVector( new std::vector< float >() );
    SPFloatVector m_colors = SPFloatVector( new std::vector< float >() );

    int xpos_arr_col = getColumnNumberByName( "posX", header.at( 0 ) );
    int ypos_arr_col = getColumnNumberByName( "posY", header.at( 0 ) );
    int zpos_arr_col = getColumnNumberByName( "posZ", header.at( 0 ) );

    float posX, posY, posZ;
    for( WDataSetCSV::Content::iterator it = data.begin(); it != data.end(); it++ )
    {
        int count = 0;
        int finish_flag = 0;

        if( it->empty() )
        {
            continue;
        }

        for( std::vector<std::string>::iterator it_inner = it->begin(); it_inner != it->end(); it_inner++ )
        {
            if( count == xpos_arr_col )
            {
                posX = std::stof( *it_inner );
                finish_flag++;
            }
            if( count == ypos_arr_col )
            {
                posY = std::stof( *it_inner );
                finish_flag++;
            }
            if( count == zpos_arr_col )
            {
                posZ = std::stof( *it_inner );
                finish_flag++;
            }
            if( finish_flag == 3 )
                break;
            count++;
        }

        m_vertices->push_back( posX );
        m_vertices->push_back( posY );
        m_vertices->push_back( posZ );

        m_colors->push_back( 0 );
        m_colors->push_back( 0 );
        m_colors->push_back( 0 );
    }

    m_points = boost::shared_ptr< WDataSetPoints >(
            new WDataSetPoints(
                    m_vertices,
                    m_colors
            )
    );

    m_output_points->updateData( m_points );
}
