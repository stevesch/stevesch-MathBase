#ifndef STEVESCH_MATHBASE_INTERNAL_INTMATH_H_
#define STEVESCH_MATHBASE_INTERNAL_INTMATH_H_

#include "mathBase.h"
#include <random>

namespace
{
  template <typename T, unsigned int N>
  char (&_arraySizeOfType(T (&)[N]))[N];
}

namespace stevesch
{
  template <typename T>
  inline size_t sizeOfArray(const T &a) { return sizeof(_arraySizeOfType(a)); }

  inline int countBits(uint32_t u)
  {
    int iBits;
    for (iBits = 0; u; u &= u - 1)
    {
      iBits++;
    }
    return iBits;
  }

  // Bit counter by Ratko Tomic
  /*
	inline int countBits(uint32_t u)
		
	{
		u = ((u & 0xAAAAAAAAU) >>  1) + (u & 0x55555555U);
		u = ((u & 0xCCCCCCCCU) >>  2) + (u & 0x33333333U);
		u = ((u & 0xF0F0F0F0U) >>  4) + (u & 0x0F0F0F0FU);
		u = ((u & 0xFF00FF00U) >>  8) + (u & 0x00FF00FFU);
		u = ((u & 0xFFFF0000U) >> 16) + (u & 0x0000FFFFU);
		return (int)u;
	}
*/

  inline uint32_t highestBit(uint32_t u)
  {
    uint32_t uHighestBit = 0x80000000U; // (((uint32_t)-1) & ~(((uint32_t)-1) >> 1));

    do
    {
      if (uHighestBit & u)
      {
        break;
      }
      uHighestBit >>= 1;
    } while (0 != uHighestBit);

    return uHighestBit;
  }

  //////////////////////////////////////////////////////////////////////

  // TODO: C++17 std::clamp
  template <typename T>
  inline T clampT(const T &x, const T &a, const T &b)
  {
    if (x < a)
    {
      return a;
    }
    if (x > b)
    {
      return b;
    }
    return x;
  }

  inline int ftoi(float f) { return (int)f; }
  inline unsigned int ftou(float f) { return (unsigned int)f; }

  // wrap value to [0, wrap)
  inline int wrapInt(int value, int wrap)
  {
    int newValue = value % wrap;
    if (newValue < 0)
    {
      newValue += wrap;
    }
    return newValue;
  }

  //////////////////////////////////////////////////////////////////////
  // random integer numbers

  // pseudo-random number generator class
  class RandGen
  {
  private:
    std::default_random_engine mGenerator;

  public:
    RandGen();
    RandGen(uint32_t nSeed) : mGenerator(nSeed) {}
    RandGen(uint16_t s1, uint16_t s2) // 2D seed (16 bits each guaranteed)
    {
      set2DSeed(s1, s2);
    }

    void setSeed(uint32_t nSeed);
    void set2DSeed(uint16_t s1, uint16_t s2); // 2D seed (16 bits each guaranteed)

    inline uint32_t getU()
    {
      std::uniform_int_distribution<uint32_t> distribution(0, 4294967295);
      uint32_t value = distribution(mGenerator);
      return value;
    }

    // return a random integer between 0..n-1. the distribution will get worse as n approaches 2^32.
    inline int getInt(int nRange)
    {
      std::uniform_int_distribution<uint32_t> distribution(0, nRange - 1);
      uint32_t value = distribution(mGenerator);
      return value;
    }

    inline float getFloat()
    {
      std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
      float value = distribution(mGenerator);
      return value;
    }

    inline float getFloatAB(float a, float b)
    {
      std::uniform_real_distribution<float> distribution(a, b);
      float value = distribution(mGenerator);
      return value;
    }
  };

  ////////////////////////////////////////////////////////////////////////

  extern RandGen S_RandGen;

  // return next 32 bit random number. this uses a not-very-random linear
  // congruential method.
  inline uint32_t SRandU() { return S_RandGen.getU(); }

  // set the current random number seed
  inline void SRandSetSeed(uint32_t n) { S_RandGen.setSeed(n); }

  // return a random integer between 0..n-1.
  inline int SRandInt(int n) { return S_RandGen.getInt(n); }

  //////////////////////////////////////////////////////////////////////

} // namespace S

#endif
