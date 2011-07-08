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


// C/C++ headers
#include <string>
//#include <cstdlib> 

// OpenCL 
#if defined __APPLE__ || defined(MACOSX)
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif

// OpenWalnut 
#include "core/kernel/WKernel.h"
#include "core/common/WIOTools.h"
#include "WMOCLgauss.xpm" // Please put a real icon here.

// ocl helpers
//#include "WMOCLgaussutil.h"


// own header
#include "WMOCLgauss.h"



// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMOCLgauss )

WMOCLgauss::WMOCLgauss():
    WModule()
{
}

WMOCLgauss::~WMOCLgauss()
{
    // Cleanup!
    delete[] gol_h;
    delete[] res_h;
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    clReleaseMemObject(gol_d);
    clReleaseMemObject(res_d);

    removeConnectors();
}

boost::shared_ptr< WModule > WMOCLgauss::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMOCLgauss() );
}

const char** WMOCLgauss::getXPMIcon() const
{
    return WMOCLgauss_xpm; // Please put a real icon here.
}
const std::string WMOCLgauss::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "OCLgauss";
}

const std::string WMOCLgauss::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    return "Gaussfiltering with OpenCL on the gpu";
}

void WMOCLgauss::connectors()
{
    // input
    m_input = boost::shared_ptr<WModuleInputData<WDataSetScalar> >(
            new WModuleInputData<WDataSetScalar> ( shared_from_this(), "in",
                    "The dataset to filter" ) );
    addConnector( m_input ); // add it to the list of connectors..
    
    // output
    m_output = boost::shared_ptr<WModuleOutputData<WDataSetScalar> >(
            new WModuleOutputData<WDataSetScalar> ( shared_from_this(), "out",
                    "The filtered data set." ) );
    addConnector( m_output );

    // call WModules initialization
    WModule::connectors();
}


void WMOCLgauss::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_iterations      = m_properties->addProperty( "Iterations",         "How often should the filter be applied.", 1, m_propCondition );
    m_iterations->setMin( 0 );
    m_iterations->setMax( 100 );

    WModule::properties();
}



