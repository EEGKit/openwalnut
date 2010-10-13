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

#include "../../../dataHandler/WDataSetFibers.h"
#include "../../../kernel/WKernel.h"

#include "WTubeGeometry.h"

WTubeGeometry::WTubeGeometry() : osg::Geometry()
{
}

// I can't say much about the methods below, but OSG seems to expect
// that we implement them.
WTubeGeometry::WTubeGeometry( const WTubeGeometry& /*pg*/, const osg::CopyOp& /*copyop*/ ):
    osg::Geometry()
{
}

osg::Object* WTubeGeometry::cloneType() const
{
    return new WTubeGeometry();
}

osg::Object* WTubeGeometry::clone( const osg::CopyOp& copyop ) const
{
    return new WTubeGeometry( *this, copyop );
}

void WTubeGeometry::setDataset( boost::shared_ptr< const WDataSetFibers > dataset )
{
    m_dataset = dataset;
}

void WTubeGeometry::setStartIndices( boost::shared_ptr< std::vector< size_t > > startIndices )
{
    m_startIndices = startIndices;
}

void WTubeGeometry::setViewPortUniform( osg::ref_ptr<osg::Uniform> uniformViewportHeight, osg::ref_ptr<osg::Uniform> uniformViewportWidth )
{
    m_uniformViewportHeight = uniformViewportHeight;
    m_uniformViewportWidth = uniformViewportWidth;
}

void WTubeGeometry::drawImplementation( osg::RenderInfo& renderInfo ) const
{
    if ( _internalOptimizedGeometry.valid() )
    {
        _internalOptimizedGeometry->drawImplementation( renderInfo );
        return;
    }

    osg::State& state = *renderInfo.getState();

    bool checkForGLErrors = state.getCheckForGLErrors() == osg::State::ONCE_PER_ATTRIBUTE;
    if (checkForGLErrors) state.checkGLErrors( "start of Geometry::drawImplementation()" );

    bool useFastPath = areFastPathsUsed();
    useFastPath = false;

    bool usingVertexBufferObjects = _useVertexBufferObjects && state.isVertexBufferObjectSupported();
    bool handleVertexAttributes = !_vertexAttribList.empty();

    osg::ArrayDispatchers& arrayDispatchers = state.getArrayDispatchers();

    arrayDispatchers.reset();
    arrayDispatchers.setUseVertexAttribAlias(useFastPath && state.getUseVertexAttributeAliasing());
    arrayDispatchers.setUseGLBeginEndAdapter(!useFastPath);

    arrayDispatchers.activateNormalArray(_normalData.binding, _normalData.array.get(), _normalData.indices.get());
    arrayDispatchers.activateColorArray(_colorData.binding, _colorData.array.get(), _colorData.indices.get());
    arrayDispatchers.activateSecondaryColorArray(_secondaryColorData.binding, _secondaryColorData.array.get(), _secondaryColorData.indices.get());
    arrayDispatchers.activateFogCoordArray(_fogCoordData.binding, _fogCoordData.array.get(), _fogCoordData.indices.get());

    if (handleVertexAttributes)
    {
        for(unsigned int unit=0;unit<_vertexAttribList.size();++unit)
        {
            arrayDispatchers.activateVertexAttribArray(_vertexAttribList[unit].binding,
            unit, _vertexAttribList[unit].array.get(), _vertexAttribList[unit].indices.get());
        }
    }
    state.disableAllVertexArrays();

    // dispatch any attributes that are bound overall
    arrayDispatchers.dispatch(BIND_OVERALL,0);

    state.lazyDisablingOfVertexAttributes();

    // set up arrays
    state.setVertexPointer( _vertexData.array.get() );

    state.setNormalPointer( _normalData.array.get() );

    state.setColorPointer( _colorData.array.get() );

    for( unsigned int unit = 0; unit < _texCoordList.size(); ++unit )
    {
        const osg::Array* array = _texCoordList[unit].array.get();
        if ( array ) state.setTexCoordPointer( unit, array );
    }

    for( unsigned int index = 0; index < _vertexAttribList.size(); ++index )
    {
        const osg::Array* array = _vertexAttribList[index].array.get();
        const AttributeBinding ab = _vertexAttribList[index].binding;
        if( ab == BIND_PER_VERTEX && array )
        {
            state.setVertexAttribPointer( index, array, _vertexAttribList[index].normalize );
        }
    }


    state.applyDisablingOfVertexAttributes();

    bool bindPerPrimitiveSetActive = arrayDispatchers.active(BIND_PER_PRIMITIVE_SET);
    bool bindPerPrimitiveActive = arrayDispatchers.active(BIND_PER_PRIMITIVE);

    unsigned int primitiveNum = 0;

    if ( checkForGLErrors ) state.checkGLErrors("Geometry::drawImplementation() after vertex arrays setup.");


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // draw the primitives themselves.
    //

    GLenum mode = _primitives[0].get()->getMode();
    switch( mode )
    {
        case( GL_POINTS ):
            {
                glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
                glEnable( GL_VERTEX_PROGRAM_POINT_SIZE );
                glEnable( GL_ALPHA_TEST );
                glEnable( GL_POINT_SPRITE );
                glAlphaFunc( GL_NOTEQUAL, 0.0 );
                glTexEnvi( GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE );
                m_uniformViewportHeight->set( static_cast<int>( ( *renderInfo.getCurrentCamera()->getViewport() ).height() ) );
                m_uniformViewportWidth->set( static_cast<int>( ( *renderInfo.getCurrentCamera()->getViewport() ).width() ) );
                break;
            }
        case( GL_QUAD_STRIP ):
            {
                glDisable( GL_VERTEX_PROGRAM_POINT_SIZE );
                glDisable( GL_POINT_SPRITE );
                glEnable( GL_ALPHA_TEST );
                glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
                glAlphaFunc( GL_NOTEQUAL, 0.0 );
                break;
            }
    }
    glEnable( GL_DEPTH_TEST );
    glDisable( GL_LIGHTING );
    glDisable( GL_BLEND );

    boost::shared_ptr< std::vector< bool > > active = WKernel::getRunningKernel()->getRoiManager()->getBitField();
    boost::shared_ptr< std::vector< size_t > > pointsPerLine = m_dataset->getLineLengths();

    if ( useFastPath )
    {
        for( unsigned int primitiveSetNum = 0; primitiveSetNum != _primitives.size(); ++primitiveSetNum )
        {
            // dispatch any attributes that are bound per primitive
            //if (bindPerPrimitiveSetActive) arrayDispatchers.dispatch(BIND_PER_PRIMITIVE_SET, primitiveSetNum);

            const osg::PrimitiveSet* primitiveset = _primitives[primitiveSetNum].get();
            primitiveset->draw( state, true );
        }
    }
    else
    {
        for ( size_t i = 0; i < active->size(); ++i )
        {
            if( (*active)[i] )
            {
                switch( mode )
                {
                    case( GL_POINTS ):
                        {
                            state.glDrawArraysInstanced( mode, m_startIndices->at( i ), ( *pointsPerLine )[i], 1);
                            break;
                        }
                    case( GL_QUAD_STRIP ):
                        {
                            state.glDrawArraysInstanced( mode, m_startIndices->at( i ), 2 * ( *pointsPerLine )[i], 1);
                            break;
                        }
                }
            }
        }
    }

    // unbind the VBO's if any are used.
    state.unbindVertexBufferObject();
    state.unbindElementBufferObject();
    state.disableVertexPointer();
    state.disableColorPointer();
    state.disableNormalPointer();
    state.disableVertexAttribPointer( 0 );
}
