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

#include <algorithm>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "WMImageExtractor.h"
#include "WMImageExtractor.xpm"
#include "core/common/WPropertyHelper.h"
#include "core/dataHandler/WDataHandler.h"
#include "core/dataHandler/WDataTexture3D.h"
#include "core/graphicsEngine/WGEColormapping.h"
#include "core/kernel/WKernel.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMImageExtractor )

WMImageExtractor::WMImageExtractor():
    WModule()
{
}

WMImageExtractor::~WMImageExtractor()
{
}

std::shared_ptr< WModule > WMImageExtractor::factory() const
{
    return std::shared_ptr< WModule >( new WMImageExtractor() );
}

const std::string WMImageExtractor::getName() const
{
    return "Image Extractor";
}

const std::string WMImageExtractor::getDescription() const
{
    return "This module allows extracting of single images from a dataset.";
}

const char** WMImageExtractor::getXPMIcon() const
{
    return imageExtractor_xpm;
}

void WMImageExtractor::connectors()
{
    m_input = std::shared_ptr< WModuleInputData < WDataSetSingle  > >(
        new WModuleInputData< WDataSetSingle >( shared_from_this(), "in", "The input dataset." ) );
    addConnector( m_input );

    m_output = std::shared_ptr< WModuleOutputData < WDataSetScalar  > >(
        new WModuleOutputData< WDataSetScalar >( shared_from_this(), "out", "The extracted image." ) );
    addConnector( m_output );

    WModule::connectors();
}

void WMImageExtractor::properties()
{
    m_propCondition = std::shared_ptr< WCondition >( new WCondition() );

    m_selectedImage = m_properties->addProperty( "Image", "The image to be extracted.", 0, m_propCondition );
    m_selectedImage->setMin( 0 );
    m_selectedImage->setMax( 0 );

    m_minValuePct = m_properties->addProperty( "Min %", "The data value percent that maps to 0.0 in the texture.", 0.0, m_propCondition );
    m_minValuePct->setMin( 0.0 );
    m_minValuePct->setMax( 100.0 );

    m_maxValuePct = m_properties->addProperty( "Max %", "The data value percent that maps to 1.0 in the texture.", 100.0, m_propCondition );
    m_maxValuePct->setMin( 0.0 );
    m_maxValuePct->setMax( 100.0 );

    WModule::properties();
}

void WMImageExtractor::activate()
{
    if( m_outData )
    {
        m_outData->getTexture()->active()->set( m_active->get() );
    }
    WModule::activate();
}

void WMImageExtractor::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    ready();

    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if( m_shutdownFlag() )
        {
            break;
        }

        std::shared_ptr< WDataSetSingle > newDataSet = m_input->getData();
        bool dataChanged = ( m_dataSet != newDataSet );
        bool dataValid   = ( newDataSet != NULL );

        if( dataValid )
        {
            if( dataChanged || m_selectedImage->changed() || m_minValuePct->changed() || m_maxValuePct->changed() )
            {
                m_dataSet = newDataSet;
                WAssert( m_dataSet, "" );
                WAssert( m_dataSet->getValueSet(), "" );

                if( dataChanged )
                {
                    m_selectedImage->setMax( m_dataSet->getValueSet()->dimension() - 1 );
                    m_selectedImage->ensureValidity( 0 );
                }

                // remove the old dataset's properties and de-register from colormapper
                if( m_outData )
                {
                    m_properties->removeProperty( m_outData->getTexture()->getProperties() );
                    m_infoProperties->removeProperty( m_outData->getTexture()->getInformationProperties() );
                }

                std::size_t i = static_cast< std::size_t >( m_selectedImage->get( true ) );

                std::shared_ptr< WDataSetScalar > oldOut = m_outData;
                m_outData = extract( i );

                if( m_outData )
                {
                    setOutputProps();
                    m_outData->setFilename( makeImageName( i ) );
                    // provide the texture's properties as own properties
                    m_properties->addProperty( m_outData->getTexture()->getProperties() );
                    m_infoProperties->addProperty( m_outData->getTexture()->getInformationProperties() );
                }

                // update colormapper
                // 1: there was some texture and there is a new texture:
                if( oldOut && m_outData )
                {
                    // according to WGEColormapper::replaceTexture, an non-existing old texture causes the new one to be inserted at the end.
                    WGEColormapping::replaceTexture( oldOut->getTexture(), m_outData->getTexture(), makeImageName( i ) );
                }
                // 2: there is no new texture. Remove old one.
                else if( oldOut )
                {
                    // no new texture. Remove old one.
                    WGEColormapping::deregisterTexture( oldOut->getTexture() );
                }
                // 3: there was no texture. Add new one.
                else if( m_outData )
                {
                    // no new texture. Remove old one.
                    WGEColormapping::registerTexture( m_outData->getTexture(), makeImageName( i ) );
                }

                m_output->updateData( m_outData );
            }
            else
            {
                if( m_outData )
                {
                    setOutputProps();
                }
            }
        }
        else // case !dataValid
        {
            if( m_outData )
            {
                m_properties->removeProperty( m_outData->getTexture()->getProperties() );
                m_infoProperties->removeProperty( m_outData->getTexture()->getInformationProperties() );
                WGEColormapping::deregisterTexture( m_outData->getTexture() );
            }
            m_outData = std::shared_ptr< WDataSetScalar >();
            m_output->updateData( m_outData );
        }
    }

    debugLog() << "Shutting down...";

    if( m_outData )
    {
        m_properties->removeProperty( m_outData->getTexture()->getProperties() );
        m_infoProperties->removeProperty( m_outData->getTexture()->getInformationProperties() );
        WGEColormapping::deregisterTexture( m_outData->getTexture() );
    }

    debugLog() << "Finished! Good Bye!";
}

