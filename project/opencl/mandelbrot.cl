


__kernel void mandelbrot(__global const float2* kinput,
                         __global int*          koutput,
												 __constant int*        params)
{
  int it;
  float x,y;
  float xc,yc;
  int gid = get_global_id(0);
	int lim = params[0];

  koutput[gid] = 255;
 
  xc = kinput[gid].x;
  yc = kinput[gid].y;

  x = y = (float) 0;

  for (it = 0; it < lim; it++)
  {
    float x2 = x * x;
    float y2 = y * y;
    if (x2+y2 > (float)4)
    {
      koutput[gid] = it + 1; 
      break; // Out!
		}
    float twoxy = (float)2*x*y;
    x = x2 - y2 + xc;
    y = twoxy + yc;
  }
}
