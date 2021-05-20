#include "scalar.h"

namespace stevesch
{
  float safeRemapf(float x0, float a0, float b0, float a1, float b1)
  {
    if (a0 != b0) {
      return remapf(x0, a0, b0, a1, b1);
    }

    if (x0 < a0) {
      return a1;
    }
    if (x0 > a0) {
      return b1;
    }
    return 0.5f*(a1 + b1);
  }

	// converts cartesian <x, y> to polar <r, theta>
	// zeros values of r where r < deadzone, otherwise
	// rescales r as follows:
	//
	// [deadzone, 1.0] ---> [0.0, 1.0]
	//
	// This effectively "chops out" the range of values
	// near 0.0 (within the dead zone)
	// resulting <r, theta> is converted back to <x, y>
	bool zeroDeadZonePolar(float & x, float & y, float deadzone)
	{
		// SASSERT(deadzone <= 1.0f);

		float rr = (x*x + y*y);	// radius squared

		if (rr < deadzone*deadzone) 
		{	
			x = y = 0.0f;
			return true;
		}

		float theta = atan2f(x, y);

		if (rr >= 1.0f)
		{
			rr = 1.0f;
		}
		else
		{
			rr = (sqrtf(rr) - deadzone) / (1.0f - deadzone);
		}
	
		// rr is now radius
		cosSinf( theta, &y, &x );
		x *= rr;
		y *= rr;

		return false;
	}

	
	// round from float to int statistically (1.2 has 20% chance of returning '2')
	int statisticalRoundftoi(float fValue)
	{
		int n = ftoi( fValue );	// truncate
		if (fValue < 0.0f)
		{
			float fFractional = (float)n - fValue;
			if (randf() < fFractional)
			{
				n--;
			}
		}
		else
		{
			float fFractional = fValue - (float)n;
			if (randf() < fFractional)
			{
				n++;
			}
		}
		return n;
	}

}
