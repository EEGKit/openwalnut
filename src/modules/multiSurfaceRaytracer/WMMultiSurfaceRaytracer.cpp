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
#include <utility>

#include <osg/Geode>
#include <osg/Group>
#include <osg/Material>
#include <osg/ShapeDrawable>
#include <osg/StateAttribute>
#include <osg/Matrixd>
#include <osg/Vec4>
#include <boost/lexical_cast.hpp>

#include "../../core/common/WColor.h"
#include "../../core/common/WPropertyHelper.h"

#include "../../core/dataHandler/WDataSetScalar.h"
#include "../../core/dataHandler/WDataSetVector.h"
#include "../../core/dataHandler/WDataTexture3D.h"

#include "../../core/graphicsEngine/WGEColormapping.h"
#include "../../core/graphicsEngine/WGEGeodeUtils.h"
#include "../../core/graphicsEngine/WGEManagedGroupNode.h"
#include "../../core/graphicsEngine/WGEUtils.h"
#include "../../core/graphicsEngine/WGETextureUtils.h"
#include "../../core/graphicsEngine/shaders/WGEPropertyUniform.h"
#include "../../core/graphicsEngine/shaders/WGEShader.h"
#include "../../core/graphicsEngine/shaders/WGEShaderDefineOptions.h"
#include "../../core/graphicsEngine/shaders/WGEShaderPropertyDefineOptions.h"
#include "../../core/graphicsEngine/postprocessing/WGEPostprocessingNode.h"
#include "../../core/graphicsEngine/WGERequirement.h"
#include "../../core/graphicsEngine/callbacks/WGENodeMaskCallback.h"

#include "../../core/kernel/WKernel.h"
#include "../../core/kernel/WModuleInputForwardData.h"

#include "WMMultiSurfaceRaytracer.xpm"

#include "WMMultiSurfaceRaytracer.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMMultiSurfaceRaytracer )

WMMultiSurfaceRaytracer::WMMultiSurfaceRaytracer():
    WModule()
{
}

WMMultiSurfaceRaytracer::~WMMultiSurfaceRaytracer()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMMultiSurfaceRaytracer::factory() const
{
    return boost::shared_ptr< WModule >( new WMMultiSurfaceRaytracer() );
}

const char** WMMultiSurfaceRaytracer::getXPMIcon() const
{
    return multisurfaceraytracer_xpm;
}

const std::string WMMultiSurfaceRaytracer::getName() const
{
    return "MultiSurfaceRaytracer";
}

const std::string WMMultiSurfaceRaytracer::getDescription() const
{
    return "Visualize scalar fields using multiple isosurfaces and isovalue-dependent transparency.";
}

void WMMultiSurfaceRaytracer::connectors()
{
    // Tractography works with scalar datasets
    m_input = WModuleInputData < WDataSetScalar >::createAndAdd( shared_from_this(), "scalars",
                                                "The scalar dataset." );

    m_gradients = WModuleInputData< WDataSetVector >::createAndAdd( shared_from_this(), "gradients", "The gradient field of the dataset to display" );

    WModule::connectors();
}

void WMMultiSurfaceRaytracer::properties()
{
    // Initialize the properties
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_surfCount = m_properties->addProperty( "Surface Count", "Number of isosurfaces drawn", 2 );
    m_surfCount->setMin( 0 );
    m_surfCount->setMax( 4 );

    m_isoValue1 = m_properties->addProperty( "Isovalue 1", "The value for the innermost surface.", 0.8 );
    m_isoColor1 = m_properties->addProperty( "Color 1",    "The color for the innermost surface.", WColor( 0.7, 0.0, 0.0, 1.0 ), m_propCondition );

    m_isoValue2 = m_properties->addProperty( "Isovalue 2", "The value for the second surface.", 0.6 );
    m_isoColor2 = m_properties->addProperty( "Color 2",    "The color for the second surface.", WColor( 0.7, 0.7, 0.0, 0.75 ), m_propCondition );

    m_isoValue3 = m_properties->addProperty( "Isovalue 3", "The value for the third surface.", 0.4 );
    m_isoColor3 = m_properties->addProperty( "Color 3",    "The color for the third surface.", WColor( 0.0, 0.7, 0.7, 0.5 ), m_propCondition );

    m_isoValue4 = m_properties->addProperty( "Isovalue 4", "The value for the fourth surface.", 0.2 );
    m_isoColor4 = m_properties->addProperty( "Color 4",    "The color for the fourth surface.", WColor( 0.0, 0.7, 0.0, 0.25 ), m_propCondition );

    m_stepCount = m_properties->addProperty( "Step Count", "The number of steps to walk along the ray during raycasting. A low value "
                                                           "may cause artifacts whilst a high value slows down rendering.", 500 );
    m_stepCount->setMin( 1 );
    m_stepCount->setMax( 1000 );

    m_colormapRatio = m_properties->addProperty( "Colormap Ratio",   "The intensity of the colormap in contrast to surface shading.", 0.0 );
    m_colormapRatio->setMin( 0.0 );
    m_colormapRatio->setMax( 1.0 );

    m_saturation = m_properties->addProperty( "Saturation", "The saturation depending on the depth.", 1.0 );
    m_saturation->setMin( 0.0 );
    m_saturation->setMax( 2.0 );

    m_isoEpsilon = m_properties->addProperty( "Isovalue Tolerance", "The amount of deviation tolerated for the isovalue", 0.05 );
    m_isoEpsilon->setMin( 0.0 );
    m_isoEpsilon->setMax( 0.5 );

    m_manualAlpha = m_properties->addProperty( "Manual Alpha", "Lets user use the slider to set a global alpha rather than "
                                                               "computing it automatically.", false );

    m_phong  = m_properties->addProperty( "Phong Shading", "If enabled, Phong shading gets applied on a per-pixel basis.", true );

    m_jitter = m_properties->addProperty( "Stochastic Jitter", "Improves image quality at low sampling rates "
                                                               "but introduces slight noise effect.", true );

    WModule::properties();
}

