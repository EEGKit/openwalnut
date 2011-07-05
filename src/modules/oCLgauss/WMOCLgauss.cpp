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

// OpenCL 
#include "CL/opencl.h"
#include "CL/cl.h"

// OpenWalnut 
#include "core/kernel/WKernel.h"
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

    cl_int error = 0;   // Used to handle error codes
    cl_platform_id platform;
    cl_context context;
    cl_command_queue queue;
    cl_device_id device;

    // Platform
    error = oclGetPlatformID(&platform);
    debugLog() << "init platform id: " << oclErrorString(error); 
    // Device
    error = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    debugLog() << "init device ids: " << oclErrorString(error); 
    // Context
    context = clCreateContext(0, 1, &device, NULL, NULL, &error);
    debugLog() << "creating context: " << oclErrorString(error); 
    // Command-queue
    queue = clCreateCommandQueue(context, device, 0, &error);
    debugLog() << "creating command queue: " << oclErrorString(error); 

    const int size = 1234567;
    float* src_a_h = new float[size];
    float* src_b_h = new float[size];
    float* res_h = new float[size];
    // Initialize both vectors
    for (int i = 0; i < size; i++) {
       src_a_h[i] = src_b_h[i] = (float) i;
    }

    const int mem_size = sizeof(float)*size;
    // Allocates a buffer of size mem_size and copies mem_size bytes from src_a_h
    cl_mem src_a_d = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, mem_size, src_a_h, &error);
    cl_mem src_b_d = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, mem_size, src_b_h, &error);
    cl_mem res_d = clCreateBuffer(context, CL_MEM_WRITE_ONLY, mem_size, NULL, &error);
    debugLog() << "mem allocated "; 
    
    // Creates the program
    int pl;
    cl_program program;
    size_t program_length;    
    std::string path("/local/sommer/OpenWalnut/src/modules/oCLgauss");//CL_SOURCE_DIR is set in the CMakeLists.txt
    path += "/" + std::string("gauss.cl");
    char* cSourceCL = file_contents(path.c_str(), &pl); //file_contents is helper function, it loads the contents of the file at the given path
    program_length = (size_t)pl;

    // create the program
    program = clCreateProgramWithSource(context, 1, (const char **) &cSourceCL, &program_length, &error);
    debugLog() << "clCreateProgramWithSource: " << oclErrorString(error);

    // Builds the program
    error = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    debugLog() << "build program: " << oclErrorString(error); 

    /*// Shows the log
    char* build_log;
    size_t log_size;
    // First call to know the proper size
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
    build_log = new char[log_size+1];
    // Second call to get the log
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, build_log, NULL);
    build_log[log_size] = '\0';
    debugLog() << "build LOG: " <<build_log;
    delete[] build_log;*/

    // Extracting the kernel
    cl_kernel vector_add_k = clCreateKernel(program, "vector_add_gpu", &error);
    debugLog() << "extracting kernel: " << oclErrorString(error); 

    // parameters
    error = clSetKernelArg(vector_add_k, 0, sizeof(cl_mem), &src_a_d);
    debugLog() << "enqueueing parameters: " << oclErrorString(error);
    error = clSetKernelArg(vector_add_k, 1, sizeof(cl_mem), &src_b_d);
    debugLog() << "enqueueing parameters: " << oclErrorString(error);
    error = clSetKernelArg(vector_add_k, 2, sizeof(cl_mem), &res_d);
    debugLog() << "enqueueing parameters: " << oclErrorString(error);
    error = clSetKernelArg(vector_add_k, 3, sizeof(size), &size);
    debugLog() << "enqueueing parameters: " << oclErrorString(error);
    
    // Launching kernel
    const size_t local_ws = 512;  // Number of work-items per work-group
    size_t gws = size;
    if ((int)size % (int)local_ws != 0)
      { gws = size + local_ws - ((int)size % (int)local_ws); }// Total number of work-items
    const size_t global_ws = gws;
    error = clEnqueueNDRangeKernel(queue, vector_add_k, 1, NULL, &global_ws, &local_ws, 0, NULL, NULL);
    debugLog() << "launching kernel: " << oclErrorString(error);
    
    // Reading back
    float* check = new float[size];
    error = clEnqueueReadBuffer(queue, res_d, CL_TRUE, 0, mem_size, check, 0, NULL, NULL);
    debugLog() << "reading back: " << oclErrorString(error);

    debugLog() << "";
    for (int i = 0; i < size; i++) { res_h[i] = src_a_h[i] + src_b_h[i]; }
    debugLog() << res_h[177];
    debugLog() << check[177];
    debugLog() <<"";

    
    // Cleaning up
    delete[] src_a_h;
    delete[] src_b_h;
    delete[] res_h;
    delete[] check;
    clReleaseKernel(vector_add_k);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    clReleaseMemObject(src_a_d);
    clReleaseMemObject(src_b_d);
    clReleaseMemObject(res_d);

    // signal ready state
    ready();

    // the number of iterations
    unsigned int iterations = 1;

    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        // Now, the moduleState variable comes into play. The module can wait for the condition, which gets fired whenever the input receives data
        // or an property changes. The main loop now waits until something happens.
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish
        if( m_shutdownFlag() )
        {
            break;
        }
        
        // 4. ?
        
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


char* WMOCLgauss::file_contents(const char *filename, int *length)
{
    FILE *f = fopen(filename, "r");
    void *buffer;

    if (!f) {
        fprintf(stderr, "Unable to open %s for reading\n", filename);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    *length = ftell(f);
    fseek(f, 0, SEEK_SET);

    buffer = malloc(*length+1);
    *length = fread(buffer, 1, *length, f);
    fclose(f);
    ((char*)buffer)[*length] = '\0';

    return (char*)buffer;
}
