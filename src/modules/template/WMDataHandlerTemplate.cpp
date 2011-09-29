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

#include "../../kernel/WKernel.h"
#include "../emptyIcon.xpm" // Please put a real icon here.

#include "WMDataHandlerTemplate.h"

WMDataHandlerTemplate::WMDataHandlerTemplate():
    WModule()
{
}

WMDataHandlerTemplate::~WMDataHandlerTemplate()
{
}

boost::shared_ptr< WModule > WMDataHandlerTemplate::factory() const
{
    return boost::shared_ptr< WModule >( new WMDataHandlerTemplate() );
}

const char** WMDataHandlerTemplate::getXPMIcon() const
{
    return emptyIcon_xpm;
}
const std::string WMDataHandlerTemplate::getName() const
{
    return "DataHandler Template";
}

const std::string WMDataHandlerTemplate::getDescription() const
{
    return "This module shows how to use the various iterators provided by the dataHandler.";
}

void WMDataHandlerTemplate::moduleMain()
{
    // OpenWalnut provides the WDataSet2<> template for data management. It has two template parameters, the first being the grid type and
    // the second is a type that describes the data stored per point or voxel. We call this the "structural type". It defines how many values
    // the stored data per voxel has and of what type these may be.
    // An example for a grid type is WGridRegular3D2, which represents a grid of cuboid cells. Examples for the structural types are
    // WScalarStructural, which allows a single value per voxel, or WVectorFixedStructural, which allows a vector of a fixed (a compile-time
    // constant) number of elements. The integral type (unsigned char, float, int, etc... ) of the values is variable in both cases.
    //
    // Examples:
    //
    // WDataSet2< WGridRegular3D2, WScalarStructural >                  may contain a single integral value per voxel (int, double, uint16_t, ... ),
    //                                                                  which is of the same type for each voxel.
    //
    // WDataSet2< WGridRegular3D2, WMatrixFixedStructural< 3, 3 > >     may contain a 3x3 matrix where all elements (of all matrices in the dataset)
    //                                                                  are of the same type (again int or double or ... ).
    //
    // You can also write your own structural type that describes the data you want to store in the dataset, as long as this data is of constant size
    // per voxel. Then just plug your new structural type into the WDataSet2 template and you are ready to go.
    //
    // Writing your own grids is a bit trickier if you want to provide your own iterator types too.

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

        m_dataSet = m_input->getData();
    }
}

void WMDataHandlerTemplate::connectors()
{
    // We add an input connector for WDataSet2Base pointers. This will allow the module to take any WDataSet2
    // as an input. To be more precise, a connector can transport the type provided to the WModuleInputData<>
    // or any derived type, as long as they derive from WTransferable.
    // As all WDataSet2<> classes are derived from WDataSet2Base, they may be input to this input connector.
    //
    // If you only want to allow specific datasets as input, use the corresponding WDataSet2<> class.
    m_input = boost::shared_ptr< WModuleInputData< WDataSet2Base > >( new WModuleInputData< WDataSet2Base >(
                    shared_from_this(), "in", "The input data." ) );

    // The same goes for the output connector. We can output any WDataSet here.
    m_output = boost::shared_ptr< WModuleOutputData< WDataSet2Base > >( new WModuleOutputData< WDataSet2Base >(
                    shared_from_this(), "out", "The output data." ) );

    addConnector( m_input );
    addConnector( m_output );

    WModule::connectors();
}

void WMDataHandlerTemplate::properties()
{
    WModule::properties();
}

void WMDataHandlerTemplate::requirements()
{
}
