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

#include <memory>
#include <string>
#include <vector>

#include "WMTransferFunction2D.h"
#include "WMTransferFunction2D.xpm"
#include "core/common/WTransferFunction2D.h"
#include "core/dataHandler/WDataSetScalar.h"
#include "core/dataHandler/WGrid.h"
#include "core/dataHandler/WGridRegular3D.h"
#include "core/dataHandler/WValueSet.h"
#include "core/dataHandler/WValueSetBase.h"
#include "core/dataHandler/datastructures/WValueSetHistogram.h"
#include "core/kernel/WKernel.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMTransferFunction2D )

WMTransferFunction2D::WMTransferFunction2D():
    WModule()
{
}

WMTransferFunction2D::~WMTransferFunction2D()
{
    // Cleanup!
}

std::shared_ptr< WModule > WMTransferFunction2D::factory() const
{
    return std::shared_ptr< WModule >( new WMTransferFunction2D() );
}

const char** WMTransferFunction2D::getXPMIcon() const
{
    return WMTransferFunction2D_xpm;
}

const std::string WMTransferFunction2D::getName() const
{
    return "Transfer Function 2D";
}

const std::string WMTransferFunction2D::getDescription() const
{
    return "A module to modify a transfer function.";
}

void WMTransferFunction2D::connectors()
{
    // the data set we use for displaying the histogram
    m_inputDataSet0 = WModuleInputData < WDataSetSingle >::createAndAdd( shared_from_this(),
            "histogram input data set 0", "The data set used to display a histogram." );

    // the data set we use for displaying the histogram
    m_inputDataSet1 = WModuleInputData < WDataSetSingle >::createAndAdd( shared_from_this(),
                                                                         "histogram input data set 1", "The data set used to display a histogram." );

    // an output connector for the transfer function created
    m_output = WModuleOutputData < WDataSetSingle >::createAndAdd( shared_from_this(),
            "TransferFunction2D", "The selected transfer function" );

    WModule::connectors();
}

void WMTransferFunction2D::properties()
{

    m_propCondition = std::shared_ptr< WCondition >( new WCondition() );
    WTransferFunction2D tf;
    m_transferFunction = m_properties->addProperty( "2D Transfer Function", "The 2D transfer function editor.", tf, m_propCondition, false );

    m_resolution = m_properties->addProperty( "Number of Samples",
            "Number of samples in the transfer function. "
            "Some modules connected to the output may have additional restrictions. Volume rendering, e.g., requires a power of two "
            "samples at the moment."
            , 128, m_propCondition );
    m_resolution->setMin( 4 );
    m_resolution->setMax( 1024 );
    WModule::properties();

}

void WMTransferFunction2D::requirements()
{
    //m_requirements.push_back( new WGERequirement() );
}

void WMTransferFunction2D::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_inputDataSet0->getDataChangedCondition() );
    m_moduleState.add( m_inputDataSet1->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    ready();

    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if( m_shutdownFlag() )
          break;

        bool tfChanged = m_transferFunction->changed();
        WTransferFunction2D tf = m_transferFunction->get( true );
        if( m_inputDataSet0->updated() || m_inputDataSet1->updated() )
        {
            std::shared_ptr< WDataSetSingle > dataSet0 = m_inputDataSet0->getData();
            std::shared_ptr< WDataSetSingle > dataSet1 = m_inputDataSet1->getData();

            bool dataValid = ( dataSet0 != NULL && dataSet1 != NULL);
            if( !dataValid )
            {
            }
            else
            {
            }
            // either way, we changed the data and want to update the TF
            m_transferFunction->set( tf );
        }

        if( m_resolution->changed() || tfChanged )
        {
        }
    }
}
