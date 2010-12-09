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

#include <string>
#include <sstream>
#include <vector>

#include "../../common/WPropertyHelper.h"
#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/WDataTexture3D.h"
#include "../../dataHandler/WSubject.h"
#include "../../graphicsEngine/WGEUtils.h"
#include "../../graphicsEngine/WROIArbitrary.h"
#include "../../kernel/WKernel.h"

#include "WMPaintTexture.xpm" // Please put a real icon here.

#include "WMPaintTexture.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMPaintTexture )

WMPaintTexture::WMPaintTexture():
    WModule()
{
}

WMPaintTexture::~WMPaintTexture()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMPaintTexture::factory() const
{
    return boost::shared_ptr< WModule >( new WMPaintTexture() );
}

const char** WMPaintTexture::getXPMIcon() const
{
    return paintTexture_xpm; // Please put a real icon here.
}
const std::string WMPaintTexture::getName() const
{
    return "Paint Texture";
}

const std::string WMPaintTexture::getDescription() const
{
    return "This module allows painting areas in a 3D texture";
}

void WMPaintTexture::connectors()
{
    // the input dataset is just used as source for resolurtion and transformation matrix
    m_input = boost::shared_ptr< WModuleInputData < WDataSetSingle  > >(
        new WModuleInputData< WDataSetSingle >( shared_from_this(), "in", "The input dataset." ) );
    addConnector( m_input );

    m_output = boost::shared_ptr< WModuleOutputData < WDataSetScalar  > >(
        new WModuleOutputData< WDataSetScalar >( shared_from_this(), "out", "The extracted image." ) );
    addConnector( m_output );

    WModule::connectors();
}

void WMPaintTexture::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    // use this callback for the other properties
    WPropertyBase::PropertyChangeNotifierType propertyCallback = boost::bind( &WMPaintTexture::propertyChanged, this, _1 );

    m_painting = m_properties->addProperty( "Paint", "If active, left click in the scene with pressed ctrl key"
                                                      " will paint something.", false, m_propCondition );

    m_pencilSelectionsList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_pencilSelectionsList->addItem( "1x1", "" );
    m_pencilSelectionsList->addItem( "3x3", "" );
    m_pencilSelectionsList->addItem( "5x5", "" );
    m_pencilSelectionsList->addItem( "3x1 (only works on orthogonal slices)", "" );
    m_pencilSelectionsList->addItem( "5x1 (only works on orthogonal slices)", "" );
    m_pencilSelection = m_properties->addProperty( "Pencil",  "Pencil type.", m_pencilSelectionsList->getSelectorFirst() );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_pencilSelection );

    m_paintIndex = m_properties->addProperty( "Paint index", "Index we paint into the output texture", 1 );
    m_paintIndex->setMin( 0 );
    m_paintIndex->setMax( 255 );

    m_opacity = m_properties->addProperty( "Opacity %", "The opacity of this data in colormaps combining"
                                            " values from several data sets.", 100, propertyCallback );
    m_opacity->setMax( 100 );
    m_opacity->setMin( 0 );

    m_colorMapSelectionsList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_colorMapSelectionsList->addItem( "Grayscale", "" );
    m_colorMapSelectionsList->addItem( "Rainbow", "" );
    m_colorMapSelectionsList->addItem( "Hot iron", "" );
    m_colorMapSelectionsList->addItem( "Red-Yellow", "" );
    m_colorMapSelectionsList->addItem( "Atlas", "" );
    m_colorMapSelectionsList->addItem( "Blue-Green-Purple", "" );

    m_colorMapSelection = m_properties->addProperty( "Colormap",  "Colormap type.", m_colorMapSelectionsList->getSelector( 4 ), m_propCondition );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_colorMapSelection );

    m_queueAdded = m_properties->addProperty( "Something paint", "", false, m_propCondition );
    m_queueAdded->setHidden();

    m_buttonCopyFromInput = m_properties->addProperty( "Copy from input", "Copies data from the input dataset into the paint texture",
                WPVBaseTypes::PV_TRIGGER_READY, m_propCondition  );

    m_buttonUpdateOutput = m_properties->addProperty( "Update output", "Updates the output connector",
            WPVBaseTypes::PV_TRIGGER_READY, m_propCondition  );
    m_buttonCreateRoi = m_properties->addProperty( "Create ROI", "Create a ROI from the currently selected paint value",
                WPVBaseTypes::PV_TRIGGER_READY, m_propCondition  );

    WModule::properties();
}

