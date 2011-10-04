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

#include <algorithm>
#include <string>
#include <vector>

#include <osg/Geode>
#include <osg/Group>
#include <osg/Material>
#include <osg/ShapeDrawable>
#include <osg/StateAttribute>

#include "../../common/WColor.h"
#include "../../kernel/WKernel.h"
#include "../../kernel/WSelectionManager.h"
#include "WMVectorPlot.h"
#include "WMVectorPlot.xpm"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMVectorPlot )

WMVectorPlot::WMVectorPlot():
    WModule()
{
}

WMVectorPlot::~WMVectorPlot()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMVectorPlot::factory() const
{
    return boost::shared_ptr< WModule >( new WMVectorPlot() );
}

const char** WMVectorPlot::getXPMIcon() const
{
    return vectorplot_xpm;
}

const std::string WMVectorPlot::getName() const
{
    return "Vector Plot";
}

const std::string WMVectorPlot::getDescription() const
{
    return "This module displays vector data as small vector representations on navigation slices and other surfaces.";
}

void WMVectorPlot::connectors()
{
    m_input = boost::shared_ptr< WModuleInputData < DataSetType > >(
        new WModuleInputData< DataSetType >( shared_from_this(), "in", "The dataset to display" )
        );

    addConnector( m_input );

    // call WModules initialization
    WModule::connectors();
}

void WMVectorPlot::properties()
{
    m_xSlice           = m_properties->addProperty( "X Pos of the slice", "Description.", 80. );
    m_ySlice           = m_properties->addProperty( "Y Pos of the slice", "Description.", 100. );
    m_zSlice           = m_properties->addProperty( "Z Pos of the slice", "Description.", 80. );

    m_xSlice->setHidden( true );
    m_ySlice->setHidden( true );
    m_zSlice->setHidden( true );

    m_projectOnSlice = m_properties->addProperty( "Projection",
                                                   "If active, the vectors are projected into the surface "
                                                   "used to place them. Thus their "
                                                   "representation is tangential to the surface.",
                                                   false );
    m_coloringMode   = m_properties->addProperty( "Direction coloring",
                                                   "Draw each vector in a color indicating its direction. ", false );
    m_aColor         = m_properties->addProperty( "Color",
                                                   "This color is used if direction coloring is deactivated.",
                                                   WColor( 1.0, 0.0, 0.0, 1.0 ) );

    m_showOnSagittal        = m_properties->addProperty( "Show sagittal", "Show vectors on sagittal slice.", true );
    m_showOnCoronal        = m_properties->addProperty( "Show coronal", "Show vectors on coronal slice.", true );
    m_showOnAxial        = m_properties->addProperty( "Show axial", "Show vectors on axial slice.", true );

    m_xSlice->setMin( 0 );
    m_ySlice->setMin( 0 );
    m_zSlice->setMin( 0 );

    WModule::properties();
}

void WMVectorPlot::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );

    ready();

    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if( m_shutdownFlag() )
        {
            break;
        }

        boost::shared_ptr< WProgress > progress = boost::shared_ptr< WProgress >( new WProgress( "Vector Plot", 2 ) );
        m_progress->addSubProgress( progress );

        boost::shared_ptr< DataSetType > newDataSet = m_input->getData();
        bool dataChanged = ( m_dataSet != newDataSet );
        bool dataValid   = ( newDataSet );

        if( dataChanged && dataValid )
        {
            m_dataSet = newDataSet;

            m_xSlice->setMax( m_dataSet->getGrid()->getNbVoxelsX() - 1 );
            m_ySlice->setMax( m_dataSet->getGrid()->getNbVoxelsY() - 1 );
            m_zSlice->setMax( m_dataSet->getGrid()->getNbVoxelsZ() - 1 );
            m_xSlice->ensureValidity( m_dataSet->getGrid()->getNbVoxelsX() / 2.0 );
            m_ySlice->ensureValidity( m_dataSet->getGrid()->getNbVoxelsY() / 2.0 );
            m_zSlice->ensureValidity( m_dataSet->getGrid()->getNbVoxelsZ() / 2.0 );
        }

        if( m_xSlice->changed() || m_ySlice->changed() || m_zSlice->changed() )
        {
            osg::ref_ptr< osg::Geode > newRootNode = new osg::Geode();

            newRootNode->addDrawable( buildPlotSlices() );
            if( m_rootNode )
            {
                WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
            }

            ++*progress;

            m_rootNode = newRootNode;
            m_rootNode->setNodeMask( m_active->get() ? 0xFFFFFFFF : 0x0 );
            m_rootNode->addUpdateCallback( new WGEFunctorCallback< osg::Node >( boost::bind( &WMVectorPlot::updateCallback, this ) ) );

            m_shader = osg::ref_ptr< WGEShader > ( new WGEShader( "WMVectorPlot", m_localPath ) );
            m_shader->apply( m_rootNode );

            WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );
        }
        progress->finish();
    }

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
}

