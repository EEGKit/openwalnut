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
#include "../../core/dataHandler/WDataTexture3D.h"
#include "../../core/dataHandler/WGridRegular3D.h"
#include "../../core/dataHandler/WSubject.h"

// TODO(aberres): check which ones are needed
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
#include "../../core/kernel/WModuleFactory.h"
#include "../../core/kernel/WPrototypeRequirement.h"
#include "../../core/kernel/WSelectionManager.h"

#include "WMProbTractContext.xpm"

#include "WMProbTractContext.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMProbTractContext )

WMProbTractContext::WMProbTractContext():
    WModuleContainer()
{
}

WMProbTractContext::~WMProbTractContext()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMProbTractContext::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMProbTractContext() );
}

const char** WMProbTractContext::getXPMIcon() const
{
    return probtractcontext_xpm;
}
const std::string WMProbTractContext::getName() const
{
    return "ProbTractContext";
}

const std::string WMProbTractContext::getDescription() const
{
    return "Provide context for probablistic tractograms.";
}

void WMProbTractContext::connectors()
{
    // TODO(aberres): document
    m_input = WModuleInputData < WDataSetScalar >::createAndAdd( shared_from_this(), "in",
                                                "The scalar dataset." );

    WModule::connectors();
}

void WMProbTractContext::properties()

{
    // Initialize the properties
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_isoValue = m_properties->addProperty( "Isovalue", "The isovalue used for the context.", 80.0 );

    m_isoColor = m_properties->addProperty( "Iso Color", "The color to blend the isosurface with.", WColor( 1.0, 1.0, 1.0, 1.0 ), m_propCondition );

    WModule::properties();
}

void WMProbTractContext::requirements()
{
    // Put the code for your requirements here. See "src/modules/template/" for an extensively documented example.
}

void WMProbTractContext::moduleMain()
{
    // create an instance using the prototypes
    m_visModule = WModuleFactory::getModuleFactory()->create(
                WModuleFactory::getModuleFactory()->getPrototypeByName( "ProbTractVis" ) );

    // add the marching cubes to the container
    add( m_visModule );

    // now wait for it to be ready
    m_visModule->isReady().wait();

    // get needed probtractvis properties
    boost::shared_ptr< WProperties > visProps = m_visModule->getProperties();

    // step count
    m_stepCount = visProps->getProperty( "Step Count" )->toPropInt();
    m_stepCount->set( 250 );
    m_stepCount->setMin( 1 );
    m_stepCount->setMax( 1000 );
    m_properties->addProperty( m_stepCount );

    // alpha
    m_alpha = visProps->getProperty( "Opacity %" )->toPropDouble();
    m_alpha->setMin( 0.0 );
    m_alpha->setMax( 1.0 );
    m_properties->addProperty( m_alpha );

    // phong shading
    m_phong = visProps->getProperty( "Phong Shading" )->toPropBool();
    m_phong->set( true );
    m_properties->addProperty( m_phong );

    // stochastic jitter
    m_jitter = visProps->getProperty( "Stochastic Jitter" )->toPropBool();
    m_jitter->set( true );
    m_properties->addProperty( m_jitter );

    //TODO(aberres): more? e.g. colormapRatio

    // the WGEShader can take the name of any glsl shader (without extension) in the shaders folder
    m_shader = osg::ref_ptr< WGEShader > ( new WGEShader( "WMProbTractContext", m_localPath ) );
    m_shader->addPreprocessor( WGEShaderPreprocessor::SPtr(
        new WGEShaderPropertyDefineOptions< WPropBool >( m_jitter, "STOCHASTICJITTER_DISABLED", "STOCHASTICJITTER_ENABLED" ) )
    );
    m_shader->addPreprocessor( WGEShaderPreprocessor::SPtr(
        new WGEShaderPropertyDefineOptions< WPropBool >( m_phong, "PHONGSHADING_DISABLED", "PHONGSHADING_ENABLED" ) )
    );
//    WGEShaderDefineSwitch::SPtr gradTexEnableDefine = m_shader->setDefine( "GRADIENTTEXTURE_DISABLED" );

    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );


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
        bool dataUpdated = m_input->updated();// || m_gradients->updated();
        boost::shared_ptr< WDataSetScalar > dataSet = m_input->getData();
        bool dataValid   = ( dataSet );

        // valid data available?
        if ( !dataValid )
        {
            // remove renderings if no data is available anymore
            rootNode->clear();
        }

        // As the data has changed, we need to recreate the texture.
        if ( dataUpdated && dataValid )
        {
            debugLog() << "Data changed. Uploading new data as texture.";

            // adapt shader to dataset's isovalues
            m_isoValue->setMin( dataSet->getTexture()->minimum()->get() );
            m_isoValue->setMax( dataSet->getTexture()->scale()->get() + dataSet->getTexture()->minimum()->get() );
            m_isoValue->set( dataSet->getTexture()->minimum()->get() + ( 0.5 * dataSet->getTexture()->scale()->get() ) );

            // get NavSlice positions
            m_axial = WKernel::getRunningKernel()->getSelectionManager()->getPropAxialPos();
            m_coronal = WKernel::getRunningKernel()->getSelectionManager()->getPropCoronalPos();
            m_sagittal = WKernel::getRunningKernel()->getSelectionManager()->getPropSagittalPos();

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
            osg::ref_ptr< WGETexture3D > texture3D = dataSet->getTexture();
            texture3D->bind( cube );

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // setup all those uniforms and additional textures
            ////////////////////////////////////////////////////////////////////////////////////////////////////
            rootState->addUniform( new WGEPropertyUniform< WPropDouble >( "u_axial", m_axial ) );
            rootState->addUniform( new WGEPropertyUniform< WPropDouble >( "u_coronal", m_coronal ) );
            rootState->addUniform( new WGEPropertyUniform< WPropDouble >( "u_sagittal", m_sagittal ) );
            rootState->addUniform( new WGEPropertyUniform< WPropColor >( "u_isocolor", m_isoColor ) );
            rootState->addUniform( new WGEPropertyUniform< WPropDouble >( "u_isovalue", m_isoValue ) );
            rootState->addUniform( new WGEPropertyUniform< WPropInt >( "u_steps", m_stepCount ) );
            rootState->addUniform( new WGEPropertyUniform< WPropDouble >( "u_alpha", m_alpha ) );
//            rootState->addUniform( new WGEPropertyUniform< WProp >( "u_", m_ ) );

            // Stochastic jitter?
            const size_t size = 64;
            osg::ref_ptr< WGETexture2D > randTex = wge::genWhiteNoiseTexture( size, size, 1 );
            wge::bindTexture( cube, randTex, 1 );

            //TODO(aberres): why?
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
