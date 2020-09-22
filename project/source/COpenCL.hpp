//
// Book:      OpenCL(R) Programming Guide
// Authors:   Aaftab Munshi, Benedict Gaster, Timothy Mattson, James Fung, Dan Ginsburg
// ISBN-10:   0-321-74964-2
// ISBN-13:   978-0-321-74964-2
// Publisher: Addison-Wesley Professional
// URLs:      http://safari.informit.com/9780132488006/
//            http://www.openclprogrammingguide.com
//

// HelloWorld.cpp
//
//    This is a simple example that demonstrates basic OpenCL setup and
//    use.

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include "CException.h"

#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif


///
//  Create an OpenCL context on the first available platform using
//  either a GPU or CPU depending on what is available.
//

template<typename TINP, typename TOUT>
cl_context COpenCL<TINP, TOUT>::CreateContext()
{
    cl_int errNum;
    cl_uint numPlatforms;
    cl_platform_id firstPlatformId;
    cl_context context = NULL;

    // First, select an OpenCL platform to run on.  For this example, we
    // simply choose the first available platform.  Normally, you would
    // query for all available platforms and select the most appropriate one.
    errNum = clGetPlatformIDs(1, &firstPlatformId, &numPlatforms);
    if ((errNum != CL_SUCCESS) || (numPlatforms <= 0))
    {
        std::cerr << "Failed to find any OpenCL platforms." << std::endl;
        return NULL;
    }

    // Next, create an OpenCL context on the platform.  Attempt to
    // create a GPU-based context, and if that fails, try to create
    // a CPU-based context.
    cl_context_properties contextProperties[] =
    {
        CL_CONTEXT_PLATFORM,
        (cl_context_properties)firstPlatformId,
        0
    };

    context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_GPU,
            NULL, NULL, &errNum);
    if (errNum != CL_SUCCESS)
    {
        std::cout << "Could not create GPU context, trying CPU..." << std::endl;
        context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_CPU,
                NULL, NULL, &errNum);
        if (errNum != CL_SUCCESS)
        {
            std::cerr << "Failed to create an OpenCL GPU or CPU context." << std::endl;
            return NULL;
        }
    }

    return context;
}


///
//  Create a command queue on the first device available on the
//  context
//

template<typename TINP, typename TOUT>
cl_command_queue COpenCL<TINP, TOUT>::CreateCommandQueue(cl_context context, cl_device_id* device)
{
    cl_int errNum;
    cl_device_id* devices;
    cl_command_queue commandQueue = NULL;
    size_t deviceBufferSize = -1;

    // First get the size of the devices buffer
    errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &deviceBufferSize);
    if (errNum != CL_SUCCESS)
    {
        std::cerr << "Failed call to clGetContextInfo(...,GL_CONTEXT_DEVICES,...)";
        return NULL;
    }

    if (deviceBufferSize <= 0)
    {
        std::cerr << "No devices available.";
        return NULL;
    }

    // Allocate memory for the devices buffer
    devices = new cl_device_id[deviceBufferSize / sizeof(cl_device_id)];
    errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, deviceBufferSize, devices, NULL);
    if (errNum != CL_SUCCESS)
    {
        delete [] devices;
        std::cerr << "Failed to get device IDs";
        return NULL;
    }

    // In this example, we just choose the first available device.  In a
    // real program, you would likely use all available devices or choose
    // the highest performance device based on OpenCL device queries

#ifdef __APPLE__
    commandQueue = clCreateCommandQueue(context, devices[0], 0, NULL);
#else
    commandQueue = clCreateCommandQueueWithProperties(context, devices[0], 0, NULL);
#endif



    if (commandQueue == NULL)
    {
        delete [] devices;
        std::cerr << "Failed to create commandQueue for device 0";
        return NULL;
    }

    *device = devices[0];
    delete [] devices;
    return commandQueue;
}


//
//  Create an OpenCL program from the kernel source file
//

