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

#include <time.h>
#include <vector>
#include <cmath>
#include <iostream>

#include "../../../dataHandler/WDataSetFibers.h"
#include "../../../kernel/WKernel.h"
#include "WTubeDrawable.h"
#include <osg/TexGen>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osg/Texture1D>
#include <osg/Texture2D>
#include <osg/BlendFunc>
#include <osg/AlphaFunc>
#include <osg/Timer>

// The constructor here does nothing. One thing that may be necessary is
// disabling display lists. This can be done by calling
//    setSupportsDisplayList (false);
// Display lists should be disabled for 'Drawable's that can change over
// time (that is, the vertices drawn change from time to time).
WTubeDrawable::WTubeDrawable():
    osg::Drawable(),
    m_useTubes( false ),
    m_globalColoring( true ),
    m_customColoring( false )
{
    setSupportsDisplayList( false );
    // This contructor intentionally left blank. Duh.
}

// I can't say much about the methods below, but OSG seems to expect
// that we implement them.
WTubeDrawable::WTubeDrawable( const WTubeDrawable& /*pg*/, const osg::CopyOp& /*copyop*/ ):
    osg::Drawable()
{
}

osg::Object* WTubeDrawable::cloneType() const
{
    return new WTubeDrawable();
}

osg::Object* WTubeDrawable::clone( const osg::CopyOp& copyop ) const
{
    return new WTubeDrawable( *this, copyop );
}

// Real work is done here. THERE IS A VERY IMPORTANT THING TO NOTE HERE:
// the 'drawImplementation()' method receives an 'osg::State' as
// parameter. This can be used to change the OpenGL state, but changing
// the OpenGL state here is something to be avoided as much as possible.
// Do this *only* if it is *absolutely* necessary to make your rendering
// algorithm work. The "right" (most efficient and flexible) way to change
// the OpenGL state in OSG is by attaching 'StateSet's to 'Node's and
// 'Drawable's.
// That said, the example below shows how to change the OpenGL state in
// these rare cases in which it is necessary. But always keep in mind:
// *Change the OpenGL state only if strictly necessary*.
void WTubeDrawable::drawImplementation( osg::RenderInfo& renderInfo ) const //NOLINT
{
    if ( m_useTubes )
    {
        drawTubes( renderInfo );
    }
    else
    {
        drawFibers( renderInfo );
    }
}

void WTubeDrawable::setDataset( boost::shared_ptr< const WDataSetFibers > dataset )
{
    m_dataset = dataset;
}

void WTubeDrawable::setUseTubes( bool flag )
{
    m_useTubes = flag;
}

void WTubeDrawable::setColoringMode( bool globalColoring )
{
    m_globalColoring = globalColoring;
}

bool WTubeDrawable::getColoringMode() const
{
    return m_globalColoring;
}

void WTubeDrawable::setCustomColoring( bool custom )
{
    m_customColoring = custom;
}

void WTubeDrawable::setBoundingBox( const osg::BoundingBox & bb )
{
    setBound( bb );
}

void WTubeDrawable::drawFibers( osg::RenderInfo& renderInfo ) const //NOLINT
{
    boost::shared_ptr< std::vector< size_t > > startIndexes = m_dataset->getLineStartIndexes();
    boost::shared_ptr< std::vector< size_t > > pointsPerLine = m_dataset->getLineLengths();
    boost::shared_ptr< std::vector< float > > verts = m_dataset->getVertices();
    boost::shared_ptr< std::vector< float > > tangents = m_dataset->getTangents();

    boost::shared_ptr< std::vector< float > > colors;
    if ( m_customColoring )
    {
        colors = WKernel::getRunningKernel()->getRoiManager()->getCustomColors();
    }
    else
    {
        colors = ( m_globalColoring ? m_dataset->getGlobalColors() : m_dataset->getLocalColors() );
    }

    boost::shared_ptr< std::vector< bool > > active = WKernel::getRunningKernel()->getRoiManager()->getBitField();

    osg::State& state = *renderInfo.getState();

    state.disableAllVertexArrays();
    state.setVertexPointer( 3, GL_FLOAT , 0, &( *verts )[0] );
    state.setColorPointer( 3 , GL_FLOAT , 0, &( *colors )[0] );
    state.setNormalPointer( GL_FLOAT , 0, &( *tangents )[0] );
    for ( size_t i = 0; i < active->size(); ++i )
    {
        if ( (*active)[i] )
        {
            state.glDrawArraysInstanced( GL_LINE_STRIP, (*startIndexes)[i], (*pointsPerLine)[i], 1);
        }
    }

    state.disableVertexPointer();
    state.disableColorPointer();
}

void WTubeDrawable::drawTubes( osg::RenderInfo& renderInfo ) const
{
    boost::shared_ptr< std::vector< size_t > > startIndexes = m_dataset->getLineStartIndexes();
    boost::shared_ptr< std::vector< size_t > > pointsPerLine = m_dataset->getLineLengths();
    boost::shared_ptr< std::vector< bool > > active = WKernel::getRunningKernel()->getRoiManager()->getBitField();
    boost::shared_ptr< std::vector< float > > colors;
    if ( m_customColoring )
    {
        colors = WKernel::getRunningKernel()->getRoiManager()->getCustomColors();
    }
    else
    {
        colors = ( m_globalColoring ? m_dataset->getGlobalColors() : m_dataset->getLocalColors() );
    }

    osg::State& state = *renderInfo.getState();

    state.disableAllVertexArrays();

    //state.setVertexPointer( m_tubeVerts);
    //state.setColorPointer( m_tubeColors );
    //state.setNormalPointer(m_tubeTangents );
    //state.setTexCoordPointer(0, m_tubeTexCoords );

    glEnable( GL_DEPTH_TEST );
    glDisable( GL_LIGHTING );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    //if(m_useQuadStrips)
    {
        for ( size_t i = 0; i < active->size(); ++i )
        {
            if ( (*active)[i] )
            {
                //state.glDrawArraysInstanced( GL_QUAD_STRIP, m_tubeStartIndexes->at(i), 2 * (*pointsPerLine)[i], 1);
            }
        }
    }

    glEnable( GL_VERTEX_PROGRAM_POINT_SIZE );
    glEnable( GL_ALPHA_TEST );
    glEnable( GL_POINT_SPRITE );

    glAlphaFunc( GL_NOTEQUAL, 0.0 );
    glTexEnvi( GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE );

    //if(m_usePointSprite)
    {
        for ( size_t i = 0; i < active->size(); ++i )
        {
            if ( (*active)[i] )
            {
                //state.glDrawArraysInstanced( GL_POINTS, m_tubeStartIndexes->at(i + active->size()), (*pointsPerLine)[i], 1);
            }
        }
    }

    glDisable( GL_VERTEX_PROGRAM_POINT_SIZE );
    glDisable( GL_ALPHA_TEST );
    glDisable( GL_POINT_SPRITE );

    state.disableVertexPointer();
    state.disableColorPointer();
}
