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

#include <string>

// Boost 1.46 uses boost::filesystem3 as new default API, inorder to support old boost versions we still use the old API
#define BOOST_FILESYSTEM_VERSION 2
#include <boost/filesystem.hpp>

#include <osg/Image>
#include <osg/Texture3D>
#include <osgDB/ReadFile>

#include "../../kernel/WKernel.h"
#include "WMAtlasCreator.xpm"
#include "WMAtlasCreator.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMAtlasCreator )

WMAtlasCreator::WMAtlasCreator():
    WModule()
{
}

WMAtlasCreator::~WMAtlasCreator()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMAtlasCreator::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMAtlasCreator() );
}

const char** WMAtlasCreator::getXPMIcon() const
{
    return WMAtlasCreator_xpm; // Please put a real icon here.
}
const std::string WMAtlasCreator::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "AtlasCreator";
}

const std::string WMAtlasCreator::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    // See "src/modules/template/" for an extensively documented example.
    return "Someone should add some documentation here. ";
}

void WMAtlasCreator::connectors()
{
    m_output = boost::shared_ptr< WModuleOutputData < WDataSetScalar  > >(
        new WModuleOutputData< WDataSetScalar >( shared_from_this(), "out", "The created image." ) );
    addConnector( m_output );

    WModule::connectors();
}

void WMAtlasCreator::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );
    // Put the code for your properties here. See "src/modules/template/" for an extensively documented example.

    //m_propMetaFile = m_properties->addProperty( "Tree file", "", WPathHelper::getAppPath() );
    m_propDirectory = m_properties->addProperty( "Directoy", "", boost::filesystem::path( "/SCR/ratte/png8/Amyg034.png" ) );

    m_propReadTrigger = m_properties->addProperty( "Do read",  "Press!", WPVBaseTypes::PV_TRIGGER_READY, m_propCondition );

    WModule::properties();
}

void WMAtlasCreator::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );
    m_moduleState.add( m_active->getUpdateCondition() );
    ready();

    debugLog() << m_volume.size();

    while ( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if ( m_shutdownFlag() )
        {
            break;
        }

        if ( m_propReadTrigger->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            boost::filesystem::path fileName = m_propDirectory->get();
            m_propReadTrigger->set( WPVBaseTypes::PV_TRIGGER_READY, true );

            loadPngs( fileName );

            updateOutDataset();
        }
    }
}

bool WMAtlasCreator::loadPngs( boost::filesystem::path sliceFile )
{
    using namespace boost::filesystem; //NOLINT

    if ( !exists( sliceFile ) )
    {
        return false;
    }

    osg::Image* osgImage;
    osgImage = osgDB::readImageFile( sliceFile.file_string().c_str() );

    debugLog() << osgImage->r() << " : " << osgImage->s() << " : " << osgImage->t();

    m_xDim = osgImage->s();
    m_yDim = 60;
    m_zDim = osgImage->t();


    path dirPath( sliceFile );
    dirPath.remove_filename().file_string();

    m_volume.resize( m_xDim * m_yDim * m_zDim, 0 );

    directory_iterator end_itr; // default construction yields past-the-end
    for ( directory_iterator itr( dirPath ); itr != end_itr; ++itr )
    {
        //debugLog() << itr->path().file_string().c_str();
        path p( itr->path() );

        if ( p.extension() == ".png" )
        {
            addPngToVolume( p );
        }
    }

    for ( size_t i = 0; i < m_regions.size(); ++i )
    {
        debugLog() << i << ": " << m_regions[i];
    }

    return true;
}

void WMAtlasCreator::addPngToVolume( boost::filesystem::path image )
{
    using namespace boost::filesystem; //NOLINT

    std::string fn = image.filename();
    fn.erase( fn.size() - 4, 4 );
    std::string number = fn.substr( fn.size() - 3 );
    fn.erase( fn.size() - 3, 3 );

    size_t pos = boost::lexical_cast<size_t>( number );

    if ( pos > 119 )
    {
        return;
    }


    uint8_t id = 0;
    bool found = false;
    for ( size_t i = 0; i < m_regions.size(); ++i )
    {
        if ( m_regions[i] == fn )
        {
            id = static_cast<uint8_t>( i );
            found = true;
        }
    }
    if ( !found )
    {
        m_regions.push_back( fn );
        id = static_cast<uint8_t>( m_regions.size() - 1 );
    }

    osg::Image* osgImage;
    osgImage = osgDB::readImageFile( image.file_string().c_str() );
    unsigned char* data = osgImage->data();

    pos /= 2;
    ++id;

    for ( int i = 0; i < m_zDim; ++i )
    {
        for ( int k = 0; k < m_xDim; ++k )
        {
            if ( data[k * 3 + ( i * m_xDim * 3 )] != 0 )
            {
                m_volume[ pos * m_xDim + ( i * m_xDim * m_yDim ) + k ] = id;
            }
        }
    }
}

void WMAtlasCreator::updateOutDataset()
{
    boost::shared_ptr< WValueSet< unsigned char > > vs =
        boost::shared_ptr< WValueSet< unsigned char > >( new WValueSet< unsigned char >(
        0, 1, boost::shared_ptr< std::vector< uint8_t > >( new std::vector< uint8_t >( m_volume ) ), W_DT_UINT8 ) );

    WMatrix< double > mat( 4, 4 );
    mat.makeIdentity();
    mat( 0, 0 ) = 0.1;
    mat( 2, 2 ) = 0.1;

    boost::shared_ptr<WGridRegular3D> grid = boost::shared_ptr<WGridRegular3D>( new WGridRegular3D( m_xDim, m_yDim, m_zDim,
                WGridTransformOrtho( mat ) ) );

    boost::shared_ptr< WDataSetScalar >outData = boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( vs, grid ) );
    m_output->updateData( outData );
}
