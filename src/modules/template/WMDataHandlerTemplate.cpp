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

#include "../../newDataHandler/WNeighborhoodIterator.h"
#include "../../newDataHandler/structuralTypes/WVectorFixedStructural.h"

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
            erodeScalarSet( scalarSet );
        }

        // Now we check if we have a vector dataset of 3D-vectors.
        WDataSet2< WGridRegular3D2, WVectorFixedStructural< 3 > >::ConstSPtr vectorSet;
        vectorSet = boost::shared_dynamic_cast< WDataSet2< WGridRegular3D2, WVectorFixedStructural< 3 > > >( m_dataSet );

        if( vectorSet )
        {
            // We will output a synthetic vector set here.
            createVectorSet();
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

    infoLog() << "Voxel 0,0,0 has a value of " << d;

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

    // Now we do the same, this time using voxel iterators. Voxel iterators iterate all voxels of a dataset.
    // Their behavior is similar to STL bidirectional iterators. They provide an operator* for data access:
    //
    // *iter = ...
    // ... = *iter
    //
    // and increment as well as decrement operators:
    //
    // ++iter;
    // iter++;
    // --iter;
    // iter--;
    //
    // The WDataAccess type specifies the correct type for the iterators (either const for WDataAccessConst
    // or non-const for WDataAccess).
    typedef WDataAccessConst< WGridRegular3D2, WDataProxyConst< double > >::VoxelIterator VI;
    VI vi, ve;

    // Next, we assign begin and end iterators to vi and ve. The tie() function creates a std::pair of references
    // from its arguments. The voxels() function returns a std::pair of a begin (.first) and an end (.second) iterator.
    tie( vi, ve ) = access.voxels();

    // Now we just use the std::max_element function.
    VI maxIter = std::max_element( vi, ve );

    // Note that std::max_element returns an interator, so we use operator* to get the value.
    infoLog() << "Maximum value of scalar dataset is: " << *maxIter;

    // Use the operator() of the iterator to get the coordinates of the maximum voxel.
    infoLog() << "Maximum value at voxel: " << maxIter()[ 0 ] << ", " << maxIter()[ 1 ] << ", " << maxIter()[ 2 ];

    // We could write it on our own of couse, it would look like this:
    max = -10e15;

    for( tie( vi, ve ) = access.voxels(); vi != ve; ++vi )
    {
        // The operator* returns the WDataProxyConst< double > for the current voxel, which gets cast to double
        // for operator <.
        if( max < *vi )
        {
            max = *vi;
        }
    }

    infoLog() << "Maximum value of scalar dataset is: " << max;



    // Until now, we used the WDataProxyConst for access to our voxel data of unknown type. We also
    // provide a way to get access to the data via the correct type of the values stored. For this,
    // you need to write a small visitor.
    // We show an example of using a visitor in the next function.

    processScalarSetCorrectType( ds );
}

// Here we define our visitor. We just need an operator() that will get an access object of the correct type
// as a parameter. Using templates, we can even write different operator()s for different value types.

/**
 * A visitor that calculates the maximum value stored in the dataset and prints is to std::cout.
 */
class MaximumVisitor
{
public:

    // When using the visitor mechanism, the dataset creates an access object of the correct type of
    // the values stored in the dataset and then calls the matching operator() of your supplied visitor that
    // has the correct WDataAccess as a parameter.
    //
    // If there are multiple operator()s to choose from, the compiler will choose the correct one if possible.
    //
    // Note that the dataset will create a WDataAccessConst<> instead of a WDataAccess<> if it is a const dataset
    // itself.

    /**
     * Operator for const datasets. Needs an operator < defined on the value type T or a specialization
     * of std::less.
     *
     * \tparam T The type of the data stored in the dataset.
     * \param access The access object of the correct type T.
     */
    template< typename T >
    void operator() ( WDataAccessConst< WGridRegular3D2, T > access )
    {
        // Do not forget the typename keyword here if T is not known.
        typedef typename WDataAccessConst< WGridRegular3D2, T >::VoxelIterator VI;

        // The type T is the actual type of the values stored in the dataset, so the operator*
        // of the VoxelIterator can return references to T ( T const& in this case, as the ::VoxelIterator
        // type defined by WDataAccessConst is a const iterator) instead of the WDataProxy(Const)<>.
        //
        // So, if T was a matrix, you would get a reference to that matrix when using operator*. If you had a non-const
        // access object (by calling applyVisitor to a non-const dataset/dataset pointer), you could also write to the matrix
        // and so on.
        VI vi, ve;
        tie( vi, ve ) = access.voxels();

        std::cout << "The maximum value in the dataset is " << *std::max_element( vi, ve ) << " (again)." << std::endl;
    }

