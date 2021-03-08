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

#include <vector>
#include <string>

#include "WMCsvConverter.h"


WMCsvConverter::WMCsvConverter( WMProtonData::SPtr protonData,  boost::shared_ptr< WMPropertyStatus > propertyStatus )
{
    m_protonData = protonData;
    m_propertyStatus = propertyStatus;
    m_vectors = WMConverterVectors::SPtr( new WMConverterVectors() );
    m_indexes = WMConverterIndexes::SPtr( new WMConverterIndexes() );


    setOutputFromCSV( );
}

boost::shared_ptr< WDataSetFibers > WMCsvConverter::getFibers()
{
    return m_fibers;
}

boost::shared_ptr< WDataSetPoints > WMCsvConverter::getPoints()
{
    return m_points;
}

void WMCsvConverter::setOutputFromCSV( )
{
    if( !m_protonData->isRequiredDataAvailable() )
    {
        return;
    }

    WDataSetCSV::ContentSPtr data = m_protonData->getCSVData();

    WColor m_plainColor = m_propertyStatus->getVisualizationPropertyHandler()->getColorSelection()->get( true );

    m_vectors->clear();
    m_indexes->update( m_protonData );

    float maxEdep = 0.0;

    for( WDataSetCSV::Content::iterator dataRow = data->begin(); dataRow < data->end(); dataRow++ )
    {
        if( !checkConditionToPass( dataRow ) )
        {
            continue;
        }

        addVertex( dataRow );
        addColor( m_plainColor );
        addEdepAndSize( dataRow, &maxEdep );
        addEventID( dataRow );
    }

    if( checkIfOutputIsNull() )
    {
        return;
    }

    normalizeEdeps( m_vectors->getEdeps(), m_vectors->getColors() );

    createOutputPoints();
    createOutputFibers();
}

boost::shared_ptr< WDataSetSingle > WMCsvConverter::getTransferFunction()
{
    return m_transferFunction;
}

boost::shared_ptr< std::vector<unsigned char> > WMCsvConverter::sampleTransferFunction()
{
    boost::shared_ptr< std::vector<unsigned char> > data( new std::vector<unsigned char>( 10 * 4 ) );

    WTransferFunction tf = m_propertyStatus->getVisualizationPropertyHandler()->getTransferFunction()->get( true );

    tf.sample1DTransferFunction( &( *data )[ 0 ], 10, 0.0, 1.0 );

    return data;
}

void WMCsvConverter::normalizeEdeps( SPFloatVector edeps, SPFloatVector colorArray )
{
    if( m_protonData->isColumnAvailable( "edep" ) )
    {
        boost::shared_ptr< std::vector< unsigned char > > data = sampleTransferFunction();

        setTransferFunction( data );

        float maxClusterSize = getClusterSize( maxEdep );

        for( std::vector< float >::iterator currentEdep = edeps->begin();
            currentEdep != edeps->end();
            currentEdep++ )
        {
            float clusterSizeNormalized = getClusterSize( *currentEdep ) / maxClusterSize;

            m_vectors->getSizes()->push_back( clusterSizeNormalized );

            if( m_propertyStatus->getVisualizationPropertyHandler()->getColorFromEdep()->get() )
            {
                clusterSizeNormalized = static_cast< int >( 9 * clusterSizeNormalized );

                for( int i = 0; i < 4; i++ )
                {
                    colorArray->push_back( data->at( clusterSizeNormalized * 4 + i ) / 255.0 );
                }
            }
        }
    }
}

bool WMCsvConverter::checkConditionToPass( WDataSetCSV::Content::iterator dataRow )
{
    if( dataRow->empty() )
    {
        return false;
    }

    if( m_protonData->isColumnAvailable( "parentID" ) &&
        m_protonData->isColumnAvailable( "trackID" ) )
    {
        int PrimaryValue = ( int )stringToFloat( dataRow->at( m_indexes->getParentID() ) );

        if( !m_propertyStatus->getFilterPropertyHandler()->getShowPrimaries()->get() && PrimaryValue == 0 )
        {
            return false;
        }

        if( !m_propertyStatus->getFilterPropertyHandler()->getShowSecondaries()->get() && PrimaryValue != 0 )
        {
            return false;
        }
    }

    if( m_protonData->isColumnAvailable( "PDGEncoding" ) )
    {
        if( !m_propertyStatus->getFilterPropertyHandler()->isPDGTypeSelected(
            ( int )stringToFloat( dataRow->at( m_indexes->getPDGEncoding( ) ) ) ) )
        {
            return false;
        }
    }

    if( m_protonData->isColumnAvailable("eventID"))
    {
        if(dataRow->at( m_indexes->getEventID() ) == "NULL")
        {
            return true;
        }

        int eventID = ( int )stringToFloat( dataRow->at( m_indexes->getEventID() ) );
        if( eventID < m_propertyStatus->getEventIDLimitationPropertyHandler()->getMinCap()->get() ||
            eventID > m_propertyStatus->getEventIDLimitationPropertyHandler()->getMaxCap()->get() )
        {
            return false;
        }
    }

    return true;
}

