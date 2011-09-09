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

#include <stdint.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>

#include <cmath>

#include "../../common/WAssert.h"
#include "../../common/WPathHelper.h"
#include "../../common/WStringUtils.h"
#include "../../kernel/WKernel.h"
#include "../../common/math/linearAlgebra/WLinearAlgebra.h"
#include "../../ext/nifti/nifti1_io.h"
#include "../../kernel/WModule.h"

#include "WMWriteNIfTI.h"

W_LOADABLE_MODULE( WMWriteNIfTI )

WMWriteNIfTI::WMWriteNIfTI() :
    WModule()
{
}

WMWriteNIfTI::~WMWriteNIfTI()
{
    removeConnectors();
}

boost::shared_ptr< WModule > WMWriteNIfTI::factory() const
{
    return boost::shared_ptr< WModule >( new WMWriteNIfTI() );
}

const char** WMWriteNIfTI::getXPMIcon() const
{
    static const char * disc_xpm[] =
    {
                    "16 16 7 1", "   c None", ".  c #000080", "+  c #000000", "@  c #FFFF00", "#  c #E0E0E0",
                    "$  c #FFFFFF", "%  c #C0C0C0", "..+++++++++++..+", "..@@@@@@@@@@@..+", "..###########..+",
                    "..$$$$$$$$$$$..+", "..###########..+", "..$$$$$$$$$$$..+", "..###########..+", "..$$$$$$$$$$$..+",
                    "...............+", "....%%%%%%%....+", "....%..%%%%....+", "....%..%%%%....+", "....%..%%%%....+",
                    "....%..%%%%....+", "+...%%%%%%%....+", "++++++++++++++++"
    };
    return disc_xpm;
}

const std::string WMWriteNIfTI::getName() const
{
    return "Write NIfTI";
}

const std::string WMWriteNIfTI::getDescription() const
{
    return "Writes a data set to a NIfTI file.";
}

void WMWriteNIfTI::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );

    ready();

    while( !m_shutdownFlag() )
    {
        m_dataSet = m_input->getData();
        if( !m_dataSet )
        {
            debugLog() << "Waiting for data ...";
            m_moduleState.wait();
            continue;
        }
        m_moduleState.wait();
    }
}

void WMWriteNIfTI::connectors()
{
    m_input = boost::shared_ptr< WModuleInputData< WDataSet2Base > >( new WModuleInputData< WDataSet2Base > (
                    shared_from_this(), "in", "The dataset to write." ) );

    addConnector( m_input );

    WModule::connectors();
}

void WMWriteNIfTI::properties()
{
    m_filename = m_properties->addProperty( "Filename", "Filename where to write the NIfTI file to.",
                                             WPathHelper::getAppPath() );
    m_saveTriggerProp = m_properties->addProperty( "Do save",  "Press!",
                                                  WPVBaseTypes::PV_TRIGGER_READY );
    m_saveTriggerProp->getCondition()->subscribeSignal( boost::bind( &WMWriteNIfTI::writeToFile, this ) );

    WModule::properties();
}

void WMWriteNIfTI::writeToFile()
{
    m_saveTriggerProp->set( WPVBaseTypes::PV_TRIGGER_READY, false );

    if( !m_dataSet )
    {
        return;
    }

    infoLog() << "Writing Data to " << m_filename->get().file_string();
    nifti_image *outField = nifti_simple_init_nim();

    outField->nifti_type = 1; // 1==NIFTI-1 (1 file)
    outField->freq_dim = 1;
    outField->phase_dim = 2;
    outField->slice_dim = 3;

    outField->qform_code = 0;
    outField->sform_code = 0;

    std::string description = m_dataSet->getName();
    description.copy( outField->descrip, 80 );

    SaveDataVisitor save( outField );

    // write data
    if( boost::shared_dynamic_cast< WDataSet2< WGridRegular3D2, WScalarStructural > const >( m_dataSet ) )
    {
        boost::shared_dynamic_cast< WDataSet2< WGridRegular3D2, WScalarStructural > const >( m_dataSet )->applyVisitor( &save );
    }
    if( boost::shared_dynamic_cast< WDataSet2< WGridRegular3D2, WVectorFixedStructural< 3 > > const >( m_dataSet ) )
    {
        boost::shared_dynamic_cast< WDataSet2< WGridRegular3D2, WVectorFixedStructural< 3 > > const >( m_dataSet )->applyVisitor( &save );
    }

    std::string s = m_filename->get().file_string();
    if( nifti_set_filenames( outField, s.c_str(), 0, 1 ) )
    {
        throw WException( std::string( "NIfTI filename Problem" ) );
    }

    nifti_image_write( outField );
    nifti_image_free( outField );

    infoLog() << "Writing data completed.";
}
