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
#include <boost/lexical_cast.hpp>


#include "../../core/common/WColor.h"
#include "../../core/common/WPropertyHelper.h"
#include "../../core/dataHandler/WDataSetScalar.h"
// TODO(aberres): check which ones are needed
#include "../../core/dataHandler/WDataSetVector.h"
#include "../../core/dataHandler/WDataTexture3D_2.h"
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
#include "WMProbTractVis.xpm"

#include "WMProbTractVis.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMProbTractVis )

WMProbTractVis::WMProbTractVis():
    WModule()
{
}

WMProbTractVis::~WMProbTractVis()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMProbTractVis::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMProbTractVis() );
}

const char** WMProbTractVis::getXPMIcon() const
{
    return probtractvis_xpm;
}

const std::string WMProbTractVis::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "ProbTractVis";
}

const std::string WMProbTractVis::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    // See "src/modules/template/" for an extensively documented example.
    return "Someone should add some documentation here. "
    "Probably the best person would be the modules's creator, i.e. \"aberres\"";
}

void WMProbTractVis::connectors()
{
    // Tractography works with scalar datasets
    m_input = WModuleInputData < WDataSetScalar >::createAndAdd( shared_from_this(), "probTract",
                                                "The probabilistic tractogram as a scalar dataset." );

    // TODO (aberres): maybe more input data? need t1 image for context info later

    m_gradients = WModuleInputData< WDataSetVector >::createAndAdd( shared_from_this(), "gradients", "The gradient field of the dataset to display" );

    WModule::connectors();
}

void WMProbTractVis::properties()
{    
    // Initialize the properties
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

//    m_colors        = m_properties->addProperty( "Iso colors",       "Up to four iso colors.",  );

    m_isoValue      = m_properties->addProperty( "Isovalue",         "The isovalue used whenever the isosurface Mode is turned on.",
                                                                      128.0 );
    m_isoColor      = m_properties->addProperty( "Iso color",        "The color to blend the isosurface with.", WColor( 0.6, 0.0, 1.0, 1.0 ),
                      m_propCondition );
    m_alpha         = m_properties->addProperty( "Opacity %",        "The opacity in %. Transparency = 1 - Opacity.", 0.7 );
    m_alpha->setMin( 0.0 );
    m_alpha->setMax( 1.0 );

    m_isoValue2      = m_properties->addProperty( "Isovalue 2",         "The isovalue used whenever the isosurface Mode is turned on.",
                                                                      128.0 );
    m_isoColor2      = m_properties->addProperty( "Iso color 2",        "The color to blend the isosurface with.", WColor( 1.0, 0.6, 0.0, 1.0 ),
                      m_propCondition );
    m_alpha2         = m_properties->addProperty( "Opacity 2 %",        "The opacity in %. Transparency = 1 - Opacity.", 0.7 );
    m_alpha2->setMin( 0.0 );
    m_alpha2->setMax( 1.0 );

    m_isoValueTolerance = m_properties->addProperty( "Isovalue tolerance", "The amount of deviation tolerated for the isovalue", 0.05 );
    m_isoValueTolerance->setMax( 0.5 );

    m_stepCount     = m_properties->addProperty( "Step count",       "The number of steps to walk along the ray during raycasting. A low value "
                                                                      "may cause artifacts whilst a high value slows down rendering.", 250 );
    m_stepCount->setMin( 1 );
    m_stepCount->setMax( 1000 );

    m_colormapRatio = m_properties->addProperty( "Colormap Ratio",   "The intensity of the colormap in contrast to surface shading.", 0.5 );
    m_colormapRatio->setMin( 0.0 );
    m_colormapRatio->setMax( 1.0 );

    m_phongShading  = m_properties->addProperty( "Phong Shading", "If enabled, Phong shading gets applied on a per-pixel basis.", true );

    m_cortexMode    = m_properties->addProperty( "Emphasize Cortex", "Emphasize the Cortex while inner parts ar not that well lighten.", false );

    m_stochasticJitter = m_properties->addProperty( "Stochastic Jitter", "Improves image quality at low sampling rates but introduces slight "
                                                                         "noise effect.", true );

    WModule::properties();
}

void WMProbTractVis::requirements()
{
    // Put the code for your requirements here. See "src/modules/template/" for an extensively documented example.
    m_requirements.push_back( new WGERequirement() );
}

