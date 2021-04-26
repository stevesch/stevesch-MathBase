#ifndef STEVESCH_MATHBASE_INTERNAL_HISTOGRAM_H_
#define STEVESCH_MATHBASE_INTERNAL_HISTOGRAM_H_
#include <cstdint>
#include <utility>
#include <math.h>
#include "intMath.h"

class Stream;

namespace stevesch {

typedef std::pair<float, float> floatRange_t;

// quantize x in range [a, b] such that [a, b) is divided
// into N (==numDivisions) intervals, where each interval
// is closed at the start and open at the end, i.e. bins are
// i0 = [ (a + 0*(b-a)/N, (a + 1*(b-a)/N )
// i1 = [ (a + 1*(b-a)/N, (a + 2*(b-a)/N )
// i2 = [ (a + 2*(b-a)/N, (a + 3*(b-a)/N )
// . . .
// In addition, x<a yields i0 (first bin), and x>=b yields i[N-1] (last bin)
int quantize(float x, float a, float b, int numDivisions);

// given the quantization rules for quantize(), above, return the
// interval for a particular bin.
floatRange_t quantizationRange(int bin, float a, float b, int numDivisions);

class Histogram
{
public:
  typedef std::pair<int32_t, int32_t> countRange_t;

  Histogram(float a, float b, uint32_t binCount);
  ~Histogram();

  void clear();

  uint32_t getBinNumber(float value) const
  {
    float a = mBegin;
    // float b = mEnd;
    // float x = (value - a) / (b - a);
    // int n = (int)(x * mBinCount);
    int n = (int)floorf((value - a) * mPerBinInv);
    int binMax = (int)mBinCount - 1;
    n = clampT(n, 0, binMax);
    return (uint32_t)n;
  }

  float getBinCenter(uint32_t binNumber) const {
    // float a = mBegin;
    // float b = mEnd;
    // float perBin = (b - a) / mBinCount;
    float binCenter = 0.5f * mPerBin*(binNumber + 1);
    return binCenter;
  }

  int32_t getBinContents(uint32_t binNumber) const {
    if (binNumber >= mBinCount) {
      return 0;
    }
    return mBin[binNumber];
  }

  int32_t add(float value, int32_t amount=1) {
    uint32_t n = getBinNumber(value);
    uint32_t count = mBin[n] + amount;
    mBin[n] = count;
    return count;
  }

  int32_t get(float value) const {
    uint32_t n = getBinNumber(value);
    return mBin[n];
  }

  // min and max values of all bins
  // (inclusive max-- countMin <= <all values> <= countMax)
  countRange_t getRange() const
  {
    int32_t countMin = 0;
    int32_t countMax = 0;
    unsigned int binCount = mBinCount;
    if (binCount > 0) {
      countMin = countMax = mBin[0];
      for (unsigned int i=1; i<binCount; ++i) {
        int32_t count = mBin[i];
        countMin = (count < countMin) ? count : countMin;
        countMax = (count > countMax) ? count : countMax;
      }
    }

    return countRange_t(countMin, countMax);
  }

  void log(Stream& out, uint32_t height);

protected:
  int32_t* mBin;
  uint32_t mBinCount;
  float mBegin;
  float mEnd;
  float mPerBin; // == (mEnd - mBegin) / mBinCount
  float mPerBinInv; // == (float)mBinCount / (mEnd - mBegin)
};

}

#endif
