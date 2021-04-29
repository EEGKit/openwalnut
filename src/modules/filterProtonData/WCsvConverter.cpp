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

#include <cmath>
#include <string>
#include <vector>

#include "WCsvConverter.h"

WCsvConverter::WCsvConverter( WProtonData::SPtr protonData, boost::shared_ptr< WPropertyStatus > propertyStatus,
                                WModule::SPtr colorBar )
{
    if( protonData == nullptr || propertyStatus == nullptr || colorBar == nullptr )
    {
        throw WException( "Could not convert CSV data! Proton data, property status or color bar are missing!" );
    }

    m_protonData = protonData;
    m_propertyStatus = propertyStatus;
    m_vectors = WConverterVectors::SPtr( new WConverterVectors() );
    m_indexes = WConverterIndexes::SPtr( new WConverterIndexes() );
    m_colorBar = colorBar;

    setOutputFromCSV( );
}

boost::shared_ptr< WDataSetFibers > WCsvConverter::getFibers()
{
    return m_fibers;
}

boost::shared_ptr< WDataSetPoints > WCsvConverter::getPoints()
{
    return m_points;
}

boost::shared_ptr< WDataSetPoints > WCsvConverter::getPointsAndIDs()
{
    return m_selectedEventIDs;
}

void WCsvConverter::setOutputFromCSV( )
{
    if( !m_protonData->isColumnAvailable( WSingleSelectorName::getX() ) ||
        !m_protonData->isColumnAvailable( WSingleSelectorName::getY() ) ||
        !m_protonData->isColumnAvailable( WSingleSelectorName::getZ() ) )
    {
        return;
    }

    WDataSetCSV::ContentSPtr data = m_protonData->getCSVData();

    WColor plainColor = m_propertyStatus->getVisualizationPropertyHandler()->getColorSelection()->get( true );

    m_vectors->clear();
    m_indexes->update( m_protonData );

    float maxEdep = 0.0;
    float minEdep = 1.0;

    for( WDataSetCSV::Content::iterator dataRow = data->begin(); dataRow < data->end(); dataRow++ )
    {
        if( !checkConditionToPass( dataRow ) )
        {
            continue;
        }

        if( m_protonData->isColumnAvailable( WSingleSelectorName::getEdep() ) )
        {
            float edep = stringToDouble( dataRow->at( m_indexes->getEdep() ) );

            if( getClusterSize( edep ) < 1.0 || getClusterSize( edep ) > 35.0 )
            {
                continue;
            }
        }

        addVertex( dataRow );
        addColor( plainColor );
        addEdepAndSize( dataRow, &maxEdep, &minEdep );
        addEventID( dataRow );
    }

    if( checkIfOutputIsNull() )
    {
        return;
    }

    normalizeEdeps( m_vectors->getEdeps(), m_vectors->getColors(),  maxEdep, minEdep );

    createOutputPoints();
    createOutputFibers();
    createOutputPointsAndEventIDs();
}

boost::shared_ptr< WDataSetSingle > WCsvConverter::getTransferFunction()
{
    return m_transferFunction;
}

boost::shared_ptr< std::vector<unsigned char> > WCsvConverter::sampleTransferFunction()
{
    boost::shared_ptr< std::vector<unsigned char> > data( new std::vector<unsigned char>( 50 * 4 ) );

    WTransferFunction tf = m_propertyStatus->getVisualizationPropertyHandler()->getTransferFunction()->get( true );

    tf.sample1DTransferFunction( &( *data )[ 0 ], 50, 0.0, 1.0 );

    return data;
}