void WMMultiSurfaceRaytracer::requirements()
{
    // Put the code for your requirements here. See "src/modules/template/" for an extensively documented example.
    m_requirements.push_back( new WGERequirement() );
}

void WMMultiSurfaceRaytracer::moduleMain()
{
    // the WGEShader can take the name of any glsl shader (without extension) in the shaders folder
    m_shader = osg::ref_ptr< WGEShader > ( new WGEShader( "WMMultiSurfaceRaytracer", m_localPath ) );
    m_shader->addPreprocessor( WGEShaderPreprocessor::SPtr(
        new WGEShaderPropertyDefineOptions< WPropBool >( m_jitter, "STOCHASTICJITTER_DISABLED", "STOCHASTICJITTER_ENABLED" ) )
    );
    m_shader->addPreprocessor( WGEShaderPreprocessor::SPtr(
        new WGEShaderPropertyDefineOptions< WPropBool >( m_phong, "PHONGSHADING_DISABLED", "PHONGSHADING_ENABLED" ) )
    );
    m_shader->addPreprocessor( WGEShaderPreprocessor::SPtr(
        new WGEShaderPropertyDefineOptions< WPropBool >( m_manualAlpha, "MANUALALPHA_DISABLED", "MANUALALPHA_ENABLED" ) )
    );
    WGEShaderDefineSwitch::SPtr gradTexEnableDefine = m_shader->setDefine( "GRADIENTTEXTURE_DISABLED" );

    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_gradients->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    ready();
    debugLog() << "Module is now ready.";

    // create the root node containing the transformation and geometry
    osg::ref_ptr< WGEGroupNode > rootNode = new WGEGroupNode();

    // create the post-processing node which actually does the nice stuff to the rendered image
    osg::ref_ptr< WGEPostprocessingNode > postNode = new WGEPostprocessingNode(
                WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getCamera()
                );
    postNode->setEnabled( true );  // use it by default
    postNode->addUpdateCallback( new WGENodeMaskCallback( m_active ) ); // disable the postNode with m_active

    // provide the properties of the post-processor to the user
    m_properties->addProperty( postNode->getProperties() );

    // insert it
    postNode->insert( rootNode, m_shader );
    bool postNodeInserted = false;

    // Normally, you will have a loop which runs as long as the module should not shutdown. In this loop you can react on changing data on input
    // connectors or on changed in your properties.
    debugLog() << "Entering main loop";

    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        if( m_shutdownFlag() )
        {
            break;
        }

        // was there an update?
        bool dataUpdated = m_input->updated();
        boost::shared_ptr< WDataSetScalar > dataSet = m_input->getData();
        bool dataValid   = ( dataSet );

        // valid data available?
        if ( !dataValid )
        {
            // remove renderings if no data is available anymore
            rootNode->clear();
        }

        // have isocolors changed?
        if ( m_isoColor1->changed() || m_isoColor2->changed() || m_isoColor3->changed() || m_isoColor4->changed() )
        {
            // a new color requires the proxy geometry to be rebuild as we store it as color in this geometry
            dataUpdated = true;
        }

        // As the data has changed, we need to recreate the texture.
        if ( dataUpdated && dataValid )
        {
            debugLog() << "Data changed. Uploading new data as texture.";

            // First, grab the grid
            boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( dataSet->getGrid() );
            if ( !grid )
            {
                errorLog() << "The dataset does not provide a regular grid. Ignoring dataset.";
                continue;
            }

            // adapt shader to dataset's isovalues
            // determine minimum isovalue and range of isovalues in dataset
            double isoMin = dataSet->getTexture()->minimum()->get();
            double isoRange = dataSet->getTexture()->scale()->get();
            double isoMax = isoMin + isoRange;

            // set minimum and maximum for all isovalues
            m_isoValue1->setMin( isoMin );
            m_isoValue1->setMax( isoMax );
            m_isoValue2->setMin( isoMin );
            m_isoValue2->setMax( isoMax );
            m_isoValue3->setMin( isoMin );
            m_isoValue3->setMax( isoMax );
            m_isoValue4->setMin( isoMin );
            m_isoValue4->setMax( isoMax );

            // set isovalues to something more meaningful and put them in a vector
            // the largest probability has to be first (in -> out compositing)!
            m_isoValue1->set( isoMin + m_isoValue1->get() * isoRange );
            m_isoValue2->set( isoMin + m_isoValue2->get() * isoRange );
            m_isoValue3->set( isoMin + m_isoValue3->get() * isoRange );
            m_isoValue4->set( isoMin + m_isoValue4->get() * isoRange );
            m_isoVals = WMatrixFixed< double, 4, 1 >( m_isoValue1->get(), m_isoValue2->get(),
                                                      m_isoValue3->get(), m_isoValue4->get() );

            // get the four picked isocolors and save them as rows in a matrix
            // the innermost isosurface (highest probability) has to be first (in -> out compositing)!
            WMatrixFixed< double, 4, 4 > isoCols = WMatrixFixed< double, 4, 4 >();
            WMatrixFixed< double, 4, 1 > col1( m_isoColor1->get() );
            WMatrixFixed< double, 4, 1 > col2( m_isoColor2->get() );
            WMatrixFixed< double, 4, 1 > col3( m_isoColor3->get() );
            WMatrixFixed< double, 4, 1 > col4( m_isoColor4->get() );
            isoCols.setRowVector( 0, col1 );
            isoCols.setRowVector( 1, col2 );
            isoCols.setRowVector( 2, col3 );
            isoCols.setRowVector( 3, col4 );
            osg::Matrixd cols = osg::Matrixd( isoCols );

            // use the OSG Shapes, create unit cube
            WBoundingBox bb( WPosition( 0.0, 0.0, 0.0 ),
                WPosition( grid->getNbCoordsX() - 1, grid->getNbCoordsY() - 1, grid->getNbCoordsZ() - 1 ) );
            osg::ref_ptr< osg::Node > cube = wge::generateSolidBoundingBoxNode( bb, m_isoColor1->get( true ) );
            cube->asTransform()->getChild( 0 )->setName( "_DVR Proxy Cube" ); // Be aware that this name is used in the pick handler.
                                                                              // because of the underscore in front it won't be picked
            // we also set the grid's transformation here
            rootNode->setMatrix( static_cast< WMatrix4d >( grid->getTransform() ) );

            // bind the texture to the node
            osg::StateSet* rootState = cube->getOrCreateStateSet();
            osg::ref_ptr< WGETexture3D > texture3D = dataSet->getTexture();
            texture3D->bind( cube );

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // setup all those uniforms and additional textures
            ////////////////////////////////////////////////////////////////////////////////////////////////////
            rootState->addUniform( new WGEPropertyUniform< WPropInt >( "u_surfCount", m_surfCount ) );
            rootState->addUniform( new WGEPropertyUniform< WPropDouble >( "u_isovalTolerance", m_isoEpsilon ) );
            rootState->addUniform( new WGEPropertyUniform< WPropInt >( "u_steps", m_stepCount ) );
            rootState->addUniform( new WGEPropertyUniform< WPropDouble >( "u_colormapRatio", m_colormapRatio ) );
            rootState->addUniform( new WGEPropertyUniform< WPropDouble >( "u_saturation", m_saturation ) );
            rootState->addUniform( new WGEPropertyUniform< WPropDouble >( "u_isoval1", m_isoValue1 ) );
            rootState->addUniform( new WGEPropertyUniform< WPropDouble >( "u_isoval2", m_isoValue2 ) );
            rootState->addUniform( new WGEPropertyUniform< WPropDouble >( "u_isoval3", m_isoValue3 ) );
            rootState->addUniform( new WGEPropertyUniform< WPropDouble >( "u_isoval4", m_isoValue4 ) );
            rootState->addUniform( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "u_isocolors", cols ) ) );

            // Stochastic jitter?
            const size_t size = 64;
            osg::ref_ptr< WGETexture2D > randTex = wge::genWhiteNoiseTexture( size, size, 1 );
            wge::bindTexture( cube, randTex, 1 );

            // if there is a gradient field available -> apply as texture too
            boost::shared_ptr< WDataSetVector > gradients = m_gradients->getData();
            if ( gradients )
            {
                debugLog() << "Uploading specified gradient field.";

                // bind the texture to the node
                osg::ref_ptr< WDataTexture3D > gradTexture3D = gradients->getTexture();
                wge::bindTexture( cube, gradTexture3D, 2, "u_gradients" );
                gradTexEnableDefine->setActive( true );
            }
            else
            {
                gradTexEnableDefine->setActive( false ); // disable gradient texture
            }
            WGEColormapping::apply( cube, m_shader, 3 );

            // update node
            debugLog() << "Adding new rendering.";
            rootNode->clear();
            rootNode->insert( cube );

            // insert root node if needed. This way, we ensure that the root node gets added only if the proxy cube has been added AND the bbox
            // can be calculated properly by the OSG to ensure the proxy cube is centered in the scene if no other item has been added earlier.
            if ( !postNodeInserted )
            {
                postNodeInserted = true;
                WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( postNode );
            }
        }
    }

    // At this point, the container managing this module signalled to shutdown. The main loop has ended and you should clean up. Always remove
    // allocated memory and remove all OSG nodes.
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( postNode );
}
