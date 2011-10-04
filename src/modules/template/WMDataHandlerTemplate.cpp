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
    // Writing your own grid is a bit trickier if you want to provide your own iterator types too.

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

        // get the data from the input connector
        m_dataSet = m_input->getData();

        // let's check if this is a scalar dataset
        WDataSet2< WGridRegular3D2, WScalarStructural >::ConstSPtr scalarSet;

        // We use boost::shared_dynamic_cast to cast the WDataSet2Base pointer to a
        // WDataSet2< WGridRegular3D2, WScalarStructural > pointer.
        scalarSet = boost::shared_dynamic_cast< WDataSet2< WGridRegular3D2, WScalarStructural > >( m_dataSet );

        // If the cast was successful, scalarSet will be a valid pointer to a scalar dataset.
        if( scalarSet )
        {
            processScalarSet( scalarSet );
        }
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

void WMDataHandlerTemplate::processScalarSet( WDataSet2< WGridRegular3D2, WScalarStructural >::ConstSPtr const& ds )
{
    // To access any data in a dataset, you first need to aquire a WDataAccess or WDataAccessConst for that dataset.
    // The data access objects provide several methods for element access as well as a pointer to the grid used by the
    // dataset.
    // You can aquire an access object by two different methods. Remember that you still do not know what kind of data is
    // actually stored in the dataset. We do know, however, that is only contains a single value per voxel, since this is
    // a scalar dataset as described by the WScalarStructural type. So the first method is to just access the data using a
    // set type, for example double:
    
    WDataAccessConst< WGridRegular3D2, WDataProxyConst< double > > access = ds->getAccess< double >();

    // Here, the first template parameter is again the grid type. The choise of iterators, interpolators and data access
    // functions depends on this parameter. For example, slice iterators are only provided for WGridRegular3D2.
    // The second parameter is the type that will be returned by any data access operations. As we do not know the type
    // stored in the dataset, we use a WDataProxy< double > here.
    // The WDataProxy<> template can be used to access values as if they were of type T, where T is the template parameter
    // of the proxy. So in our case, we can work with double values, even if the actual type of the data was int.
    // Values will be converted via the operator= of the data types or appropriate cast operators, if provided.
    //
    // For scalar datasets, this is no problem. If we had a WDataSet2< WGridRegular3D2, WVectorFixedStructural< 3 > >
    // instead, which may store a WMatrixFixed< float, 3, 1 > for example, we would need either
    //
    // .. WMatrixFixed< double, 3, 1 >::operator= ( WMatrixFixed< float, 3, 1 > const& m )
    //
    // or
    //
    //    WMatrixFixed< float, 3, 1 >::operator WMatrixFixed< double, 3, 1 >()
    //
    // to read data.

    // The WDataProxy< T > itself can be cast to T, and can be set via the operator= ( T const& t ). The WDataProxyConst< T >
    // can only be cast to T.
    // This has an implication on the written code, because while you can do this (suppose i is an iterator):
    //
    // T t = *i;
    // *i = t;
    // *i = t + 5;
    // t = 5 + *i;
    //
    // you cannot do this:
    //
    // t = *i + 5;
    //
    // Instead you have to do this:
    //
    // t = static_cast< T >( *i ) + 5;
    //
    // Also make sure to use the WDataProxyConst<> for const data access objects.

    // Now we will get the grid to check the dimensions of the dataset.

    WGridRegular3D2::ConstSPtr grid = access.getGrid();

    // We need at least one voxel.
    if( grid->getNbVoxelsX() < 1 || grid->getNbVoxelsY() < 1 || grid->getNbVoxelsZ() < 1 )
    {
        return;
    }

    // Now we can access voxel (0,0,0).
    // Note that bounds checking is only done in debug mode.
    double d = access.getAt( 0, 0, 0 );

    // We could also do something like access.getAt( 0, 0, 0 ) = d if we had a mutable access object.

    // Let's find the maximum value in our dataset. We will do this with a naive implementation first.
    WGridRegular3D2::VoxelIndex idx;
    double max = -10e15;

    for( idx[ 0 ] = 0; idx[ 0 ] < grid->getNbVoxelsX(); ++idx[ 0 ] )
    {
        for( idx[ 1 ] = 0; idx[ 1 ] < grid->getNbVoxelsY(); ++idx[ 1 ] )
        {
            for( idx[ 2 ] = 0; idx[ 2 ] < grid->getNbVoxelsZ(); ++idx[ 2 ] )
            {
                if( max < access.getAt( idx ) )
                {
                    max = access.getAt( idx );
                }
            }
        }
    }

    infoLog() << "Maximum value of scalar dataset is: " << max;
}

