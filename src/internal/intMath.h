#ifndef INCLUDE_STEVESCH_MATHBASE_INTERNAL_INTMATH_H_
#define INCLUDE_STEVESCH_MATHBASE_INTERNAL_INTMATH_H_

#include "mathBase.h"

namespace stevesch
{
	inline int countBits(uint32_t u)
	{
		int iBits;
		for (iBits=0; u; u &= u-1 )
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
		uint32_t uHighestBit = 0x80000000U;	// (((uint32_t)-1) & ~(((uint32_t)-1) >> 1));
		
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

  inline int ftoi(float f)  { return (int)f; }
  inline unsigned int ftou(float f) { return (unsigned int)f; }

	//////////////////////////////////////////////////////////////////////
	// random integer numbers

	// pseudo-random number generator class
	class RandGen
	{
	private:
		unsigned long mSeed;


		uint32_t _ru();	// return random 32-bit unsigned int

		inline int _ri( int nRange )
		{
			double a = ((double)nRange) / 4294967296.0;	// == n / (double)0xffffffff
			return (int)( double(_ru()) * a);
		}

		inline float _rf()
		{
			return ( (float)_ru() / ((float)0xffffffffU) );
		}

	public:
		RandGen();
		RandGen( unsigned long nSeed ) : mSeed(nSeed)	{}
		RandGen( int s1, int s2 )	// 2D seed (16 bits each guaranteed)
		{
			set2DSeed( s1, s2 );
		}
		unsigned long getSeed() const;
		void setSeed( unsigned long nSeed );

		void set2DSeed( int s1, int s2 );	// 2D seed (16 bits each guaranteed)

		// return next 32 bit random number. this uses a not-very-random linear congruential method.
		inline uint getU()			{ return _ru(); }

		// return a random integer between 0..n-1. the distribution will get worse as n approaches 2^32.
		inline int getInt(int nRange)	{ return _ri(nRange); }

		inline float getFloat()		{ return _rf(); }
		inline float getFloatAB( float a, float b )
		{
//		return lerpf( a, b, _rf() );
			return a + _rf()*(b - a);
		}
	};

	////////////////////////////////////////////////////////////////////////

	extern RandGen	S_RandGen;

	// return next 32 bit random number. this uses a not-very-random linear
	// congruential method.
	inline uint32_t SRandU()							{ return S_RandGen.getU(); }
	
	// get the current random number seed
	inline unsigned long SRandGetSeed()			{ return S_RandGen.getSeed(); }
	
	// set the current random number seed
	inline void SRandSetSeed(unsigned long n)		{ S_RandGen.setSeed(n); }

	// return a random integer between 0..n-1. the distribution will get worse
	// as n approaches 2^32.
	inline int SRandInt( int n )					{ return S_RandGen.getInt( n ); }

	// return 1 if the random number generator is working
	int STestRand();

	//////////////////////////////////////////////////////////////////////

}	// namespace S

#endif	// _INCLUDE_SINTMATH_H_
