//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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
#include <string>
#include <sstream>
#include <vector>

#include "../../kernel/WKernel.h"
#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/WDataTexture3D.h"
#include "../../common/WPropertyHelper.h"

#include "WMImageExtractor.xpm"
#include "WMImageExtractor.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMImageExtractor )

WMImageExtractor::WMImageExtractor():
    WModule()
{
}

WMImageExtractor::~WMImageExtractor()
{
}

boost::shared_ptr< WModule > WMImageExtractor::factory() const
{
    return boost::shared_ptr< WModule >( new WMImageExtractor() );
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
    m_input = boost::shared_ptr< WModuleInputData < WDataSetSingle  > >(
        new WModuleInputData< WDataSetSingle >( shared_from_this(), "in", "The input dataset." ) );
    addConnector( m_input );

    m_output = boost::shared_ptr< WModuleOutputData < WDataSetScalar  > >(
        new WModuleOutputData< WDataSetScalar >( shared_from_this(), "out", "The extracted image." ) );
    addConnector( m_output );

    WModule::connectors();
}

void WMImageExtractor::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_selectedImage = m_properties->addProperty( "Image", "The image to be extracted.", 0, m_propCondition );
    m_selectedImage->setMin( 0 );
    m_selectedImage->setMax( 0 );

    m_minValuePct = m_properties->addProperty( "Min %", "The data value percent that maps to 0.0 in the texture.", 0.0, m_propCondition );
    m_minValuePct->setMin( 0.0 );
    m_minValuePct->setMax( 100.0 );

    m_maxValuePct = m_properties->addProperty( "Max %", "The data value percent that maps to 1.0 in the texture.", 100.0, m_propCondition );
    m_maxValuePct->setMin( 0.0 );
    m_maxValuePct->setMax( 100.0 );

    // these are taken from WMData
    m_interpolation = m_properties->addProperty( "Interpolation",
                                                  "If active, the boundaries of single voxels"
                                                  " will not be visible in colormaps. The transition between"
                                                  " them will be smooth by using interpolation then.",
                                                  true,
                                                  m_propCondition );
    m_threshold = m_properties->addProperty( "Threshold", "Values below this threshold will not be "
                                              "shown in colormaps.", 0., m_propCondition );
    m_opacity = m_properties->addProperty( "Opacity %", "The opacity of this data in colormaps combining"
                                            " values from several data sets.", 100, m_propCondition );
    m_opacity->setMax( 100 );
    m_opacity->setMin( 0 );

    m_colorMapSelectionsList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_colorMapSelectionsList->addItem( "Grayscale", "" );
    m_colorMapSelectionsList->addItem( "Rainbow", "" );
    m_colorMapSelectionsList->addItem( "Hot iron", "" );
    m_colorMapSelectionsList->addItem( "Red-Yellow", "" );
    m_colorMapSelectionsList->addItem( "Blue-Lightblue", "" );
    m_colorMapSelectionsList->addItem( "Blue-Green-Purple", "" );

    m_colorMapSelection = m_properties->addProperty( "Colormap",  "Colormap type.", m_colorMapSelectionsList->getSelectorFirst(), m_propCondition );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_colorMapSelection );

    WModule::properties();
}

void WMImageExtractor::activate()
{
    if( m_outData )
    {
        m_outData->getTexture()->setGloballyActive( m_active->get() );
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

        boost::shared_ptr< WDataSetSingle > newDataSet = m_input->getData();
        bool dataChanged = ( m_dataSet != newDataSet );
        bool dataValid   = ( newDataSet );

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

                if( m_outData )
                {
                    WDataHandler::deregisterDataSet( m_outData );
                }

                std::size_t i = static_cast< std::size_t >( m_selectedImage->get( true ) );

                boost::shared_ptr< WDataSetScalar > oldOut = m_outData;
                m_outData = extract( i );

                if( m_outData )
                {
                    if( m_outData != oldOut )
                    {
                        m_threshold->setMin( m_outData->getMin() - 1 );
                        m_threshold->setMax( m_outData->getMax() );
                        m_threshold->set( m_outData->getMin() - 1 );
                    }

                    setOutputProps();
                    m_outData->setFileName( makeImageName( i ) );
                    WDataHandler::registerDataSet( m_outData );
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
                WDataHandler::deregisterDataSet( m_outData );
            }
            m_outData = boost::shared_ptr< WDataSetScalar >();
            m_output->updateData( m_outData );
        }
    }

    debugLog() << "Shutting down...";

    if( m_outData )
    {
        WDataHandler::deregisterDataSet( m_outData );
    }

    debugLog() << "Finished! Good Bye!";
}

