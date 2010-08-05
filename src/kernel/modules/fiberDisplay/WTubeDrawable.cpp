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
#include "WTubeDrawable.h"
#include <osg/TexGen>
#include <osg/Texture1D>
#include <osg/Texture2D>

#include <osg/Timer>
#include <cmath>
#include <iostream>
#include <time.h>

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
	m_uniformViewportHeight = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "u_viewportHeight",
																			static_cast<int>( 0 ) ) );
	m_uniformViewportWidth = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "u_viewportWidth",
																			static_cast<int>( 0 ) ) );
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
	// calculate fps
	osg::Timer *clock = osg::Timer::instance();
	clock->setStartTick();

	if ( m_useTubes )
    {
		drawTubes( renderInfo );
    }
    else
    {
        drawFibers( renderInfo );
    }
	//std::cout << "FPS: " << (float)1000/clock->time_m() << std::endl;
}

void WTubeDrawable::setDataset( boost::shared_ptr< const WDataSetFibers > dataset )
{
    m_dataset = dataset;

	boost::shared_ptr< std::vector< size_t > > pointsPerLine = m_dataset->getLineLengths();
	boost::shared_ptr< std::vector< float > > verts = m_dataset->getVertices();
	boost::shared_ptr< std::vector< float > > tangents = m_dataset->getTangents();
	boost::shared_ptr< std::vector< float > > colors;
	boost::shared_ptr< std::vector< size_t > > startIndices = m_dataset->getLineStartIndexes();
	if ( m_customColoring )
	{
		colors = WKernel::getRunningKernel()->getRoiManager()->getCustomColors();
	}
	else
	{
		colors = ( m_globalColoring ? m_dataset->getGlobalColors() : m_dataset->getLocalColors() );
	}

	m_tubeVerts = new osg::Vec3Array;
	m_tubeTangents = new osg::Vec3Array;
	m_tubeTexCoords = new osg::Vec2Array;
	m_pointTexCoords = new osg::Vec2Array;
	m_tubeColors = new osg::Vec3Array;
	m_tubeStartIndexes = new osg::VectorGLuint;
	unsigned int offset = 0;

	// add vertexdata for quadstrips
	for( size_t i = 0; i < startIndices->size(); ++i )
	{
		int idx = startIndices->at( i ) * 3;
		m_tubeStartIndexes->push_back( idx + offset);
		for ( size_t k = 0; k < pointsPerLine->at( i ); ++k )
		{
			m_tubeTangents->push_back(osg::Vec3f( tangents->at( idx ), tangents->at( idx + 1 ), tangents->at( idx + 2 ) ));
			m_tubeTangents->push_back(osg::Vec3f( tangents->at( idx ), tangents->at( idx + 1 ), tangents->at( idx + 2 ) ));

			m_tubeColors->push_back(osg::Vec3f( colors->at( idx ), colors->at( idx + 1 ), colors->at( idx + 2 ) ));
			m_tubeColors->push_back(osg::Vec3f( colors->at( idx ), colors->at( idx + 1 ), colors->at( idx + 2 ) ));

			m_tubeVerts->push_back(osg::Vec3f( verts->at( idx ), verts->at( idx + 1 ), verts->at( idx + 2 ) ));
			m_tubeVerts->push_back(osg::Vec3f( verts->at( idx ), verts->at( idx + 1 ), verts->at( idx + 2 ) ));

			// TexCoords: first 1DTexturePosition, second Positionflag
			// SouthPointPosition
			m_tubeTexCoords->push_back(osg::Vec2f( -1.0f , 0.0f ));

			// NorthPointPosition
			m_tubeTexCoords->push_back(osg::Vec2f( 1.0f, 1.0f ));

			idx += 3;
		}
		offset += 2*pointsPerLine->at(i);
	}

	// add vertexdata for pointsprites
	for( size_t i = 0; i < startIndices->size(); ++i )
	{
		int idx = startIndices->at( i ) * 3;
		for ( size_t k = 0; k < pointsPerLine->at( i ); ++k )
		{
			m_tubeTangents->push_back(osg::Vec3f( tangents->at( idx ), tangents->at( idx + 1 ), tangents->at( idx + 2 ) ));

			m_tubeColors->push_back(osg::Vec3f( colors->at( idx ), colors->at( idx + 1 ), colors->at( idx + 2 ) ));

			m_tubeVerts->push_back(osg::Vec3f( verts->at( idx ), verts->at( idx + 1 ), verts->at( idx + 2 ) ));

			// if k=0 || k=pointPerLine-1 then draw endtexture
			if(k == 0 || k == pointsPerLine->at(i)-1)
			{
				m_tubeTexCoords->push_back(osg::Vec2f( 0.0f , 0.0f ));
			}
			else
			{
				m_tubeTexCoords->push_back(osg::Vec2f( 0.0f , 1.0f ));
			}

			idx += 3;
			}
	}
	/*
	m_tubeVerts = boost::shared_ptr< std::vector< float > >(new std::vector< float >());
	m_tubeTangents = boost::shared_ptr< std::vector< float > >(new std::vector< float >());
	m_tubeTexCoords = boost::shared_ptr< std::vector< float > >(new std::vector< float >());
	m_pointTexCoords = boost::shared_ptr< std::vector< float > >(new std::vector< float >());
	m_tubeColors = boost::shared_ptr< std::vector< float > >(new std::vector< float >());

	m_tubeVerts->resize(2*verts->size());
	m_tubeTangents->resize(2*verts->size());
	m_tubeTexCoords->resize(4*verts->size()/3);
	m_tubeColors->resize(2*verts->size());
	m_pointTexCoords->resize(2*verts->size()/3);

	size_t tmp = 0;
	size_t tmpTex = 0;
	size_t tmpTexPoints = 0;
	int stride = 3;
	std::vector< size_t >::iterator itPointsPerLine = pointsPerLine->begin();
	int k = (*itPointsPerLine);
	for ( size_t i = 0; i < verts->size(); ++i )
	{
		if(stride == 0)
		{
			m_tubeTexCoords->at( tmpTex ) = -1.0f;
			m_tubeTexCoords->at( tmpTex + 1) = 0.0f;
			m_tubeTexCoords->at( tmpTex + 2 ) = 1.0f;
			m_tubeTexCoords->at( tmpTex + 3 ) = 1.0f;

			if(k == 1 || k == (*itPointsPerLine))
			{
				m_tubeTexCoords->at( tmpTexPoints ) = 0.0f;
				m_tubeTexCoords->at( tmpTexPoints + 1) = 0.0f;
			}
			else
			{
				m_tubeTexCoords->at( tmpTexPoints ) = 0.0f;
				m_tubeTexCoords->at( tmpTexPoints + 1) = 1.0f;
			}

			tmp += 3;
			tmpTex += 4;
			stride = 3;
		}
		m_tubeVerts->at(tmp) = verts->at(i);
		m_tubeVerts->at(tmp+3) = verts->at(i);
		m_tubeTangents->at(tmp) = tangents->at(i);
		m_tubeTangents->at(tmp+3) = tangents->at(i);

		m_tubeColors->at(tmp) = colors->at(i);
		m_tubeColors->at(tmp+3) = colors->at(i);
		--stride;
	}
	*/
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



void WTubeDrawable::createTextures(osg::StateSet* m_rootState) const
{
	create1DTextureRectLightning(m_rootState);
	create2DTextureCycleLightning(m_rootState);

	create2DTexDiffuseLightning(m_rootState);
	create2DTexSpecularLightning(m_rootState);
}

void WTubeDrawable::create2DTexDiffuseLightning(osg::StateSet* m_rootState) const
{
	osg::Image* image = new osg::Image;

	int noPixels = 64;

	// allocate the image data, noPixels x noPixels x 1 with 4 rgba floats - equivalent to a Vec4!
	image->allocateImage(noPixels,noPixels,1,GL_RGBA,GL_FLOAT);
	image->setInternalTextureFormat(GL_RGBA);

	float stepAlpha = M_PI/4*(float)noPixels;
	float stepLt = M_PI/2*(float)noPixels;
	float alpha, lt, diffuse = 0.0f;

	// fill in the image data.
	osg::Vec4* dataPtr = (osg::Vec4*)image->data();
	osg::Vec4 color;
	for(int i=0;i<noPixels;++i)
	{
		alpha = (float)i*stepAlpha;

		for(int j=0;j<noPixels;++j)
		{
			lt = (float)j * stepLt;
			diffuse = sqrt(1- pow(lt,2)) * (sin(alpha) + (M_PI-alpha) * cos(alpha)) / 4;
			// Einheitskreis,
			color = osg::Vec4(diffuse, 0.0f, 0.0f, 0.0f);

			*dataPtr++ = color;
		}
	}
	osg::Texture2D* texture = new osg::Texture2D;
	texture->setDataVariance(osg::Object::STATIC);
	texture->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::CLAMP);
	texture->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
	texture->setImage(image);

	m_rootState->setTextureAttribute(2,texture,osg::StateAttribute::OVERRIDE);
	m_rootState->setTextureMode(2,GL_TEXTURE_2D,osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
	m_rootState->addUniform(osg::ref_ptr<osg::Uniform>( new osg::Uniform( "textureDiff2D", 3 ) ) );

}

void WTubeDrawable::create2DTexSpecularLightning(osg::StateSet* m_rootState) const
{
	osg::Image* image = new osg::Image;

	int noPixels = 64;

	// allocate the image data, noPixels x noPixels x 1 with 4 rgba floats - equivalent to a Vec4!
	image->allocateImage(noPixels,noPixels,1,GL_RGBA,GL_FLOAT);
	image->setInternalTextureFormat(GL_RGBA);

	float stepAlpha = M_PI/4*(float)noPixels;
	float stepLt = M_PI/2*(float)noPixels;
	float alpha, lt, diffuse = 0.0f;

	// fill in the image data.
	osg::Vec4* dataPtr = (osg::Vec4*)image->data();
	osg::Vec4 color;
	for(int i=0;i<noPixels;++i)
	{
		alpha = (float)i*stepAlpha;

		for(int j=0;j<noPixels;++j)
		{
			lt = (float)j * stepLt;
			diffuse = sqrt(1- pow(lt,2)) * (sin(alpha) + (M_PI-alpha) * cos(alpha)) / 4;
			// Einheitskreis,
			color = osg::Vec4(diffuse, 0.0f, 0.0f, 0.0f);

			*dataPtr++ = color;
		}
	}
	osg::Texture2D* texture = new osg::Texture2D;
	texture->setDataVariance(osg::Object::STATIC);
	texture->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::CLAMP);
	texture->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
	texture->setImage(image);

	m_rootState->setTextureAttribute(2,texture,osg::StateAttribute::OVERRIDE);
	m_rootState->setTextureMode(2,GL_TEXTURE_2D,osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
	m_rootState->addUniform(osg::ref_ptr<osg::Uniform>( new osg::Uniform( "textureSpec2D", 4 ) ) );

}

void WTubeDrawable::create1DTextureRectLightning(osg::StateSet* m_rootState) const
{
    osg::Image* image = new osg::Image;

    int noPixels = 64;

    // allocate the image data, noPixels x 1 x 1 with 4 rgba floats - equivalent to a Vec4!
    image->allocateImage(noPixels,1,1,GL_RGBA,GL_FLOAT);
    image->setInternalTextureFormat(GL_RGBA);

    float step = M_PI/(float)noPixels;
    float diffuse, specular = 0.0f;

    // fill in the image data.
    osg::Vec4* dataPtr = (osg::Vec4*)image->data();
    osg::Vec4 color;
    for(int i=0;i<noPixels;++i)
    {
        diffuse = sin((float)i*step);
        specular = pow(diffuse, 16);

        color = osg::Vec4(diffuse, specular, 0.0f, 1.0f);
        *dataPtr++ = color;
    }

    osg::Texture1D* texture = new osg::Texture1D;
	texture->setDataVariance(osg::Object::STATIC);
    texture->setWrap(osg::Texture1D::WRAP_S,osg::Texture1D::CLAMP);
    texture->setFilter(osg::Texture1D::MIN_FILTER,osg::Texture1D::LINEAR);
    texture->setImage(image);

    m_rootState->setTextureAttribute(1,texture,osg::StateAttribute::OVERRIDE);
    m_rootState->setTextureMode(1,GL_TEXTURE_1D,osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
    m_rootState->addUniform(osg::ref_ptr<osg::Uniform>( new osg::Uniform( "texture", 1 ) ) );

}


void WTubeDrawable::create2DTextureCycleLightning(osg::StateSet* m_rootState) const
{
    osg::Image* image = new osg::Image;

    int noPixels = 64;

    // allocate the image data, noPixels x noPixels x 1 with 4 rgba floats - equivalent to a Vec4!
    image->allocateImage(noPixels,noPixels,1,GL_RGBA,GL_FLOAT);
    image->setInternalTextureFormat(GL_RGBA);

    float step = M_PI/(float)noPixels;
    float alpha, diffuse, specular = 0.0f;
    float check;

    // fill in the image data.
    osg::Vec4* dataPtr = (osg::Vec4*)image->data();
    osg::Vec4 color;
    for(int i=0;i<noPixels;++i)
    {
		diffuse = sin((float)i*step);
		specular = pow(diffuse, 16);

		for(int j=0;j<noPixels;++j)
        {


			// Einheitskreis,
			check = pow(j-32,2) + pow(i-32,2);
			if(check > 1024)
			{
				alpha = 0.0f;
			}
			else
			{
				alpha = 1.0f;
			}
			color = osg::Vec4(diffuse, specular, alpha, 0.0f);

            *dataPtr++ = color;
        }
    }
    osg::Texture2D* texture = new osg::Texture2D;
	texture->setDataVariance(osg::Object::STATIC);
	texture->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::CLAMP);
    texture->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
    texture->setImage(image);

    m_rootState->setTextureAttribute(2,texture,osg::StateAttribute::OVERRIDE);
    m_rootState->setTextureMode(2,GL_TEXTURE_2D,osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
	m_rootState->addUniform(osg::ref_ptr<osg::Uniform>( new osg::Uniform( "texturePS", 2 ) ) );

}
void WTubeDrawable::setRootState(osg::StateSet* rootState)
{
	m_rootState = rootState;
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
	m_uniformViewportHeight->set( static_cast<int>( (*renderInfo.getCurrentCamera()->getViewport()).height() ) );
	m_uniformViewportWidth->set( static_cast<int>( (*renderInfo.getCurrentCamera()->getViewport()).width() ) );


	osg::State& state = *renderInfo.getState();

	state.disableAllVertexArrays();

	state.setVertexPointer( m_tubeVerts);
	state.setColorPointer( m_tubeColors );
	state.setNormalPointer(m_tubeTangents );
	state.setTexCoordPointer(0, m_tubeTexCoords );

	state.setModeValidity(GL_DEPTH_TEST, osg::StateAttribute::ON);
	state.setModeValidity(GL_LIGHTING, osg::StateAttribute::OFF);

	for ( size_t i = 0; i < active->size(); ++i )
	{
		if ( (*active)[i] )
		{
			state.glDrawArraysInstanced( osg::PrimitiveSet::QUAD_STRIP, m_tubeStartIndexes->at(i), 2*(*pointsPerLine)[i], 1);
		}
	}

	state.disableVertexPointer();
	state.disableColorPointer();


/*
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

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	for( size_t i = 0; i < active->size(); ++i )
    {
        if ( active->at( i ) )
        {
			glBegin( GL_TRIANGLE_STRIP );
            int idx = startIndexes->at( i ) * 3;
            for ( size_t k = 0; k < pointsPerLine->at( i ); ++k )
            {

                glNormal3f( tangents->at( idx ), tangents->at( idx + 1 ), tangents->at( idx + 2 ) );
                glColor3f( colors->at( idx ), colors->at( idx + 1 ), colors->at( idx + 2 ) );
                // Set TexCoords: first 1DTexturePosition, second Positionflag
                // SouthPointPosition
				glTexCoord2f( -1.0f , 0.0f );
                glVertex3f( verts->at( idx ), verts->at( idx + 1 ), verts->at( idx + 2 ) );
                // NorthPointPosition
                glTexCoord2f( 1.0f, 1.0f );
                glVertex3f( verts->at( idx ), verts->at( idx + 1 ), verts->at( idx + 2 ) );

                idx += 3;
            }
            glEnd();
        }
	}
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL, 0.0);
	glEnable(GL_POINT_SPRITE);
	glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
	for( size_t i = 0; i < active->size(); ++i )
	{
		if ( active->at( i ) )
		{


			glBegin( GL_POINTS );
			int idx = startIndexes->at( i ) * 3;
			for ( size_t k = 0; k < pointsPerLine->at( i ); ++k )
			{

				glNormal3f( tangents->at( idx ), tangents->at( idx + 1 ), tangents->at( idx + 2 ) );
				glColor3f( colors->at( idx ), colors->at( idx + 1 ), colors->at( idx + 2 ) );

				// if k=0 || k=pointPerLine-1 then draw endtexture
				if(k == 0 || k == pointsPerLine->at(i)-1)
				{
					glTexCoord2f( 0.0f , 0.0f);
				}
				else
				{
					glTexCoord2f( 0.0f , 1.0f);
				}
				glVertex3f( verts->at( idx ), verts->at( idx + 1 ), verts->at( idx + 2 ) );
				idx += 3;
			}
			glEnd();
		}
	}
	glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_POINT_SPRITE);*/
}
