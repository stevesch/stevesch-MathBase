#include "mathBase.h"
#include "histogram.h"

#include <Stream.h>

namespace stevesch {

int quantize(float x, float a, float b, int numDivisions)
{
  // float perBin = (b - a) / numDivisions;
  float perBinInv = (float)numDivisions / (b - a);
  int n = (int)floorf((x - a) * perBinInv);
  int binMax = numDivisions - 1;
  n = clampT(n, 0, binMax);
  return (uint32_t)n;
}

floatRange_t quantizationRange(int bin, float a, float b, int numDivisions)
{
  int binMax = numDivisions - 1;
  int n = clampT(bin, 0, binMax);
  float perBin = (b - a) / numDivisions;
  float bina = a + perBin*n;
  float binb = bina + perBin;
  return floatRange_t(bina, binb);
}

Histogram::Histogram(float a, float b, uint32_t binCount) : mBinCount(binCount), mBegin(a), mEnd(b)
{
  mBin = new int32_t[binCount]{0};
  float perBin = (b - a) / binCount;
  mPerBin = perBin;
  mPerBinInv = 1.0f / perBin;
}

Histogram::~Histogram()
{
  delete[] mBin;
}

void Histogram::clear()
{
  uint32_t numBins = mBinCount;
  for (uint32_t i=0; i<numBins; ++i) {
    mBin[i] = 0;
  }
}

void Histogram::log(Stream& out, uint32_t height)
{
  auto range = getRange();
  int32_t ya = range.first;
  int32_t yb = range.second;
  uint32_t numBins = mBinCount;

  int xBegin = 0;
  int xEnd = (int)numBins;
  int yBegin = (int)height - 1;
  int yEnd = -1;

  out.print("Ymax=");
  out.print(yb);
  out.println();
  for (int yi=yBegin; yi>yEnd; --yi) {
    auto yirange = quantizationRange(yi, (float)(ya - 1), (float)yb, height);
    float yra = yirange.first;
    float yrb = yirange.second;
    float yrc = 0.5f * (yra + yrb);
    for (int xi=xBegin; xi<xEnd; ++xi) {
      int total = getBinContents(xi);
      if (total >= yrb) {
        out.print('*');
      } else if (total >= yrc) {
        out.print('-');
      } else {
        out.print('.');
      }
    }
    out.println();
  }
  out.print("Ymin=");
  out.print(ya);
  out.println();

  out.print("X: [");
  out.print(mBegin);
  out.print(", ");
  out.print(mEnd);
  out.print("]");
  out.println();
}

}
