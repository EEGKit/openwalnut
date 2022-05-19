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

#include <fstream>
#include <list>
#include <memory>
#include <string>
#include <vector>

#include "WMWriteCSV.h"
#include "core/common/WPathHelper.h"
#include "core/kernel/WKernel.h"

W_LOADABLE_MODULE( WMWriteCSV )

WMWriteCSV::WMWriteCSV() :
    WModule()
{
}

WMWriteCSV::~WMWriteCSV()
{
    removeConnectors();
}

std::shared_ptr< WModule > WMWriteCSV::factory() const
{
    return std::shared_ptr< WModule >( new WMWriteCSV() );
}

const char** WMWriteCSV::getXPMIcon() const
{
    static const char * disc_xpm[] =
    {
                    "16 16 7 1", "   c None", ".  c #000080", "+  c #000000", "@  c #FFFF00", "#  c #E0E0E0",
                    "$  c #FFFFFF", "%  c #C0C0C0", "..+++++++++++..+", "..@@@@@@@@@@@..+", "..###########..+",
                    "..$$$$$$$$$$$..+", "..###########..+", "..$$$$$$$$$$$..+", "..###########..+", "..$$$$$$$$$$$..+",
                    "...............+", "....%%%%%%%....+", "....%..%%%%....+", "....%..%%%%....+", "....%..%%%%....+",
                    "....%..%%%%....+", "+...%%%%%%%....+", "++++++++++++++++"
    };
    return disc_xpm;
}

const std::string WMWriteCSV::getName() const
{
    return "Write CSV";
}

const std::string WMWriteCSV::getDescription() const
{
    return "Writes a connected data set to a CSV file.";
}

void WMWriteCSV::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_CSVInput->getDataChangedCondition() );
    m_moduleState.add( m_fibersInput->getDataChangedCondition() );

    ready();

    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting for data ...";

        m_moduleState.wait();
    }
}

void WMWriteCSV::connectors()
{
    m_fibersInput = WModuleInputData< WDataSetFibers >::createAndAdd(
                                                                shared_from_this(),
                                                                "Fibers_in",
                                                                "The dataset of the connected points" );
    m_CSVInput = WModuleInputData< WDataSetCSV >::createAndAdd(
                                                    shared_from_this(),
                                                    "CSV_in",
                                                    "The dataset to write in" );

    WModule::connectors();
}

void WMWriteCSV::properties()
{
    WPropertyBase::PropertyChangeNotifierType notifier = boost::bind(
        &WMWriteCSV::propertyCallback, this );

    m_filename = m_properties->addProperty( "Filename", "Filename where to write the NIfTI file to.", WPathHelper::getHomePath(), notifier );
    WPropertyHelper::PC_CONFIRMOVERWRITE::addTo( m_filename );

    WModule::properties();
}

void WMWriteCSV::propertyCallback()
{
    std::shared_ptr< WDataSetCSV > csvdataSet = m_CSVInput->getData();
    std::shared_ptr< WDataSetFibers > fibersdataSet = m_fibersInput->getData();

    if( !fibersdataSet )
    {
        throw WException( "The Point-Connector-connection is missing." );
    }

    if( !csvdataSet )
    {
        writeOnlyFibersToFile();
    }
    else
    {
        writeToFile();
    }
}

std::list< std::tuple < float, float, float, size_t > > WMWriteCSV::getListOfInternalVertex( WDataSetFibers::SPtr fibers )
{
    std::list< std::tuple < float, float, float, size_t > > listOfInternalVertex;

    WDataSetFibers::VertexArray vertices = fibers->getVertices();
    WDataSetFibers::IndexArray verticesReverse = fibers->getVerticesReverse();

    size_t vertexCounter = 0;
    size_t reverseCounter = 0;

    float vertexX = 0; // initialized to quiet avoid warning
    float vertexY = 0; // initialized to quiet avoid warning
    float vertexZ = 0; // initialized to quiet avoid warning

    for( size_t idx = 0; idx <= vertices->size(); idx++ )
    {
        switch( vertexCounter )
        {
            case 0: vertexX = vertices->at( idx ); break;
            case 1: vertexY = vertices->at( idx ); break;
            case 2: vertexZ = vertices->at( idx ); break;
            case 3:
            {
                osg::Vec3 vecVertex( vertexX, vertexY, vertexZ );
                listOfInternalVertex.push_back(
                    std::make_tuple(
                        vertexX,
                        vertexY,
                        vertexZ,
                        verticesReverse->at( reverseCounter++ ) ) );

                if( idx < vertices->size() )
                {
                    vertexX = vertices->at( idx );
                }

                vertexCounter = 0;
                break;
            }
        }
        vertexCounter++;
    }

    return listOfInternalVertex;
}