void WCsvConverter::normalizeEdeps( SPFloatVector edeps, SPFloatVector colorArray, float maxEdep, float minEdep )
{
    if( m_protonData->isColumnAvailable( WSingleSelectorName::getEdep() ) )
    {
        boost::shared_ptr< std::vector< unsigned char > > data = sampleTransferFunction();

        setTransferFunction( data );

        float maxClusterSize = getClusterSize( maxEdep );
        float minClusterSize = getClusterSize( minEdep );

        for( std::vector< float >::iterator currentEdep = edeps->begin();
            currentEdep != edeps->end();
            currentEdep++ )
        {
            float clusterSizeNormalized = getClusterSize( *currentEdep ) / maxClusterSize;

            m_vectors->getSizes()->push_back( clusterSizeNormalized );

            if( m_propertyStatus->getVisualizationPropertyHandler()->getColorFromEdep()->get() )
            {
                clusterSizeNormalized = static_cast< int >( 49 * clusterSizeNormalized );

                for( int i = 0; i < 4; i++ )
                {
                    colorArray->push_back( data->at( clusterSizeNormalized * 4 + i ) / 255.0 );
                }
            }
        }

        m_colorBar->getProperties()->getProperty( "Max scale value" )->set( 0.0 );
        m_colorBar->getProperties()->getProperty( "Max scale value" )->set( maxClusterSize );
        m_colorBar->getProperties()->getProperty( "Min scale value" )->set( minClusterSize );
        m_colorBar->getProperties()->getProperty( "Description" )->set( std::string( "Clustersize " ) );

        bool activated = m_propertyStatus->getVisualizationPropertyHandler()->getColorFromEdep()->get();

        m_colorBar->getProperties()->getProperty( "active" )->toPropBool()->set( activated );
    }
}

bool WCsvConverter::checkConditionToPass( WDataSetCSV::Content::iterator dataRow )
{
    if( dataRow->empty() )
    {
        return false;
    }

    if( m_protonData->isColumnAvailable( WSingleSelectorName::getParentId() ) )
    {
        int PrimaryValue = stringToInt( dataRow->at( m_indexes->getParentID() ) );

        if( !m_propertyStatus->getFilterPropertyHandler()->getShowPrimaries()->get() && PrimaryValue == 0 )
        {
            return false;
        }

        if( !m_propertyStatus->getFilterPropertyHandler()->getShowSecondaries()->get() && PrimaryValue != 0 )
        {
            return false;
        }
    }

    if( m_protonData->isColumnAvailable( WSingleSelectorName::getPDG() ) )
    {
        if( !m_propertyStatus->getFilterPropertyHandler()->isPDGTypeSelected(
           ( int )stringToDouble( dataRow->at( m_indexes->getPDGEncoding( ) ) ) ) )
        {
            return false;
        }
    }

    if( m_protonData->isColumnAvailable( WSingleSelectorName::getEventId() ) )
    {
        if( dataRow->at( m_indexes->getEventID() ) == "NULL" )
        {
            return true;
        }

        int eventID = stringToInt( dataRow->at( m_indexes->getEventID() ) );
        if( eventID < m_propertyStatus->getEventIDLimitationPropertyHandler()->getMinCap()->get() ||
            eventID > m_propertyStatus->getEventIDLimitationPropertyHandler()->getMaxCap()->get() )
        {
            return false;
        }
    }

    return true;
}

void WCsvConverter::addVertex( WDataSetCSV::Content::iterator dataRow )
{
    m_vectors->getVertices()->push_back( stringToDouble( dataRow->at( m_indexes->getPosX() ) ) );
    m_vectors->getVertices()->push_back( stringToDouble( dataRow->at( m_indexes->getPosY() ) ) );
    m_vectors->getVertices()->push_back( stringToDouble( dataRow->at( m_indexes->getPosZ() ) ) );
}

void WCsvConverter::addColor( WColor plainColor )
{
    if( !m_protonData->isColumnAvailable( WSingleSelectorName::getEdep() ) ||
        !m_propertyStatus->getVisualizationPropertyHandler()->getColorFromEdep()->get() )
    {
        m_vectors->getColors()->push_back( plainColor[0] );
        m_vectors->getColors()->push_back( plainColor[1] );
        m_vectors->getColors()->push_back( plainColor[2] );
    }
}

void WCsvConverter::addEdepAndSize( WDataSetCSV::Content::iterator dataRow, float* maxEdep, float* minEdep )
{
    if( !m_protonData->isColumnAvailable( WSingleSelectorName::getEdep() ) )
    {
        return;
    }

    float edep = stringToDouble( dataRow->at( m_indexes->getEdep() ) );
    if( edep > *maxEdep )
    {
        *maxEdep = edep;
    }

    if( edep < *minEdep )
    {
        *minEdep = edep;
    }

    m_vectors->getEdeps()->push_back( edep );
}

