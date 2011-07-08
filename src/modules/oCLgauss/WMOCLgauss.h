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

#ifndef WMOCLGAUSS_H
#define WMOCLGAUSS_H

#include <string>

#include <osg/Geode>

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

#include "core/dataHandler/WDataSetScalar.h"

#include "CL/opencl.h"

/** 
 * Gauss filtering using OpenCL 
 *
 * \ingroup modules
 */
class WMOCLgauss: public WModule
{
public:

    /**
     * standard constructor
     */
    WMOCLgauss();

    /**
     * standard destructor
     */
    virtual ~WMOCLgauss();

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
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     * Get the icon for this module in XPM format.
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
    /** Returns the error message to a given error number
     *
     *  @return error message
     *  @param cl_int     error number as returned by many funcions */
    const char* oclErrorString(cl_int error);

    /** Gets the platform ID for NVIDIA if available, otherwise default to platform 0
     *
     *  @return the id 
     *  @param clSelectedPlatformID         OpenCL platform ID  */
    cl_int oclGetPlatformID(cl_platform_id* clSelectedPlatformID);
    
    /** OpenCL parameters */
    cl_platform_id platform;
    cl_context context;
    cl_command_queue queue;
    cl_device_id device;
    cl_program program;
    size_t program_length;  
    cl_kernel kernel;
    cl_mem gol_d;
    cl_mem res_d;
    
    int* gol_h;
    int* res_h;
    
    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * The number of iterations to use for filtering
     */
    WPropInt m_iterations;
    
    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_input;  //!< Input connector required by this module.
    boost::shared_ptr< WModuleOutputData< WDataSetScalar > > m_output; //!< The only output of this filter module.
    boost::shared_ptr< WDataSetScalar > m_dataSet; //!< Pointer providing access to the treated data set in the whole module.
  
};

#endif  // WMOCLGAUSS_H