void WMOCLgauss::moduleMain()
{
    // let the main loop awake if the data changes or the properties changed.
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    cl_int error = 0;  //Used to handle error codes
    std::string path;
    std::string cSourceCL;
    
    const int num = 3200;
    const int size = num*num;
    gol_h = new int[size];
    res_h = new int[size];
    // Initialize both vectors
    for (int i = 0; i < size; i++) {
       gol_h[i] = (rand()%10 > 8)?1:0;
       res_h[i] = 0;
    }
    
    int* zwischen = new int[size];

    const int mem_size = sizeof(int)*size;


    std::stringstream lStream;
    int max_x = 8;
    int max_y = 80;
    debugLog() << "";
    for(int x=0;x<max_x;x++){
      std::stringstream lStream;
      for(int y=0;y<max_y;y++){
        if (gol_h[num*y+x]==0) { lStream << "-";}
        if (gol_h[num*y+x]==1) { lStream << "X";}
      }
      debugLog() << lStream.str();
    }
    debugLog() <<"";
    
    
    // OpenCL Init
    // Platform
    error = oclGetPlatformID(&platform);
    if (error != CL_SUCCESS) { errorLog() << "OpenCL error while getting platform id: " << oclErrorString(error); }
    // Device
    error = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    if (error != CL_SUCCESS) { errorLog() << "OpenCL error while getting device ids: " << oclErrorString(error); }
    // Context
    context = clCreateContext(0, 1, &device, NULL, NULL, &error);
    if (error != CL_SUCCESS) { errorLog() << "OpenCL error while creating context: " << oclErrorString(error); }
    // Command-queue
    queue = clCreateCommandQueue(context, device, 0, &error);
    if (error != CL_SUCCESS) { errorLog() << "OpenCL error while creating command queue: " << oclErrorString(error); }  
    // Creates the program 
    path = "/local/sommer/openwalnut/src/modules/oCLgauss/gauss.cl";//CL_SOURCE_DIR is to be set in the CMakeLists.txt 
    cSourceCL = readFileIntoString( path );
    program_length = (size_t)(cSourceCL.length());
    program = clCreateProgramWithSource(context, 1, (const char **) &cSourceCL, &program_length, &error);
    if (error != CL_SUCCESS) { errorLog() << "OpenCL error while clCreateProgramWithSource: " << oclErrorString(error); }
    // Builds the program
    error = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    if (error != CL_SUCCESS) { errorLog() << "OpenCL error while building program: " << oclErrorString(error); }
    // Extracting the kernel
    kernel = clCreateKernel(program, "clkernel", &error);
    if (error != CL_SUCCESS) { errorLog() << "OpenCL error while extracting kernel: " << oclErrorString(error); }
    
    
    // signal ready state
    ready();

    // the number of iterations
    unsigned int iterations = 1;

    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        // Now, the moduleState variable comes into play. The module can wait for the condition, which gets fired whenever the input receives data
        // or an property changes. The main loop now waits until something happens.
        debugLog() << "OCLgauss Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish
        if( m_shutdownFlag() )
        {
            break;
        }
        
        // 4. ?
        if( m_iterations->changed() )
        {
            // a changed number of iteration also requires recalculation
            iterations = m_iterations->get( true );
            //dataChanged = ( iterations >= 1 );

            if( iterations > 0 ){
              for(int i=0;i<size;i++){ zwischen[i] = gol_h[i]; }
              for(int i=0;i<iterations;i++){

                // Allocates a buffer of size mem_size and copies mem_size bytes from src_a_h
                gol_d = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, mem_size, zwischen, &error);
                res_d = clCreateBuffer(context, CL_MEM_WRITE_ONLY, mem_size, NULL, &error);
                if (error != CL_SUCCESS) { errorLog() << "OpenCL error while allocating device memory "; }
                // parameters
                error = clSetKernelArg(kernel, 0, sizeof(cl_mem), &gol_d);
                if (error != CL_SUCCESS) { errorLog() << "OpenCL error while enqueueing parameters: " << oclErrorString(error);    }
                error = clSetKernelArg(kernel, 1, sizeof(cl_mem), &res_d);
                if (error != CL_SUCCESS) { errorLog() << "OpenCL error while enqueueing parameters: " << oclErrorString(error); }
                error = clSetKernelArg(kernel, 2, sizeof(num), &num);
                if (error != CL_SUCCESS) { errorLog() << "OpenCL error while enqueueing parameters: " << oclErrorString(error); }


                // Launching kernel
                const size_t local_ws = 512;  // Number of work-items per work-group
                size_t gws = size;
                if ((int)size % (int)local_ws != 0)
                  { gws = size + local_ws - ((int)size % (int)local_ws); }// Total number of work-items
                const size_t global_ws = gws;
                error = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_ws, &local_ws, 0, NULL, NULL);
                if (error != CL_SUCCESS) { errorLog() << "launching kernel: " << oclErrorString(error); }
                // Reading back
                error = clEnqueueReadBuffer(queue, res_d, CL_TRUE, 0, mem_size, zwischen, 0, NULL, NULL);
                if (error != CL_SUCCESS) { errorLog() << "OpenCL error while reading back: " << oclErrorString(error); }
                    clReleaseMemObject(gol_d);
                    clReleaseMemObject(res_d);

              }
              for(int i=0;i<size;i++){ res_h[i] = zwischen[i]; }
            }
            debugLog() << "" << "Spielfeld nach " << iterations << " Iterationen";
            for(int x=0;x<max_x;x++){
              std::stringstream lStream;
              for(int y=0;y<max_y;y++){
                if (res_h[num*y+x]==0) { lStream << "-";}
                if (res_h[num*y+x]==1) { lStream << "X";}
              }
              debugLog() << lStream.str();
            }
            debugLog() <<"";
        } // m_iterations->changed()
        // 5. profit!
        
        // filling m_output
        // if the number of iterations is 0 -> simply set the input as output
        if( iterations == 0 )
        {
            m_output->updateData( m_dataSet );
        }
        
    }
}


