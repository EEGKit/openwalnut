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

#ifndef WMCALCULATETENSORS_H
#define WMCALCULATETENSORS_H

#include <string>
#include <vector>

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"
#include "core/common/WThreadedFunction.h"
#include "core/common/math/WMatrix.h"
#include "core/dataHandler/WThreadedPerVoxelOperation.h"
#include "core/dataHandler/WDataSetSphericalHarmonics.h"
#include "core/dataHandler/WDataSetDTI.h"

/**
 * \class WMCalculateTensors
 *
 * A module that calculates tensors from the input SH dataset.
 *
 * \ingroup modules
 */
class WMCalculateTensors: public WModule
{
    //! a conveniance typedef
    typedef WMCalculateTensors This;

public:
    /**
     * Standard constructor.
     */
    WMCalculateTensors();

    /**
     * Destructor.
     */
    virtual ~WMCalculateTensors();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description to module.
     */
    virtual const std::string getDescription() const;

    /**
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual std::shared_ptr< WModule > factory() const;

    /**
     * Get the icon for this module in XPM format.
     * \return The icon.
     */
    virtual const char** getXPMIcon() const;

protected:
    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void moduleMain();

    /**
     * Initialize the connectors this module is using.
     */
    virtual void connectors();

    /**
     * Initialize the properties for this module.
     */
    virtual void properties();

private:
    //! the threaded function type for tensor computation, order 4 to 2
    typedef WThreadedPerVoxelOperation< double, 6, double, 6 > TensorFuncType;

    //! the threadpool
    typedef WThreadedFunction< TensorFuncType > TensorPoolType;

    /**
     * A function that gets called for every voxel in the input SH-dataset. Calculates a
     * diffusion tensor.
     *
     * \param s An array of SH-coefficients.
     * \return A boost::array of size 6 that contains the result for the given voxel.
     */
    boost::array< double, 6 > perVoxelTensorFunc( WValueSet< double >::SubArray const& s );

    /**
     * Reset the threaded functions.
     */
    void resetTensorPool();

    /**
     * Handle an exception thrown by a worker thread.
     *
     * \param e The exception that was thrown during multithreaded computation.
     */
    void handleException( WException const& e );

    /**
     * Reset the progress indicator in the ui.
     *
     * \param todo The number of steps needed to complete the job.
     */
    void resetProgress( std::size_t todo );

    //! A pointer to the input dataset.
    std::shared_ptr< WDataSetSphericalHarmonics > m_dataSet;

    //! The output dataset.
    std::shared_ptr< WDataSetDTI > m_result;

    //! The output Connector.
    std::shared_ptr< WModuleOutputData< WDataSetDTI > > m_output;

    //! The input Connector for the SH data.
    std::shared_ptr< WModuleInputData< WDataSetSphericalHarmonics > > m_input;

    //! The object that keeps track of the current progress.
    std::shared_ptr< WProgress > m_currentProgress;

    //! The last exception thrown by any worker thread.
    std::shared_ptr< WException > m_lastException;

    //! Condition indicating if any exception was thrown.
    std::shared_ptr< WCondition > m_exceptionCondition;

    //! The threaded function object.
    std::shared_ptr< TensorFuncType > m_tensorFunc;

    //! The threadpool.
    std::shared_ptr< TensorPoolType > m_tensorPool;

    //! The sh->tensor conversion.
    WMatrix<double> m_SHToTensorMat;
};

#endif  // WMCALCULATETENSORS_H
