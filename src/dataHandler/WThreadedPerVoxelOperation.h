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

#ifndef WTHREADEDPERVOXELOPERATION_H
#define WTHREADEDPERVOXELOPERATION_H

#include <vector>

#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

#include "../common/WException.h"
#include "../common/WThreadedJobs.h"
#include "../common/WSharedObject.h"
#include "../common/WSharedSequenceContainer.h"
#include "WDataSetSingle.h"
#include "WValueSet.h"
#include "WDataHandlerEnums.h"

//! forward declaration for the test
class WThreadedPerVoxelOperationTest;

/**
 * \class WThreadedPerVoxelOperation
 *
 * A template that performs an operation on a per voxel basis. This
 * template is intended to be used in combination with WThreadedFunction.
 *
 * The template parameters are the internal datatype of the input datasets valueset,
 * the number of input data elements per voxel, the type of the output data and the number of
 * outputs per voxel.
 *
 * Example: Suppose one wants to calculate the largest eigenvector of a symmetric tensor of order
 * 2 per voxel, where the input tensors are stored als 6 floats. The output could be 3 double values.
 * The corresponding template parameters would be float, 6, double, 3.
 */
template< typename Value_T, std::size_t numValues, typename Output_T, std::size_t numOutputs >
class WThreadedPerVoxelOperation : public WThreadedJobs< WValueSet< Value_T >, std::size_t >
{
    //! the test is a friend
    friend class WThreadedPerVoxelOperationTest;

    //! the base class
    typedef WThreadedJobs< WValueSet< Value_T >, std::size_t > BaseType;

public:
    //! the input valueset's type
    typedef WValueSet< Value_T > ValueSetType;

    //! the output valueset's type
    typedef WValueSet< Output_T > OutValueSetType;

    //! the input type for the per-voxel operation
    typedef typename ValueSetType::SubArray const TransmitType;

    //! the output type for the per-voxel operation
    typedef boost::array< Output_T, numOutputs > OutTransmitType;

    //! the function type
    typedef boost::function< OutTransmitType const ( TransmitType const& ) > FunctionType;

    /**
     * Construct a per-voxel operation.
     *
     * \param dataset The input dataset.
     * \param func The function to be evaluated per voxel.
     */
    WThreadedPerVoxelOperation( boost::shared_ptr< WDataSetSingle const > dataset, FunctionType func );

    /**
     * Destructor.
     */
    virtual ~WThreadedPerVoxelOperation();

    /**
     * Get a job for every voxel.
     *
     * \param job The job to be done (in this case the voxel to be processed).
     * \return false, iff no more voxels need to be processed.
     */
    virtual bool getJob( std::size_t& job ); // NOLINT

    /**
     * Perform the computation for a specific voxel.
     *
     * \param The input dataset.
     * \param job The job to be done (in this case the voxel to be processed).
     */
    virtual void compute( boost::shared_ptr< ValueSetType const > input, std::size_t const& job );

    /**
     * Get the output dataset.
     * \return The oupput dataset.
     */
    boost::shared_ptr< WDataSetSingle > getResult();

private:
    //! a threadsafe vector (container)
    typedef WSharedSequenceContainer< std::vector< Output_T > > OutputVectorType;

    //! stores the output of the per-voxel-operation
    OutputVectorType m_output;

    //! the current position in the dataset
    WSharedObject< std::size_t > m_position;

    //! the size of the valueset
    std::size_t m_size;

    //! the function
    FunctionType m_func;

    //! store the grid
    boost::shared_ptr< WGrid > m_grid;
};

template< typename Value_T, std::size_t numValues, typename Output_T, std::size_t numOutputs >
WThreadedPerVoxelOperation< Value_T, numValues, Output_T, numOutputs >::WThreadedPerVoxelOperation(
                                                        boost::shared_ptr< WDataSetSingle const > dataset,
                                                        FunctionType func )
    : BaseType( ( dataset ? boost::shared_dynamic_cast< ValueSetType >( dataset->getValueSet() )
                          : boost::shared_ptr< ValueSetType >() ) ) // NOLINT
{
    if( !dataset )
    {
        throw WException( "No input dataset." );
    }
    if( !dataset->getValueSet() )
    {
        throw WException( "The input dataset has no valueset." );
    }
    if( !dataset->getGrid() )
    {
        throw WException( "The input dataset has no grid." );
    }
    if( dataset->getValueSet()->order() > 1 )
    {
        throw WException( "An order of 2 or more is currently not supported." );
    }
    if( dataset->getValueSet()->dimension() != numValues )
    {
        throw WException( "Invalid valueset dimension." );
    }
    if( !func )
    {
        throw WException( "No valid function provided." );
    }

    m_position.getWriteTicket()->get() = 0;
    m_size = dataset->getValueSet()->size();
    try
    {
        // allocate enough memory for the output data
        m_output.getWriteTicket()->get().resize( m_size * numOutputs );
    }
    catch( std::exception const& e )
    {
        throw WException( e.what() );
    }
    m_func = func;
    m_grid = dataset->getGrid();
}

template< typename Value_T, std::size_t numValues, typename Output_T, std::size_t numOutputs >
WThreadedPerVoxelOperation< Value_T, numValues, Output_T, numOutputs >::~WThreadedPerVoxelOperation()
{
}

template< typename Value_T, std::size_t numValues, typename Output_T, std::size_t numOutputs >
bool WThreadedPerVoxelOperation< Value_T, numValues, Output_T, numOutputs >::getJob( std::size_t& job ) // NOLINT
{
    typename WSharedObject< std::size_t >::WriteTicket t = m_position.getWriteTicket();
    bool b = t->get() < m_size;
    job = t->get();
    t->get() += 1 * static_cast< std::size_t >( b );
    return b;
}

template< typename Value_T, std::size_t numValues, typename Output_T, std::size_t numOutputs >
void WThreadedPerVoxelOperation< Value_T, numValues, Output_T, numOutputs >::compute( boost::shared_ptr< ValueSetType const > input,
                                                               std::size_t const& job )
{
    TransmitType t = input->getSubArray( job * numValues, numValues );
    OutTransmitType o = m_func( t );
    typename OutputVectorType::WriteTicket w = m_output.getWriteTicket();
    for( std::size_t k = 0; k < numOutputs; ++k )
    {
        w->get()[ job * numOutputs + k ] = o[ k ];
    }
}

template< typename Value_T, std::size_t numValues, typename Output_T, std::size_t numOutputs >
boost::shared_ptr< WDataSetSingle > WThreadedPerVoxelOperation< Value_T, numValues, Output_T, numOutputs >::getResult()
{
    boost::shared_ptr< OutValueSetType > values( new OutValueSetType( 1, numOutputs,
                                                                m_output.getReadTicket()->get(),
                                                                DataType< Output_T >::type ) );
    return boost::shared_ptr< WDataSetSingle >( new WDataSetSingle( values, m_grid ) );
}

#endif  // WTHREADEDPERVOXELOPERATION_H
