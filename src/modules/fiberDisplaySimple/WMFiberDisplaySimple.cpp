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

#include "../../common/WPropertyHelper.h"
#include "../../common/WPropertyObserver.h"
#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/WDataSetFibers.h"
#include "../../dataHandler/WDataTexture3D.h"
#include "../../graphicsEngine/callbacks/WGEFunctorCallback.h"
#include "../../graphicsEngine/callbacks/WGENodeMaskCallback.h"
#include "../../graphicsEngine/WGEColormapping.h"
#include "../../graphicsEngine/shaders/WGEShader.h"
#include "../../graphicsEngine/shaders/WGEShaderDefineOptions.h"
#include "../../graphicsEngine/shaders/WGEShaderPropertyDefineOptions.h"
#include "../../graphicsEngine/WGEManagedGroupNode.h"
#include "../../kernel/WKernel.h"
#include "WMFiberDisplaySimple.h"
#include "WMFiberDisplaySimple.xpm"

// This line is needed by the module loader to actually find your module. You need to add this to your module too. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMFiberDisplaySimple )

WMFiberDisplaySimple::WMFiberDisplaySimple():
    WModule()
{
}

WMFiberDisplaySimple::~WMFiberDisplaySimple()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMFiberDisplaySimple::factory() const
{
    return boost::shared_ptr< WModule >( new WMFiberDisplaySimple() );
}

const char** WMFiberDisplaySimple::getXPMIcon() const
{
    return WMFiberDisplaySimple_xpm;
}

const std::string WMFiberDisplaySimple::getName() const
{
    return "Fiber Display Simple";
}

const std::string WMFiberDisplaySimple::getDescription() const
{
    return "Display fibers. This module, unlike the full-fletched Fiber Display, can't handle ROIs. It simply draws fibers.";
}

void WMFiberDisplaySimple::connectors()
{
    // The input fiber dataset
    m_fiberInput = boost::shared_ptr< WModuleInputData < WDataSetFibers > >(
        new WModuleInputData< WDataSetFibers >( shared_from_this(), "fibers", "The fiber dataset to color" )
    );

    // As properties, every connector needs to be added to the list of connectors.
    addConnector( m_fiberInput );

    // call WModule's initialization
    WModule::connectors();
}

void WMFiberDisplaySimple::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_illuminationEnable = m_properties->addProperty( "Illumination", "Enable line illumination.", true );

    m_colormapRatio = m_properties->addProperty( "Colormap Ratio", "Defines the ratio between colormap and fiber color.", 0.0 );
    m_colormapRatio->setMin( 0.0 );
    m_colormapRatio->setMax( 1.0 );

    m_clipPlaneGroup = m_properties->addPropertyGroup( "Clipping",  "Clip the fiber data basing on an arbitrary plane." );
    m_clipPlaneEnabled = m_clipPlaneGroup->addProperty( "Enabled", "If set, clipping of fibers is done using an arbitrary plane and plane distance.",
                                                        false );
    m_clipPlaneShowPlane = m_clipPlaneGroup->addProperty( "Show Clip Plane", "If set, the clipping plane will be shown.", false, m_propCondition );
    m_clipPlanePoint = m_clipPlaneGroup->addProperty( "Plane point", "An point on the plane.", WPosition( 0.0, 0.0, 0.0 ) );
    m_clipPlaneVector = m_clipPlaneGroup->addProperty( "Plane normal", "The normal of the plane.", WPosition( 1.0, 0.0, 0.0 ) );
    m_clipPlaneDistance= m_clipPlaneGroup->addProperty( "Clip distance", "The distance from the plane where fibers get clipped.",  10.0 );
    //m_clipPlaneDistance->removeConstraint( m_clipPlaneDistance->getMax() ); // there simply is no max.
    m_clipPlaneDistance->setMin( 0.0 );
    m_clipPlaneDistance->setMax( 1000.0 );

    m_tubeGroup = m_properties->addPropertyGroup( "Tube Rendering", "If true, advanced fake-tube rendering is used." );
    m_tubeEnable = m_tubeGroup->addProperty( "Enable Tubes", "If set, fake-tube rendering is used.", false, m_propCondition  );
    m_tubeRibbon = m_tubeGroup->addProperty( "Ribbon mode", "If set, the tubes look like flat ribbons.", false );
    m_tubeZoomable = m_tubeGroup->addProperty( "Zoomable", "If set, fake-tube get thicker when zoomed in. If not set, they always keep the same "
                                                            "size in screen-space. This emulates standard OpenGL lines.", true );
    m_tubeSize = m_tubeGroup->addProperty( "Width", "The size of the tubes.", 2.0 );
    m_tubeSize->setMin( 0.10 );
    m_tubeSize->setMax( 25.0 );

    // call WModule's initialization
    WModule::properties();
}