    // We could define an operator for a specific type, for example 3d-vectors:
    //
    // template< typename T >
    // void operator () ( WDataAccessConst< WGridRegular3D, WMatrixFixed< T, 3, 1 > > access )
    //
    // We could also define an operator for any dataset, be it const or non-const:
    //
    // template< typename T >
    // void operator () ( T access )
    //
    // Another option may be to define an operator only for a certain type of values (this time for a non-const dataset).
    //
    // void operator () ( WDataAccess< WGridRegular3D, double > access )
    //
    // Of course we can also use any grid:
    //
    // template< typename GridT, typename T >
    // void operator () ( WDataAccess< GridT, T > access )
    //
    // Note however, that different grids cause the access objects to provide different types of iterators.
};

void WMDataHandlerTemplate::processScalarSetCorrectType( WDataSet2< WGridRegular3D2, WScalarStructural >::ConstSPtr const& ds )
{
    // To apply a visitor, just instantiate it and pass a pointer to the applyVisitor function of the dataset.
    MaximumVisitor maxVis;
    ds->applyVisitor( &maxVis );

    // Note that the design allows to reuse visitors. There is no need to define a maximum finding visitor everytime you need one.
    // Just put all your visitors in some central place.
}

/**
 * This binarizes a scalar dataset.
 */
class BinarizeVisitor
{
public:

    /**
     * Operator for const datasets. Needs an operator < defined on the value type T or a specialization
     * of std::less.
     *
     * \tparam T The type of the data stored in the dataset.
     * \param access The access object of the correct type T.
     */
    template< typename T >
    void operator() ( WDataAccessConst< WGridRegular3D2, T > access )
    {
        typedef typename WDataAccessConst< WGridRegular3D2, T >::VoxelIterator VI;
        VI vi, ve;
        tie( vi, ve ) = access.voxels();

        VI maxIter = std::max_element( vi, ve );
        VI minIter = std::min_element( vi, ve );

        double dist = static_cast< double >( *maxIter - *minIter );
        double middle = static_cast< double >( *minIter ) + dist * 0.5;

        m_result = WDataSet2< WGridRegular3D2, WScalarStructural >::SPtr( new WDataSet2< WGridRegular3D2, WScalarStructural >( access.getGrid(), uint8_t() ) );

        typedef WDataAccess< WGridRegular3D2, WDataProxy< uint8_t > >::VoxelIterator UI;
        WDataAccess< WGridRegular3D2, WDataProxy< uint8_t > > access2 = m_result->getAccess< uint8_t >();
        UI ui = access2.voxels().first;

        for( tie( vi, ve ) = access.voxels(); vi != ve; ++vi )
        {
            if( *vi < middle )
            {
                *ui = 0;
            }
            else
            {
                *ui = 255;
            }
            ++ui;
        }
    }

    /**
     * Get the binarized dataset.
     *
     * \return The result.
     */
    WDataSet2< WGridRegular3D2, WScalarStructural >::SPtr result()
    {
        return m_result;
    }

private:

    //! The resulting dataset.
    WDataSet2< WGridRegular3D2, WScalarStructural >::SPtr m_result;
};

/**
 * Erodes a dataset using neighborhood iteration.
 */
class ErosionVisitor
{
public:

    /**
     * Operator for non-const datasets.
     *
     * \tparam T The type of the data stored in the dataset.
     * \param access The access object of the correct type T.
     */
    template< typename T >
    void operator() ( WDataAccess< WGridRegular3D2, T > access )
    {
        // First we have to choose a type of neighborhood iterator. There are three types that
        // have different behavior at the boundary. ::VoxelNeighborIterator uses a default value
        // for all voxels that are not part of the grid. There are also iterators with clamping and
        // wrapping behavior: ::VoxelNeighborIteratorClamp and ::VoxelNeighborIteratorWrap.

        typedef typename WDataAccess< WGridRegular3D2, T >::VoxelNeighborIterator NI;
        typedef typename WDataAccess< WGridRegular3D2, T >::VoxelIterator VI;

        // There are two functions defined for standard cases of neighborhoods in 3D: makeMoore and
        // makeVonNeumann, which create the respective neighborhoods in 3D. You can also instanciate a
        // neighborhood and add neighbors yourself using WNeighborhood::add:
        //
        // nbh.add( -1, 0, 1 )
        //
        // The coordinates are relative to the voxel position.
        //
        // Also note that makeMoore and makeVonNeumann do not include the position 0,0,0 - i.e. the voxel
        // itself.
        WNeighborhood nbh = makeMoore( 2 );

        // We use the default value 0 for boundary/out-of-grid voxels here.
        WBoundaryStrategyDefault< T > bs( 0 );

        VI vi, ve;

        // Create a dataset to write to.
        m_result = WDataSet2< WGridRegular3D2, WScalarStructural >::SPtr( new WDataSet2< WGridRegular3D2, WScalarStructural >( access.getGrid(), uint8_t() ) );

        typedef WDataAccess< WGridRegular3D2, WDataProxy< uint8_t > >::VoxelIterator UI;

        WDataAccess< WGridRegular3D2, WDataProxy< uint8_t > > access2 = m_result->getAccess< uint8_t >();
        UI ui = access2.voxels().first;

        for( tie( vi, ve ) = access.voxels(); vi != ve; ++vi )
        {
            uint8_t k = 255;

            // Here we instanciate the iterators.
            NI ni, ne;

            // The voxel is not a neighbor of itself if the neighborhood was constructed with makeMoore, so we
            // process it independently.
            if( *vi == 0 )
            {
                k = 0;
            }

            // This uses a similar syntax as with the voxel iteration. The voxel iterator provides a ::neighbors() function that
            // takes two parameters: The neighborhood and an instance of the boundary strategy for that neighborhood iterator:
            //
            // WBoundaryStrategyDefault< T >            for         VoxelNeighborIterator
            // WBoundaryStrategyWrap< T >               for         VoxelNeighborIteratorWrap
            // WBoundaryStrategyClamp< T >              for         VoxelNeighborIteratorClamp
            //
            // Note that the neighbors() function expects pointers for both parameters.

            // This will iterate all neighbors of vi. note that a change to vi while iteration will not effect the neighbor iteration.
            for( tie( ni, ne ) = vi.neighbors( &nbh, &bs ); ni != ne; ++ni )
            {
                // If the value in at least one of the neighbor voxels is 0, we will write 0 to the target voxel in the created dataset.
                if( *ni == 0 )
                {
                    k = 0;
                }
            }

            // Write k to the target voxel.
            *ui = k;

            // Increment ui (output dataset voxel iterator) to match the respective voxel in the input dataset.
            ++ui;
        }
    }