void WMCsvConverter::addVertex( WDataSetCSV::Content::iterator dataRow )
{
    m_vectors->getVertices()->push_back( stringToFloat( dataRow->at( m_indexes->getPosX() ) ) );
    m_vectors->getVertices()->push_back( stringToFloat( dataRow->at( m_indexes->getPosY() ) ) );
    m_vectors->getVertices()->push_back( stringToFloat( dataRow->at( m_indexes->getPosZ() ) ) );
}

void WMCsvConverter::addColor( WColor plainColor )
{
    if(!m_protonData->isColumnAvailable("edep"))
    {
        m_vectors->getColors()->push_back( 0 );
        m_vectors->getColors()->push_back( 0 );
        m_vectors->getColors()->push_back( 0 );
    }

    if( !m_propertyStatus->getVisualizationPropertyHandler()->getColorFromEdep()->get() )
    {
        m_vectors->getColors()->push_back( plainColor[0] );
        m_vectors->getColors()->push_back( plainColor[1] );
        m_vectors->getColors()->push_back( plainColor[2] );
    }
}

void WMCsvConverter::addEdepAndSize( WDataSetCSV::Content::iterator dataRow, float* maxEdep )
{
    if( !m_protonData->isColumnAvailable( "edep" ) )
    {
        return;
    }

    float edep = stringToFloat( dataRow->at( m_indexes->getEdep() ) );
    if( edep > *maxEdep )
    {
        *maxEdep = edep;
    }

    m_vectors->getEdeps()->push_back( edep );
}

void WMCsvConverter::calculateFibers()
{
    if(!m_protonData->isColumnAvailable("eventID"))
    {
        return;
    }

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

void WMCsvConverter::createOutputPoints()
{
    if( m_protonData->isColumnAvailable( "edep" ) )
    {
        if( m_propertyStatus->getVisualizationPropertyHandler()->getSizesFromEdep()->get() )
        {
            m_points = boost::shared_ptr< WDataSetPointsAndSizes >(
                    new WDataSetPointsAndSizes(
                            m_vectors->getVertices(),
                            m_vectors->getColors(),
                            m_vectors->getSizes()
                    )
            );
            return;
        }
    }
    m_points = boost::shared_ptr < WDataSetPoints >(
                    new WDataSetPoints(
                    m_vectors->getVertices(),
                    m_vectors->getColors()
            )
    );
}

void WMCsvConverter::createOutputFibers()
{
    calculateFibers();
    if(!m_protonData->isColumnAvailable("eventID"))
    {
        m_fibers = boost::shared_ptr< WDataSetFibers >(
            new WDataSetFibers(
                    SPFloatVector( new std::vector< float >() ),
                    SPSizeVector( new std::vector< size_t >() ),
                    SPSizeVector( new std::vector< size_t >() ),
                    SPSizeVector( new std::vector< size_t >() )
            )
        );

        return;
    }
    else
    {
        m_fibers = boost::shared_ptr< WDataSetFibers >(
            new WDataSetFibers(
                    m_vectors->getVertices(),
                    m_vectors->getFiberStartIndexes(),
                    m_vectors->getFiberLengths(),
                    m_vectors->getVerticesReverse()
            )
        );
    }

    if(m_protonData->isColumnAvailable("edep"))
    {
        m_fibers->addColorScheme( m_vectors->getColors(), "Energy deposition", "Color fibers based on their energy." );
    }
}

void WMCsvConverter::addEventID( WDataSetCSV::Content::iterator dataRow )
{
    if(m_protonData->isColumnAvailable("eventID"))
        {
            if(dataRow->at( m_indexes->getEventID() ) == "NULL")
            {
                return;
            }

            m_vectors->getEventIDs()->push_back( ( int )stringToFloat( dataRow->at( m_indexes->getEventID() ) ) );
        }
}

void WMCsvConverter::setTransferFunction( boost::shared_ptr< std::vector<unsigned char> > data )
{
    boost::shared_ptr< WValueSetBase > newValueSet( new WValueSet<unsigned char>( 1, 4, data, W_DT_UNSIGNED_CHAR ) );

    WGridTransformOrtho transform;
    boost::shared_ptr< WGridRegular3D > newGrid( new WGridRegular3D( 10, 1, 1, transform ) );
    boost::shared_ptr< WDataSetSingle > newData( new WDataSetSingle( newValueSet, newGrid ) );

    m_transferFunction = newData;
}

bool WMCsvConverter::checkIfOutputIsNull()
{
    if( m_vectors->getVertices()->empty() )
    {
        m_points = NULL;
        m_fibers = NULL;
        return true;
    }
    return false;
}

float WMCsvConverter::getClusterSize( float edep )
{
    return 2.4 * pow( edep, 0.338 );
}

float WMCsvConverter::stringToFloat( std::string str )
{
    try
    {
        return boost::lexical_cast< float >( str );
    }
    catch( boost::bad_lexical_cast e )
    {
        throw WException( "The selected column has an incorrect format. Numbers are expected. " + std::string( e.what() ) );
    }
}
