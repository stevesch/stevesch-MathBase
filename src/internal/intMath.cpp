#include "intMath.h"

namespace stevesch
{
  // std::uniform_real_distribution<float> dist01(0.0f, 1.0f);
  // std::uniform_real_distribution<float> distN1P1(-1.0f, 1.0f);
  // std::uniform_int_distribution<uint8_t> distu8(0, 255);
  // std::uniform_int_distribution<uint32_t> distu32(0, 4294967295);

	RandGen::RandGen() : mGenerator(micros())
	{
	}

	void RandGen::setSeed( uint32_t nSeed )
	{
    mGenerator = std::default_random_engine(nSeed);
	}

	RandGen S_RandGen(micros());

}	// namespace S
