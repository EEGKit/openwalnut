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

#include <boost/filesystem/path.hpp>
#include <boost/shared_ptr.hpp>

#include "../../common/WAssert.h"
#include "../../common/WCondition.h"
#include "../../common/WProgress.h"
#include "../../common/WPropertyTypes.h"
#include "../../common/WPropertyHelper.h"
#include "../../dataHandler/WDataSetFiberVector.h"
#include "../../dataHandler/WDataSetFibers.h"
#include "../../dataHandler/io/WWriterFiberVTK.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../kernel/WModuleOutputData.h"
#include "WMFiberTransform.h"
#include "fiberTransform.xpm"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMFiberTransform )

WMFiberTransform::WMFiberTransform()
    : WModule(),
      m_recompute( new WCondition() )
{
}

boost::shared_ptr< WModule > WMFiberTransform::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMFiberTransform() );
}

const char** WMFiberTransform::getXPMIcon() const
{
    return fiberTransform_xpm;
}

void WMFiberTransform::connectors()
{
    // Put the code for your connectors here. See "src/modules/template/" for an extensively documented example.
    typedef WModuleInputData< WDataSetFibers > FiberInputData;  // just an alias
    m_fiberInput = boost::shared_ptr< FiberInputData >( new FiberInputData( shared_from_this(), "fiberInput", "A loaded fiber dataset." ) );

    typedef WModuleOutputData< WDataSetFibers > FiberOutputData;  // just an alias
    m_output = boost::shared_ptr< FiberOutputData >( new FiberOutputData( shared_from_this(), "fiberOutput", "The transformed fibers." ) );

    addConnector( m_fiberInput );
    addConnector( m_output );

    // call WModules initialization
    WModule::connectors();
}

void WMFiberTransform::properties()
{
    // Put the code for your properties here. See "src/modules/template/" for an extensively documented example.
    m_save     = m_properties->addProperty( "Save result", "If true the transformed fibers are saved to a file", false );
    m_savePath = m_properties->addProperty( "Save path", "Where to save the result", boost::filesystem::path( "/no/such/file" ) );
    m_run      = m_properties->addProperty( "Run", "Do the transformation", WPVBaseTypes::PV_TRIGGER_READY, m_recompute );
    m_run->get( true ); // reset so no initial run occurs
    WPropertyHelper::PC_NOTEMPTY::addTo( m_savePath );

    m_translationProp = m_properties->addProperty( "Translation",
                                                   "Translation part of the transformation. You need to press enter to make the values effective.",
                                                   wmath::WPosition( 0.0, 0.0, 0.0 ) );
    m_matrix0Prop = m_properties->addProperty( "M Row 0",
                                               "Row 0 of matrix part of the transformation. You need to press enter to make the values effective.",
                                               wmath::WPosition( 1.0, 0.0, 0.0 ) );
    m_matrix1Prop = m_properties->addProperty( "M Row 1",
                                               "Row 1 of matrix part of the transformation. You need to press enter to make the values effective.",
                                               wmath::WPosition( 0.0, 1.0, 0.0 ) );
    m_matrix2Prop = m_properties->addProperty( "M Row 2",
                                               "Row 2 of matrix part of the transformation. You need to press enter to make the values effective.",
                                               wmath::WPosition( 0.0, 0.0, 1.0 ) );
}

void WMFiberTransform::moduleMain()
{
    // When conditions are firing while wait() is not reached: wait terminates
    // and behaves as if the appropriate conditions have had fired. But it is
    // not detectable how many times a condition has fired.
    m_moduleState.setResetable();
    m_moduleState.add( m_fiberInput->getDataChangedCondition() );
    m_moduleState.add( m_recompute );



    ready();

    while ( !m_shutdownFlag() ) // loop until the module container requests the module to quit
    {
        if ( !m_fiberInput->getData().get() ) // ok, the output has not yet sent data
        {
            m_moduleState.wait();
            continue;
        }
        if( m_rawDataset != m_fiberInput->getData() ) // in case data has changed
        {
            m_rawDataset = m_fiberInput->getData();
            WAssert( m_rawDataset, "Couldn't load dataset" );
        }

        WAssert( m_savePath, "No save path property" );
        if( m_savePath->get().string() == "/no/such/file" )
        {
            m_savePath->set( saveFileName( m_rawDataset->getFileName() ) );
        }

        if( m_run->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            update();
            m_run->set( WPVBaseTypes::PV_TRIGGER_READY, false );
        }

        m_moduleState.wait();
    }
}

void WMFiberTransform::update()
{
    // get parameter from GUI (properties)
    bool   save = m_save->get();
    boost::filesystem::path savePath = m_savePath->get();

    // set the transformation matrix
    wmath::WMatrix< double > transformationMatrix( 3, 3 ); //!< matrix which is multiplied with each point to linear transform it.
    transformationMatrix( 0, 0 ) = m_matrix0Prop->get()[0];
    transformationMatrix( 0, 1 ) = m_matrix0Prop->get()[1];
    transformationMatrix( 0, 2 ) = m_matrix0Prop->get()[2];

    transformationMatrix( 1, 0 ) = m_matrix1Prop->get()[0];
    transformationMatrix( 1, 1 ) = m_matrix1Prop->get()[1];
    transformationMatrix( 1, 2 ) = m_matrix1Prop->get()[2];

    transformationMatrix( 2, 0 ) = m_matrix2Prop->get()[0];
    transformationMatrix( 2, 1 ) = m_matrix2Prop->get()[1];
    transformationMatrix( 2, 2 ) = m_matrix2Prop->get()[2];

    boost::shared_ptr< WProgress > progress( new WProgress( "Transforming", 4 + save ) );
    m_progress->addSubProgress( progress );

    infoLog() << "Start: WDataSetFibers => WDataSetFiberVector";
    boost::shared_ptr< WDataSetFiberVector > dataset( new WDataSetFiberVector( m_rawDataset ) );
    infoLog() << "Stop:  WDataSetFibers => WDataSetFiberVector";
    ++*progress;

    //transform
    for( std::size_t fiberID = 0; fiberID < dataset->size(); ++fiberID )
    {
        wmath::WFiber& fiber = (*dataset)[fiberID];
        for( std::size_t positionID = 0; positionID < fiber.size(); ++positionID )
        {
            fiber[positionID] = transformationMatrix * fiber[positionID] + m_translationProp->get();
        }
    }
    ++*progress;

    infoLog() << "Start: WDataSetFibers <= WDataSetFiberVector";
    m_output->updateData( dataset->toWDataSetFibers() );
    infoLog() << "Stop:  WDataSetFibers <= WDataSetFiberVector";
    ++*progress;

    if( save )
    {
        WWriterFiberVTK w( savePath, true );
        w.writeFibs( dataset );
        ++*progress;
    }

    progress->finish();
}

boost::filesystem::path WMFiberTransform::saveFileName( std::string dataFileName ) const
{
    boost::filesystem::path fibFileName( dataFileName );
    return fibFileName.replace_extension( ".transformed.fib" );
}
