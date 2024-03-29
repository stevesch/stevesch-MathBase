#ifndef STEVESCH_MATHBASE_INTERNAL_SCALAR_H_
#define STEVESCH_MATHBASE_INTERNAL_SCALAR_H_
// Scalar floating-point library header
//
// Copyright © 2002, Stephen Schlueter, All Rights Reserved. https://github.com/stevesch
//
// History:
//	Created:	4/30/2002, Stephen Schlueter
//	Modified:

#include <math.h>
#include <type_traits>
#include <limits>
#include <functional>

#include "mathBase.h"
#include "intMath.h"

namespace stevesch
{
  constexpr float c_fpi = 3.1415926535897932384626433832795029f;         // pi
  constexpr float c_f2pi = 6.28318530717958647692f;                      // 2*pi
  constexpr float c_f3pi = 9.42477796077f;                               // 3*pi
  constexpr float c_fpi_2 = 1.57079632679489661923f;                     // pi/2
  constexpr float c_fpi_3 = 1.0471975512f;                               // pi/3
  constexpr float c_fpi_4 = 0.78539816339744830962f;                     // pi/4
  constexpr float c_fSqrt2 = 1.414213562373f;                            // sqrt(2)
  constexpr float c_fRecipSqrt2 = 0.7071067811865475244008443621048490f; // 1/sqrt(2)
  constexpr float c_fSqrt3 = 1.732050807569f;                            // sqrt(3)
  constexpr float c_fRecipSqrt3 = 0.5773502691896f;                      // 1/sqrt(3)
  constexpr float c_fSqrt3_2 = 0.8660254037844f;                         // sqrt(3) / 2

  constexpr float floatInfinity = 1e+20f;
  //constexpr float SFloatInfinity	= std::numeric_limits<float>::infinity();

  inline float maxf(float a, float b) { return std::max(a, b); }
  inline float minf(float a, float b) { return std::min(a, b); }

  // round to nearest integer ( +0.6 -> +1.0;  -0.6 -> -1.0
  inline int roundftoi(float fValue)
  {
    return ftoi((fValue >= 0) ? floorf(fValue + 0.5f) : ceil(fValue - 0.5f));
  }

  int statisticalRoundftoi(float fValue); // round from float to int statistically (1.2 has 20% chance of returning '2')

  inline float recipf(float x) { return (1.0f / x); }
  inline float rsqrtf(float x) { return 1.0f / sqrtf(x); }
  inline float rsqrtfApprox(float x)
  {
    // See http://en.wikipedia.org/wiki/Fast_inverse_square_root
    // TODO: include <bit> and use std::bit_cast when compilers are up to it.
    const float x2 = x * 0.5f;
    const float threehalfs = 1.5f;

    union
    {
      float floatVal;
      uint32_t i;
    } conv = {.floatVal = x};
    conv.i = 0x5f3759df - (conv.i >> 1);
    float float2 = conv.floatVal;
    float2 *= threehalfs - (x2 * float2 * float2);
    return float2;
  }

  // future expansion for simultaneous cosine & sine computation
  // given a single theta value
  inline void cosSinf(float theta, float *pCosf, float *pSinf)
  {
    // SASSERT( 0 != pCosf );
    // SASSERT( 0 != pSinf );
    *pCosf = cosf(theta);
    *pSinf = sinf(theta);
  }

  // i.e. constexpr version for DegToRad, RadToDeg:
  template <class T>
  typename std::enable_if<std::is_arithmetic<T>::value, float>::type inline constexpr degToRad(T &&fDeg) { return fDeg * 0.01745329251994f; }
  template <class T>
  typename std::enable_if<std::is_arithmetic<T>::value, float>::type inline constexpr radToDeg(T &&fRad) { return fRad * 57.29577951308f; }

  // i.e. non-constexpr (regular) version for DegToRad, RadToDeg:
  template <class T>
  typename std::enable_if<!std::is_arithmetic<T>::value, float>::type inline degToRad(T &&fDeg) { return fDeg * 0.01745329251994f; }
  template <class T>
  typename std::enable_if<!std::is_arithmetic<T>::value, float>::type inline radToDeg(T &&fRad) { return fRad * 57.29577951308f; }

