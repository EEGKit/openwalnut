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

#include <teem/elf.h>

#include <string>

#include <osg/LightModel>

#include "../../common/WAssert.h"
#include "../../common/WPropertyHelper.h"
#include "../../common/WLimits.h"
#include "../../kernel/WKernel.h"
#include "home.xpm"

#include "WMHomeGlyphs.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMHomeGlyphs )

const size_t WMHomeGlyphs::m_nbVertCoords = 4;

WMHomeGlyphs::WMHomeGlyphs():
    WModule(),
    m_recompute( boost::shared_ptr< WCondition >( new WCondition() ) )
{
}

WMHomeGlyphs::~WMHomeGlyphs()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMHomeGlyphs::factory() const
{
    return boost::shared_ptr< WModule >( new WMHomeGlyphs() );
}

const char** WMHomeGlyphs::getXPMIcon() const
{
    return home_xpm;
}


const std::string WMHomeGlyphs::getName() const
{
    return "HOME Glyphs";
}

const std::string WMHomeGlyphs::getDescription() const
{
    return "Higher-Order Tensor Glyphs as described at http://www.ci.uchicago.edu/~schultz/sphinx/home-glyph.html";
}

void WMHomeGlyphs::connectors()
{
    m_input = boost::shared_ptr< WModuleInputData < WDataSetSphericalHarmonics > >(
        new WModuleInputData< WDataSetSphericalHarmonics >( shared_from_this(), "in", "The input dataset." ) );
    addConnector( m_input );


    // call WModules initialization
    WModule::connectors();
}

void WMHomeGlyphs::properties()
{
    m_sliceOrientations = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_sliceOrientations->addItem( "x", "x-slice" );
    m_sliceOrientations->addItem( "y", "y-slice" );
    m_sliceOrientations->addItem( "z", "z-slice" );
    m_sliceOrientationSelection = m_properties->addProperty( "Slice Orientation",
                                                             "Which slice will be shown?",
                                                             m_sliceOrientations->getSelector( 1 ),
                                                             m_recompute );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_sliceOrientationSelection );

    m_glyphSizeProp = m_properties->addProperty( "Glyph Size", "Size of the displayed glyphs.", 0.5, m_recompute );
    m_glyphSizeProp->setMin( 0 );
    m_glyphSizeProp->setMax( 100. );

    m_sliceIdProp = m_properties->addProperty( "Slice Id", "Number of the slice to display", 0, m_recompute );
    m_sliceIdProp->setMin( 0 );
    m_sliceIdProp->setMax( 128 );

    m_usePolarPlotProp = m_properties->addProperty( "Use Polar Plot", "Use polar plot for glyph instead of HOME?", false, m_recompute );
}

void WMHomeGlyphs::moduleMain()
{
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_recompute );

    ready();

    // loop until the module container requests the module to quit
    while ( !m_shutdownFlag() )
    {
        if ( !m_input->getData().get() )
        {
            // OK, the output has not yet sent data
            debugLog() << "Waiting for data ...";
            m_moduleState.wait();
            continue;
        }
//        execute();
        renderSlice( m_sliceIdProp->get() );

        m_moduleState.wait();
    }
}