    /**
     * Get the binarized dataset.
     *
     * \return The result.
     */
    WDataSet2< WGridRegular3D2, WScalarStructural >::SPtr result()
    {
        return m_result;
    }

private:

    //! The resulting dataset.
    WDataSet2< WGridRegular3D2, WScalarStructural >::SPtr m_result;
};

void WMDataHandlerTemplate::erodeScalarSet( WDataSet2< WGridRegular3D2, WScalarStructural >::ConstSPtr const& ds )
{
    // In this function, we will demonstrate the use of the neighborhood iterators. We will binarize the dataset and
    // then apply an erosion image filter.

    BinarizeVisitor binVis;
    ds->applyVisitor( &binVis );

    // We get the binary dataset from the visitor.
    WDataSet2< WGridRegular3D2, WScalarStructural >::SPtr binaryDataSet = binVis.result();

    // We have a non-const dataset now, so we can call the operator() of the ErosionVisitor that takes
    // a WDataAccess<> as parameter. This means we could change voxel values if we wanted.

    infoLog() << "Starting erosion.";

    ErosionVisitor eroVis;
    binaryDataSet->applyVisitor( &eroVis );

    infoLog() << "Erosion done.";

    WDataSet2< WGridRegular3D2, WScalarStructural >::SPtr erodedBinaryDataSet = eroVis.result();

    // We output the result using our output connector.
    if( erodedBinaryDataSet )
    {
        m_output->updateData( erodedBinaryDataSet );
    }
}

void WMDataHandlerTemplate::createVectorSet()
{
    WGridRegular3D2::SPtr grid( new WGridRegular3D2( 64, 64, 64 ) );
    
    typedef WDataSet2< WGridRegular3D2, WVectorFixedStructural< 3 > > DS;
    DS::SPtr ds;
    ds = DS::SPtr( new DS( grid, WMatrixFixed< float, 3, 1 >() ) );

    WDataAccess< WGridRegular3D2, WDataProxy< WMatrixFixed< float, 3, 1 > > > access = ds->getAccess< WMatrixFixed< float, 3, 1 > >();

    // In the following few lines we clear all data by copying a (0,0,0)-vector to the voxel data. 
    WMatrixFixed< float, 3, 1 > m;
    m[ 0 ] = m[ 1 ] = m[ 2 ] = 0.0f;

    WDataAccess< WGridRegular3D2, WDataProxy< WMatrixFixed< float, 3, 1 > > >::VoxelIterator vi, ve;

    for( tie( vi, ve ) = access.voxels(); vi != ve; ++vi )
    {
        *vi = m;
    }

    // Now we iterate all voxels of x-y-slice 32.
    WDataAccess< WGridRegular3D2, WDataProxy< WMatrixFixed< float, 3, 1 > > >::SliceIterator si, se;

    for( tie( si, se ) = access.slice( SLICE_XY, 32 ); si != se; ++si )
    {
        // For each voxel of the slice (which has a size of 64x64) we calculate the (normalized) direction vector
        // from the center of the slice to the current voxel, and then write it to the vector data for that voxel.
        m[ 0 ] = si()[ 0 ] - 32;
        m[ 1 ] = si()[ 1 ] - 32;
        m[ 2 ] = 0.0f;

        m = normalize( m );

        *si = m;
   }

    m_output->updateData( ds );
}