  inline float lerpf(float a, float b, float t)
  {
    // (1-t)*a + t*b == a - t*a + t*b == a + t*(b - a)
    return a + t * (b - a);
  }

  // generic lerp, also overloaded in vector2, vector3 classes in stevesch-MathVec:
  inline void lerp(float& dst, float a, float b, float t)
  {
    dst = lerpf(a, b, t);
  }

  // linearly map value from range [a0, b0] to new range [a1, b1]
  // initial range, [a0, b0], must be non-zero in length (a0 != b0)
  inline float remapf(float x0, float a0, float b0, float a1, float b1)
  {
    float t = (x0 - a0) / (b0 - a0);
    float x1 = lerpf(a1, b1, t);
    return x1;
  }

  // linearly map value from range [a0, b0] to new range [a1, b1]
  // if the initial range is zero in length (a0 == b0), map either to
  // - beginning of new range if x0 < value
  // - end of the new range if x0 > value
  // - midpoint of new range if x == value
  float safeRemapf(float x0, float a0, float b0, float a1, float b1);

  // random number (0.0, 1.0)
  inline float randf()
  {
    return S_RandGen.getFloat();
    //		return ( ((float)rand()) / RAND_MAX );
    //		return ( (float)SRandU() / ((float)0xffffffffU) );
  }

  // random number (a, b)
  inline float randfAB(float a, float b)
  {
    return S_RandGen.getFloatAB(a, b);
    //		return Lerpf( a, b, Randf() );
  }

  // @return: value such that fMin <= fTest <= fMax
  inline float clampf(float fTest, float fMin, float fMax)
  {
    // templated version doesn't always optimize well yet, so:
    if (fTest < fMin) {
      return fMin;
    }
    if (fTest > fMax) {
      return fMax;
    }
    return fTest;
  }

  // until C++17 std::clamp is available:
  template <typename T>
  inline const T& clamp(const T& x, const T& a, const T& b) {
    if (std::less<T>{}(x, a)) {
      return a;
    }
    if (std::less<T>{}(b, x)) {
      return b;
    }
    return x;
  }

  inline void swapf(float &f1, float &f2)
  {
    float fTemp = f1;
    f1 = f2;
    f2 = fTemp;
  }

  // wrap value to [0.0, 1.0)
  inline float wrapUnit(float value)
  {
    float wh;
    float y = modff(value, &wh);
    if (y < 0.0f)
    {
      y += 1.0f;
    }
    return y;
  }

  // Mod to -pi<=x<=pi where x is no more than +/- 2*pi from the range
  inline float closeMod2pi(float x)
  {
    // SASSERT( (x >= -c_f3pi) && (x <= c_f3pi) );

    if (x > c_fpi)
      return (x - c_f2pi);
    else if (x < -c_fpi)
      return (x + c_f2pi);
    return x;
  }

  inline float mod2pi(float x)
  {
    x = fmodf(x, c_f2pi);
    return closeMod2pi(x);
  }

  // takes a value, -1.0 <= t <= 1.0, zeros values of r where
  // -deadzone < t < deadzone, and rescales t as follows:
  //
  // [-1.0, -deadzone] ---> [-1.0, 0.0]
  // [+deadzone, +1.0] ---> [0.0, +1.0]
  //
  // This effectively "chops out" the range of values
  // near 0.0 (within the dead zone)
  inline float zeroDeadZone(float t, float deadzone)
  {
    // remove dead-zone
    if (t > 0.0f)
    {
      t = (t - deadzone) / (1.0f - deadzone);
      t = maxf(t, 0.0f);
    }
    else
    {
      t = (t + deadzone) / (1.0f - deadzone);
      t = minf(t, 0.0f);
    }

    return t;
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
  // returns 'true' if <x, y> is within dead-zone (output x=y=0)
  bool zeroDeadZonePolar(float &x, float &y, float deadzone);

  // linearly interpolate between two integer values and return the nearest integer to that result
  inline int lerpInt(int a, int b, float t)
  {
    float fa = (float)a;
    float fb = (float)b;
    float f = lerpf(fa, fb, t);
    f = (fa < fb) ? clampf(f, fa, fb) : clampf(f, fb, fa);
    int i = roundftoi(f);
    return i;
  }
}
#endif
