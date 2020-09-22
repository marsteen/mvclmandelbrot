#if defined(cl_khr_fp64)  // Khronos extension available?
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#elif defined(cl_amd_fp64)  // AMD extension available?
#pragma OPENCL EXTENSION cl_amd_fp64 : enable
#endif

__kernel void mandelbrot(__global const double2* kinput,
                         __global int*           koutput,
												 __constant int*         params)
{
  int it;
  double x,y;
  double xc,yc;
  int gid = get_global_id(0);
	int lim = params[0];

  koutput[gid] = 255;
 
  xc = kinput[gid].x;
  yc = kinput[gid].y;

  x = y = (double) 0;

  for (it = 0; it < lim; it++)
  {
    double x2 = x * x;
    double y2 = y * y;
    if (x2+y2 > (double)4)
    {
      koutput[gid] = it + 1; 
      break; // Out!
		}
    double twoxy = (double)2*x*y;
    x = x2 - y2 + xc;
    y = twoxy + yc;
  }
}
