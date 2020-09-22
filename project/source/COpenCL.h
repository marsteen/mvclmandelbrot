#ifndef COPENCL_H
#define COPENCL_H

#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

#include <cstring>


template<typename T1, int T2>
struct clvec
{
    T1 v[T2];
};

typedef clvec<float, 2>		clvecf2;
typedef clvec<double, 2>	clvecd2;
typedef clvec<int, 2>		clveci2;

typedef clvec<float, 3>		clvecf3;
typedef clvec<double, 3>	clvecd3;
typedef clvec<int, 3>		clveci3;

template<typename TINP, typename TOUT>
class COpenCL
{
    public:

        COpenCL()
        {
            memset(this, 0, sizeof(COpenCL));
        }


        bool Init(const char* KernelFileCL, TINP* inp, int sizein, TOUT* out, int sizeout);
        void SetGlobalParam(int index, int value);

        void RefreshInput();
        int Execute(int nKernels);
        int ReadBuffer();
        void CleanUp();

    protected:

        int CreateMemObjects();
        bool CreateBuffers();

        int mSizeIn;    // Anzahl der Input-Elemente
        int mSizeOut;   // Anzahl der Output-Elemente

        TINP* mInputArray;
        TOUT* mOutputArray;
        int mGlobalParams[8];


        cl_context CreateContext();
        cl_command_queue CreateCommandQueue(cl_context context, cl_device_id* device);
        cl_program CreateProgram(cl_context context, cl_device_id device, const char* fileName);
        void Cleanup(cl_context context, cl_command_queue commandQueue, cl_program program, cl_kernel kernel, cl_mem memObjects[2]);


        cl_context context;
        cl_command_queue commandQueue;
        cl_program program;
        cl_device_id device;
        cl_kernel kernel;
        cl_mem memObjects[3];           // Input, Output und globale Parameter
        cl_int errNum;
};


#include "COpenCL.hpp"

#endif
