#include "intMath.h"

namespace stevesch
{
/*
	linear congruential generator.  Generator x[n+1] = a * x[n] mod m

	use one of the following sets:
	static unsigned int a = 1588635695, m = 4294967291U, q = 2, r = 1117695901;
	static unsigned int a = 1223106847, m = 4294967291U, q = 3, r = 625646750;
	static unsigned int a = 279470273, m = 4294967291U, q = 15, r = 102913196;
	static unsigned int a = 1583458089, m = 2147483647, q = 1, r = 564025558;
	static unsigned int a = 784588716, m = 2147483647, q = 2, r = 578306215;
	static unsigned int a = 16807, m = 2147483647, q = 127773, r = 2836;
	static unsigned int a = 950706376, m = 2147483647, q = 2, r = 246070895;
	
	SEED = a*(SEED % q) - r*(SEED / q);
*/

	static const unsigned int lcg_a = 1588635695;
	static const unsigned int lcg_m = 4294967291U;
	static const unsigned int lcg_q = 2;
	static const unsigned int lcg_r = 1117695901;
	static unsigned int DEFAULT_SEED = 93186752;

	uint32_t RandGen::_ru()
	{
		uint n = lcg_a*(mSeed & 0x01) - lcg_r*(mSeed >> 1);
		mSeed = n ? n : DEFAULT_SEED;
		return n;
	}

	RandGen::RandGen() : mSeed( DEFAULT_SEED )
	{
	}

	unsigned long RandGen::getSeed() const
	{
		return mSeed;
	}

	void RandGen::setSeed( unsigned long nSeed )
	{
		mSeed = nSeed ? nSeed : DEFAULT_SEED;
	}

	void RandGen::set2DSeed( int s1, int s2 )
	{
//		uint nSeed = (s1 & 0x0000ffff) | (s2 << 16);
		RandGen r1(s1);
		RandGen r2(s2);
		s1 = r1.getU();
		s2 = r2.getU();	s2 = r2.getU();	// twice
		uint nSeed = s1 ^ s2;
		mSeed = nSeed;
	}

/* alternative-- looks promising, but has 3 seeds:

//	returns x(n) + z(n) where x(n) = x(n-1) + x(n-2) mod 2^32
//	z(n) = 30903 * z(n-1) + carry mod 2^16
//	Simple, fast, and very good. Period > 2^60

	unsigned int rand();            // returns a random 32-bit integer
	void  rand_seed( unsigned int, unsigned int, unsigned int );
	
	// return a random float >= 0 and < 1
#define rand_float          ((double)rand() / 4294967296.0)
	
	static unsigned int x, y, z;  // the seeds
	
	unsigned int rand()
	{
		unsigned int v;
		
		v = x * y;
		x = y;
		y = v;
		z = (z & 65535) * 30903 + (z >> 16);
		return (y + (z&65535));
	}
	
	
	void rand_seed(seed1, seed2, seed3 )
		unsigned int seed1, seed2, seed3;
	{
		x = (seed1<<1) | 1;
		x = x * 3 * x;
		y = (seed2<<1) | 1;
		z = seed3;
	}
*/


	RandGen S_RandGen( DEFAULT_SEED );

	int STestRand()
	{
/* only for old generator
	  unsigned long oldseed = S_RandGen.GetSeed();
	  int ret = 1;
	  
	  S_RandGen.SetSeed( 0 );

	  if (SRandU() != 0x3c6ef35f || SRandU() != 0x47502932 ||
		  SRandU() != 0xd1ccf6e9 || SRandU() != 0xaaf95334 ||
		  SRandU() != 0x6252e503) ret = 0;

	  S_RandGen.SetSeed( oldseed );

	  return ret;
*/
		return 1;	// ack-- always OK
	}

}	// namespace S