boost::shared_ptr< WDataSetScalar > WMImageExtractor::extract( std::size_t i ) const
{
    WAssert( m_dataSet, "" );
    WAssert( m_dataSet->getValueSet(), "" );
    WAssert( m_dataSet->getGrid(), "" );

    if( m_dataSet->getValueSet()->order() > 1 || i >= m_dataSet->getValueSet()->dimension() )
    {
        return boost::shared_ptr< WDataSetScalar >();
    }

    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() );
    WAssert( grid, "" );

    std::size_t dim = m_dataSet->getValueSet()->dimension();

    // TODO( reichenbach ): use the new valueset function mechanism
    switch( m_dataSet->getValueSet()->getDataType() )
    {
    case W_DT_FLOAT:
        {
            std::vector< float > values( m_dataSet->getGrid()->size() );
            boost::shared_ptr< WValueSet< float > > v = boost::shared_dynamic_cast< WValueSet< float > >( m_dataSet->getValueSet() );
            WAssert( v, "" );
            for( std::size_t k = 0; k < grid->size(); ++k )
            {
                values[ k ] =  v->rawData()[ dim * k + i ];
            }

            boost::shared_ptr< WValueSet< float > > vs =
                    boost::shared_ptr< WValueSet< float > >( new WValueSet< float >( 0, 1, values, W_DT_FLOAT ) );

            return boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( vs, grid ) );
        }
    case W_DT_DOUBLE:
        {
            std::vector< double > values( m_dataSet->getGrid()->size() );
            boost::shared_ptr< WValueSet< double > > v = boost::shared_dynamic_cast< WValueSet< double > >( m_dataSet->getValueSet() );
            WAssert( v, "" );
            for( std::size_t k = 0; k < grid->size(); ++k )
            {
                values[ k ] =  v->rawData()[ dim * k + i ];
            }

            boost::shared_ptr< WValueSet< double > > vs =
                    boost::shared_ptr< WValueSet< double > >( new WValueSet< double >( 0, 1, values, W_DT_DOUBLE ) );

            return boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( vs, grid ) );
        }
    case W_DT_UINT8:
        {
            std::vector< uint8_t > values( m_dataSet->getGrid()->size() );
            boost::shared_ptr< WValueSet< uint8_t > > v = boost::shared_dynamic_cast< WValueSet< uint8_t > >( m_dataSet->getValueSet() );
            WAssert( v, "" );
            for( std::size_t k = 0; k < grid->size(); ++k )
            {
                values[ k ] =  v->rawData()[ dim * k + i ];
            }

            boost::shared_ptr< WValueSet< uint8_t > > vs =
                    boost::shared_ptr< WValueSet< uint8_t > >( new WValueSet< uint8_t >( 0, 1, values, W_DT_UINT8 ) );

            return boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( vs, grid ) );
        }
    case W_DT_UINT16:
        {
            std::vector< uint16_t > values( m_dataSet->getGrid()->size() );
            boost::shared_ptr< WValueSet< uint16_t > > v = boost::shared_dynamic_cast< WValueSet< uint16_t > >( m_dataSet->getValueSet() );
            WAssert( v, "" );
            for( std::size_t k = 0; k < grid->size(); ++k )
            {
                values[ k ] =  v->rawData()[ dim * k + i ];
            }

            boost::shared_ptr< WValueSet< uint16_t > > vs =
                    boost::shared_ptr< WValueSet< uint16_t > >( new WValueSet< uint16_t >( 0, 1, values, W_DT_UINT16 ) );

            return boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( vs, grid ) );
        }
    case W_DT_UINT32:
        {
            std::vector< uint32_t > values( m_dataSet->getGrid()->size() );
            boost::shared_ptr< WValueSet< uint32_t > > v = boost::shared_dynamic_cast< WValueSet< uint32_t > >( m_dataSet->getValueSet() );
            WAssert( v, "" );
            for( std::size_t k = 0; k < grid->size(); ++k )
            {
                values[ k ] =  v->rawData()[ dim * k + i ];
            }

            boost::shared_ptr< WValueSet< uint32_t > > vs =
                    boost::shared_ptr< WValueSet< uint32_t > >( new WValueSet< uint32_t >( 0, 1, values, W_DT_UINT32 ) );

            return boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( vs, grid ) );
        }
    case W_DT_INT8:
        {
            std::vector< int8_t > values( m_dataSet->getGrid()->size() );
            boost::shared_ptr< WValueSet< int8_t > > v = boost::shared_dynamic_cast< WValueSet< int8_t > >( m_dataSet->getValueSet() );
            WAssert( v, "" );
            for( std::size_t k = 0; k < grid->size(); ++k )
            {
                values[ k ] =  v->rawData()[ dim * k + i ];
            }

            boost::shared_ptr< WValueSet< int8_t > > vs =
                    boost::shared_ptr< WValueSet< int8_t > >( new WValueSet< int8_t >( 0, 1, values, W_DT_INT8 ) );

            return boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( vs, grid ) );
        }
    case W_DT_INT16:
        {
            std::vector< int16_t > values( m_dataSet->getGrid()->size() );
            boost::shared_ptr< WValueSet< int16_t > > v = boost::shared_dynamic_cast< WValueSet< int16_t > >( m_dataSet->getValueSet() );
            WAssert( v, "" );
            for( std::size_t k = 0; k < grid->size(); ++k )
            {
                values[ k ] =  v->rawData()[ dim * k + i ];
            }

            boost::shared_ptr< WValueSet< int16_t > > vs =
                    boost::shared_ptr< WValueSet< int16_t > >( new WValueSet< int16_t >( 0, 1, values, W_DT_INT16 ) );

            return boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( vs, grid ) );
        }
    case W_DT_SIGNED_INT:
        {
            std::vector< int32_t > values( m_dataSet->getGrid()->size() );
            boost::shared_ptr< WValueSet< int32_t > > v = boost::shared_dynamic_cast< WValueSet< int32_t > >( m_dataSet->getValueSet() );
            WAssert( v, "" );
            for( std::size_t k = 0; k < grid->size(); ++k )
            {
                values[ k ] = v->rawData()[ dim * k + i ];
            }

            boost::shared_ptr< WValueSet< int32_t > > vs =
                    boost::shared_ptr< WValueSet< int32_t > >( new WValueSet< int32_t >( 0, 1, values, W_DT_SIGNED_INT ) );

            return boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( vs, grid ) );
        }
    default:
        warnLog() << "Encountered unsupported data format in ImageExtractor!";
        return boost::shared_ptr< WDataSetScalar >();
    }
}

const std::string WMImageExtractor::makeImageName( std::size_t i )
{
    std::stringstream s;
    WAssert( m_dataSet, "" );
    WAssert( m_dataSet->getValueSet(), "" );

    s << m_dataSet->getFileName() << " (" << i << " of " << m_dataSet->getValueSet()->dimension() << ")";
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

        m_outData->getTexture()->setMinValue( fmin );
        m_outData->getTexture()->setMaxValue( std::max( fmax, fmin + 1.0f ) );
        m_outData->getTexture()->setThreshold( m_threshold->get( true ) );
        m_outData->getTexture()->setOpacity( m_opacity->get( true ) );
        m_outData->getTexture()->setInterpolation( m_interpolation->get( true ) );
        m_outData->getTexture()->setSelectedColormap( m_colorMapSelection->get( true ).getItemIndexOfSelected( 0 ) );
    }
}
