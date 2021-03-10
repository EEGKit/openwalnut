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
#include <list>

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

        // acquire data from the input connector
        boost::shared_ptr< WDataSetCSV > csvdataSet = m_CSVInput->getData();
        boost::shared_ptr< WDataSetPointsAndFibers > pointsAndfibersdataSet = m_PointsAndFibersInput->getData();

        if( !csvdataSet || !pointsAndfibersdataSet)
        {
            continue;
        }
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
        &writeToFile, this );

    m_filename = m_properties->addProperty( "Filename", "Filename where to write the NIfTI file to.", WPathHelper::getHomePath(), notifier );

    WModule::properties();
}

std::list< std::tuple < osg::Vec3, int > > WMWriteCSV::getListOfInternalVertex( WDataSetFibers::SPtr fibers )
{
    std::list< std::tuple < osg::Vec3, int > > listOfInternalVertex;

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
                listOfInternalVertex.push_back( std::make_tuple( vecVertex, verticesReverse->at( reverseCounter++ ) ) );

                if(idx < vertices->size())
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

std::list< std::tuple < int, int > > WMWriteCSV::getListOfPositionAndID(
    std::list< std::tuple < osg::Vec3, int > > listOfInternalVertex,
    WDataSetPoints::SPtr points )
{
    std::list< std::tuple < int, int > > listOfPositionAndID;

    for( size_t pos = 0; pos < points->size(); pos++ )
    {
        osg::Vec3 vertexPoints = points->operator[]( pos );
        for( auto element = listOfInternalVertex.begin(); element != listOfInternalVertex.end(); element++  )
        {
            osg::Vec3 selectedVertex = std::get< 0 >( *element );
            int selectedVertexIndex = std::get< 1 >( *element );

            if( vertexPoints.x() == selectedVertex.x() &&
                vertexPoints.y() == selectedVertex.y() &&
                vertexPoints.z() == selectedVertex.z() )
            {
                listOfInternalVertex.erase( element );
                listOfPositionAndID.push_back( std::make_tuple( pos, selectedVertexIndex ) );
                break;
            }
        }

        if(listOfInternalVertex.size() <= 0)
        {
            break;
        }
    }

    return listOfPositionAndID;
}



void WMWriteCSV::writeToFile()
{
    std::string sourceFilename = m_filename->get().string();
    sourceFilename = sourceFilename.substr( 0, sourceFilename.find( ".csv" ) );

    std::string outputFilename = sourceFilename + "-edited.csv";

    WDataSetCSV::UnseperatedRowSPtr csvContent = m_CSVInput->getData()->getRawDataSet();
    WDataSetFibers::SPtr fibers = m_PointsAndFibersInput->getData()->getFibers();
    WDataSetPoints::SPtr points = m_PointsAndFibersInput->getData()->getPoints();

    std::list< std::tuple < int, int > > listOfPositionAndID = getListOfPositionAndID(
                                                                    getListOfInternalVertex( fibers ),
                                                                    points );

    std::ofstream newCSVFile( outputFilename );

    if( !newCSVFile.is_open() )
    {
        throw WException( "Could not create new CSV in the selected source folder" );
    }

    bool isMatch = false;

    newCSVFile << csvContent->at( 0 ) << "," << "SelectedEventID" << std::endl;
    csvContent->erase( csvContent->begin() );

    for( size_t row = 0; row < csvContent->size(); row++ )
    {
        for( auto element = listOfPositionAndID.begin(); element != listOfPositionAndID.end(); element++  )
        {
            size_t selectedPosition = std::get< 0 >( *element );
            int selectedVertexIndex = std::get< 1 >( *element );

            if( row == selectedPosition )
            {
                newCSVFile << csvContent->at( row ) << "," << selectedVertexIndex << std::endl;

                listOfPositionAndID.erase( element );
                isMatch = true;
                break;
            }
        }

        if( isMatch )
        {
            isMatch = false;
        }
        else
        {
            newCSVFile << csvContent->at( row ) << ",-1"  << std::endl;
        }
    }

    newCSVFile.close();
}
