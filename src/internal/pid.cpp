#include "pid.h"
// Copyright © 2002, Stephen Schlueter, All Rights Reserved. https://github.com/stevesch

namespace stevesch
{

// #if defined(DEBUG) || defined(_DEBUG)
#if 0
#define PIDWARN(fnName,dt)	Warn(fnName, dt)
	static int nWarn = 20;
	static inline void Warn(const char* fnName, float dt)
	{
		if ((dt > 10.0f) && (nWarn))
		{
			STrace("%s - too many PID iterations (dt = %5.2f)\n", fnName, dt);
			nWarn--;
			if (!nWarn)
			{
				STrace("[ warning count limit reached - no more PID warnings will be displayed ]\n");
			}
		}
	}
#else
#define PIDWARN(fnName,dt)
#endif


	void Pid::stabilize(pidSimpleFn fn, float dtLimit,
						   float dt)
	{
		PIDWARN("Pid::stabilize()", dt);
		
		while (dt > dtLimit) 
		{
			(this->*fn)(dtLimit);
			dt -= dtLimit;
		}
		if (dt > 0.0f)
		{
			(this->*fn)(dt);
		}
	}


	void Pid::stabilizeClamp(pidClampFn fn, float dtLimit,
								float dt, float clamp)
	{
		PIDWARN("Pid::stabilizeClamp()", dt);
		
		while (dt > dtLimit) 
		{
			(this->*fn)(dtLimit, clamp);
			dt -= dtLimit;
		}
		if (dt > 0.0f)
		{
			(this->*fn)(dt, clamp);
		}
	}


	int Pid::stabilizeSticky(pidStickyFn fn, float dtLimit,
								float dt, float xthreshold, float vthreshold)
	{
		int bStationary = 0;

		PIDWARN("Pid::stabilizeSticky()", dt);

		while (dt > dtLimit) 
		{
	//		bStationary = (this->*fn)(dtLimit, xthreshold, vthreshold);
	//		if (bStationary) return bStationary;
			bStationary |= (this->*fn)(dtLimit, xthreshold, vthreshold);

			dt -= dtLimit;
		}
		if (dt > 0.0f)
		{
	//		bStationary = (this->*fn)(dt, xthreshold, vthreshold);
			bStationary |= (this->*fn)(dt, xthreshold, vthreshold);
		}

		return bStationary;
	}


	int Pid::stabilizeClampSticky(pidClampStickyFn fn, float dtLimit,
									 float dt, float clamp, float xthreshold, float vthreshold)
	{
		int bStationary = 0;

		PIDWARN("Pid::stabilizeClampSticky()", dt);

		while (dt > dtLimit) 
		{
	//		bStationary = (this->*fn)(dtLimit, clamp, xthreshold, vthreshold);
	//		if (bStationary) return bStationary;
			bStationary |= (this->*fn)(dtLimit, clamp, xthreshold, vthreshold);

			dt -= dtLimit;
		}
		if (dt > 0.0f)
		{
	//		bStationary = (this->*fn)(dt, clamp, xthreshold, vthreshold);
			bStationary |= (this->*fn)(dt, clamp, xthreshold, vthreshold);
		}

		return bStationary;
	}


	int Pid::advanceClampStickyInt (float dt, float clamp, float xthreshold, float vthreshold)
	{
		float s = eq - x;
		float dvdt;
		
		if ( (fabsf(s) < xthreshold) && (fabsf(v) < vthreshold) )
		{
			x = eq;
			return 0;
		}

		dvdt = (s*a - v*b + i*c);

		v += dt*dvdt;
		if (v > clamp)
			v = clamp;
		else if (v < -clamp)
			v = -clamp;

		i += dt*s;
		x += dt*v;
		
		return 1;
	}


}