void WMPaintTexture::propertyChanged( boost::shared_ptr< WPropertyBase > property )
{
    if ( property == m_opacity )
    {
        WKernel::getRunningKernel()->getSelectionManager()->setTextureOpacity( m_opacity->get( true ) / 100.0 );
        WDataHandler::getDefaultSubject()->getChangeCondition()->notify();
    }
}

void WMPaintTexture::moduleMain()
{
    boost::signals2::connection con = WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getPickHandler()->getPickSignal()->
        connect( boost::bind( &WMPaintTexture::queuePaint, this, _1 ) );

    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );
    m_moduleState.add( m_active->getUpdateCondition() );

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
            if( dataChanged )
            {
                m_dataSet = newDataSet;
                WAssert( m_dataSet, "" );
                WAssert( m_dataSet->getValueSet(), "" );

                createTexture();

                updateOutDataset();
            }

            if ( m_painting->changed() )
            {
                if ( m_painting->get( true ) )
                {
                    WKernel::getRunningKernel()->getSelectionManager()->setPaintMode( PAINTMODE_PAINT );
                }
                else
                {
                    WKernel::getRunningKernel()->getSelectionManager()->setPaintMode( PAINTMODE_NONE );
                }
            }

            if ( m_active->changed() )
            {
                if ( m_active->get( true ) )
                {
                    if ( m_painting->get( true ) )
                    {
                        WKernel::getRunningKernel()->getSelectionManager()->setPaintMode( PAINTMODE_PAINT );
                    }
                    WKernel::getRunningKernel()->getSelectionManager()->setUseTexture( true );
                    WDataHandler::getDefaultSubject()->getChangeCondition()->notify();
                }
                else
                {
                    WKernel::getRunningKernel()->getSelectionManager()->setUseTexture( false );
                    WKernel::getRunningKernel()->getSelectionManager()->setPaintMode( PAINTMODE_NONE );
                    WDataHandler::getDefaultSubject()->getChangeCondition()->notify();
                }
            }

            if ( m_buttonCopyFromInput->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
            {
                copyFromInput();
                m_buttonCopyFromInput->set( WPVBaseTypes::PV_TRIGGER_READY, false );
            }

            if ( m_buttonCreateRoi->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
            {
                m_painting->set( false );
                createROI();
                m_buttonCreateRoi->set( WPVBaseTypes::PV_TRIGGER_READY, false );
            }
        }
        else // case !dataValid
        {
            if( m_outData )
            m_outData = boost::shared_ptr< WDataSetScalar >();
            m_output->updateData( m_outData );
        }
        if ( m_queueAdded->changed() && m_queueAdded->get( true ) )
        {
            doPaint();
        }

        if ( m_buttonUpdateOutput->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            updateOutDataset();
            m_buttonUpdateOutput->set( WPVBaseTypes::PV_TRIGGER_READY, false );
        }
    }

    debugLog() << "Shutting down...";

    WKernel::getRunningKernel()->getSelectionManager()->setUseTexture( false );
    WKernel::getRunningKernel()->getSelectionManager()->setPaintMode( PAINTMODE_NONE );
    WDataHandler::getDefaultSubject()->getChangeCondition()->notify();

    con.disconnect();

    debugLog() << "Finished! Good Bye!";
}

void WMPaintTexture::activate()
{
    WModule::activate();
}

