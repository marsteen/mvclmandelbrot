//---------------------------------------------------------------------------
//
// Klasse:
// Methode:
//
//
//---------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <COpenCL.h>
#include <NStringTool.h>

using namespace std;

typedef clvec<double, 2>	clvect2d;
typedef clvec<float, 2>		clvect2f;


extern void WriteTga(const char* Filename, const int* Data, int w, int h);

//---------------------------------------------------------------------------
//
// Klasse:
// Methode:
//
//
//---------------------------------------------------------------------------
//
// Fuellen des Eingabefeldes (input) mit Werten
//

template<typename Tfloat>
static void SetRect(clvec<Tfloat, 2>* input, int mdim, double x1, double y1, double x2, double y2)
{
    clvec<Tfloat, 2>* inputPtr = input;
    double stepx = (x2 - x1) / mdim;
    double stepy = (y2 - y1) / mdim;

    Tfloat sy = y1;

    for (int y = 0; y < mdim; y++)
    {
        double sx = x1;

        for (int x = 0; x < mdim; x++)
        {
            inputPtr->v[0] = sx;
            inputPtr->v[1] = sy;
            inputPtr++;
            sx += stepx;
        }
        sy += stepy;
    }
}


//---------------------------------------------------------------------------
//
// Klasse:
// Methode:
//
//
//---------------------------------------------------------------------------

template<typename Tfloat>
void CalcMandelbrot(const char* kernelFile, int mdim, int flimit, double x1, double y1, double x2, double y2)
{
    typedef clvec<Tfloat, 2> clvect2;
    COpenCL<clvect2, int>* opencl = new COpenCL<clvect2, int>;

    int msize = mdim * mdim;
    clvect2* input = new clvect2[msize];
    int* output = new int[msize];
    int nKernels = mdim * sizeof(clvect2);

    cout << "CalcMandelbrot START msize=" << msize << endl;


    //  SetRect(input, mdim, -2.0, -1.5, 1.0, 1.5);
    //  SetRect(input, -1.4797315064831544,	-0.00044288606420912107,	-1.4797314376239232,	-0.00044281720497790561);
    //  SetRect(input,    -1.4797253558889452,	-0.00045266823155998281,	-1.4797228558584266,	-0.00045016820104137878);
    
    SetRect(input, mdim, x1, y1, x2, y2);

    cout << "SetRect OK" << endl;



    try
    {
        if (sizeof(Tfloat) > 4)
        {
            cout << "precision=DOUBLE" << endl;
            opencl->SetGlobalParam(0, flimit);
            opencl->Init(kernelFile, input, msize, output, msize);
        }
        else
        {
            cout << "precision=FLOAT" << endl;
            opencl->SetGlobalParam(0, flimit);
            opencl->Init(kernelFile, input, msize, output, msize);
        }


        //
        // PASS 1:
        //
        cout << "pass 1" << endl;


        opencl->Execute(nKernels);

        cout << "ok" << endl;
        opencl->ReadBuffer();
    }
    catch (CException ex)
    {
        cout << "***** Error in: " << ex.mErrstr << " "  << ex.mErrnum << endl;
    }


    // Output the result buffer

    cout << "PASS ok." << endl;

    WriteTga("mandelbrot.tga", output, mdim, mdim);
    opencl->CleanUp();
    delete[] output;
    delete[] input;
    delete opencl;
}


enum
{
    FPREC_FLOAT,
    FPREC_DOUBLE
};


//---------------------------------------------------------------------------
//
// Klasse:
// Methode:
//
//
//---------------------------------------------------------------------------
#define MDIM    1024



int main(int argc, char** argv)
{
    cout << "clmandelbrot version 2.0" << endl;
    cout << "options:" << endl;
    cout << "         -file <kernelfile.cl>" << endl;
    cout << "         -double   = double precision" << endl;
    cout << "         -float    = single precision (default)" << endl;
    cout << "         -res [x]  = resolution" << endl;
    cout << "         -lim [x]  = iteration limit" << endl;
    cout << "         -rect [x1,y1,x2,y2]" << endl;
    cout << "argc=" << argc << endl;

    int nKernels = 1024;
    int prec = FPREC_FLOAT;
    int mdim = 1024;
    int flimit = 255;   // Anzahl der Interationen fuer Mandelbrot
    double x1 = -2.0;
    double y1 = -1.5;
    double x2 = 1.0;
    double y2 = 1.5;
    const char* kernelFile;

    for (int i = 1; i < argc; i++)
    {
        std::string cmd = argv[i];
        cout << "cmd=" << cmd << endl;

        if (cmd == "-file")
        {
            kernelFile = argv[i + 1];
            i += 1;
        }
        if (cmd == "-kernels")
        {
            if (i < argc-1)
            {
                nKernels = NStringTool::Cast<int>(argv[i + 1]);
                i += 1;
            }
        }
        else
        if (cmd == "-float")
        {
            prec = FPREC_FLOAT;
        }
        else
        if (cmd == "-double")
        {
            prec = FPREC_DOUBLE;
        }
        else
        if (cmd == "-res")
        {
            if (i < argc-1)
            {
                mdim = NStringTool::Cast<int>(argv[i + 1]);
                i += 1;
            }
        }
        else
        if (cmd == "-lim")
        {
            if (i < argc-1)
            {
                flimit = NStringTool::Cast<int>(argv[i + 1]);
                i += 1;
            }
        }
        else
        if (cmd == "-rect")
        {
            if (i < argc-1)
            {
                stringvector sp;
                NStringTool::Split(argv[i + 1], &sp, ',');
                x1 = NStringTool::Cast<double>(sp[0]);
                y1 = NStringTool::Cast<double>(sp[1]);
                x2 = NStringTool::Cast<double>(sp[2]);
                y2 = NStringTool::Cast<double>(sp[3]);


                i += 1;
            }
        }
    }


    if (prec == FPREC_FLOAT)
    {
        CalcMandelbrot<float>(kernelFile, mdim, flimit, x1, y1, x2, y2);
    }
    else
    {
        CalcMandelbrot<double>(kernelFile, mdim, flimit, x1, y1, x2, y2);
    }

    return 0;
}
