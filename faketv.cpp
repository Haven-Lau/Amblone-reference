// Amblone code: Implementation file of the TV colour simulator
// Author: Bart van der Drift

// License:
// Anyone is free to change, redistribute or copy parts of this code
// as long as it is not for commercial purposes / monetary gain
// Please be so kind to pay credit where due

#include "stdafx.h"
#include "faketv.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

TvPeriod::TvPeriod() {
  // Make a new random period
  int maxt = 20 + rand() % 50;
  times[0] = 0;
  times[1] = 0.1 + (rand() % maxt) / 10;
  times[2] = times[1] + 0.1 + (rand() % maxt) / 10;
  times[3] = times[2] + 0.1 + (rand() % maxt) / 10;
  COLORREF RandRGB = RGB(50 + rand() % 205, 50 + rand() % 205, 50 + rand() % 205);
  rgbT[0] = RandRGB;
  rgbT[1] = RandRGB;
  RandRGB = RGB(50 + rand() % 205, 50 + rand() % 205, 50 + rand() % 205);
  rgbT[2] = RandRGB;
  rgbT[3] = RandRGB;
}

FakeTV::FakeTV() {
  srand(time(NULL));
  pPeriod = NULL;
  etime = 0;
}

COLORREF FakeTV::GetColour() {
  return rgbCur;
}

void FakeTV::Tick(int fps) {
  // Add the elapsed time
  etime += 1 / (float) fps;

  // Check if we need to create a new period
  if (pPeriod == NULL || etime > pPeriod->times[3]) {
    if (!pPeriod) pPeriod = new TvPeriod();
    else {
      if (rand() % 4) { // Don't create new period 1/4 of the time (repeat)
        delete pPeriod;
        pPeriod = new TvPeriod();
      }
    }
    etime = 0;
  }

  // Now get the colour
  if (etime < pPeriod->times[1]) {
    float p1 = (pPeriod->times[1] - etime) / (pPeriod->times[1] - pPeriod->times[0]);
    float p2 = 1 - p1;
    rgbCur = RGB((int) (p1 * (float) GetRValue(pPeriod->rgbT[0])) + (int) (p2 * (float) GetRValue(pPeriod->rgbT[1])),
                 (int) (p1 * (float) GetGValue(pPeriod->rgbT[0])) + (int) (p2 * (float) GetGValue(pPeriod->rgbT[1])),
                 (int) (p1 * (float) GetBValue(pPeriod->rgbT[0])) + (int) (p2 * (float) GetBValue(pPeriod->rgbT[1])));
  }
  else if (etime < pPeriod->times[2]) {
    float p1 = (pPeriod->times[2] - etime) / (pPeriod->times[2] - pPeriod->times[1]);
    float p2 = 1 - p1;
    rgbCur = RGB((int) (p1 * (float) GetRValue(pPeriod->rgbT[1])) + (int) (p2 * (float) GetRValue(pPeriod->rgbT[2])),
                 (int) (p1 * (float) GetGValue(pPeriod->rgbT[1])) + (int) (p2 * (float) GetGValue(pPeriod->rgbT[2])),
                 (int) (p1 * (float) GetBValue(pPeriod->rgbT[1])) + (int) (p2 * (float) GetBValue(pPeriod->rgbT[2])));
  }
  else {
    float p1 = (pPeriod->times[3] - etime) / (pPeriod->times[3] - pPeriod->times[2]);
    float p2 = 1 - p1;
    rgbCur = RGB((int) (p1 * (float) GetRValue(pPeriod->rgbT[2])) + (int) (p2 * (float) GetRValue(pPeriod->rgbT[3])),
                 (int) (p1 * (float) GetGValue(pPeriod->rgbT[2])) + (int) (p2 * (float) GetGValue(pPeriod->rgbT[3])),
                 (int) (p1 * (float) GetBValue(pPeriod->rgbT[2])) + (int) (p2 * (float) GetBValue(pPeriod->rgbT[3])));
  }
}
