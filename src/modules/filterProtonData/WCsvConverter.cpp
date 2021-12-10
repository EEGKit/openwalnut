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
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <core/common/datastructures/WFiber.h>
#include <core/dataHandler/WDataSetFiberVector.h>

#include "WCsvConverter.h"

WCsvConverter::WCsvConverter( WProtonData::SPtr protonData, std::shared_ptr< WPropertyStatus > propertyStatus,
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

std::shared_ptr< WDataSetFibers > WCsvConverter::getFibers()
{
    return m_fibers;
}

std::shared_ptr< WDataSetPoints > WCsvConverter::getPoints()
{
    return m_points;
}

std::shared_ptr< WDataSetPoints > WCsvConverter::getPointsAndData()
{
    return m_pointsAndData;
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

    float maxEdep = wlimits::MIN_FLOAT;
    float minEdep = wlimits::MAX_FLOAT;

    for( WDataSetCSV::Content::iterator dataRow = data->begin(); dataRow < data->end(); dataRow++ )
    {
        if( !checkConditionToPass( dataRow ) )
        {
            continue;
        }

        if( m_protonData->isColumnAvailable( WSingleSelectorName::getEdep() ) )
        {
            float edep = stringToDouble( dataRow->at( m_indexes->getEdep() ) );

            if( m_propertyStatus->getVisualizationPropertyHandler()->getEnableClusterSize()->get() &&
                ( getClusterSize( edep ) < 1.0 || getClusterSize( edep ) > 35.0 ) )
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
    createOutputPointsAndData();
}

std::shared_ptr< WDataSetSingle > WCsvConverter::getTransferFunction()
{
    return m_transferFunction;
}

std::shared_ptr< std::vector<unsigned char> > WCsvConverter::sampleTransferFunction()
{
    std::shared_ptr< std::vector<unsigned char> > data( new std::vector<unsigned char>( 50 * 4 ) );

    WTransferFunction tf = m_propertyStatus->getVisualizationPropertyHandler()->getTransferFunction()->get( true );

    tf.sample1DTransferFunction( &( *data )[ 0 ], 50, 0.0, 1.0 );

    return data;
}

void WCsvConverter::normalizeEdeps( SPFloatVector edeps, SPFloatVector colorArray, float maxEdep, float minEdep )
{
    if( m_protonData->isColumnAvailable( WSingleSelectorName::getEdep() ) )
    {
        std::shared_ptr< std::vector< unsigned char > > data = sampleTransferFunction();

        setTransferFunction( data );

        bool clusterEnabled = m_propertyStatus->getVisualizationPropertyHandler()->getEnableClusterSize()->get();

        float maxClusterSize = clusterEnabled ? getClusterSize( maxEdep ) : maxEdep;
        float minClusterSize = clusterEnabled ? getClusterSize( minEdep ) : minEdep;

        for( std::vector< float >::iterator currentEdep = edeps->begin();
            currentEdep != edeps->end();
            currentEdep++ )
        {
            float clusterSizeNormalized = clusterEnabled ? getClusterSize( *currentEdep ) : *currentEdep;
            clusterSizeNormalized = ( clusterSizeNormalized - minClusterSize ) / ( maxClusterSize - minClusterSize );

            m_vectors->getSizes()->push_back( clusterSizeNormalized );

            if( m_propertyStatus->getVisualizationPropertyHandler()->getColorFromEdep()->get() )
            {
                clusterSizeNormalized = fmax( static_cast< int >( 49 * clusterSizeNormalized ), 0 );

                for( int i = 0; i < 4; i++ )
                {
                    colorArray->push_back( data->at( clusterSizeNormalized * 4 + i ) / 255.0 );
                }
            }
        }

        m_colorBar->getProperties()->getProperty( "Max scale value" )->set( 0.0 );
        m_colorBar->getProperties()->getProperty( "Max scale value" )->set( maxClusterSize );
        m_colorBar->getProperties()->getProperty( "Min scale value" )->set( minClusterSize );

        if( clusterEnabled )
        {
            m_colorBar->getProperties()->getProperty( "Description" )->set( std::string( "Clustersize " ) );
        }
        else
        {
            std::string columnName = m_protonData->getCSVHeader()->at( 0 ).at( m_indexes->getEdep() );
            m_colorBar->getProperties()->getProperty( "Description" )->set( columnName + " " );
        }

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
        m_vectors->getColors()->push_back( plainColor[3] );
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
    size_t skippedPoints = 0;
    std::shared_ptr< std::map< size_t, std::shared_ptr< WFiber > > > fibers( new std::map< size_t, std::shared_ptr< WFiber > >() );
    std::shared_ptr< std::map< size_t, SPFloatVector > > colors( new std::map< size_t, SPFloatVector >() );

    for( size_t i = 0; i < m_vectors->getEventIDs()->size(); i++ )
    {
        size_t eID = m_vectors->getEventIDs()->at( i );
        WPosition pos( m_vectors->getVertices()->at( i * 3 ), m_vectors->getVertices()->at( i * 3 + 1 ), m_vectors->getVertices()->at( i * 3 + 2 ) );

        std::shared_ptr< WFiber > fib;
        SPFloatVector col;

        if( fibers->find( eID ) != fibers->end() )
        {
            fib = fibers->operator[]( eID );
            col = colors->operator[]( eID );
        }
        else
        {
            fib = std::shared_ptr< WFiber >( new WFiber() );
            col = SPFloatVector( new std::vector< float > );
            fibers->operator[]( eID ) = fib;
            colors->operator[]( eID ) = col;
        }

        if( std::find( fib->begin(), fib->end(), pos ) == fib->end() )
        {
            fib->push_back( pos );
            col->push_back( m_vectors->getColors()->at( i * 4 ) );
            col->push_back( m_vectors->getColors()->at( i * 4 + 1 ) );
            col->push_back( m_vectors->getColors()->at( i * 4 + 2 ) );
            col->push_back( m_vectors->getColors()->at( i * 4 + 3 ) );
        }
        else
        {
            skippedPoints++;
        }
    }

    WDataSetFiberVector::SPtr newDS( new WDataSetFiberVector() );
    SPFloatVector cols = SPFloatVector( new std::vector< float > );
    for( auto it = fibers->begin(); it != fibers->end(); it++ )
    {
        if( it->second->size() > 1 )
        {
            newDS->push_back( *( it->second ) );
            SPFloatVector col = colors->operator[]( it->first );
            cols->insert( cols->end(), col->begin(), col->end() );
        }
    }

    m_fibers = newDS->toWDataSetFibers();
    m_fibers->addColorScheme( cols, "Energy deposition", "Color fibers based on their energy." );
    m_fibers->setSelectedColorScheme( 3 );

    if( skippedPoints > 0 )
    {
        wlog::warn( "WCsvConverter" ) << "Skipped " << skippedPoints << " duplicated points for \"WDataSetFibers\".";
    }
}

void WCsvConverter::createOutputPoints()
{
    if( m_protonData->isColumnAvailable( WSingleSelectorName::getEdep() ) )
    {
        if( m_propertyStatus->getVisualizationPropertyHandler()->getSizesFromEdep()->get() )
        {
            m_points = std::shared_ptr< WDataSetPoints >(
                    new WDataSetPoints(
                            m_vectors->getVertices(),
                            m_vectors->getColors(),
                            std::shared_ptr< WValueSet< float > >( new WValueSet< float >( 0, 1, m_vectors->getSizes() ) )
                    )
            );
            return;
        }
    }
    m_points = std::shared_ptr < WDataSetPoints >(
                    new WDataSetPoints(
                    m_vectors->getVertices(),
                    m_vectors->getColors()
            )
    );
}

void WCsvConverter::createOutputFibers()
{
    if( !m_protonData->isColumnAvailable( WSingleSelectorName::getEventId() ) )
    {
        m_fibers = std::shared_ptr< WDataSetFibers >(
            new WDataSetFibers(
                    SPFloatVector( new std::vector< float >() ),
                    SPSizeVector( new std::vector< size_t >() ),
                    SPSizeVector( new std::vector< size_t >() ),
                    SPSizeVector( new std::vector< size_t >() )
            )
        );

        return;
    }

    calculateFibers();
}

void WCsvConverter::createOutputPointsAndData()
{
    bool edep = m_propertyStatus->getOutputPropertyHandler()->getEnergyDeposition()->get() &&
                m_protonData->isColumnAvailable( WSingleSelectorName::getEdep() );

    bool eventID = m_propertyStatus->getOutputPropertyHandler()->getEventID()->get() &&
                   m_protonData->isColumnAvailable( WSingleSelectorName::getEventId() );

    if( edep && eventID )
    {
        std::tuple< SPFloatVector, SPSizeVector > data = std::make_tuple( m_vectors->getEdeps(), m_vectors->getEventIDs() );
        m_pointsAndData = WDataSetPoints::SPtr( new WDataSetPoints( m_vectors->getVertices(), m_vectors->getColors(), data ) );
    }
    else if( edep )
    {
        std::tuple< SPFloatVector > data = std::make_tuple( m_vectors->getEdeps() );
        m_pointsAndData = WDataSetPoints::SPtr( new WDataSetPoints( m_vectors->getVertices(), m_vectors->getColors(), data ) );
    }
    else if( eventID )
    {
        std::tuple< SPSizeVector > data = std::make_tuple( m_vectors->getEventIDs() );
        m_pointsAndData = WDataSetPoints::SPtr( new WDataSetPoints( m_vectors->getVertices(), m_vectors->getColors(), data ) );
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

void WCsvConverter::setTransferFunction( std::shared_ptr< std::vector<unsigned char> > data )
{
    std::shared_ptr< WValueSetBase > newValueSet( new WValueSet<unsigned char>( 1, 4, data, W_DT_UNSIGNED_CHAR ) );

    WGridTransformOrtho transform;
    std::shared_ptr< WGridRegular3D > newGrid( new WGridRegular3D( 50, 1, 1, transform ) );
    std::shared_ptr< WDataSetSingle > newData( new WDataSetSingle( newValueSet, newGrid ) );

    m_transferFunction = newData;
}

bool WCsvConverter::checkIfOutputIsNull()
{
    if( m_vectors->getVertices()->empty() )
    {
        m_points = NULL;
        m_fibers = NULL;
        m_pointsAndData = NULL;
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