void  WMHomeGlyphs::renderSlice( size_t sliceId )
{
    enum sliceTypeEnum
    {
        xSlice = 0,
        ySlice,
        zSlice
    };

    size_t sliceType = m_sliceOrientationSelection->get( true ).getItemIndexOfSelected( 0 );

    debugLog() << "Rendering slice ... " << sliceId;
    // Please look here  http://www.ci.uchicago.edu/~schultz/sphinx/home-glyph.htm
    if( m_moduleNode )
    {
       WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_moduleNode );
    }

    boost::shared_ptr< WDataSetSphericalHarmonics > dataSet = boost::shared_dynamic_cast< WDataSetSphericalHarmonics >( m_input->getData() );

    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( dataSet->getGrid() );

    // convenience names for grid dimensions
    size_t nX =  grid->getNbCoordsX();
    size_t nY =  grid->getNbCoordsY();
    size_t nZ =  grid->getNbCoordsZ();
    WAssert( sliceId < nX, "Slice id to large." );

    unsigned int level = 3; // subdivision level of sphere
    unsigned int infoBitFlag = ( 1 << limnPolyDataInfoNorm ) | ( 1 << limnPolyDataInfoRGBA );
    limnPolyData *sphere = limnPolyDataNew();
    limnPolyDataIcoSphere( sphere, infoBitFlag, level );

    const tijk_type *type = tijk_4o3d_sym;

    // memory for the tensor and spherical harmonics data.
    float* ten = new float[type->num];
    float* res = new float[type->num];
    float* esh = new float[type->num];

    // preparation of osg stuff for the glyphs
    m_moduleNode = new WGEGroupNode();

    debugLog() << "start loop ... " << sliceId;
    size_t nA = 0; // initialized to quiet compiler
    size_t nB = 0; // initialized to quiet compiler
    switch( sliceType )
    {
        case xSlice:
            nA = nY;
            nB = nZ;
            break;
        case ySlice:
            nA = nX;
            nB = nZ;
            break;
        case zSlice:
            nA = nX;
            nB = nY;
            break;
    }

    size_t nbGlyphs = nA *nB;


    osg::Geometry* glyphGeometry = new osg::Geometry();
    m_glyphsGeode = osg::ref_ptr< osg::Geode >( new osg::Geode );
    m_glyphsGeode->setName( "glyphs" );
    osg::StateSet* state = m_glyphsGeode->getOrCreateStateSet();
    osg::ref_ptr<osg::LightModel> lightModel = new osg::LightModel();
    lightModel->setTwoSided( true );
    state->setAttributeAndModes( lightModel.get(), osg::StateAttribute::ON );
    state->setMode(  GL_BLEND, osg::StateAttribute::ON  );

    WAssert( sphere->xyzwNum == sphere->normNum, "Wrong size of arrays." );
    WAssert( sphere->xyzwNum == sphere->rgbaNum, "Wrong size of arrays." );
    size_t nbVerts = sphere->xyzwNum;

    osg::ref_ptr< osg::Vec3Array > vertArray = new osg::Vec3Array( nbVerts * nbGlyphs );
    osg::ref_ptr< osg::Vec3Array > normals = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array( nbVerts * nbGlyphs ) );
    osg::ref_ptr< osg::Vec4Array > colors = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array( nbVerts * nbGlyphs ) );

    osg::ref_ptr< osg::DrawElementsUInt > glyphElements =
        osg::ref_ptr< osg::DrawElementsUInt >( new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 ) );
    glyphElements->reserve( sphere->indxNum * nbGlyphs );

    bool usePolar = m_usePolarPlotProp->get();
    // run through the positions in the slice and draw the glyphs
    for( size_t aId = 0; aId < nA; ++aId )
    {
        for( size_t bId = 0; bId < nB; ++bId )
        {
            size_t glyphId = aId * nB + bId;
            size_t vertsUpToCurrentIteration = glyphId * nbVerts;

            size_t posId = 0; // initialized to quiet compiler
            switch( sliceType )
            {
                case xSlice:
                    posId = sliceId + aId * nX + bId * nX * nY;
                    break;
                case ySlice:
                    posId = aId + sliceId * nX + bId * nX * nY;
                    break;
                case zSlice:
                    posId = aId + bId * nX + sliceId * nX * nY;
                    break;
            }

            wmath::WValue< double > coeffs = dataSet->getSphericalHarmonicAt( posId ).getCoefficients();
            WAssert( coeffs.size() == 15,
                     "This module can handle only 4th order spherical harmonics."
                     "Thus the input has to be 15 dimensional vectors." );

            for( size_t coeffId = 0; coeffId < 15; coeffId++ )
            {
                esh[coeffId] = coeffs[coeffId];
            }
            // convert even-order spherical harmonics to higher-order tensor
            tijk_esh_to_3d_sym_f( ten, esh, 4 );

            // create positive approximation of the tensor
            tijk_refine_rankk_parm *parm = tijk_refine_rankk_parm_new();
            parm->pos = 1;
            int ret = tijk_approx_rankk_3d_f( NULL, NULL, res, ten, type, 6, parm );
            WAssert( ret == 0, "Error condition in call." );
            parm = tijk_refine_rankk_parm_nix( parm );
            tijk_sub_f( ten, ten, res, type );

            const char normalize = 0;

            limnPolyData *glyph = limnPolyDataNew();
            limnPolyDataCopy( glyph, sphere );

            if( usePolar )
            {
                char isdef = 3; // some initialization
                elfGlyphPolar( glyph, 1, ten, type, &isdef, 0, normalize, NULL, NULL );
                WAssert( isdef != 0, "Tensor is non positive definite. Think about that." );
            }
            else
            {
                elfGlyphHOME( glyph, 1, ten, type, NULL, normalize );
            }

            // -------------------------------------------------------------------------------------------------------
            // One can insert per-peak coloring here (see http://www.ci.uchicago.edu/~schultz/sphinx/home-glyph.html )
            // -------------------------------------------------------------------------------------------------------

            minMaxNormalization( glyph );

            wmath::WPosition glyphPos = grid->getPosition( posId );


            //-------------------------------
            // vertex indices
            for( unsigned int vertId = 0; vertId < glyph->indxNum; ++vertId )
            {
                glyphElements->push_back( vertsUpToCurrentIteration + glyph->indx[vertId] );
            }

            float radius = 1.0 / m_glyphSizeProp->get(); // glyph size

            for( unsigned int vertId = 0; vertId < glyph->xyzwNum; ++vertId )
            {
                //-------------------------------
                // vertices
                ( *vertArray )[vertsUpToCurrentIteration+vertId][0] = glyph->xyzw[m_nbVertCoords*vertId  ] / radius + glyphPos[0];
                ( *vertArray )[vertsUpToCurrentIteration+vertId][1] = glyph->xyzw[m_nbVertCoords*vertId+1] / radius + glyphPos[1];
                ( *vertArray )[vertsUpToCurrentIteration+vertId][2] = glyph->xyzw[m_nbVertCoords*vertId+2] / radius + glyphPos[2];

                // ------------------------------------------------
                // normals
                ( *normals )[vertsUpToCurrentIteration+vertId][0] = glyph->norm[3*vertId];
                ( *normals )[vertsUpToCurrentIteration+vertId][1] = glyph->norm[3*vertId+1];
                ( *normals )[vertsUpToCurrentIteration+vertId][2] = glyph->norm[3*vertId+2];
                ( *normals )[vertsUpToCurrentIteration+vertId].normalize();

                // ------------------------------------------------
                // colors
                const size_t nbColCoords = 4;
                ( *colors )[vertsUpToCurrentIteration+vertId][0] = glyph->rgba[nbColCoords*vertId] / 255.0;
                ( *colors )[vertsUpToCurrentIteration+vertId][1] = glyph->rgba[nbColCoords*vertId+1] / 255.0;
                ( *colors )[vertsUpToCurrentIteration+vertId][2] = glyph->rgba[nbColCoords*vertId+2] / 255.0;
                ( *colors )[vertsUpToCurrentIteration+vertId][3] = glyph->rgba[nbColCoords*vertId+3] / 255.0;
            }

            // free memory
            glyph = limnPolyDataNix( glyph );
        }
    }
    debugLog() << "end loop ... " << sliceId;

    glyphGeometry->setVertexArray( vertArray );
    glyphGeometry->addPrimitiveSet( glyphElements );
    glyphGeometry->setNormalArray( normals );
    glyphGeometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );
    glyphGeometry->setColorArray( colors );
    glyphGeometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

    m_glyphsGeode->addDrawable( glyphGeometry );

    m_shader = osg::ref_ptr< WShader > ( new WShader( "WMHomeGlyphs", m_localPath ) );
    m_shader->apply( m_glyphsGeode );

    m_moduleNode->insert( m_glyphsGeode );


    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_moduleNode );


    // free memory
    sphere = limnPolyDataNix( sphere );

    delete[] ten;
    delete[] res;
    delete[] esh;
}


