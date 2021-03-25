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

#include <list>
#include <string>
#include <vector>

#include "core/common/WPathHelper.h"
#include "core/kernel/WKernel.h"

#include "WMWriteCSV.h"

W_LOADABLE_MODULE( WMWriteCSV )

WMWriteCSV::WMWriteCSV() :
    WModule()
{
}

WMWriteCSV::~WMWriteCSV()
{
    removeConnectors();
}

boost::shared_ptr< WModule > WMWriteCSV::factory() const
{
    return boost::shared_ptr< WModule >( new WMWriteCSV() );
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
    m_moduleState.add( m_PointsAndFibersInput->getDataChangedCondition() );

    ready();

    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting for data ...";

        m_moduleState.wait();
    }
}

void WMWriteCSV::connectors()
{
    m_PointsAndFibersInput = WModuleInputData< WDataSetPointsAndFibers >::createAndAdd(
                                                                            shared_from_this(),
                                                                            "Fibers_Points_in",
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
    boost::shared_ptr< WDataSetCSV > csvdataSet = m_CSVInput->getData();
    boost::shared_ptr< WDataSetPointsAndFibers > pointsAndfibersdataSet = m_PointsAndFibersInput->getData();

    if( !csvdataSet )
    {
        throw WException( "The Data-Modul-CSV-connection is missing." );
    }

    if( !pointsAndfibersdataSet )
    {
        throw WException( "The Point-Connector-connection is missing." );
    }

    writeToFile();
}

std::list< std::tuple < float, float, float, int > > WMWriteCSV::getListOfInternalVertex( WDataSetFibers::SPtr fibers )
{
    std::list< std::tuple < float, float, float, int > > listOfInternalVertex;

    WDataSetFibers::VertexArray vertices = fibers->getVertices();
    WDataSetFibers::IndexArray verticesReverse = fibers->getVerticesReverse();

    int vertexCounter = 0;
    int reverseCounter = 0;

    float vertexX;
    float vertexY;
    float vertexZ;

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

size_t WMWriteCSV::createStartCounter( std::list< std::tuple < float, float, float, int > > listOfInternalVertex )
{
    size_t eventIDcounter = 0;

    for( auto element = listOfInternalVertex.begin(); element != listOfInternalVertex.end(); element++  )
    {
        int selectedVertexIndex = std::get< 3 >( *element );

        if( eventIDcounter < selectedVertexIndex )
        {
            eventIDcounter = selectedVertexIndex;
        }
    }
    return eventIDcounter + 1;
}

void WMWriteCSV::writeToFile()
{
    WDataSetCSV::SeperatedRowSPtr csvContent = m_CSVInput->getData()->getRawDataSet();
    WDataSetFibers::SPtr fibers = m_PointsAndFibersInput->getData()->getFibers();
    std::list< std::tuple < float, float, float, int > > listOfInternalVertex = getListOfInternalVertex( fibers );
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

            int selectedVertexIndex = std::get< 3 >( *element );
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


