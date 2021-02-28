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


WMCsvConverter::WMCsvConverter(WMProtonData::SPtr protonData,  boost::shared_ptr< WMPropertyStatus > propertystatus)
{
    setOutputFromCSV(protonData, propertystatus);
}

boost::shared_ptr< WDataSetFibers > WMCsvConverter::getFibers()
{
    return m_fibers;
}

boost::shared_ptr< WDataSetPoints > WMCsvConverter::getPoints()
{
    return m_points;
}

void WMCsvConverter::normalizeEdeps( SPFloatVector edeps, SPFloatVector colorArray, float maxEdep )
{
    for( std::vector< float >::iterator currentEdep = edeps->begin(); currentEdep != edeps->end(); currentEdep++ )
    {
        *currentEdep = *currentEdep / maxEdep;

        colorArray->push_back( *currentEdep * 10 );
        colorArray->push_back( *currentEdep * 100 );
        colorArray->push_back( *currentEdep * 1 );
    }
}

bool WMCsvConverter::canShow( WDataSetCSV::Content::iterator dataRow )
{
    if( dataRow->empty() )
    {
        return false;
    }

    if( !m_propertyStatus->getShowPrimaries()->get() && std::stoi( dataRow->at( m_indexes->getParentID() ) ) == 0 )
    {
        return false;
    }

    if( !m_propertyStatus->getShowSecondaries()->get() && std::stoi( dataRow->at( m_indexes->getParentID() ) ) != 0 )
    {
        return false;
    }

    if( !m_propertyStatus->getColumnPropertyHandler()->isPDGTypeSelected( std::stoi( dataRow->at( m_indexes->getPDGEncoding() ) ) ) )
    {
        return false;
    }

    int eventID = std::stoi( dataRow->at( m_indexes->getEventID() ) );
    if( eventID < m_propertyStatus->getMinCap()->get() || eventID > m_propertyStatus->getMaxCap()->get() )
    {
        return false;
    }

    return true;
}

void WMCsvConverter::addVertexAndColor( WDataSetCSV::Content::iterator dataRow )
{
    m_vectors->getVertices()->push_back( boost::lexical_cast< float >( dataRow->at( m_indexes->getPosX() ) ) );
    m_vectors->getVertices()->push_back( boost::lexical_cast< float >( dataRow->at( m_indexes->getPosY() ) ) );
    m_vectors->getVertices()->push_back( boost::lexical_cast< float >( dataRow->at( m_indexes->getPosZ() ) ) );

    if( !m_propertyStatus->getColorFromEdep()->get() )
    {
        m_vectors->getColors()->push_back( m_plainColor[0] );
        m_vectors->getColors()->push_back( m_plainColor[1] );
        m_vectors->getColors()->push_back( m_plainColor[2] );
    }
}

void WMCsvConverter::addEdepAndSize( WDataSetCSV::Content::iterator dataRow, float* maxEdep )
{
    float edep = boost::lexical_cast< float >( dataRow->at( m_indexes->getEdep() ) );
    if( edep > *maxEdep )
    {
        *maxEdep = edep;
    }
    m_vectors->getSizes()->push_back( edep );
    m_vectors->getEdeps()->push_back( edep );
}

void WMCsvConverter::calculateFibers()
{
    int fiberLength = 0;
    int fiberStartIndex = 0;
    int reversePos = 0;
    size_t currentEventID = m_vectors->getEventIDs()->at( 0 );

    m_vectors->getFiberStartIndexes()->push_back( fiberStartIndex );

    for( size_t eID : *( m_vectors->getEventIDs() ) )
    {
        if( currentEventID != eID )
        {
            m_vectors->getFiberStartIndexes()->push_back( fiberStartIndex );
            m_vectors->getFiberLengths()->push_back( fiberLength );

            currentEventID = eID;
            fiberLength = 0;

            reversePos++;
        }
        fiberLength++;
        fiberStartIndex++;
        m_vectors->getVerticesReverse()->push_back( reversePos );
    }

    m_vectors->getFiberLengths()->push_back( fiberLength );
}

void WMCsvConverter::createPointsAndFibers()
{
    if( m_propertyStatus->getSizesFromEdep()->get() )
    {
        m_points = boost::shared_ptr< WDataSetPointsAndSizes >(
                new WDataSetPointsAndSizes(
                        m_vectors->getVertices(),
                        m_vectors->getColors(),
                        m_vectors->getSizes()
                )
        );
    }
    else
    {
        m_points = boost::shared_ptr < WDataSetPoints >(
                new WDataSetPoints(
                        m_vectors->getVertices(),
                        m_vectors->getColors()
                )
        );
    }

    m_fibers = boost::shared_ptr< WDataSetFibers >(
            new WDataSetFibers(
                    m_vectors->getVertices(),
                    m_vectors->getFiberStartIndexes(),
                    m_vectors->getFiberLengths(),
                    m_vectors->getVerticesReverse()
            )
    );

    m_fibers->addColorScheme( m_vectors->getColors(), "Energy deposition", "Color fibers based on their energy." );
}

void WMCsvConverter::setOutputFromCSV( )
{
    if( !m_protonData->columnsInitialized() )
    {
        return;
    }

    WDataSetCSV::ContentSPtr data = m_protonData->getCSVData();
    m_plainColor = m_propertyStatus->getColorSelection()->get( true );

    m_vectors->clear();
    m_indexes->update( m_protonData );

    float maxEdep = 0.0;

    for( WDataSetCSV::Content::iterator dataRow = data->begin(); dataRow < data->end(); dataRow++ )
    {
        if( !canShow( dataRow ) )
        {
            continue;
        }

        addVertexAndColor( dataRow );
        addEdepAndSize( dataRow, &maxEdep );

        m_vectors->getEventIDs()->push_back( std::stoi( dataRow->at( m_indexes->getEventID() ) ) );
    }

    if( m_propertyStatus->getColorFromEdep()->get() )
    {
        normalizeEdeps( m_vectors->getEdeps(), m_vectors->getColors(), maxEdep );
    }

    calculateFibers();
    createPointsAndFibers();
}