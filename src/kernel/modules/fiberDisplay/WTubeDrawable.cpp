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
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osg/Texture1D>
#include <osg/Texture2D>
#include <osg/BlendFunc>
#include <osg/AlphaFunc>
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
	m_usePointSprite = false;
	m_useQuadStrips = false;
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
		if(m_useQuadStrips)
		{
			m_rootState->getUniform("u_viewportHeight")->set( static_cast<int>( (*renderInfo.getCurrentCamera()->getViewport()).height() ) );
			m_rootState->getUniform("u_viewportWidth")->set( static_cast<int>( (*renderInfo.getCurrentCamera()->getViewport()).width() ) );
		}
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
		m_tubeStartIndexes->push_back( offset);
		offset += 2*pointsPerLine->at(i);
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
	}

	// add vertexdata for pointsprites
	for( size_t i = 0; i < startIndices->size(); ++i )
	{
		int idx = startIndices->at( i ) * 3;
		m_tubeStartIndexes->push_back( offset);
		offset += pointsPerLine->at(i);
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

void WTubeDrawable::setWShader(osg::ref_ptr< WShader > shaderTubes)
{
	m_shaderTubes = shaderTubes;
}

void WTubeDrawable::setWShaders(osg::ref_ptr< WShader >shaderTubesPS, osg::ref_ptr< WShader >shaderTubesQS)
{
	m_shaderTubesPS = shaderTubesPS;
	m_shaderTubesQS = shaderTubesQS;
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



void WTubeDrawable::createTextures(osg::StateSet* m_rootState) const
{
	create1DTextureRectLightning(m_rootState);
	create2DTextureCycleLightning(m_rootState);

	create2DTexDiffuseLightning(m_rootState);
	create2DTexSpecularLightning(m_rootState);

	const osg::BoundingBox & bb = this->getBound();
	osg::Uniform* nearPos = new osg::Uniform("nearPos",osg::Vec3(bb.xMin(),bb.yMin(),bb.zMin()));
	osg::Uniform* farPos = new osg::Uniform("farPos",osg::Vec3(bb.xMax(),bb.yMax(),bb.zMax()));
	m_rootState->addUniform(nearPos);
	m_rootState->addUniform(farPos);
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

	m_rootState->setTextureAttribute(3,texture,osg::StateAttribute::OVERRIDE);
	m_rootState->setTextureMode(3,GL_TEXTURE_2D,osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
	m_rootState->addUniform(osg::ref_ptr<osg::Uniform>( new osg::Uniform( "textureDiff2D", 3 ) ) );

}

void WTubeDrawable::create2DTexSpecularLightning(osg::StateSet* m_rootState) const
{
	osg::Image* image = new osg::Image;

	int noPixels = 64;
	int noExponents = 64;

	//vector<float> sn;

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

	m_rootState->setTextureAttribute(4,texture,osg::StateAttribute::OVERRIDE);
	m_rootState->setTextureMode(4,GL_TEXTURE_2D,osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
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

    const size_t resX = 64;
    osg::ref_ptr< osg::Image > randImage = new osg::Image();
    randImage->allocateImage( resX, resX, 1, GL_RGBA, GL_UNSIGNED_BYTE );
    unsigned char *randomLuminance = randImage->data();  // should be 4 megs
    for( unsigned int x = 0; x < resX; x++ )
    {

        diffuse = sin((float)x*step);
		    specular = pow(diffuse, 16);
        for( unsigned int y = 0; y < resX; y++ )
        {
          check = pow( 2.0 * ( static_cast< float >( x ) / static_cast< float >( resX ) ) - 1.0, 2.0) +
                  pow( 2.0 * ( static_cast< float >( y ) / static_cast< float >( resX ) ) - 1.0, 2.0);
          if(check > 1.0)
          {
            alpha = 0.0f;
          }
          else
          {
            alpha = 1.0f;
          }

            randomLuminance[ ( 4 * y * resX ) + ( 4 * x ) + 0 ] = static_cast< unsigned char >( diffuse * 255.0 );
            randomLuminance[ ( 4 * y * resX ) + ( 4 * x ) + 1 ] = static_cast< unsigned char >( specular * 255.0 );
            randomLuminance[ ( 4 * y * resX ) + ( 4 * x ) + 2 ] = static_cast< unsigned char >( alpha * 255.0 );
            randomLuminance[ ( 4 * y * resX ) + ( 4 * x ) + 3 ] = 255;
        }
    }
	osg::Texture2D* texture = new osg::Texture2D;
	texture->setDataVariance(osg::Object::STATIC);
	texture->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::CLAMP_TO_BORDER);
	texture->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::CLAMP_TO_BORDER);
	texture->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR_MIPMAP_LINEAR);
	texture->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);
    texture->setImage(randImage);

	m_rootState->setTextureAttribute(2,texture,osg::StateAttribute::OVERRIDE);
	m_rootState->setTextureMode(2,GL_TEXTURE_2D,osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
	m_rootState->addUniform(osg::ref_ptr<osg::Uniform>( new osg::Uniform( "texturePS", 2 ) ) );

}
void WTubeDrawable::setRootState(osg::StateSet* rootState)
{
	m_rootState = rootState;
}

void WTubeDrawable::setOSGNode(osg::ref_ptr< osg::Group > osgNode)
{
	this->m_osgNode = osgNode;
}

void WTubeDrawable::setActiveRenderingMode(bool usePointSprite, bool useQuadStrips)
{
	m_usePointSprite = usePointSprite;
	m_useQuadStrips = useQuadStrips;
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

	state.setVertexPointer( m_tubeVerts);
	state.setColorPointer( m_tubeColors );
	state.setNormalPointer(m_tubeTangents );
	state.setTexCoordPointer(0, m_tubeTexCoords );

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	if(m_useQuadStrips)
	{
		for ( size_t i = 0; i < active->size(); ++i )
		{
			if ( (*active)[i] )
			{
				state.glDrawArraysInstanced( GL_QUAD_STRIP, m_tubeStartIndexes->at(i), 2 * (*pointsPerLine)[i], 1);
			}
		}
	}

	//osg::TexEnv* texEnvPS = new osg::TexEnv( osg::TexEnv::REPLACE);
	//osg::AlphaFunc* alphaFunc = new osg::AlphaFunc(osg::AlphaFunc::NOTEQUAL,0.0f);

	//state.setModeValidity(GL_VERTEX_PROGRAM_POINT_SIZE, osg::StateAttribute::ON);
	//state.setModeValidity(GL_ALPHA_TEST, osg::StateAttribute::ON);
	//state.setModeValidity(GL_POINT_SPRITE, osg::StateAttribute::ON);

	//m_shaderTubes->setDefine("USE_POINTSPRITE");

	//m_shaderTubesPS->apply(state);
	//m_shaderTubesPS->applyDirect(state);

	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_POINT_SPRITE);

	glAlphaFunc(GL_NOTEQUAL, 0.0);
	glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);

	//state.setModeValidity(alphaFunc, osg::StateAttribute::ON);
	//state.setModeValidity(texEnvPS, osg::StateAttribute::ON);
	if(m_usePointSprite)
	{
		for ( size_t i = 0; i < active->size(); ++i )
		{
			if ( (*active)[i] )
			{
				state.glDrawArraysInstanced( GL_POINTS, m_tubeStartIndexes->at(i + active->size()), (*pointsPerLine)[i], 1);
			}
		}
	}
		//	m_shaderTubesPS->deactivate(this);
	//state.setModeValidity(GL_VERTEX_PROGRAM_POINT_SIZE, osg::StateAttribute::OFF);
	//state.setModeValidity(GL_ALPHA_TEST, osg::StateAttribute::OFF);
	//state.setModeValidity(GL_POINT_SPRITE, osg::StateAttribute::OFF);

	glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_POINT_SPRITE);

	state.disableVertexPointer();
	state.disableColorPointer();
	//m_shaderTubes->eraseDefine("USE_POINTSPRITE");

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
	glEnable(GL_BLEND);

	for( size_t i = 0; i < active->(); ++i )
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
	glAlphaFunc(GL_NOTEQUAL, 0.0f);
	glEnable(GL_ALPHA_TEST);
	glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
	glEnable(GL_POINT_SPRITE);

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
