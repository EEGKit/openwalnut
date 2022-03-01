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
#include <cmath>
#include <memory>
#include <string>
#include <utility>

#include <osg/Geode>
#include <osg/Group>
#include <osg/Material>
#include <osg/ShapeDrawable>
#include <osg/StateAttribute>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include "WMDirectVolumeRendering2TF.h"
#include "WMDirectVolumeRendering2TF.xpm"
#include "core/common/WColor.h"
#include "core/common/WPropertyHelper.h"
#include "core/dataHandler/WDataSetScalar.h"
#include "core/graphicsEngine/WGEColormapping.h"
#include "core/graphicsEngine/WGEGeodeUtils.h"
#include "core/graphicsEngine/WGEManagedGroupNode.h"
#include "core/graphicsEngine/WGERequirement.h"
#include "core/graphicsEngine/WGETextureUtils.h"
#include "core/graphicsEngine/WGEUtils.h"
#include "core/graphicsEngine/shaders/WGEShader.h"
#include "core/graphicsEngine/shaders/WGEShaderDefine.h"
#include "core/graphicsEngine/shaders/WGEShaderDefineOptions.h"
#include "core/kernel/WKernel.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMDirectVolumeRendering2TF )

WMDirectVolumeRendering2TF::WMDirectVolumeRendering2TF():
    WModule()
{
    // Initialize members
}

WMDirectVolumeRendering2TF::~WMDirectVolumeRendering2TF()
{
    // Cleanup!
}

std::shared_ptr< WModule > WMDirectVolumeRendering2TF::factory() const
{
    return std::shared_ptr< WModule >( new WMDirectVolumeRendering2TF() );
}

const char** WMDirectVolumeRendering2TF::getXPMIcon() const
{
    return WMDirectVolumeRendering2TF_xpm;
}

const std::string WMDirectVolumeRendering2TF::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "Direct Volume Rendering for 2 TF";
}

const std::string WMDirectVolumeRendering2TF::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    return "Direct volume rendering of regular volumetric data.";
}

void WMDirectVolumeRendering2TF::connectors()
{
    // DVR needs one input: the scalar dataset
    m_input_ds0 = WModuleInputData< WDataSetScalar >::createAndAdd( shared_from_this(), "scalar data set 1", "The scalar dataset." );

    // optional: second input for a second scalar dataset
    m_input_ds1 = WModuleInputData< WDataSetScalar >::createAndAdd( shared_from_this(), "scalar data set 2", "The scalar dataset." );

    // The transfer function for our DVR
    m_transferFunction = WModuleInputData< WDataSetSingle >::createAndAdd( shared_from_this(),
                                                                               "transfer function data set 1",
                                                                               "The 2D transfer function for the two data sets." );
//    // The transfer function for our DVR
//    m_transferFunction_ds0 = WModuleInputData< WDataSetSingle >::createAndAdd( shared_from_this(),
//                                                                              "transfer function data set 1",
//                                                                              "The 1D transfer function for the first data set." );
//
//    // The transfer function for our DVR
//    m_transferFunction_ds1 = WModuleInputData< WDataSetSingle >::createAndAdd( shared_from_this(),
//                                                                              "transfer function data set 2",
//                                                                              "The 1D transfer function for the second data set." );

    // Optional: the gradient field
    m_gradients = WModuleInputData< WDataSetVector >::createAndAdd( shared_from_this(),
            "gradients", "<u>Optional:</u> Gradient field of the dataset to display." );

    // call WModules initialization
    WModule::connectors();
}