/**
 * Enables everything which is needed for proper transparency.
 *
 * \param state the state of the node
 */
void enableTransparency( osg::StateSet* state )
{
    state->setMode( GL_BLEND, osg::StateAttribute::ON );
    state->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

    // Enable depth test so that an opaque polygon will occlude a transparent one behind it.
    state->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );

    // Conversely, disable writing to depth buffer so that a transparent polygon will allow polygons behind it to shine through.
    // OSG renders transparent polygons after opaque ones.
    osg::Depth* depth = new osg::Depth;
    depth->setWriteMask( false );
    state->setAttributeAndModes( depth, osg::StateAttribute::ON );

    // disable light for this geode as lines can't be lit properly
    state->setMode( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );
}

/**
 * Disables everything which is needed for proper transparency.
 *
 * \param state the state of the node
 */
void disableTransparency( osg::StateSet* state )
{
    // TODO(ebaum): this transparency is problematic. Depth write is on because we need the depth buffer for post-processing
    state->setMode( GL_BLEND, osg::StateAttribute::OFF );

    // Enable depth test so that an opaque polygon will occlude a transparent one behind it.
    state->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );

    // disable light for this geode as lines can't be lit properly
    state->setMode( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );
}

void WMFiberDisplaySimple::moduleMain()
{
    m_shader = osg::ref_ptr< WGEShader > ( new WGEShader( "WMFiberDisplaySimple", m_localPath ) );

    // initialize illumination shader
    m_shader->addPreprocessor( WGEShaderPreprocessor::SPtr(
        new WGEShaderPropertyDefineOptions< WPropBool >( m_illuminationEnable, "ILLUMINATION_DISABLED", "ILLUMINATION_ENABLED" ) )
    );

    // initialize clipping shader
    osg::ref_ptr< WGEPropertyUniform< WPropPosition > > clipPlanePointUniform    = new WGEPropertyUniform< WPropPosition >( "u_planePoint",
                                                                                                                            m_clipPlanePoint );
    osg::ref_ptr< WGEPropertyUniform< WPropPosition > > clipPlaneVectorUniform   = new WGEPropertyUniform< WPropPosition >( "u_planeVector",
                                                                                                                            m_clipPlaneVector );
    osg::ref_ptr< WGEPropertyUniform< WPropDouble > >   clipPlaneDistanceUniform = new WGEPropertyUniform< WPropDouble >( "u_distance",
                                                                                                                           m_clipPlaneDistance );
    m_shader->addPreprocessor( WGEShaderPreprocessor::SPtr(
        new WGEShaderPropertyDefineOptions< WPropBool >( m_clipPlaneEnabled, "CLIPPLANE_DISABLED", "CLIPPLANE_ENABLED" ) )
    );

    // init tube shader
    m_shader->addPreprocessor( WGEShaderPreprocessor::SPtr(
        new WGEShaderPropertyDefineOptions< WPropBool >( m_tubeEnable, "TUBE_DISABLED", "TUBE_ENABLED" ) )
    );
    m_shader->addPreprocessor( WGEShaderPreprocessor::SPtr(
        new WGEShaderPropertyDefineOptions< WPropBool >( m_tubeZoomable, "ZOOMABLE_DISABLED", "ZOOMABLE_ENABLED" ) )
    );
    m_shader->addPreprocessor( WGEShaderPreprocessor::SPtr(
        new WGEShaderPropertyDefineOptions< WPropBool >( m_tubeRibbon, "RIBBON_DISABLED", "RIBBON_ENABLED" ) )
    );
    osg::ref_ptr< WGEPropertyUniform< WPropDouble > > tubeSizeUniform = new WGEPropertyUniform< WPropDouble >( "u_tubeSize", m_tubeSize );
    osg::ref_ptr< WGEPropertyUniform< WPropDouble > > colormapRationUniform =
        new WGEPropertyUniform< WPropDouble >( "u_colormapRatio", m_colormapRatio );

    // get notified about data changes
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_fiberInput->getDataChangedCondition() );
    // Remember the condition provided to some properties in properties()? The condition can now be used with this condition set.
    m_moduleState.add( m_propCondition );

    ready();

    // this node keeps the geode
    osg::ref_ptr< WGEManagedGroupNode > rootNode = osg::ref_ptr< WGEManagedGroupNode >( new WGEManagedGroupNode( m_active ) );
    osg::StateSet* rootState = rootNode->getOrCreateStateSet();

    // do not forget to add the uniforms
    rootState->addUniform( clipPlanePointUniform );
    rootState->addUniform( clipPlaneVectorUniform );
    rootState->addUniform( clipPlaneDistanceUniform );
    rootState->addUniform( tubeSizeUniform );
    rootState->addUniform( colormapRationUniform );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( rootNode );

    // needed to observe the properties of the input connector data
    boost::shared_ptr< WPropertyObserver > propObserver = WPropertyObserver::create();
    m_moduleState.add( propObserver );

    m_plane = createClipPlane();

    // main loop
    while ( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish?
        if ( m_shutdownFlag() )
        {
            break;
        }

        //////////////////////////////////////////////////////////////////////////////////////////
        // Get and check data
        //////////////////////////////////////////////////////////////////////////////////////////

        // To query whether an input was updated, simply ask the input:
        bool dataUpdated = m_fiberInput->updated();
        boost::shared_ptr< WDataSetFibers > fibers = m_fiberInput->getData();
        bool dataValid = ( fibers );
        bool dataPropertiesUpdated = propObserver->updated();
        bool propertiesUpdated = m_clipPlaneShowPlane->changed();

        // reset graphics if noting is on the input
        if ( !dataValid )
        {
            debugLog() << "Resetting.";
            // remove geode if no valid data is available
            rootNode->clear();

            // remove the fib's properties from my props
            m_properties->removeProperty( m_fibProps );
            m_fibProps.reset();
        }

        // something happened we are interested in?
        if ( !( dataValid && ( propertiesUpdated || dataPropertiesUpdated || dataUpdated ) ) )
        {
            continue;
        }

        // update the prop observer if new data is available
        m_properties->removeProperty( m_fibProps );
        m_fibProps = fibers->getProperties();
        propObserver->observe( m_fibProps );
        propObserver->handled();
        // also add the fib props to own props. This allows the user to modify the fib props directly
        m_properties->addProperty( m_fibProps );

        //////////////////////////////////////////////////////////////////////////////////////////
        // Create new rendering
        //////////////////////////////////////////////////////////////////////////////////////////

        // add geode to module node
        rootNode->clear();

        // create the fiber geode
        osg::ref_ptr< osg::Node > geode = createFiberGeode( fibers );

        // Apply the shader. This is for clipping.
        m_shader->apply( geode );

        // apply colormapping
        WGEColormapping::apply( geode, m_shader );

        // Add geometry
        // Add geometry
        if ( m_clipPlaneShowPlane->get() )
        {
            rootNode->insert( m_plane );
        }
        else
        {
            rootNode->remove( m_plane );
        }

        rootNode->insert( geode );
    }

    // At this point, the container managing this module signalled to shutdown. The main loop has ended and you should clean up. Always remove
    // allocated memory and remove all OSG nodes.
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( rootNode );
}