void WMProbTractVis::moduleMain()
{
    // the WGEShader can take the name of any glsl shader (without extension) in the shaders folder
    m_shader = osg::ref_ptr< WGEShader > ( new WGEShader( "WMProbTractVis", m_localPath ) );
    m_shader->addPreprocessor( WGEShaderPreprocessor::SPtr(
        new WGEShaderPropertyDefineOptions< WPropBool >( m_cortexMode, "CORTEXMODE_DISABLED", "CORTEXMODE_ENABLED" ) )
    );
    m_shader->addPreprocessor( WGEShaderPreprocessor::SPtr(
        new WGEShaderPropertyDefineOptions< WPropBool >( m_stochasticJitter, "STOCHASTICJITTER_DISABLED", "STOCHASTICJITTER_ENABLED" ) )
    );
    m_shader->addPreprocessor( WGEShaderPreprocessor::SPtr(
        new WGEShaderPropertyDefineOptions< WPropBool >( m_phongShading, "PHONGSHADING_DISABLED", "PHONGSHADING_ENABLED" ) )
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
    postNode->setEnabled( false );  // do not use it by default
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

        // m_isoColor or shading changed
        if ( m_isoColor->changed() || m_isoColor2->changed() )
        {
            // a new color requires the proxy geometry to be rebuild as we store it as color in this geometry
            dataUpdated = true;
        }

                // As the data has changed, we need to recreate the texture.
        if ( dataUpdated && dataValid )
        {
            debugLog() << "Data changed. Uploading new data as texture.";

            // set isovalue range to that of dataset
            m_isoValue->setMin( dataSet->getTexture2()->minimum()->get() );
            m_isoValue->setMax( dataSet->getTexture2()->scale()->get() + dataSet->getTexture2()->minimum()->get() );
            // set the isovalue to the middle of the range
            m_isoValue->set( dataSet->getTexture2()->minimum()->get() + ( 0.5 * dataSet->getTexture2()->scale()->get() ) );

            // set isovalue range to that of dataset
            m_isoValue2->setMin( dataSet->getTexture2()->minimum()->get() );
            m_isoValue2->setMax( dataSet->getTexture2()->scale()->get() + dataSet->getTexture2()->minimum()->get() );
            // set the isovalue to the middle of the range
            m_isoValue2->set( dataSet->getTexture2()->minimum()->get() + ( 0.2 * dataSet->getTexture2()->scale()->get() ) );


            // First, grab the grid
            boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( dataSet->getGrid() );
            if ( !grid )
            {
                errorLog() << "The dataset does not provide a regular grid. Ignoring dataset.";
                continue;
            }

            // use the OSG Shapes, create unit cube
            WBoundingBox bb( WPosition( 0.0, 0.0, 0.0 ),
                WPosition( grid->getNbCoordsX() - 1, grid->getNbCoordsY() - 1, grid->getNbCoordsZ() - 1 ) );
            osg::ref_ptr< osg::Node > cube = wge::generateSolidBoundingBoxNode( bb, m_isoColor->get( true ) );
            cube->asTransform()->getChild( 0 )->setName( "_DVR Proxy Cube" ); // Be aware that this name is used in the pick handler.
                                                                              // because of the underscore in front it won't be picked
            // we also set the grid's transformation here
            rootNode->setMatrix( static_cast< WMatrix4d >( grid->getTransform() ) );

            // bind the texture to the node
            osg::StateSet* rootState = cube->getOrCreateStateSet();
            osg::ref_ptr< WGETexture3D > texture3D = dataSet->getTexture2();
            texture3D->bind( cube );

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // setup all those uniforms and additional textures
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            rootState->addUniform( new WGEPropertyUniform< WPropDouble >( "u_isovalue", m_isoValue ) );
            rootState->addUniform( new WGEPropertyUniform< WPropDouble >( "u_isovalue2", m_isoValue2 ) );
            rootState->addUniform( new WGEPropertyUniform< WPropColor >( "u_isocolor", m_isoColor ) );
            rootState->addUniform( new WGEPropertyUniform< WPropColor >( "u_isocolor2", m_isoColor2) );
            rootState->addUniform( new WGEPropertyUniform< WPropDouble >( "u_isovaltolerance", m_isoValueTolerance ) );
            rootState->addUniform( new WGEPropertyUniform< WPropInt >( "u_steps", m_stepCount ) );
            rootState->addUniform( new WGEPropertyUniform< WPropDouble >( "u_alpha", m_alpha ) );
            rootState->addUniform( new WGEPropertyUniform< WPropDouble >( "u_alpha2", m_alpha2 ) );
            rootState->addUniform( new WGEPropertyUniform< WPropDouble >( "u_colormapRatio", m_colormapRatio ) );


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
                osg::ref_ptr< WDataTexture3D_2 > gradTexture3D = gradients->getTexture2();
                wge::bindTexture( cube, gradTexture3D, 2, "u_gradients" );
                gradTexEnableDefine->setActive( true );
            }
            else
            {
                gradTexEnableDefine->setActive( false ); // disable gradient texture
            }

//            WGEColormapping::apply( cube, grid->getTransformationMatrix(), m_shader, 3 );
            WGEColormapping::apply( cube, m_shader );

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
