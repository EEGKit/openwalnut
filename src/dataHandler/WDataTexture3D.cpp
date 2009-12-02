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

#include "WDataSetSingle.h"

#include "WDataTexture3D.h"

WDataTexture3D::WDataTexture3D( boost::shared_ptr<WValueSetBase> valueSet, boost::shared_ptr<WGrid> grid ):
    m_alpha( 1.0 ),
    m_threshold( 0.0 ),
    m_texture( osg::ref_ptr< osg::Texture3D >() ),
    m_valueSet( valueSet ),
    m_grid( grid )
{
    // initialize members
}

WDataTexture3D::~WDataTexture3D()
{
    // cleanup
}


float WDataTexture3D::getAlpha() const
{
    return m_alpha;
}

void WDataTexture3D::setAlpha( float alpha )
{
    if ( ( alpha > 1.0 ) || ( alpha < 0.0 ) )
    {
        return;
    }

    m_alpha = alpha;
}

float WDataTexture3D::getThreshold() const
{
    return m_threshold;
}

void WDataTexture3D::setThreshold( float threshold )
{
    m_threshold = threshold;
}

osg::ref_ptr< osg::Texture3D > WDataTexture3D::getTexture()
{
    if ( !m_texture )
    {
        createTexture();
    }

    return m_texture;
}

void WDataTexture3D::createTexture3D( unsigned char* source, int components )
{
    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_grid );

    if ( components == 1 )
    {
        osg::ref_ptr< osg::Image > ima = new osg::Image;
        ima->allocateImage( grid->getNbCoordsX(), grid->getNbCoordsY(), grid->getNbCoordsZ(), GL_LUMINANCE, GL_UNSIGNED_BYTE );

        unsigned char* data = ima->data();

        for ( unsigned int i = 0; i < grid->getNbCoordsX() * grid->getNbCoordsY() * grid->getNbCoordsZ(); ++i )
        {
            data[i] = source[i];
        }
        m_texture = osg::ref_ptr<osg::Texture3D>( new osg::Texture3D );
        m_texture->setFilter( osg::Texture3D::MIN_FILTER, osg::Texture3D::LINEAR );
        m_texture->setFilter( osg::Texture3D::MAG_FILTER, osg::Texture3D::LINEAR );
        m_texture->setWrap( osg::Texture3D::WRAP_R, osg::Texture3D::REPEAT );
        m_texture->setImage( ima );
        m_texture->setResizeNonPowerOfTwoHint( false );
    }
    else if ( components == 3 )
    {
        osg::ref_ptr< osg::Image > ima = new osg::Image;
        ima->allocateImage( grid->getNbCoordsX(), grid->getNbCoordsY(), grid->getNbCoordsZ(), GL_RGB, GL_UNSIGNED_BYTE );

        unsigned char* data = ima->data();

        for ( unsigned int i = 0; i < grid->getNbCoordsX() * grid->getNbCoordsY() * grid->getNbCoordsZ() * 3; ++i )
        {
            data[i] = source[i];
        }
        m_texture = osg::ref_ptr<osg::Texture3D>( new osg::Texture3D );
        m_texture->setFilter( osg::Texture3D::MIN_FILTER, osg::Texture3D::LINEAR );
        m_texture->setFilter( osg::Texture3D::MAG_FILTER, osg::Texture3D::LINEAR );
        m_texture->setWrap( osg::Texture3D::WRAP_R, osg::Texture3D::REPEAT );
        m_texture->setImage( ima );
        m_texture->setResizeNonPowerOfTwoHint( false );
    }

    // TODO(seralph): throw exception if components!=1 or 3
}