template<typename TINP, typename TOUT>
cl_program COpenCL<TINP, TOUT>::CreateProgram(cl_context context, cl_device_id device, const char* fileName)
{
    cl_int errNum;
    cl_program program;

    std::ifstream kernelFile(fileName, std::ios::in);

    if (!kernelFile.is_open())
    {
        std::cerr << "Failed to open file for reading: " << fileName << std::endl;
        return NULL;
    }

    std::ostringstream oss;

    oss << kernelFile.rdbuf();

    std::string srcStdStr = oss.str();
    const char* srcStr = srcStdStr.c_str();

    program = clCreateProgramWithSource(context, 1,
            (const char**)&srcStr,
            NULL, NULL);
    if (program == NULL)
    {
        std::cerr << "Failed to create CL program from source." << std::endl;
        return NULL;
    }

    errNum = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (errNum != CL_SUCCESS)
    {
        // Determine the reason for the error
        char buildLog[16384];
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
            sizeof(buildLog), buildLog, NULL);

        std::cerr << "Error in kernel: " << std::endl;
        std::cerr << buildLog;
        clReleaseProgram(program);
        return NULL;
    }

    return program;
}


///
//  Cleanup any created OpenCL resources
//

template<typename TINP, typename TOUT>
void COpenCL<TINP, TOUT>::Cleanup(cl_context context, cl_command_queue commandQueue,
    cl_program program, cl_kernel kernel, cl_mem memObjects[2])
{
    for (int i = 0; i < 2; i++)
    {
        if (memObjects[i] != 0)
        {
            clReleaseMemObject(memObjects[i]);
        }
    }
    if (commandQueue != 0)
    {
        clReleaseCommandQueue(commandQueue);
    }

    if (kernel != 0)
    {
        clReleaseKernel(kernel);
    }

    if (program != 0)
    {
        clReleaseProgram(program);
    }

    if (context != 0)
    {
        clReleaseContext(context);
    }
}


///
//  Create memory objects used as the arguments to the kernel
//  The kernel takes three arguments: result (output), a (input),
//  and b (input)
//

template<typename TINP, typename TOUT>
bool COpenCL<TINP, TOUT>::CreateBuffers()
{
    memObjects[0] = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            sizeof(TINP) * mSizeIn, mInputArray, NULL);

    memObjects[1] = clCreateBuffer(context, CL_MEM_READ_WRITE,
            sizeof(TOUT) * mSizeOut, NULL, NULL);

    memObjects[2] = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            sizeof(int) * 8, mGlobalParams, NULL);



    if ((memObjects[0] == NULL) || (memObjects[1] == NULL))
    {
        std::cerr << "Error creating memory objects." << std::endl;
        return false;
    }

    return true;
}


template<typename TINP, typename TOUT>
void COpenCL<TINP, TOUT>::SetGlobalParam(int index, int value)
{
    mGlobalParams[index] = value;
}


//---------------------------------------------------------------------------
//
// Klasse:    COpenCL
// Methode:   CleanUp
//
//
//---------------------------------------------------------------------------

template<typename TINP, typename TOUT>
void COpenCL<TINP, TOUT>::CleanUp()
{
    Cleanup(context, commandQueue, program, kernel, memObjects);
}


//---------------------------------------------------------------------------
//
// Klasse:    COpenCL
// Methode:   ReadBuffer
//
//
//---------------------------------------------------------------------------

template<typename TINP, typename TOUT>
int COpenCL<TINP, TOUT>::ReadBuffer()
{
    int r = 0;

    errNum = clEnqueueReadBuffer(commandQueue, memObjects[1], CL_TRUE,
            0, mSizeOut * sizeof(TOUT), mOutputArray,
            0, NULL, NULL);
    if (errNum != CL_SUCCESS)
    {
        std::cerr << "Error reading result buffer." << std::endl;
        Cleanup(context, commandQueue, program, kernel, memObjects);
        throw CException(4, __func__);
        r = 1;
    }
    return r;
}


//---------------------------------------------------------------------------
//
// Klasse:    COpenCL
// Methode:   Init
//
//
//---------------------------------------------------------------------------

