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

#include <iostream>
#include <string>
#include <vector>

#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Texture3D>

#include "boost/smart_ptr.hpp"

#include "WNavigationSliceModule.h"
#include "../../kernel/WKernel.h"

#include "../../dataHandler/WDataSetSingle.h"
#include "../../dataHandler/WSubject.h"
#include "../../dataHandler/WValueSet.hpp"

#include "../../graphicsEngine/WShader.h"

WNavigationSliceModule::WNavigationSliceModule():
    WModule()
{
    // initialize members
    std::string shaderPath = WKernel::getRunningKernel()->getGraphicsEngine()->getShaderPath();
    m_shader = boost::shared_ptr< WShader > ( new WShader( "slice", shaderPath ) );
    m_textureAssigned = false;

    m_axialSlice = 80;
    m_coronalSlice = 100;
    m_sagittalSlice = 80;

    m_maxAxial = 160;
    m_maxCoronal = 200;
    m_maxSagittal = 160;

    m_showAxial = true;
    m_showCoronal = true;
    m_showSagittal = true;
}

WNavigationSliceModule::~WNavigationSliceModule()
{
    // cleanup
}

WNavigationSliceModule::WNavigationSliceModule( const WNavigationSliceModule& other )
    : WModule()
{
    *this = other;
}

const std::string WNavigationSliceModule::getName() const
{
    return "Navigation Slice Module";
}

const std::string WNavigationSliceModule::getDescription() const
{
    return "This module shows 3 orthogonal navigation slices.";
}

void WNavigationSliceModule::threadMain()
{
    createSlices();

    // Since the modules run in a separate thread: such loops are possible
    while ( !m_FinishRequested )
    {
        if ( WKernel::getRunningKernel()->getDataHandler()->getNumberOfSubjects() > 0 )
        {
            if ( WKernel::getRunningKernel()->getDataHandler()->getSubject(0)->getNumberOfDataSets() > 0 )
            {
                if ( !m_textureAssigned )
                {
                    boost::shared_ptr< WDataSetSingle > ds = boost::shared_dynamic_cast< WDataSetSingle >(
                            WKernel::getRunningKernel()->getDataHandler()->getSubject( 0 )->getDataSet( 0 ) );
                    boost::shared_ptr< WValueSet< int8_t > > vs = boost::shared_dynamic_cast< WValueSet<
                            int8_t > >( ds->getValueSet() );
                    int8_t* source = const_cast< int8_t* > ( vs->rawData() );

                    osg::ref_ptr< osg::Image > ima = new osg::Image;
                    ima->allocateImage( 160, 200, 160, GL_LUMINANCE, GL_UNSIGNED_BYTE );

                    unsigned char* data = ima->data();

                    for ( unsigned int i = 0; i < 160* 200* 160 ; ++i )
                    {
                        data[i] = source[i];
                    }
                    osg::Texture3D* texture3D = new osg::Texture3D;
                    texture3D->setFilter( osg::Texture3D::MIN_FILTER, osg::Texture3D::LINEAR );
                    texture3D->setFilter( osg::Texture3D::MAG_FILTER, osg::Texture3D::LINEAR );
                    texture3D->setWrap( osg::Texture3D::WRAP_R, osg::Texture3D::REPEAT );
                    texture3D->setImage( ima );
                    texture3D->setResizeNonPowerOfTwoHint( false );

                    osg::StateSet* sliceState = m_sliceNode->getOrCreateStateSet();

                    sliceState->setTextureAttributeAndModes( 0, texture3D, osg::StateAttribute::ON );

                    m_textureAssigned = true;

                    WKernel::getRunningKernel()->getGui()->addDatasetToBrowser( ds->getFileName(), 0 );
                }
            }
        }
        // do fancy stuff
        sleep( 1 );
    }

    // clean up stuff
}

