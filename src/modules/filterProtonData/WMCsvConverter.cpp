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

#include "WMCsvConverter.h"


WMCsvConverter::WMCsvConverter()
{
}

boost::shared_ptr< WDataSetFibers > WMCsvConverter::getFibers()
{
    return m_fibers;
}

boost::shared_ptr< WDataSetPoints > WMCsvConverter::getPoints()
{
    return m_points;
}

void WMCsvConverter::normalizeEdeps( std::vector< float > edeps, SPFloatVector colorArray, float maxEdep )
{
    for( std::vector< float >::iterator currentEdep = edeps.begin(); currentEdep != edeps.end(); currentEdep++ )
    {
        *currentEdep = *currentEdep / maxEdep;

        colorArray->push_back( *currentEdep * 10 );
        colorArray->push_back( *currentEdep * 100 );
        colorArray->push_back( *currentEdep * 1 );
    }
}

void WMCsvConverter::setOutputFromCSV( WMProtonData::SPtr protonData,  boost::shared_ptr< WMPropertyStatus > propertystatus)
{
    if( !protonData->columnsInitialized() )
    {
        return;
    }

    SPFloatVector vertices = SPFloatVector( new std::vector< float >() );
    SPSizeVector lineStartIndexes = SPSizeVector( new std::vector < size_t >() );
    SPSizeVector lineLengths  = SPSizeVector( new std::vector < size_t >() );
    SPSizeVector verticesReverse = SPSizeVector( new std::vector < size_t >() );
    SPFloatVector sizes = SPFloatVector( new std::vector< float >() );
    SPFloatVector colors = SPFloatVector( new std::vector< float >() );

    std::vector< int > eventIDs;
    std::vector< float > edeps;

    WColor plainColor = propertystatus->getColorSelection()->get( true );
    

    float maxEdep = 0.0;
    WDataSetCSV::ContentSPtr data = protonData->getCSVData();
    int parentIDIndex = protonData->getColumnIndex( "parentID" );
    int posXIndex = protonData->getColumnIndex( "posX" );
    int posYIndex = protonData->getColumnIndex( "posY" );
    int posZIndex = protonData->getColumnIndex( "posZ" );
    int edepIndex = protonData->getColumnIndex( "edep" );
    int PDGEncodingIndex = protonData->getColumnIndex( "PDGEncoding" );
    int eventIDIndex = protonData->getColumnIndex( "eventID" );

    float posX, posY, posZ, edep;
    int eventID, pdgType;

    for( WDataSetCSV::Content::iterator dataRow = data->begin(); dataRow < data->end(); dataRow++ )
    {

        if( dataRow->empty() )
        {
            continue;
        }
        
        if( !propertystatus->getShowPrimaries()->get() && std::stoi( dataRow->at( parentIDIndex ) ) == 0 )
        {
            continue;
        }

        if( !propertystatus->getShowSecondaries()->get() && std::stoi( dataRow->at( parentIDIndex ) ) != 0 )
        {
            continue;
        }

        posX = boost::lexical_cast< float >( dataRow->at( posXIndex ) );
        posY = boost::lexical_cast< float >( dataRow->at( posYIndex ) );
        posZ = boost::lexical_cast< float >( dataRow->at( posZIndex ) );
        edep = boost::lexical_cast< float >( dataRow->at( edepIndex ) );

        pdgType = std::stoi( dataRow->at( PDGEncodingIndex ) );
        eventID = std::stoi( dataRow->at( eventIDIndex ) );
        

        
        if( !propertystatus->getColumnPropertyHandler()->isPDGTypeSelected( pdgType ) )
        {
            continue;
        }

        
        if( eventID < propertystatus->getMinCap()->get() || eventID > propertystatus->getMaxCap()->get() ) 
        {
            continue;
        }

        if( edep > maxEdep )
        {
            maxEdep = edep;
        }

        vertices->push_back( posX );
        vertices->push_back( posY );
        vertices->push_back( posZ );
        
        if( !propertystatus->getColorFromEdep()->get() )
        {
            colors->push_back( plainColor[0] );
            colors->push_back( plainColor[1] );
            colors->push_back( plainColor[2] );
        }

        sizes->push_back( edep );
        eventIDs.push_back( eventID );
        edeps.push_back( edep );
    }

    if( propertystatus->getColorFromEdep()->get() )
    {
        normalizeEdeps( edeps, colors, maxEdep );
    }

    int fiberLength = 0;
    int fiberStartIndex = 0;
    int reversePos = 0;
    int currentEventID = eventIDs.at( 0 );

    lineStartIndexes->push_back( fiberStartIndex );

    for(std::vector<int>::iterator eID = eventIDs.begin(); eID != eventIDs.end(); eID++ )
    {
        if( currentEventID != *eID )
        {
            currentEventID = *eID;
            lineStartIndexes->push_back( fiberStartIndex );
            lineLengths->push_back( fiberLength );
            fiberLength = 0;
            reversePos++;
        }
        fiberLength++;
        fiberStartIndex++;
        verticesReverse->push_back( reversePos );
    }

    lineLengths->push_back( fiberLength );


    if( propertystatus->getSizesFromEdep()->get() )
    {
        m_points = boost::shared_ptr< WDataSetPointsAndSizes >(
                new WDataSetPointsAndSizes(
                        vertices,
                        colors,
                        sizes
                )
        );
    }
    else
    {
        m_points = boost::shared_ptr < WDataSetPoints >(
                new WDataSetPoints(
                        vertices,
                        colors
                        )
                );
    }

    m_fibers = boost::shared_ptr< WDataSetFibers >(
            new WDataSetFibers(
                    vertices,
                    lineStartIndexes,
                    lineLengths,
                    verticesReverse
            )
    );

    m_fibers->addColorScheme( colors, "Energy deposition", "Color fibers based on their energy." );

}