osg::ref_ptr<osg::Geometry> WMVectorPlot::buildPlotSlices()
{
    boost::shared_ptr< WProgress > progress = boost::shared_ptr< WProgress >( new WProgress( "Vector Plot", 4 ) );
    m_progress->addSubProgress( progress );

    boost::shared_ptr< WGridRegular3D2 const > grid = m_dataSet->getGrid();

    // When working with the OpenSceneGraph, always use ref_ptr to store pointers to OSG objects. This allows OpenSceneGraph to manage
    // its resources automatically.

    using osg::ref_ptr;
    ref_ptr< osg::Vec3Array > vertices = ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    ref_ptr< osg::Vec4Array > colors = ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    ref_ptr< osg::Geometry > geometry = ref_ptr< osg::Geometry >( new osg::Geometry );

    WColor color = m_aColor->get( true );

    boost::array< int, 3 > slices = { { m_zSlice->get( true ), m_ySlice->get( true ), m_xSlice->get( true ),  } };  // NOLINT braces
    boost::array< bool, 3 > show = { { m_showOnSagittal->get( true ), m_showOnCoronal->get( true ), m_showOnAxial->get( true ) } };  // NOLINT braces

    typedef WDataAccessConst< WGridRegular3D2, WDataProxyConst< WMatrixFixed< float, 3, 1 > > > AccessType;
    AccessType access = static_cast< DataSetType const& >( *m_dataSet ).getAccess< WMatrixFixed< float, 3, 1 > >();

    for( int i = 0; i < 3; ++i )
    {
        if( show[ i ] )
        {
            AccessType::SliceIterator si, se;
            for( tie( si, se ) = access.slice( i, slices[ i ] ); si != se; ++si )
            {
                WMatrixFixed< float, 3, 1 > pos;
                pos[ 0 ] = si()[ 0 ];
                pos[ 1 ] = si()[ 1 ];
                pos[ 2 ] = si()[ 2 ];
                // TODO( reichenbach ):  apply grid transform to pos

                WMatrixFixed< float, 3, 1 > vec = *si;
                vec /= 2.0;

                if( !m_projectOnSlice->get( true ) )
                {
                    vertices->push_back( osg::Vec3( pos[ 0 ] - vec[ 0 ], pos[ 1 ] - vec[ 1 ], pos[ 2 ] - vec[ 2 ] ) );
                    vertices->push_back( osg::Vec3( pos[ 0 ] + vec[ 0 ], pos[ 1 ] + vec[ 1 ], pos[ 2 ] + vec[ 2 ] ) );
                }
                else
                {
                    vertices->push_back( osg::Vec3( pos[ 0 ] - vec[ 0 ], pos[ 1 ] - vec[ 1 ], pos[ 2 ] - vec[ 2 ] ) );
                    vertices->back()[ i ] = slices[ i ];
                    vertices->push_back( osg::Vec3( pos[ 0 ] + vec[ 0 ], pos[ 1 ] + vec[ 1 ], pos[ 2 ] + vec[ 2 ] ) );
                    vertices->back()[ i ] = slices[ i ];
                }

                if( m_coloringMode->get( true ) )
                {
                    colors->push_back( osg::Vec4( fabs( vec[ 0 ] ), fabs( vec[ 1 ] ), fabs( vec[ 2 ] ), 1.0 ) );
                    colors->push_back( osg::Vec4( fabs( vec[ 0 ] ), fabs( vec[ 1 ] ), fabs( vec[ 2 ] ), 1.0 ) );
                }
                else
                {
                    colors->push_back( color );
                    colors->push_back( color );
                }
            }
        }

        ++*progress;

        for( size_t i = 0; i < vertices->size(); ++i )
        {
            osg::DrawElementsUInt* line = new osg::DrawElementsUInt( osg::PrimitiveSet::LINES, 0 );
            line->push_back( i );
            line->push_back( i + 1 );
            ++i;
            geometry->addPrimitiveSet( line );
        }
    }

    geometry->setVertexArray( vertices );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

    progress->finish();

    return geometry;
}

void WMVectorPlot::updateCallback()
{
}

void WMVectorPlot::activate()
{
    if( m_rootNode )   // always ensure the root node exists
    {
        if( m_active->get() )
        {
            m_rootNode->setNodeMask( 0xFFFFFFFF );
        }
        else
        {
            m_rootNode->setNodeMask( 0x0 );
        }
    }

    // Always call WModule's activate!
    WModule::activate();
}