void WNavigationSliceModule::createSlices()
{
    float texAxial = ( float )m_axialSlice / ( float )m_maxAxial;
    float texCoronal = ( float )m_coronalSlice / ( float )m_maxCoronal;
    float texSagittal = ( float )m_sagittalSlice / ( float )m_maxSagittal;

    m_sliceNode = new osg::Geode();

    osg::Geometry* sliceGeometry = new osg::Geometry();

    m_sliceNode->addDrawable( sliceGeometry );

    osg::Vec3Array* sliceVertices = new osg::Vec3Array;
    sliceVertices->push_back( osg::Vec3( 0, m_coronalSlice, 0 ) );
    sliceVertices->push_back( osg::Vec3( 0, m_coronalSlice, m_maxSagittal ) );
    sliceVertices->push_back( osg::Vec3( m_maxAxial, m_coronalSlice, m_maxSagittal ) );
    sliceVertices->push_back( osg::Vec3( m_maxAxial, m_coronalSlice, 0 ) );

    sliceVertices->push_back( osg::Vec3( m_axialSlice, 0, 0 ) );
    sliceVertices->push_back( osg::Vec3( m_axialSlice, 0, m_maxSagittal ) );
    sliceVertices->push_back( osg::Vec3( m_axialSlice, m_maxCoronal, m_maxSagittal ) );
    sliceVertices->push_back( osg::Vec3( m_axialSlice, m_maxCoronal, 0 ) );

    sliceVertices->push_back( osg::Vec3( 0, 0, m_sagittalSlice ) );
    sliceVertices->push_back( osg::Vec3( 0, m_maxCoronal, m_sagittalSlice ) );
    sliceVertices->push_back( osg::Vec3( m_maxAxial, m_maxCoronal, m_sagittalSlice ) );
    sliceVertices->push_back( osg::Vec3( m_maxAxial, 0, m_sagittalSlice ) );

    sliceGeometry->setVertexArray( sliceVertices );

    osg::Vec3Array* texCoords = new osg::Vec3Array;
    texCoords->push_back( osg::Vec3( 0.0, texCoronal, 0.0 ) );
    texCoords->push_back( osg::Vec3( 0.0, texCoronal, 1.0 ) );
    texCoords->push_back( osg::Vec3( 1.0, texCoronal, 1.0 ) );
    texCoords->push_back( osg::Vec3( 1.0, texCoronal, 0.0 ) );

    texCoords->push_back( osg::Vec3( texAxial, 0.0, 0.0 ) );
    texCoords->push_back( osg::Vec3( texAxial, 0.0, 1.0 ) );
    texCoords->push_back( osg::Vec3( texAxial, 1.0, 1.0 ) );
    texCoords->push_back( osg::Vec3( texAxial, 1.0, 0.0 ) );

    texCoords->push_back( osg::Vec3( 0.0, 0.0, texSagittal ) );
    texCoords->push_back( osg::Vec3( 0.0, 1.0, texSagittal ) );
    texCoords->push_back( osg::Vec3( 1.0, 1.0, texSagittal ) );
    texCoords->push_back( osg::Vec3( 1.0, 0.0, texSagittal ) );

    sliceGeometry->setTexCoordArray( 0, texCoords );

    osg::DrawElementsUInt* slice0 = new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 0 );
    slice0->push_back( 3 );
    slice0->push_back( 2 );
    slice0->push_back( 1 );
    slice0->push_back( 0 );

    osg::DrawElementsUInt* slice1 = new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 0 );
    slice1->push_back( 7 );
    slice1->push_back( 6 );
    slice1->push_back( 5 );
    slice1->push_back( 4 );

    osg::DrawElementsUInt* slice2 = new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 0 );
    slice2->push_back( 11 );
    slice2->push_back( 10 );
    slice2->push_back( 9 );
    slice2->push_back( 8 );

    sliceGeometry->addPrimitiveSet( slice0 );
    sliceGeometry->addPrimitiveSet( slice1 );
    sliceGeometry->addPrimitiveSet( slice2 );

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->addChild( m_sliceNode );

    osg::StateSet* sliceState = m_sliceNode->getOrCreateStateSet();

    sliceState->setAttributeAndModes( m_shader->getProgramObject(), osg::StateAttribute::ON );
}

