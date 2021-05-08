#include <Arduino.h>
// This is a test example for MathBase

#include <stevesch-MathBase.h>

using stevesch::RandGen;
using stevesch::Histogram;

void testNumerics();
void testFloatTiming();
void testRmsError_rsqrtfApprox();
void testRandomNumbers();

void setup()
{
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Setup initializing...");

  testNumerics();
  testFloatTiming();
  testRmsError_rsqrtfApprox();
  testRandomNumbers();

  Serial.println("Setup complete.");
}

void loop()
{
  const long kLogInterval = 5000;
  static long nextLog = kLogInterval;
  long now = millis();
  if (now >= nextLog) {
    nextLog += kLogInterval;
    Serial.println("Reset to re-run tests/demo");
  }
}


void testNumerics()
{
  float a = -5.0f;
  float b = 5.0f;
  float step = (b - a) / 20.0f;
  Serial.printf("    x  ftoi round cl(-2.5,1.3)  mod2pi\n");
  for (float x=a; x<=b; x += step)
  {
    Serial.printf("%5.2f   %3d   %3d        %5.2f   %5.2f\n",
      x,
      stevesch::ftoi(x),
      stevesch::roundftoi(x),
      stevesch::clampf(x, -2.5f, 1.3f),
      stevesch::closeMod2pi(x));
  }
}


void testRandomNumbers()
{
  const int kGraphHeight = 10;
  const int kGraphWidth = 80;

  const int kSamplesPerGraph = 10000;

  RandGen r(micros());

  Serial.println();
  // when graphing random ints, generate them so there will be (or should be) some
  // empty bins to the left and right of generated range:
  const int kGraphEmptySlots = 4;
  int xa = kGraphEmptySlots; // lower bound of generated ints
  int xb = kGraphWidth - kGraphEmptySlots; // upper bound (max + 1) of generated ints
  Serial.printf("Distribution of %d random ints [%d, %d]:", kSamplesPerGraph, xa, xb);
  Serial.println();
  {
    Histogram h(-0.5f, (float)kGraphWidth - 0.5f, kGraphWidth);
    for (int i=0; i<kSamplesPerGraph; ++i) {
      int vi = xa + r.getInt(xb - xa);
      h.add((float)vi);
      if (i < 10) {
        Serial.printf("  %d", vi);
      }
    }
    Serial.println(". . .");
    h.log(Serial, kGraphHeight);
  }

  Serial.println();
  Serial.printf("Distribution of %d random floats [0.0, 1.0):", kSamplesPerGraph);
  Serial.println();
  {
    Histogram h(-5.0f, 5.0f, kGraphWidth);
    for (int i=0; i<kSamplesPerGraph; ++i) {
      float vf = r.getFloat();
      h.add(vf);
      if (i < 10) {
        Serial.printf("  %5.3f", vf);
      }
    }
    Serial.println("");
    h.log(Serial, kGraphHeight);
  }

  Serial.println();
  Serial.printf("Distribution of %d random floats in [-3.0f, 3.0f]:", kSamplesPerGraph);
  Serial.println();
  {
    Histogram h(-5.0f, 5.0f, kGraphWidth);
    for (int i=0; i<kSamplesPerGraph; ++i) {
      float vf = r.getFloatAB(-3.0f, 3.0f);
      h.add(vf);
      if (i < 10) {
        Serial.printf("  %5.3f", vf);
      }
    }
    Serial.println("");
    h.log(Serial, kGraphHeight);
  }
}

float timeFloatFn(float(*fn)(float), float a, float b, int testCount)
{
  long t0 = micros();
  RandGen r((uint32_t)t0);

  for (int i=0; i<testCount; ++i) {
    float x = r.getFloatAB(a, b);
    volatile float y = fn(x);
    y; // circumvent compiler warning about unused var
  }

  long t1 = micros();
  long tdif = t1 - t0;
  float invCount = 1.0f / testCount;
  float usecondsPerCall = tdif * invCount;
  return usecondsPerCall;
}

void testFloatTiming()
{
  float a = 0.0f;
  float b = 1.0e+6f;
  int testCount = 100000;

  float time0 = timeFloatFn([](float x) { return x; }, a, b, testCount);
  float time1 = timeFloatFn(sqrtf, a, b, testCount);
  float time2 = timeFloatFn([](float x) { return sqrtf(x); }, a, b, testCount);

  a = 0.00001f;
  float time3 = timeFloatFn([](float x) { return 1.0f / sqrtf(x); }, a, b, testCount);
  float time4 = timeFloatFn([](float x) { return powf(x, -0.5f); }, a, b, testCount);
  float time5 = timeFloatFn(stevesch::rsqrtfApprox, a, b, testCount);

  a = 0.001f; b = 1.0e+6f;
  float time6 = timeFloatFn([](float x) { return 1.0f / x; }, a, b, testCount);
  a = -1.0e+6f; b = -0.001f;
  float time7 = timeFloatFn([](float x) { return 1.0f / x; }, a, b, testCount);

  Serial.printf("Function costs (%d tests each):\n", testCount);
  Serial.printf("nop(x):            %5.2f us\n", time0);
  Serial.printf("sqrtf:             %5.2f us\n", time1);
  Serial.printf("sqrtf(x):          %5.2f us\n", time2);
  Serial.printf("1.0f / sqrtf(x):   %5.2f us\n", time3);
  Serial.printf("powf(x, -0.5f):    %5.2f us\n", time4);
  Serial.printf("rsqrtfApprox:      %5.2f us\n", time5);
  Serial.printf("1/x (x>0):         %5.2f us\n", time6);
  Serial.printf("1/x (x<0):         %5.2f us\n", time7);
}

void testRmsError_rsqrtfApprox()
{
  long t0 = micros();

  float a = 0.00001f;
  float b = 1.0e+6f;
  int testCount = 100000;
  double sumOfSquares = 0.0f;
  double maxErrorFactor = 0.0f;

  RandGen r((uint32_t)t0);
  for (int i=0; i<testCount; ++i) {
    float x = r.getFloatAB(a, b);
    double y0 = 1.0 / sqrt((double)x);
    double y1 = (double)stevesch::rsqrtfApprox(x);
    double e = y1 - y0;
    double factor = e / y0;  // error as a factor of the baseline value
    maxErrorFactor = std::max(maxErrorFactor, fabs(factor));
    sumOfSquares += factor*factor;
  }

  double ermsPct = 100.0f * sqrt(sumOfSquares / testCount);
  double maxPct = 100.0f * maxErrorFactor;
  Serial.printf("rsqrtfApprox RMS error: %8.6f %%  max: %8.6f %%\n", ermsPct, maxPct);
}
