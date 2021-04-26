#ifndef STEVESCH_MATHBASE_INTERNAL_MATHAPPROX_H_
#define STEVESCH_MATHBASE_INTERNAL_MATHAPPROX_H_

#include "scalar.h"

namespace stevesch
{
	// internal function for SSinApprox (below)
	inline float _sinApprox(float x)
	{
		// assert:  0 <= fT <= PI/2
		// maximum absolute error = 1.6415e-04
		
		float xSqr = x*x;
		float fResult = 7.61e-03f;
		fResult *= xSqr;
		fResult -= 1.6605e-01f;
		fResult *= xSqr;
		fResult += 1.0f;
		fResult *= x;
		return fResult;
	}
	
	
	// sine approximation based on polynomial expansion
	inline float sinApprox(float x)
	{
		x = fmodf(x, c_f2pi);
		if (x < -c_fpi) x += c_f2pi;
		else if (x > c_fpi) x -= c_f2pi;
		
		if (x < 0.0f)
		{
			x = -x;
			if (x > c_fpi_2) x = c_fpi - x;
			return -_sinApprox(x);
		}
		else
		{
			if (x > c_fpi_2) x = c_fpi - x;
			return _sinApprox(x);
		}
	}
	
	
	// internal function for SCosApprox (below)
	inline float _cosApprox(float x)
	{
		// assert:  0 <= fT <= PI/2
		// maximum absolute error = 1.1880e-03
		
		float xSqr = x*x;
		float fResult = 3.705e-02f;
		fResult *= xSqr;
		fResult -= 4.967e-01f;
		fResult *= xSqr;
		fResult += 1.0f;
		return fResult;
	}
	
	// cosine approximation based on polynomial expansion
	inline float cosApprox(float x)
	{
		x = fmodf(x, c_f2pi);
		if (x < 0.0f) x = -x;
		if (x > c_fpi) x -= c_f2pi;
		if (x < 0.0f) x = -x;
		
		if (x > c_fpi_2) return -_cosApprox(c_fpi - x);
		return _cosApprox(x);
	}
	
	
	// parametric sinusoidal interpolation, 0.0f <= t <= 1.0f.
	// maps linear t to smooth sinusoidal curve between 0 and 1
	inline float sinInterp(float t)
	{
		if (t < 0.0f) t = 0.0f;
		else if (t > 1.0f) t = 1.0f;
		return 0.5f * (1.0f - cosf(c_fpi * t));
	}

}

#endif