void WMFiberDisplaySimple::clipPlaneCallback( osg::Node* node ) const
{
    // NOTE: this callback is only executed if the plane is enabled since the NodeMaskCallback ensures proper activation of the node
    osg::MatrixTransform* transform = static_cast< osg::MatrixTransform* >( node );

    WPosition v = m_clipPlaneVector->get();
    WPosition p = m_clipPlanePoint->get();

    // the point p can be interpreted as translation:
    osg::Matrix translation = osg::Matrix::translate( p );

    // the geometry that was specified has the normal ( 1.0, 0.0, 0.0 ). So it is possible to interpret any other normal as a rotation
    osg::Matrix rotation = osg::Matrix::rotate( osg::Vec3d( 1.0, 0.0, 0.0 ), v );

    transform->setMatrix( rotation * translation );
}

osg::ref_ptr< osg::Node > WMFiberDisplaySimple::createClipPlane() const
{
    // add the clipping plane
    osg::ref_ptr< osg::Geode > planeGeode = new osg::Geode();
    osg::ref_ptr< osg::MatrixTransform > planeTransform = new osg::MatrixTransform();
    osg::ref_ptr< osg::Vec3Array > planeVertices = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    osg::ref_ptr< osg::Vec4Array > planeColor = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    osg::ref_ptr< osg::Geometry > planeGeometry = osg::ref_ptr< osg::Geometry >( new osg::Geometry );

    // the plane vertices
    planeColor->push_back( osg::Vec4( 1.0, 0.0, 0.0, 0.125 ) );
    planeVertices->push_back( osg::Vec3( 0.0, -100.0, -100.0 ) );
    planeVertices->push_back( osg::Vec3( 0.0, -100.0,  100.0 ) );
    planeVertices->push_back( osg::Vec3( 0.0,  100.0,  100.0 ) );
    planeVertices->push_back( osg::Vec3( 0.0,  100.0, -100.0 ) );

    // build geometry
    planeGeometry->setVertexArray( planeVertices );
    planeGeometry->setColorArray( planeColor );
    planeGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );
    planeGeometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, 4 ) );
    planeGeode->addDrawable( planeGeometry );

    enableTransparency( planeGeode->getOrCreateStateSet() );

    // add a callback for showing and hiding the plane
    planeTransform->addUpdateCallback( new WGENodeMaskCallback( m_clipPlaneShowPlane ) );
    // add a callback which actually moves, scales and rotates the plane according to the plane parameter
    planeTransform->addUpdateCallback( new WGEFunctorCallback< osg::Node >(
        boost::bind( &WMFiberDisplaySimple::clipPlaneCallback, this, _1 ) )
    );

    // add the geode to the root and provide an callback
    planeTransform->addChild( planeGeode );

    return planeTransform;
}