std::string WMWriteCSV::getPathToSave()
{
    std::string sourceFilename = m_filename->get().string();
    sourceFilename = sourceFilename.substr( 0, sourceFilename.find( ".csv" ) );
    return sourceFilename + ".csv";
}

std::string WMWriteCSV::getNewCSVHeader()
{
    std::vector< std::string > csvHeader = m_CSVInput->getData()->getHeader()->at( 0 );
    std::string newColumnName = "SelectedEventID";
    size_t counter = 1;
    while( std::find( csvHeader.begin(), csvHeader.end(), newColumnName ) != csvHeader.end() )
    {
        newColumnName = "SelectedEventID_" + boost::lexical_cast< std::string >( counter );
        counter++;
    }

    return newColumnName;
}

bool WMWriteCSV::contains( std::string sourceString, float num )
{
    std::stringstream ss;
    ss << num;
    return sourceString.find( ss.str() ) != std::string::npos;
}

size_t WMWriteCSV::createStartCounter( std::list< std::tuple < float, float, float, size_t > > listOfInternalVertex )
{
    size_t eventIDcounter = 0;

    for( auto element = listOfInternalVertex.begin(); element != listOfInternalVertex.end(); element++  )
    {
        size_t selectedVertexIndex = std::get< 3 >( *element );

        if( eventIDcounter < selectedVertexIndex )
        {
            eventIDcounter = selectedVertexIndex;
        }
    }
    return eventIDcounter + 1;
}

void WMWriteCSV::writeOnlyFibersToFile()
{
    std::list< std::tuple < float, float, float, size_t > > listOfInternalVertex = getListOfInternalVertex( m_fibersInput->getData() );
    std::ofstream newCSVFile( getPathToSave() );

    if( !newCSVFile.is_open() )
    {
        throw WException( "Could not create new CSV in the selected source folder" );
    }

    newCSVFile << "posX,posY,posZ,eventID" << std::endl;
    for( auto elm = listOfInternalVertex.begin(); elm != listOfInternalVertex.end(); elm++ )
    {
        float  posX = std::get< 0 >( *elm );
        float  posY = std::get< 1 >( *elm );
        float  posZ = std::get< 2 >( *elm );
        size_t evnt = std::get< 3 >( *elm );

        newCSVFile << posX << "," << posY << "," << posZ << "," << evnt << std::endl;
    }
    newCSVFile.close();
}

void WMWriteCSV::writeToFile()
{
    WDataSetCSV::SeperatedRowSPtr csvContent = m_CSVInput->getData()->getRawDataSet();
    std::list< std::tuple < float, float, float, size_t > > listOfInternalVertex = getListOfInternalVertex( m_fibersInput->getData() );
    std::ofstream newCSVFile( getPathToSave() );

    if( !newCSVFile.is_open() )
    {
        throw WException( "Could not create new CSV in the selected source folder" );
    }

    bool isMatch = false;
    size_t eventIDcounter = createStartCounter( listOfInternalVertex );

    //set new csv-header ( SelectedEventID )
    newCSVFile << csvContent->at( 0 ) << "," << getNewCSVHeader() << std::endl;

    //set new csv-content ( content of SelectedEventID )
    for( size_t row = 1; row < csvContent->size(); row++ )
    {
        std::string rowAsString = csvContent->at( row );

        for( auto element = listOfInternalVertex.begin(); element != listOfInternalVertex.end(); element++  )
        {
            float posX = std::get< 0 >( *element );
            if( !contains( rowAsString, posX ) )
            {
                continue;
            }

            float posY = std::get< 1 >( *element );
            if( !contains( rowAsString, posY ) )
            {
                continue;
            }

            float posZ = std::get< 2 >( *element );
            if( !contains( rowAsString, posZ ) )
            {
                continue;
            }

            size_t selectedVertexIndex = std::get< 3 >( *element );
            newCSVFile << rowAsString << "," << selectedVertexIndex << std::endl;

            isMatch = true;
            listOfInternalVertex.erase( element );
            break;
        }

        if( isMatch )
        {
            isMatch = false;
        }
        else
        {
            newCSVFile << rowAsString << "," << std::to_string( eventIDcounter++ )  << std::endl;
        }
    }

    newCSVFile.close();
}