void WDataTexture3D::createTexture3D( int16_t* source, int components )
{
    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_grid );

    // TODO(seralph): throw exception if components!=1 or 3
    if ( components == 1)
    {
        int nSize = grid->getNbCoordsX() * grid->getNbCoordsY() * grid->getNbCoordsZ();

        std::vector<int16_t> tempSource( nSize );

        for ( int i = 0; i < nSize; ++i )
        {
            tempSource[i] = static_cast<int16_t>( source[i] );
        }

        int max = 0;
        std::vector< int > histo( 65536, 0 );
        for ( int i = 0; i < nSize; ++i )
        {
            if ( max < tempSource[i])
            {
                max = tempSource[i];
            }
            ++histo[tempSource[i]];
        }
        int fivepercent = static_cast<int>( nSize * 0.001 );

        int newMax = 65535;
        int adder = 0;
        for ( int i = 65535; i > 0; --i )
        {
            adder += histo[i];
            newMax = i;
            if ( adder > fivepercent )
                break;
        }
        for ( int i = 0; i < nSize; ++i )
        {
            if ( tempSource[i] > newMax )
            {
                tempSource[i] = newMax;
            }
        }

        int mult = 65535 / newMax;
        for ( int i = 0; i < nSize; ++i )
        {
            tempSource[i] *= mult;
        }

        osg::ref_ptr< osg::Image > ima = new osg::Image;
        ima->allocateImage( grid->getNbCoordsX(), grid->getNbCoordsY(), grid->getNbCoordsZ(), GL_LUMINANCE, GL_UNSIGNED_SHORT );

        unsigned char* data = ima->data();

        unsigned char* charSource = ( unsigned char* )&tempSource[0];

        for ( unsigned int i = 0; i < grid->getNbCoordsX() * grid->getNbCoordsY() * grid->getNbCoordsZ() * 2 ; ++i )
        {
            data[i] = charSource[i];
        }
        m_texture = osg::ref_ptr<osg::Texture3D>( new osg::Texture3D );
        m_texture->setFilter( osg::Texture3D::MIN_FILTER, osg::Texture3D::LINEAR );
        m_texture->setFilter( osg::Texture3D::MAG_FILTER, osg::Texture3D::LINEAR );
        m_texture->setWrap( osg::Texture3D::WRAP_R, osg::Texture3D::REPEAT );
        m_texture->setImage( ima );
        m_texture->setResizeNonPowerOfTwoHint( false );
    }
}

void WDataTexture3D::createTexture3D( float* source, int components )
{
     boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_grid );

    // TODO(seralph): throw exception if texture generation failed
    if ( components == 1)
    {
        osg::ref_ptr< osg::Image > ima = new osg::Image;
        ima->allocateImage( grid->getNbCoordsX(), grid->getNbCoordsY(), grid->getNbCoordsZ(), GL_LUMINANCE, GL_FLOAT );

        unsigned char* data = ima->data();

        unsigned char* charSource = ( unsigned char* )source;

        for ( unsigned int i = 0; i < grid->getNbCoordsX() * grid->getNbCoordsY() * grid->getNbCoordsZ() * 4 ; ++i )
        {
            data[i] = charSource[i];
        }
        m_texture = osg::ref_ptr<osg::Texture3D>( new osg::Texture3D );
        m_texture->setFilter( osg::Texture3D::MIN_FILTER, osg::Texture3D::LINEAR );
        m_texture->setFilter( osg::Texture3D::MAG_FILTER, osg::Texture3D::LINEAR );
        m_texture->setWrap( osg::Texture3D::WRAP_R, osg::Texture3D::REPEAT );
        m_texture->setImage( ima );
        m_texture->setResizeNonPowerOfTwoHint( false );
    }
}

void WDataTexture3D::createTexture()
{
    if ( !m_texture )
    {
        if ( m_valueSet->getDataType() == 2 )
        {
            boost::shared_ptr< WValueSet< unsigned char > > vs = boost::shared_dynamic_cast< WValueSet< unsigned char > >( m_valueSet );
            unsigned char* source = const_cast< unsigned char* > ( vs->rawData() );
            createTexture3D( source, m_valueSet->dimension() );
        }

        else if ( m_valueSet->getDataType() == 4 )
        {
            boost::shared_ptr< WValueSet< int16_t > > vs = boost::shared_dynamic_cast< WValueSet< int16_t > >( m_valueSet );
            int16_t* source = const_cast< int16_t* > ( vs->rawData() );
            createTexture3D( source, m_valueSet->dimension() );
        }

        else if ( m_valueSet->getDataType() == 16 )
        {
            boost::shared_ptr< WValueSet< float > > vs = boost::shared_dynamic_cast< WValueSet< float > >( m_valueSet );
            float* source = const_cast< float* > ( vs->rawData() );
            createTexture3D( source, m_valueSet->dimension() );
        }
    }
}