void WMDirectVolumeRendering2TF::properties()
{
    // Initialize the properties
    m_propCondition = std::shared_ptr< WCondition >( new WCondition() );

    m_samples     = m_properties->addProperty( "Sample count", "The number of samples to walk along the ray during raycasting. A low value "
            "may cause artifacts whilst a high value slows down rendering.", 256 );
    m_samples->setMin( 1 );
    m_samples->setMax( 5000 );

    // illumination model
    m_localIlluminationSelections = std::shared_ptr< WItemSelection >( new WItemSelection() );
    m_localIlluminationSelections->addItem( "No Local Illumination", "Volume Renderer only uses the classified voxel color." );
    m_localIlluminationSelections->addItem( "Blinn-Phong", "Blinn-Phong lighting is used for shading each classified voxel." );
    m_localIlluminationAlgo = m_properties->addProperty( "Local illumination model", "The illumination algorithm to use.",
            m_localIlluminationSelections->getSelectorFirst(), m_propCondition );

    WPropertyHelper::PC_SELECTONLYONE::addTo( m_localIlluminationAlgo );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_localIlluminationAlgo );

    // additional artifact removal methods
    m_improvementGroup = m_properties->addPropertyGroup( "Improvements", "Methods for improving image quality. Most of these methods imply "
            "additional calculation/texture overhead and therefore slow down rendering." );

    m_stochasticJitterEnabled = m_improvementGroup->addProperty( "Stochastic jitter", "With stochastic jitter, wood-grain artifacts can be "
            "removed with the cost of possible noise artifacts.", true,
            m_propCondition );

    m_opacityCorrectionEnabled = m_improvementGroup->addProperty( "Opacity correction", "If enabled, opacities are assumed to be relative to the "
            "sample count. If disabled, changing the sample count "
            "varies brightness of the image.", true,
            m_propCondition );

    m_maximumIntensityProjectionEnabled = m_improvementGroup->addProperty( "MIP", "If enabled, MIP is used.", false,
            m_propCondition );

    m_depthProjectionEnabled = m_improvementGroup->addProperty( "Depth projection", "If enabled, depth projection mode is used", false,
            m_propCondition );

    WModule::properties();
}

void WMDirectVolumeRendering2TF::requirements()
{
    m_requirements.push_back( new WGERequirement() );
}

/**
 * Generates a white noise texture with given resolution.
 *
 * \param resX the resolution
 *
 * \return a image with resX*resX resolution.
 */
osg::ref_ptr< osg::Image > genWhiteNoise( size_t resX )
{
    std::srand( time( 0 ) );

    osg::ref_ptr< osg::Image > randImage = new osg::Image();
    randImage->allocateImage( resX, resX, 1, GL_LUMINANCE, GL_UNSIGNED_BYTE );
    unsigned char *randomLuminance = randImage->data();  // should be 4 megs
    for( unsigned int x = 0; x < resX; x++ )
    {
        for( unsigned int y = 0; y < resX; y++ )
        {
            // - stylechecker says "use rand_r" but I am not sure about portability.
            unsigned char r = ( unsigned char )( std::rand() % 255 );  // NOLINT
            randomLuminance[ ( y * resX ) + x ] = r;
        }
    }

    return randImage;
}

