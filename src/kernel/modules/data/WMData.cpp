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

#include <string>
#include <vector>

#include "../../../common/WAssert.h"
#include "../../../common/WIOTools.h"
#include "../../../common/WPropertyHelper.h"

#include "../../../newDataHandler/WDataSet2.h"
#include "WReaderNIfTI.h"

#include "../../../graphicsEngine/WGEColormapping.h"
#include "../../../kernel/WModuleOutputData.h"
#include "WMData.h"
#include "data.xpm"

WMData::WMData():
    WModule(),
    m_fileNameSet( false ),
    m_isTexture( false ),
    m_transformNoMatrix( 4, 4 ),
    m_transformSForm( 4, 4 ),
    m_transformQForm( 4, 4 )
{
    // initialize members
}

WMData::~WMData()
{
    // cleanup
}

boost::shared_ptr< WModule > WMData::factory() const
{
    return boost::shared_ptr< WModule >( new WMData() );
}

const char** WMData::getXPMIcon() const
{
    return data_xpm;
}

const std::string WMData::getName() const
{
    return "Data Module";
}

const std::string WMData::getDescription() const
{
    return "This module encapsulates data.";
}

boost::shared_ptr< WDataSet > WMData::getDataSet()
{
    return boost::shared_ptr< WDataSet >();
}

void WMData::setFilename( boost::filesystem::path fname )
{
    if ( !m_fileNameSet )
    {
        m_fileNameSet = true;
        m_fileName = fname;
    }
}

boost::filesystem::path WMData::getFilename() const
{
    return m_fileName;
}

MODULE_TYPE WMData::getType() const
{
    return MODULE_DATA;
}

void WMData::connectors()
{
    // initialize connectors
    m_output= boost::shared_ptr< WModuleOutputData< WDataSet2Base > >(
                new WModuleOutputData< WDataSet2Base >(
                shared_from_this(), "out", "A loaded dataset." )
            );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_output );

    // call WModules initialization
    WModule::connectors();
}

void WMData::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    // properties
    m_dataName = m_infoProperties->addProperty( "Filename", "The filename of the dataset.", std::string( "" ) );
    m_dataType = m_infoProperties->addProperty( "Data type", "The type of the the single data values.", std::string( "" ) );

    m_matrixSelectionsList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_matrixSelectionsList->addItem( "No matrix", "" );
    m_matrixSelectionsList->addItem( "sform", "" );
    m_matrixSelectionsList->addItem( "qform", "" );

    m_matrixSelection = m_properties->addProperty( "Transformation matrix",  "matrix",
            m_matrixSelectionsList->getSelectorFirst(), m_propCondition );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_matrixSelection );

    // use this callback for the other properties
    WPropertyBase::PropertyChangeNotifierType propertyCallback = boost::bind( &WMData::propertyChanged, this, _1 );
}

void WMData::propertyChanged( boost::shared_ptr< WPropertyBase > property )
{
    if( m_isTexture )
    {
        if ( property == m_active )
        {
            // forward to texture
        }
    }
    else
    {
        if ( property == m_active )
        {
            if( m_active->get() )
            {
                m_output->updateData( m_dataSet );
            }
            else
            {
                m_output->updateData( boost::shared_ptr< WDataSet2Base >() );
            }
        }
    }
}

void WMData::moduleMain()
{
    WAssert( m_fileNameSet, "No filename specified." );

    m_transformNoMatrix.makeIdentity();
    m_transformSForm.makeIdentity();
    m_transformQForm.makeIdentity();

    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );

    // init readers
    std::vector< WReader2::SPtr > readers;
    readers.push_back( WReader2::SPtr( new WReaderNIfTI() ) );

    std::string fileName = m_fileName.string();

    debugLog() << "Loading data from \"" << fileName << "\".";
    m_dataName->set( fileName );

    // remove the path up to the file name and set it as a convenient name for this module instance
    if ( fileName != "" )
    {
        m_runtimeName->set( string_utils::tokenize( fileName, "/" ).back() );
    }

    // load it now

    m_dataSet.reset();
    for( std::vector< WReader2::SPtr >::iterator it = readers.begin(); it != readers.end(); ++it )
    {
        boost::filesystem::path p( fileName );
        if( !( *it )->acceptFile( p ) )
        {
            continue;
        }
        std::vector< WDataSet2Base::SPtr > suggestions = ( *it )->suggestDataSets( p );
        if( suggestions.empty() )
        {
            continue;
        }
        m_dataSet = ( *it )->load( p, *suggestions.begin() );
        break;
    }

    readers.clear();

    debugLog() << "Loading data done.";

    // I am interested in the active property ( manually subscribe signal )
    m_active->getCondition()->subscribeSignal( boost::bind( &WMData::propertyChanged, this, m_active ) );

    // textures also provide properties

    // notify
    if( !m_dataSet )
    {
        infoLog() << "No data loaded!";
    }
    
    m_output->updateData( m_dataSet );
    ready();

    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();
        if( m_shutdownFlag() )
        {
            break;
        }
    }
}

// TODO(wiebel): move this to some central place.
std::string WMData::getDataTypeString( boost::shared_ptr< WDataSet2Base > /* dss */ )
{
    std::string result = "double";
    return result;
}