osg::ref_ptr< osg::Node > WMFiberDisplaySimple::createFiberGeode( boost::shared_ptr< WDataSetFibers > fibers ) const
{
    // geode and geometry
    osg::ref_ptr< osg::Geode > geode = new osg::Geode();
    osg::StateSet* state = geode->getOrCreateStateSet();

    // disable light for this geode as lines can't be lit properly
    state->setMode( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );

    // create everytring needed for the line_strip drawable
    osg::ref_ptr< osg::Vec3Array > vertices = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    osg::ref_ptr< osg::Vec4Array > colors = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    osg::ref_ptr< osg::Vec3Array > tangents = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    osg::ref_ptr< osg::FloatArray > texcoords = osg::ref_ptr< osg::FloatArray >( new osg::FloatArray );
    osg::ref_ptr< osg::Geometry > geometry = osg::ref_ptr< osg::Geometry >( new osg::Geometry );

    // needed arrays for iterating the fibers
    WDataSetFibers::IndexArray  fibStart = fibers->getLineStartIndexes();
    WDataSetFibers::LengthArray fibLen   = fibers->getLineLengths();
    WDataSetFibers::VertexArray fibVerts = fibers->getVertices();
    WDataSetFibers::TangentArray fibTangents = fibers->getTangents();

    // get current color scheme - the mode is important as it defines the number of floats in the color array per vertex.
    WDataSetFibers::ColorScheme::ColorMode fibColorMode = fibers->getColorScheme()->getMode();
    debugLog() << "Color mode is " << fibColorMode << ".";
    WDataSetFibers::ColorArray  fibColors = fibers->getColorScheme()->getColor();

    // enable blending, select transparent bin
    if ( fibColorMode == WDataSetFibers::ColorScheme::RGBA )
    {
        enableTransparency( state );
    }
    else
    {
        disableTransparency( state );
    }

    // progress indication
    boost::shared_ptr< WProgress > progress1 = boost::shared_ptr< WProgress >( new WProgress( "Adding fibers to geode", fibStart->size() ) );
    m_progress->addSubProgress( progress1 );

    // for each fiber:
    debugLog() << "Iterating over all fibers.";
    size_t currentStart = 0;
    for( size_t fidx = 0; fidx < fibStart->size() ; ++fidx )
    {
        ++*progress1;

        // the start vertex index
        size_t sidx = fibStart->at( fidx ) * 3;
        size_t csidx = fibStart->at( fidx ) * fibColorMode;

        // the length of the fiber
        size_t len = fibLen->at( fidx );

        // walk along the fiber
        for ( size_t k = 0; k < len; ++k )
        {
            osg::Vec3 vert = osg::Vec3( fibVerts->at( ( 3 * k ) + sidx ),
                                        fibVerts->at( ( 3 * k ) + sidx + 1 ),
                                        fibVerts->at( ( 3 * k ) + sidx + 2 ) );
            osg::Vec4 color = osg::Vec4( fibColors->at( ( fibColorMode * k ) + csidx + ( 0 % fibColorMode ) ),
                                         fibColors->at( ( fibColorMode * k ) + csidx + ( 1 % fibColorMode ) ),
                                         fibColors->at( ( fibColorMode * k ) + csidx + ( 2 % fibColorMode ) ),
                                         ( fibColorMode == WDataSetFibers::ColorScheme::RGBA ) ?
                                            fibColors->at( ( fibColorMode * k ) + csidx + ( 3 % fibColorMode ) ) : 1.0 );
            osg::Vec3 tangent = osg::Vec3( fibTangents->at( ( 3 * k ) + sidx ),
                                           fibTangents->at( ( 3 * k ) + sidx + 1 ),
                                           fibTangents->at( ( 3 * k ) + sidx + 2 ) );
            tangent.normalize();

            vertices->push_back( vert );
            colors->push_back( color );
            tangents->push_back( tangent );

            if ( m_tubeEnable->get() )
            {
                vertices->push_back( vert );
                colors->push_back( color );
                tangents->push_back( tangent );

                // tex coords are only needed for fake-tubes
                // NOTE: another possibility to transport the information of top and bottom vertex is to use the sign of tangent.x for example.
                // This saves some mem.
                texcoords->push_back( 1.0 );
                texcoords->push_back( -1.0 );
            }
        }

        // add the above line-strip
        if ( m_tubeEnable->get() )
        {
            geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUAD_STRIP, 2 * currentStart, 2 * len ) );
        }
        else
        {
            geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, currentStart, len ) );
        }

        currentStart += len;
    }

    // combine these arrays to the geometry
    geometry->setVertexArray( vertices );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
    geometry->setNormalArray( tangents );
    geometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );
    if ( m_tubeEnable->get() )    // tex coords are only needed for fake-tubes
    {
        geometry->setTexCoordArray( 0, texcoords );
    }

    // set drawable
    geode->addDrawable( geometry );

    debugLog() << "Iterating over all fibers: done!";
    progress1->finish();

    return geode;
}