void WMPaintTexture::doPaint()
{
    unsigned char* data = m_texture->getImage()->data();

    while ( !m_paintQueue.empty() )
    {
        WPickInfo pickInfo = m_paintQueue.front();
        wmath::WPosition paintPosition = pickInfo.getPickPosition();
        m_paintQueue.pop();

        int voxelNum = m_grid->getVoxelNum( paintPosition );
        if ( voxelNum != -1 )
        {
            switch ( m_pencilSelection->get( true ).getItemIndexOfSelected( 0 ) )
            {
                case 0:
                    data[ voxelNum ] = m_paintIndex->get();
                    break;
                case 1:
                {
                    data[ voxelNum ] = m_paintIndex->get();
                    std::vector< size_t > ids = m_grid->getNeighbours27( voxelNum );
                    for ( size_t i = 0; i < ids.size(); ++i )
                    {
                        data[ ids[i] ] = m_paintIndex->get();
                    }
                    break;
                }
                case 2:
                {
                    data[ voxelNum ] = m_paintIndex->get();
                    std::vector< size_t > ids = m_grid->getNeighbours27( voxelNum );
                    for ( size_t i = 0; i < ids.size(); ++i )
                    {
                        std::vector< size_t > allIds = m_grid->getNeighbours27( ids[i] );
                        for ( size_t k = 0; k < allIds.size(); ++k )
                        {
                            data[ allIds[k] ] = m_paintIndex->get();
                        }
                    }
                    break;
                }
                case 3:
                {
                    if ( pickInfo.getName() == "Axial Slice" )
                    {
                        data[ voxelNum ] = m_paintIndex->get();
                        std::vector< size_t > ids = m_grid->getNeighbours9XY( voxelNum );
                        for ( size_t i = 0; i < ids.size(); ++i )
                        {
                            data[ ids[i] ] = m_paintIndex->get();
                        }
                    }
                    if ( pickInfo.getName() == "Coronal Slice" )
                    {
                        data[ voxelNum ] = m_paintIndex->get();
                        std::vector< size_t > ids = m_grid->getNeighbours9XZ( voxelNum );
                        for ( size_t i = 0; i < ids.size(); ++i )
                        {
                            data[ ids[i] ] = m_paintIndex->get();
                        }
                    }
                    if ( pickInfo.getName() == "Sagittal Slice" )
                    {
                        data[ voxelNum ] = m_paintIndex->get();
                        std::vector< size_t > ids = m_grid->getNeighbours9YZ( voxelNum );
                        for ( size_t i = 0; i < ids.size(); ++i )
                        {
                            data[ ids[i] ] = m_paintIndex->get();
                        }
                    }
                    break;
                }
                case 4:
                {
                    if ( pickInfo.getName() == "Axial Slice" )
                    {
                        data[ voxelNum ] = m_paintIndex->get();
                        std::vector< size_t > ids = m_grid->getNeighbours9XY( voxelNum );
                        for ( size_t i = 0; i < ids.size(); ++i )
                        {
                            std::vector< size_t > allIds = m_grid->getNeighbours9XY( ids[i] );
                            for ( size_t k = 0; k < allIds.size(); ++k )
                            {
                                data[ allIds[k] ] = m_paintIndex->get();
                            }
                        }
                    }
                    if ( pickInfo.getName() == "Coronal Slice" )
                    {
                        data[ voxelNum ] = m_paintIndex->get();
                        std::vector< size_t > ids = m_grid->getNeighbours9XZ( voxelNum );
                        for ( size_t i = 0; i < ids.size(); ++i )
                        {
                            std::vector< size_t > allIds = m_grid->getNeighbours9XZ( ids[i] );
                            for ( size_t k = 0; k < allIds.size(); ++k )
                            {
                                data[ allIds[k] ] = m_paintIndex->get();
                            }
                        }
                    }
                    if ( pickInfo.getName() == "Sagittal Slice" )
                    {
                        data[ voxelNum ] = m_paintIndex->get();
                        std::vector< size_t > ids = m_grid->getNeighbours9YZ( voxelNum );
                        for ( size_t i = 0; i < ids.size(); ++i )
                        {
                            std::vector< size_t > allIds = m_grid->getNeighbours9YZ( ids[i] );
                            for ( size_t k = 0; k < allIds.size(); ++k )
                            {
                                data[ allIds[k] ] = m_paintIndex->get();
                            }
                        }
                    }
                    break;
                }

                default:
                    break;
            }
        }
    }
    m_queueAdded->set( false );

    m_texture->dirtyTextureObject();
}

void WMPaintTexture::queuePaint( WPickInfo pickInfo )
{
    if ( pickInfo.getModifierKey() == WPickInfo::SHIFT )
    {
        setColorFromPick( pickInfo );
        return;
    }

    if ( !m_painting->get() || ( pickInfo.getMouseButton() != WPickInfo::MOUSE_LEFT ) || ( pickInfo.getName() == "unpick" ) )
    {
        return;
    }

    m_paintQueue.push( pickInfo );
    m_queueAdded->set( true );
}

void WMPaintTexture::setColorFromPick( WPickInfo pickInfo )
{
    wmath::WPosition paintPosition = pickInfo.getPickPosition();
    int voxelNum = m_grid->getVoxelNum( paintPosition );

    if ( voxelNum != -1 )
    {
        unsigned char* data = m_texture->getImage()->data();
        m_paintIndex->set( data[ voxelNum ] );
    }
}