void WMDirectVolumeRendering2TF::moduleMain()
{
    m_shader = osg::ref_ptr< WGEShader > ( new WGEShader( "WMDirectVolumeRendering2TF", m_localPath ) );
    // setup all the defines needed

    // local illumination model
    WGEShaderDefineOptions::SPtr illuminationAlgoDefines = WGEShaderDefineOptions::SPtr(
            new WGEShaderDefineOptions( "LOCALILLUMINATION_NONE", "LOCALILLUMINATION_PHONG" )
            );
    m_shader->addPreprocessor( illuminationAlgoDefines );

    // gradient texture settings
    WGEShaderDefine< std::string >::SPtr gradTexSamplerDefine = m_shader->setDefine( "GRADIENTTEXTURE_SAMPLER", std::string( "tex1" ) );
    WGEShaderDefineSwitch::SPtr gradTexEnableDefine = m_shader->setDefine( "GRADIENTTEXTURE_ENABLED" );

    // transfer function texture settings
    WGEShaderDefine< std::string >::SPtr tfTexSamplerDefine = m_shader->setDefine( "TRANSFERFUNCTION_SAMPLER", std::string( "tex2" ) );
    WGEShaderDefineSwitch::SPtr tfTexEnableDefine = m_shader->setDefine( "TRANSFERFUNCTION_ENABLED" );

    // jitter
    WGEShaderDefine< std::string >::SPtr jitterSamplerDefine = m_shader->setDefine( "JITTERTEXTURE_SAMPLER", std::string( "tex3" ) );
    WGEShaderDefine< int >::SPtr jitterSizeXDefine = m_shader->setDefine( "JITTERTEXTURE_SIZEX", 0 );
    WGEShaderDefineSwitch::SPtr jitterEnable = m_shader->setDefine( "JITTERTEXTURE_ENABLED" );

    // opacity correction enabled?
    WGEShaderDefineSwitch::SPtr opacityCorrectionEnableDefine = m_shader->setDefine( "OPACITYCORRECTION_ENABLED" );

    WGEShaderDefineSwitch::SPtr maximumIntensityProjectionEnabledDefine = m_shader->setDefine( "MIP_ENABLED" );
    WGEShaderDefineSwitch::SPtr depthProjectionEnabledDefine = m_shader->setDefine( "DEPTH_PROJECTION_ENABLED" );

    // the texture used for the transfer function
    //osg::ref_ptr< osg::Image > tfImage_ds0 = new osg::Image();
    //osg::ref_ptr< osg::Image > tfImage_ds1 = new osg::Image();

    osg::ref_ptr< osg::Texture2D > tfTexture2D = new osg::Texture2D();
    osg::ref_ptr< osg::Image > tfImage2D = new osg::Image();
    bool updateTF = false;  // if true, update of TF is enforced

    // let the main loop awake if the data changes or the properties changed.
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_transferFunction->getDataChangedCondition() );
//    m_moduleState.add( m_transferFunction_ds0->getDataChangedCondition() );
//    m_moduleState.add( m_transferFunction_ds1->getDataChangedCondition() );
    m_moduleState.add( m_input_ds0->getDataChangedCondition() );
    m_moduleState.add( m_input_ds1->getDataChangedCondition() );
    m_moduleState.add( m_gradients->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    // Signal ready state.
    ready();
    debugLog() << "Module is now ready.";

    osg::ref_ptr< WGEManagedGroupNode > rootNode = new WGEManagedGroupNode( m_active );
    bool rootInserted = false;

    // Normally, you will have a loop which runs as long as the module should not shutdown. In this loop you can react on changing data on input
    // connectors or on changed in your properties.
    debugLog() << "Entering main loop";
    while( !m_shutdownFlag() )
    {
        // Now, the moduleState variable comes into play. The module can wait for the condition, which gets fired whenever the input receives data
        // or an property changes. The main loop now waits until something happens.
        m_moduleState.wait();

        // quit if requested
        if( m_shutdownFlag() )
        {
            break;
        }

        // was there an update?
        bool dataUpdated = m_input_ds0->updated() || m_input_ds1->updated() || m_gradients->updated();
        std::shared_ptr< WDataSetScalar > dataSet0= m_input_ds0->getData();
        std::shared_ptr< WDataSetScalar > dataSet1 = m_input_ds1->getData();
        bool dataValid   = ( dataSet0 != NULL ) && ( dataSet1 != NULL );
        bool propUpdated = m_localIlluminationAlgo->changed() || m_stochasticJitterEnabled->changed() ||  m_opacityCorrectionEnabled->changed() ||
            m_maximumIntensityProjectionEnabled->changed() || m_depthProjectionEnabled->changed();


        // reset module in case of invalid data. This accounts only for the scalar field input
        if( !dataValid )
        {
            cube.release();
            debugLog() << "Resetting.";
            rootNode->clear();
            continue;
        }

        // As the data has changed, we need to recreate the texture.
        if( ( propUpdated || dataUpdated ) && dataValid )
        {
            debugLog() << "Data changed. Uploading new data as texture.";

            // there are several updates. Clear the root node and later on insert the new rendering.
            rootNode->clear();

            // First, grab the grid
            std::shared_ptr< WGridRegular3D > grid_ds0 = std::dynamic_pointer_cast< WGridRegular3D >( dataSet0->getGrid() );

            if( !grid_ds0 )
            {
                errorLog() << "The dataset does not provide a regular grid. Ignoring dataset.";
                continue;
            }

            // use the OSG Shapes, create unit cube
            WBoundingBox bb_ds0( WPosition( 0.0, 0.0, 0.0 ),
                                WPosition( grid_ds0->getNbCoordsX() - 1, grid_ds0->getNbCoordsY() - 1, grid_ds0->getNbCoordsZ() - 1 ) );
            cube = wge::generateSolidBoundingBoxNode( bb_ds0, WColor( 1.0, 1.0, 1.0, 1.0 ) );

            cube->asTransform()->getChild( 0 )->setName( "_DVR Proxy Cube Data Set 1" ); // Be aware that this name is used in the pick handler.

            // because of the underscore in front it won't be picked
            // we also set the grid's transformation here
            rootNode->setMatrix( static_cast< WMatrix4d >( grid_ds0->getTransform() ) );


            m_shader->apply( cube );


            // bind the texture to the node
            osg::ref_ptr< WDataTexture3D > texture3D_ds0 = dataSet0->getTexture();
            wge::bindTexture( cube, texture3D_ds0, 0, "u_volume_ds0" );
            osg::ref_ptr< WDataTexture3D > texture3D_ds1 = dataSet1->getTexture();
            wge::bindTexture( cube, texture3D_ds1, 1, "u_volume_ds1" );

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // setup illumination
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            // enable transparency
            osg::StateSet* rootState_ds0 = cube->getOrCreateStateSet();
            rootState_ds0->setMode( GL_BLEND, osg::StateAttribute::ON );
            rootState_ds0->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

            // set proper illumination define
            illuminationAlgoDefines->activateOption( m_localIlluminationAlgo->get( true ).getItemIndexOfSelected( 0 ) );

            // if there is a gradient field available -> apply as texture too
            // TODO(Kai): Depending on the approach, make this available later
            std::shared_ptr< WDataSetVector > gradients = m_gradients->getData();
            if( gradients )
            {
                debugLog() << "Uploading specified gradient field.";

                // bind the texture to the node
                osg::ref_ptr< WDataTexture3D > gradTexture3D = gradients->getTexture();
                wge::bindTexture( cube, gradTexture3D, 2, "u_gradients" );
                //wge::bindTexture( cube, gradTexture3D, 1, "u_gradients_ds1" );
                gradTexEnableDefine->setActive( true );
            }
            else
            {
                gradTexEnableDefine->setActive( false ); // disable gradient texture
            }

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // stochastic jittering texture
            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // TODO(Kai): Add jitter for second data set
            // create some random noise
            jitterSamplerDefine->setActive( false );
            jitterEnable->setActive( false );
            if( m_stochasticJitterEnabled->get( true ) )
            {
                const size_t size = 64;
                osg::ref_ptr< WGETexture2D > randTexture = new WGETexture2D( genWhiteNoise( size ) );
                randTexture->setFilterMinMag( osg::Texture2D::NEAREST );
                randTexture->setWrapSTR( osg::Texture2D::REPEAT );
                wge::bindTexture( cube, randTexture, 3, "u_jitter" );
                //wge::bindTexture( cube, randTexture, 2, "u_jitter_ds1" );
                jitterSamplerDefine->setActive( true );
                jitterEnable->setActive( true );
                jitterSizeXDefine->setValue( size );
            }

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // transfer function texture
            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // Default "texture" when no transfer function is connected
            tfImage2D->allocateImage( 300, 300, 1, GL_RGBA, GL_UNSIGNED_BYTE );
            unsigned char *data = tfImage2D->data();
            for( int x = 0; x < 300; x++ )
            {
                for( int y = 0; y < 300; y++ )
                {
                    data[ 4 * 300 * x + 4 * y + 0 ] = 0.;
                    data[ 4 * 300 * x + 4 * y + 1 ] = 0.;
                    data[ 4 * 300 * x + 4 * y + 2 ] = 255.;
                    data[ 4 * 300 * x + 4 * y + 3 ] = 128.;
                }
            }

            osg::ref_ptr< osg::Texture2D > tfTexture2D = new osg::Texture2D();
            tfTexture2D->setDataVariance( osg::Object::DYNAMIC );
            tfTexture2D->setImage( tfImage2D );

            wge::bindTexture( cube, tfTexture2D, 4, "u_transferFunction2D" );

            // permanently enable the TF texture. As we have no alternative way to set the TF, always use a TF texture
            tfTexEnableDefine->setActive( true );

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // opacity correction
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            if( m_opacityCorrectionEnabled->get( true ) )
            {
                opacityCorrectionEnableDefine->setActive( true );
            }
            else
            {
                opacityCorrectionEnableDefine->setActive( false );
            }

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // maximum intensity projection (MIP)
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            if( m_maximumIntensityProjectionEnabled->get( true ) )
            {
                maximumIntensityProjectionEnabledDefine->setActive( true );
            }
            else
            {
                maximumIntensityProjectionEnabledDefine->setActive( false );
            }

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // depth projection
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            if( m_depthProjectionEnabled->get( true ) )
            {
                depthProjectionEnabledDefine->setActive( true );
            }
            else
            {
                depthProjectionEnabledDefine->setActive( false );
            }

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // setup all those uniforms
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            rootState_ds0->addUniform( new WGEPropertyUniform< WPropInt >( "u_samples", m_samples ) );

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // build spatial search structure
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            // update node
            debugLog() << "Adding new rendering.";
            rootNode->insert( cube );

            // insert root node if needed. This way, we ensure that the root node gets added only if the proxy cube has been added AND the bbox
            // can be calculated properly by the OSG to ensure the proxy cube is centered in the scene if no other item has been added earlier.
            if( !rootInserted )
            {
                rootInserted = true;
                WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( rootNode );
            }

            updateTF = true;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        // load transfer function
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        if( ( updateTF || propUpdated || m_transferFunction->updated() )
                && dataValid && cube )
        {
            updateTF = false;
            std::shared_ptr< WDataSetSingle > dataSet = m_transferFunction->getData();

            if( !dataSet )
            {
                debugLog() << "no data set?";
            }
            else
            {
                WAssert( dataSet, "data set" );
                std::shared_ptr< WValueSetBase > valueSet = dataSet->getValueSet();
                WAssert( valueSet, "value set" );
                std::shared_ptr< WValueSet< unsigned char > > cvalueSet( std::dynamic_pointer_cast<WValueSet< unsigned char> >( valueSet ) );

                if( !valueSet )
                {
                    debugLog() << "invalid type";
                }
                else
                {
                    //create image for 2D TF
                    size_t tfsize = cvalueSet->rawSize();
                    tfImage2D->allocateImage( tfsize / 4, tfsize / 4, 1, GL_RGBA, GL_UNSIGNED_BYTE );
                    tfImage2D->setInternalTextureFormat( GL_RGBA );
                    unsigned char* data = reinterpret_cast< unsigned char* >( tfImage2D->data() );
                    //TODO(Kai): I need to adept this
                    std::copy( cvalueSet->rawData(), &cvalueSet->rawData()[ tfsize ], data );

                    // Just a test if the data is going to be rendered red half transparent
//                    for( int row = 0; row < tfImage2D->s(); ++row )
//                    {
//                        for( int col = 0; col < tfImage2D->t(); ++col )
//                        {
//                            tfImage2D->data( col, row )[ 0 ] = 255.;
//                            tfImage2D->data( col, row )[ 1 ] = 0.;
//                            tfImage2D->data( col, row )[ 2 ] = 0.;
//                            tfImage2D->data( col, row )[ 3 ] = 128.;
//                        }
//                    }

                    // force OpenGl to use the new texture
                    tfTexture2D->dirtyTextureObject();
                }
            }
        }
    }

    // At this point, the container managing this module signalled to shutdown. The main loop has ended and you should clean up. Always remove
    // allocated memory and remove all OSG nodes.
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( rootNode );
}