void WCsvConverter::calculateFibers()
{
    if( !m_protonData->isColumnAvailable( WSingleSelectorName::getEventId() ) )
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

void WCsvConverter::createOutputPoints()
{
    if( m_protonData->isColumnAvailable( WSingleSelectorName::getEdep() ) )
    {
        if( m_propertyStatus->getVisualizationPropertyHandler()->getSizesFromEdep()->get() )
        {
            m_points = boost::shared_ptr< WDataSetPoints >(
                    new WDataSetPoints(
                            m_vectors->getVertices(),
                            m_vectors->getColors(),
                            boost::shared_ptr< WValueSet< float > >( new WValueSet< float >( 0, 1, m_vectors->getSizes() ) )
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

void WCsvConverter::createOutputFibers()
{
    calculateFibers();
    if( !m_protonData->isColumnAvailable( WSingleSelectorName::getEventId() ) )
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

    if( m_protonData->isColumnAvailable( WSingleSelectorName::getEdep() ) )
    {
        if( m_propertyStatus->getVisualizationPropertyHandler()->getColorFromEdep()->get() )
        {
            m_fibers->removeColorScheme( m_fibers->getColorScheme( "Global Color" )->getColor() );
            m_fibers->removeColorScheme( m_fibers->getColorScheme( "Local Color" )->getColor() );
            m_fibers->removeColorScheme( m_fibers->getColorScheme( "Custom Color" )->getColor() );
            m_fibers->addColorScheme( m_vectors->getColors(), "Energy deposition",
                                      "Color fibers based on their energy." );
        }
    }
}

void WCsvConverter::createOutputPointsAndEventIDs()
{
    if( m_protonData->isColumnAvailable( WSingleSelectorName::getEventId() ) )
    {
        m_selectedEventIDs = boost::shared_ptr < WDataSetPoints >(
                new WDataSetPoints(
                        m_vectors->getVertices(),
                        m_vectors->getColors(),
                        boost::shared_ptr< WValueSet< int > >( new WValueSet< int >( 0, 1, m_vectors->getEventIDs() ) )
                )
        );
    }
}

void WCsvConverter::addEventID( WDataSetCSV::Content::iterator dataRow )
{
    if( m_protonData->isColumnAvailable( WSingleSelectorName::getEventId() ) )
    {
        if( dataRow->at( m_indexes->getEventID() ) == "NULL" )
        {
            return;
        }

        m_vectors->getEventIDs()->push_back( stringToInt( dataRow->at( m_indexes->getEventID() ) ) );
    }
}

void WCsvConverter::setTransferFunction( boost::shared_ptr< std::vector<unsigned char> > data )
{
    boost::shared_ptr< WValueSetBase > newValueSet( new WValueSet<unsigned char>( 1, 4, data, W_DT_UNSIGNED_CHAR ) );

    WGridTransformOrtho transform;
    boost::shared_ptr< WGridRegular3D > newGrid( new WGridRegular3D( 50, 1, 1, transform ) );
    boost::shared_ptr< WDataSetSingle > newData( new WDataSetSingle( newValueSet, newGrid ) );

    m_transferFunction = newData;
}

bool WCsvConverter::checkIfOutputIsNull()
{
    if( m_vectors->getVertices()->empty() )
    {
        m_points = NULL;
        m_fibers = NULL;
        m_selectedEventIDs = NULL;
        return true;
    }
    return false;
}

float WCsvConverter::getClusterSize( float edep )
{
    return 7.6626f * powf( edep * 40.0f, 0.420307f );
}

float WCsvConverter::stringToDouble( std::string str )
{
    try
    {
        return boost::lexical_cast< double >( str );
    }
    catch( const boost::bad_lexical_cast &e )
    {
        std::string errorMessage = "The selected column has an incorrect format. Received: " + str + ". " +
                                        "Required: Numbers are expected. " +
                                        std::string( e.what() );

        throw WException( errorMessage );
    }
}

int WCsvConverter::stringToInt( std::string str )
{
    //lexical_cast <int> cannot cast numbers as exponential notation, so we take this way.
    float numAsDouble = stringToDouble( str );
    return ( int )numAsDouble;
}