template<typename TINP, typename TOUT>
bool COpenCL<TINP, TOUT>::Init(const char* KernelFileCL, TINP* inp, int sizein, TOUT* out, int sizeout)
{
    int r = 0;

    context = 0;
    commandQueue = 0;
    program = 0;
    device = 0;
    kernel = 0;

    mInputArray = inp;
    mOutputArray = out;
    mSizeIn = sizein;
    mSizeOut = sizeout;

    // Create an OpenCL context on first available platform
    context = CreateContext();
    if (context == NULL)
    {
        std::cerr << "Failed to create OpenCL context." << std::endl;
        return 1;
    }

    // Create a command-queue on the first device available
    // on the created context
    commandQueue = CreateCommandQueue(context, &device);
    if (commandQueue == NULL)
    {
        Cleanup(context, commandQueue, program, kernel, memObjects);
        throw CException(2, __func__);
        return 2;
    }

    // Create OpenCL program from HelloWorld.cl kernel source
    program = CreateProgram(context, device, KernelFileCL); //"HelloWorld.cl");
    if (program == NULL)
    {
        Cleanup(context, commandQueue, program, kernel, memObjects);
        return 1;
    }

    // Create OpenCL kernel
    kernel = clCreateKernel(program, "mandelbrot", NULL);
    if (kernel == NULL)
    {
        std::cerr << "Failed to create kernel" << std::endl;
        Cleanup(context, commandQueue, program, kernel, memObjects);
        throw CException(3, "clCreateKernel");
        r = 1;
    }

    if (CreateMemObjects())
    {
        throw CException(4, __func__);
        r = 1;
    }


    return r;
}


//---------------------------------------------------------------------------
//
// Klasse:    COpenCL
// Methode:   CreateMemObjects
//
//
//---------------------------------------------------------------------------

template<typename TINP, typename TOUT>
int COpenCL<TINP, TOUT>::CreateMemObjects()
{
    int r = 0;

    if (!CreateBuffers())
    {
        Cleanup(context, commandQueue, program, kernel, memObjects);
        throw CException(2);
        return 1;
    }

    // Set the kernel arguments (result, a, b)
    errNum = clSetKernelArg(kernel, 0, sizeof(cl_mem), &memObjects[0]);
    errNum |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &memObjects[1]);
    errNum |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &memObjects[2]);

    if (errNum != CL_SUCCESS)
    {
        std::cerr << "Error setting kernel arguments." << std::endl;
        Cleanup(context, commandQueue, program, kernel, memObjects);
        throw CException(2, __func__);
        r = 1;
    }
    return r;
}


//---------------------------------------------------------------------------
//
// Klasse:    COpenCL
// Methode:   RefreshInput
//
//
//---------------------------------------------------------------------------

template<typename TINP, typename TOUT>
void COpenCL<TINP, TOUT>::RefreshInput()
{
    clReleaseMemObject(memObjects[0]);

    memObjects[0] = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            sizeof(TINP) * mSizeIn, mInputArray, NULL);


    clSetKernelArg(kernel, 0, sizeof(cl_mem), &memObjects[0]);
}


//---------------------------------------------------------------------------
//
// Klasse:    COpenCL
// Methode:   Execute
//
//
//---------------------------------------------------------------------------

template<typename TINP, typename TOUT>
int COpenCL<TINP, TOUT>::Execute(int nKernels)
{
    int r = 0;
    size_t globalWorkSize[1];
    size_t localWorkSize[1];

    globalWorkSize[0] = mSizeIn;
    localWorkSize[0] = mSizeIn / nKernels;


    std::cout << "Input size=" << mSizeIn << std::endl;
    std::cout << "Kernels   =" << nKernels << std::endl;
    std::cout << "Local size=" << localWorkSize[0] << std::endl;


    // Queue the kernel up for execution across the array

    errNum = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL,
            globalWorkSize, localWorkSize,
            0, NULL, NULL);

    if (errNum != CL_SUCCESS)
    {
        std::cerr << "Error queuing kernel for execution." << std::endl;
        Cleanup(context, commandQueue, program, kernel, memObjects);
        throw CException(3, __func__);
        r = 1;
    }
    return r;
}