void WMHomeGlyphs::activate()
{
    if ( m_moduleNode )
    {
        if ( m_active->get() )
        {
            m_moduleNode->setNodeMask( 0xFFFFFFFF );
        }
        else
        {
            m_moduleNode->setNodeMask( 0x0 );
        }
    }
    WModule::activate();
}

void WMHomeGlyphs::minMaxNormalization( limnPolyData *glyph )
{
    double min = wlimits::MAX_DOUBLE;
    double max = -wlimits::MAX_DOUBLE;
    for( size_t i = 0; i < glyph->xyzwNum; ++i )
    {
        wmath::WPosition pos( glyph->xyzw[m_nbVertCoords*i], glyph->xyzw[m_nbVertCoords*i+1],  glyph->xyzw[m_nbVertCoords*i+2] );
        double norm = pos.norm();
        if( norm < min )
        {
            min = norm;
        }
        if( norm > max )
        {
            max = norm;
        }
    }
    double dist = max - min;

    if( dist != 0 )
    {
        WAssert( dist > 0, "Max has to be larger than min." );

        for( size_t i = 0; i < glyph->xyzwNum; ++i )
        {
            wmath::WPosition pos( glyph->xyzw[m_nbVertCoords*i], glyph->xyzw[m_nbVertCoords*i+1],  glyph->xyzw[m_nbVertCoords*i+2] );
            double norm = pos.norm();
            wmath::WPosition newPos = ( ( norm - min ) / dist ) * pos.normalized();
            glyph->xyzw[m_nbVertCoords*i] = newPos[0];
            glyph->xyzw[m_nbVertCoords*i+1] = newPos[1];
            glyph->xyzw[m_nbVertCoords*i+2] = newPos[2];
        }
    }
    // else do nothing because all values are equal.
}