// Helper function to get error string
// *********************************************************************
const char* WMOCLgauss::oclErrorString(cl_int error)
{
    static const char* errorString[] = {
        "CL_SUCCESS",
        "CL_DEVICE_NOT_FOUND",
        "CL_DEVICE_NOT_AVAILABLE",
        "CL_COMPILER_NOT_AVAILABLE",
        "CL_MEM_OBJECT_ALLOCATION_FAILURE",
        "CL_OUT_OF_RESOURCES",
        "CL_OUT_OF_HOST_MEMORY",
        "CL_PROFILING_INFO_NOT_AVAILABLE",
        "CL_MEM_COPY_OVERLAP",
        "CL_IMAGE_FORMAT_MISMATCH",
        "CL_IMAGE_FORMAT_NOT_SUPPORTED",
        "CL_BUILD_PROGRAM_FAILURE",
        "CL_MAP_FAILURE",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "CL_INVALID_VALUE",
        "CL_INVALID_DEVICE_TYPE",
        "CL_INVALID_PLATFORM",
        "CL_INVALID_DEVICE",
        "CL_INVALID_CONTEXT",
        "CL_INVALID_QUEUE_PROPERTIES",
        "CL_INVALID_COMMAND_QUEUE",
        "CL_INVALID_HOST_PTR",
        "CL_INVALID_MEM_OBJECT",
        "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR",
        "CL_INVALID_IMAGE_SIZE",
        "CL_INVALID_SAMPLER",
        "CL_INVALID_BINARY",
        "CL_INVALID_BUILD_OPTIONS",
        "CL_INVALID_PROGRAM",
        "CL_INVALID_PROGRAM_EXECUTABLE",
        "CL_INVALID_KERNEL_NAME",
        "CL_INVALID_KERNEL_DEFINITION",
        "CL_INVALID_KERNEL",
        "CL_INVALID_ARG_INDEX",
        "CL_INVALID_ARG_VALUE",
        "CL_INVALID_ARG_SIZE",
        "CL_INVALID_KERNEL_ARGS",
        "CL_INVALID_WORK_DIMENSION",
        "CL_INVALID_WORK_GROUP_SIZE",
        "CL_INVALID_WORK_ITEM_SIZE",
        "CL_INVALID_GLOBAL_OFFSET",
        "CL_INVALID_EVENT_WAIT_LIST",
        "CL_INVALID_EVENT",
        "CL_INVALID_OPERATION",
        "CL_INVALID_GL_OBJECT",
        "CL_INVALID_BUFFER_SIZE",
        "CL_INVALID_MIP_LEVEL",
        "CL_INVALID_GLOBAL_WORK_SIZE",
    };

    const int errorCount = sizeof(errorString) / sizeof(errorString[0]);

    const int index = -error;

    return (index >= 0 && index < errorCount) ? errorString[index] : "";

}

// NOTE: NVIDIA Code, free but check if its allowed to use here
cl_int WMOCLgauss::oclGetPlatformID(cl_platform_id* clSelectedPlatformID)
{
      char chBuffer[1024];
    cl_uint num_platforms;
    cl_platform_id* clPlatformIDs;
    cl_int ciErrNum;
    *clSelectedPlatformID = NULL;
    cl_uint i = 0;

    // Get OpenCL platform count
    ciErrNum = clGetPlatformIDs (0, NULL, &num_platforms);
    if (ciErrNum != CL_SUCCESS)
    {
        //shrLog(" Error %i in clGetPlatformIDs Call !!!\n\n", ciErrNum);
        printf(" Error %i in clGetPlatformIDs Call !!!\n\n", ciErrNum);
        return -1000;
    }
    else
    {
        if(num_platforms == 0)
        {
            //shrLog("No OpenCL platform found!\n\n");
            printf("No OpenCL platform found!\n\n");
            return -2000;
        }
        else
        {
            // if there's a platform or more, make space for ID's
            if ((clPlatformIDs = (cl_platform_id*)malloc(num_platforms * sizeof(cl_platform_id))) == NULL)
            {
                //shrLog("Failed to allocate memory for cl_platform ID's!\n\n");
                printf("Failed to allocate memory for cl_platform ID's!\n\n");
                return -3000;
            }

            // get platform info for each platform and trap the NVIDIA platform if found
            ciErrNum = clGetPlatformIDs (num_platforms, clPlatformIDs, NULL);
            printf("Available platforms:\n");
            for(i = 0; i < num_platforms; ++i)
            {
                ciErrNum = clGetPlatformInfo (clPlatformIDs[i], CL_PLATFORM_NAME, 1024, &chBuffer, NULL);
                if(ciErrNum == CL_SUCCESS)
                {
                    printf("platform %d: %s\n", i, chBuffer);
                    if(strstr(chBuffer, "NVIDIA") != NULL)
                    {
                        printf("selected platform %d\n", i);
                        *clSelectedPlatformID = clPlatformIDs[i];
                        break;
                    }
                }
            }

            // default to zeroeth platform if NVIDIA not found
            if(*clSelectedPlatformID == NULL)
            {
                //shrLog("WARNING: NVIDIA OpenCL platform not found - defaulting to first platform!\n\n");
                //printf("WARNING: NVIDIA OpenCL platform not found - defaulting to first platform!\n\n");
                printf("selected platform: %d\n", 0);
                *clSelectedPlatformID = clPlatformIDs[0];
            }

            free(clPlatformIDs);
        }
    }

    return CL_SUCCESS;
}