void WMPaintTexture::createTexture()
{
    m_grid = boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() );

    osg::ref_ptr< osg::Image > ima = new osg::Image;
    ima->allocateImage( m_grid->getNbCoordsX(), m_grid->getNbCoordsY(), m_grid->getNbCoordsZ(), GL_LUMINANCE, GL_UNSIGNED_BYTE );

    unsigned char* data = ima->data();

    for ( unsigned int i = 0; i < m_grid->size(); ++i )
    {
        data[i] = 0.0;
    }

    m_texture = osg::ref_ptr<osg::Texture3D>( new osg::Texture3D );
    m_texture->setFilter( osg::Texture3D::MIN_FILTER, osg::Texture3D::LINEAR );
    m_texture->setFilter( osg::Texture3D::MAG_FILTER, osg::Texture3D::LINEAR );
    m_texture->setWrap( osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_BORDER );
    m_texture->setWrap( osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_BORDER );
    m_texture->setWrap( osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_BORDER );
    m_texture->setImage( ima );
    m_texture->setResizeNonPowerOfTwoHint( false );

    WKernel::getRunningKernel()->getSelectionManager()->setTexture( m_texture, m_grid );
    WKernel::getRunningKernel()->getSelectionManager()->setShader( 4 );
    WKernel::getRunningKernel()->getSelectionManager()->setUseTexture( true );

    WDataHandler::getDefaultSubject()->getChangeCondition()->notify();
}

void WMPaintTexture::updateOutDataset()
{
    WAssert( m_dataSet, "" );
    WAssert( m_dataSet->getValueSet(), "" );
    WAssert( m_dataSet->getGrid(), "" );

    unsigned char* data = m_texture->getImage()->data();
    boost::shared_ptr< std::vector< unsigned char > > values =
        boost::shared_ptr< std::vector< unsigned char > >( new std::vector< unsigned char >( m_grid->size(), 0.0 ) );

    for ( unsigned int i = 0; i < m_grid->size(); ++i )
    {
        values->at( i ) = data[i];
    }

    boost::shared_ptr< WValueSet< unsigned char > > vs =
        boost::shared_ptr< WValueSet< unsigned char > >( new WValueSet< unsigned char >( 0, 1, values, W_DT_UINT8 ) );

    m_outData = boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( vs, m_grid ) );
    m_output->updateData( m_outData );
}

void WMPaintTexture::copyFromInput()
{
    m_grid = boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() );

    unsigned char* data = m_texture->getImage()->data();

    boost::shared_ptr< WValueSet< unsigned char > > vals;
    vals =  boost::shared_dynamic_cast< WValueSet< unsigned char > >( m_dataSet->getValueSet() );

    for ( unsigned int i = 0; i < m_grid->size(); ++i )
    {
        data[i] = vals->getScalar( i );
    }
    m_texture->dirtyTextureObject();
}

void WMPaintTexture::createROI()
{
    bool valid = false;
    std::vector<float>roiVals( m_grid->size(), 0 );
    unsigned char index = m_paintIndex->get();

    unsigned char* data = m_texture->getImage()->data();

    for ( size_t i = 0; i < m_grid->size(); ++i )
    {
        if ( data[i] == index )
        {
            roiVals[i] = 1.0;
            valid = true;
        }
    }

    if ( valid )
    {
        osg::ref_ptr< WROI > newRoi = osg::ref_ptr< WROI >(
                new WROIArbitrary( m_grid->getNbCoordsX(), m_grid->getNbCoordsY(), m_grid->getNbCoordsZ(),
                                   m_grid->getTransformationMatrix(), roiVals, 1.0, wge::createColorFromIndex( index ) ) );

        if ( WKernel::getRunningKernel()->getRoiManager()->getSelectedRoi() == NULL )
        {
            WLogger::getLogger()->addLogMessage( " new roi without parent ", "WMPaintTexture", LL_DEBUG );
            WKernel::getRunningKernel()->getRoiManager()->addRoi( newRoi );
        }
        else
        {
            WLogger::getLogger()->addLogMessage( " new roi with parent ", "WMPaintTexture", LL_DEBUG );
            WKernel::getRunningKernel()->getRoiManager()->addRoi( newRoi, WKernel::getRunningKernel()->getRoiManager()->getSelectedRoi() );
        }
    }
}
