#include <Arduino.h>
// This is a test example for MathBase

#include <stevesch-MathBase.h>

using stevesch::RandGen;
using stevesch::Histogram;

void testRandomNumbers();

void setup()
{
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Setup initializing...");

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