void WNavigationSliceModule::updateSlices()
{
    float texAxial = ( float )m_axialSlice / ( float )m_maxAxial;
    float texCoronal = ( float )m_coronalSlice / ( float ) m_maxCoronal;
    float texSagittal = ( float )m_sagittalSlice / ( float )m_maxSagittal;

    osg::Geometry* sliceGeometry = new osg::Geometry();

    osg::Vec3Array* sliceVertices = new osg::Vec3Array;
    sliceVertices->push_back( osg::Vec3( 0, m_coronalSlice, 0 ) );
    sliceVertices->push_back( osg::Vec3( 0, m_coronalSlice, m_maxSagittal ) );
    sliceVertices->push_back( osg::Vec3( m_maxAxial, m_coronalSlice, m_maxSagittal ) );
    sliceVertices->push_back( osg::Vec3( m_maxAxial, m_coronalSlice, 0 ) );

    sliceVertices->push_back( osg::Vec3( m_axialSlice, 0, 0 ) );
    sliceVertices->push_back( osg::Vec3( m_axialSlice, 0, m_maxSagittal ) );
    sliceVertices->push_back( osg::Vec3( m_axialSlice, m_maxCoronal, m_maxSagittal ) );
    sliceVertices->push_back( osg::Vec3( m_axialSlice, m_maxCoronal, 0 ) );

    sliceVertices->push_back( osg::Vec3( 0, 0, m_sagittalSlice ) );
    sliceVertices->push_back( osg::Vec3( 0, m_maxCoronal, m_sagittalSlice ) );
    sliceVertices->push_back( osg::Vec3( m_maxAxial, m_maxCoronal, m_sagittalSlice ) );
    sliceVertices->push_back( osg::Vec3( m_maxAxial, 0, m_sagittalSlice ) );

    sliceGeometry->setVertexArray( sliceVertices );

    osg::Vec3Array* texCoords = new osg::Vec3Array;
    texCoords->push_back( osg::Vec3( 0.0, texCoronal, 0.0 ) );
    texCoords->push_back( osg::Vec3( 0.0, texCoronal, 1.0 ) );
    texCoords->push_back( osg::Vec3( 1.0, texCoronal, 1.0 ) );
    texCoords->push_back( osg::Vec3( 1.0, texCoronal, 0.0 ) );

    texCoords->push_back( osg::Vec3( texAxial, 0.0, 0.0 ) );
    texCoords->push_back( osg::Vec3( texAxial, 0.0, 1.0 ) );
    texCoords->push_back( osg::Vec3( texAxial, 1.0, 1.0 ) );
    texCoords->push_back( osg::Vec3( texAxial, 1.0, 0.0 ) );

    texCoords->push_back( osg::Vec3( 0.0, 0.0, texSagittal ) );
    texCoords->push_back( osg::Vec3( 0.0, 1.0, texSagittal ) );
    texCoords->push_back( osg::Vec3( 1.0, 1.0, texSagittal ) );
    texCoords->push_back( osg::Vec3( 1.0, 0.0, texSagittal ) );

    sliceGeometry->setTexCoordArray( 0, texCoords );

    osg::DrawElementsUInt* slice0 = new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 0 );
    slice0->push_back( 3 );
    slice0->push_back( 2 );
    slice0->push_back( 1 );
    slice0->push_back( 0 );

    osg::DrawElementsUInt* slice1 = new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 0 );
    slice1->push_back( 7 );
    slice1->push_back( 6 );
    slice1->push_back( 5 );
    slice1->push_back( 4 );

    osg::DrawElementsUInt* slice2 = new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 0 );
    slice2->push_back( 11 );
    slice2->push_back( 10 );
    slice2->push_back( 9 );
    slice2->push_back( 8 );

    if ( m_showAxial )
    {
        sliceGeometry->addPrimitiveSet( slice2 );
    }

    if ( m_showCoronal )
    {
        sliceGeometry->addPrimitiveSet( slice0 );
    }

    if ( m_showSagittal )
    {
        sliceGeometry->addPrimitiveSet( slice1 );
    }

    osg::Drawable* old = m_sliceNode->getDrawable( 0 );

    m_sliceNode->replaceDrawable( old, sliceGeometry );

    osg::StateSet* sliceState = m_sliceNode->getOrCreateStateSet();
    sliceState->setAttributeAndModes( m_shader->getProgramObject(), osg::StateAttribute::ON );
}

void WNavigationSliceModule::connectToGui()
{
    WKernel::getRunningKernel()->getGui()->getAxialSliderSignal()->connect(
            boost::bind( &WNavigationSliceModule::sliderAxialMoved, this, _1 ) );
    WKernel::getRunningKernel()->getGui()->getCoronalSliderSignal()->connect(
            boost::bind( &WNavigationSliceModule::sliderCoronalMoved, this, _1 ) );
    WKernel::getRunningKernel()->getGui()->getSagittalSliderSignal()->connect(
            boost::bind( &WNavigationSliceModule::sliderSagittalMoved, this, _1 ) );

    WKernel::getRunningKernel()->getGui()->getAxialButtonSignal()->connect(
            boost::bind( &WNavigationSliceModule::buttonAxialChanged, this, _1 ) );
    WKernel::getRunningKernel()->getGui()->getCoronalButtonSignal()->connect(
            boost::bind( &WNavigationSliceModule::buttonCoronalChanged, this, _1 ) );
    WKernel::getRunningKernel()->getGui()->getSagittalButtonSignal()->connect(
            boost::bind( &WNavigationSliceModule::buttonSagittalChanged, this, _1 ) );
}

void WNavigationSliceModule::sliderAxialMoved( int value )
{
    m_axialSlice = value;
    updateSlices();
}

void WNavigationSliceModule::sliderCoronalMoved( int value )
{
    m_coronalSlice = value;
    updateSlices();
}

void WNavigationSliceModule::sliderSagittalMoved( int value )
{
    m_sagittalSlice = value;
    updateSlices();
}

void WNavigationSliceModule::buttonAxialChanged( bool check )
{
    m_showAxial = check;
    updateSlices();
}

void WNavigationSliceModule::buttonCoronalChanged( bool check )
{
    m_showCoronal = check;
    updateSlices();
}

void WNavigationSliceModule::buttonSagittalChanged( bool check )
{
    m_showSagittal = check;
    updateSlices();
}