std::shared_ptr< WDataSetScalar > WMImageExtractor::extract( std::size_t i ) const
{
    WAssert( m_dataSet, "" );
    WAssert( m_dataSet->getValueSet(), "" );
    WAssert( m_dataSet->getGrid(), "" );

    if( m_dataSet->getValueSet()->order() > 1 || i >= m_dataSet->getValueSet()->dimension() )
    {
        return std::shared_ptr< WDataSetScalar >();
    }

    std::shared_ptr< WGridRegular3D > grid = std::dynamic_pointer_cast< WGridRegular3D >( m_dataSet->getGrid() );
    WAssert( grid, "" );

    std::size_t dim = m_dataSet->getValueSet()->dimension();

    // TODO(reichenbach): use the new valueset function mechanism
    switch( m_dataSet->getValueSet()->getDataType() )
    {
    case W_DT_FLOAT:
        {
            std::shared_ptr< std::vector< float > > values = std::shared_ptr< std::vector< float > >(
                new std::vector< float >( m_dataSet->getGrid()->size() ) );
            std::shared_ptr< WValueSet< float > > v = std::dynamic_pointer_cast< WValueSet< float > >( m_dataSet->getValueSet() );
            WAssert( v, "" );
            for( std::size_t k = 0; k < grid->size(); ++k )
            {
                ( *values )[k] =  v->rawData()[ dim * k + i ];
            }

            std::shared_ptr< WValueSet< float > > vs =
                    std::shared_ptr< WValueSet< float > >( new WValueSet< float >( 0, 1, values, W_DT_FLOAT ) );

            return std::shared_ptr< WDataSetScalar >( new WDataSetScalar( vs, grid ) );
        }
    case W_DT_DOUBLE:
        {
            std::shared_ptr< std::vector< double > > values = std::shared_ptr< std::vector< double > >(
                new std::vector< double >( m_dataSet->getGrid()->size() ) );
            std::shared_ptr< WValueSet< double > > v = std::dynamic_pointer_cast< WValueSet< double > >( m_dataSet->getValueSet() );
            WAssert( v, "" );
            for( std::size_t k = 0; k < grid->size(); ++k )
            {
                ( *values )[k] =  v->rawData()[ dim * k + i ];
            }

            std::shared_ptr< WValueSet< double > > vs =
                    std::shared_ptr< WValueSet< double > >( new WValueSet< double >( 0, 1, values, W_DT_DOUBLE ) );

            return std::shared_ptr< WDataSetScalar >( new WDataSetScalar( vs, grid ) );
        }
    case W_DT_UINT8:
        {
            std::shared_ptr< std::vector< uint8_t > > values = std::shared_ptr< std::vector< uint8_t > >(
                new std::vector< uint8_t >( m_dataSet->getGrid()->size() ) );
            std::shared_ptr< WValueSet< uint8_t > > v = std::dynamic_pointer_cast< WValueSet< uint8_t > >( m_dataSet->getValueSet() );
            WAssert( v, "" );
            for( std::size_t k = 0; k < grid->size(); ++k )
            {
                ( *values )[k] =  v->rawData()[ dim * k + i ];
            }

            std::shared_ptr< WValueSet< uint8_t > > vs =
                    std::shared_ptr< WValueSet< uint8_t > >( new WValueSet< uint8_t >( 0, 1, values, W_DT_UINT8 ) );

            return std::shared_ptr< WDataSetScalar >( new WDataSetScalar( vs, grid ) );
        }
    case W_DT_UINT16:
        {
            std::shared_ptr< std::vector< uint16_t > > values = std::shared_ptr< std::vector< uint16_t > >(
                new std::vector< uint16_t >( m_dataSet->getGrid()->size() ) );
            std::shared_ptr< WValueSet< uint16_t > > v = std::dynamic_pointer_cast< WValueSet< uint16_t > >( m_dataSet->getValueSet() );
            WAssert( v, "" );
            for( std::size_t k = 0; k < grid->size(); ++k )
            {
                ( *values )[k] =  v->rawData()[ dim * k + i ];
            }

            std::shared_ptr< WValueSet< uint16_t > > vs =
                    std::shared_ptr< WValueSet< uint16_t > >( new WValueSet< uint16_t >( 0, 1, values, W_DT_UINT16 ) );

            return std::shared_ptr< WDataSetScalar >( new WDataSetScalar( vs, grid ) );
        }
    case W_DT_UINT32:
        {
            std::shared_ptr< std::vector< uint32_t > > values = std::shared_ptr< std::vector< uint32_t > >(
                new std::vector< uint32_t >( m_dataSet->getGrid()->size() ) );
            std::shared_ptr< WValueSet< uint32_t > > v = std::dynamic_pointer_cast< WValueSet< uint32_t > >( m_dataSet->getValueSet() );
            WAssert( v, "" );
            for( std::size_t k = 0; k < grid->size(); ++k )
            {
                ( *values )[k] =  v->rawData()[ dim * k + i ];
            }

            std::shared_ptr< WValueSet< uint32_t > > vs =
                    std::shared_ptr< WValueSet< uint32_t > >( new WValueSet< uint32_t >( 0, 1, values, W_DT_UINT32 ) );

            return std::shared_ptr< WDataSetScalar >( new WDataSetScalar( vs, grid ) );
        }
    case W_DT_INT8:
        {
            std::shared_ptr< std::vector< int8_t > > values = std::shared_ptr< std::vector< int8_t > >(
                new std::vector< int8_t >( m_dataSet->getGrid()->size() ) );
            std::shared_ptr< WValueSet< int8_t > > v = std::dynamic_pointer_cast< WValueSet< int8_t > >( m_dataSet->getValueSet() );
            WAssert( v, "" );
            for( std::size_t k = 0; k < grid->size(); ++k )
            {
                ( *values )[k] =  v->rawData()[ dim * k + i ];
            }

            std::shared_ptr< WValueSet< int8_t > > vs =
                    std::shared_ptr< WValueSet< int8_t > >( new WValueSet< int8_t >( 0, 1, values, W_DT_INT8 ) );

            return std::shared_ptr< WDataSetScalar >( new WDataSetScalar( vs, grid ) );
        }
    case W_DT_INT16:
        {
            std::shared_ptr< std::vector< int16_t > > values = std::shared_ptr< std::vector< int16_t > >(
                new std::vector< int16_t >( m_dataSet->getGrid()->size() ) );
            std::shared_ptr< WValueSet< int16_t > > v = std::dynamic_pointer_cast< WValueSet< int16_t > >( m_dataSet->getValueSet() );
            WAssert( v, "" );
            for( std::size_t k = 0; k < grid->size(); ++k )
            {
                ( *values )[k] =  v->rawData()[ dim * k + i ];
            }

            std::shared_ptr< WValueSet< int16_t > > vs =
                    std::shared_ptr< WValueSet< int16_t > >( new WValueSet< int16_t >( 0, 1, values, W_DT_INT16 ) );

            return std::shared_ptr< WDataSetScalar >( new WDataSetScalar( vs, grid ) );
        }
    case W_DT_SIGNED_INT:
        {
            std::shared_ptr< std::vector< int32_t > > values = std::shared_ptr< std::vector< int32_t > >(
                new std::vector< int32_t >( m_dataSet->getGrid()->size() ) );
            std::shared_ptr< WValueSet< int32_t > > v = std::dynamic_pointer_cast< WValueSet< int32_t > >( m_dataSet->getValueSet() );
            WAssert( v, "" );
            for( std::size_t k = 0; k < grid->size(); ++k )
            {
                ( *values )[k] = v->rawData()[ dim * k + i ];
            }

            std::shared_ptr< WValueSet< int32_t > > vs =
                    std::shared_ptr< WValueSet< int32_t > >( new WValueSet< int32_t >( 0, 1, values, W_DT_SIGNED_INT ) );

            return std::shared_ptr< WDataSetScalar >( new WDataSetScalar( vs, grid ) );
        }
    default:
        warnLog() << "Encountered unsupported data format in ImageExtractor!";
        return std::shared_ptr< WDataSetScalar >();
    }
}

const std::string WMImageExtractor::makeImageName( std::size_t i )
{
    std::stringstream s;
    WAssert( m_dataSet, "" );
    WAssert( m_dataSet->getValueSet(), "" );

    s << m_dataSet->getFilename() << " (" << i << " of " << m_dataSet->getValueSet()->dimension() << ")";
    return s.str();
}

void WMImageExtractor::setOutputProps()
{
    if( m_outData )
    {
        double min = m_outData->getMin();
        double max = m_outData->getMax();
        float fmin = static_cast< float >( min + ( max - min ) * m_minValuePct->get( true ) * 0.01 );
        float fmax = static_cast< float >( min + ( max - min ) * m_maxValuePct->get( true ) * 0.01 );

        m_outData->getTexture()->minimum()->set( fmin );
        m_outData->getTexture()->scale()->set( std::max( fmax, fmin + 1.0f ) - fmin );
    }
}
