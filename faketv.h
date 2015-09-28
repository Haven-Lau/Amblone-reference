// Amblone code: Header file of the TV colour simulator
// Author: Bart van der Drift

// License:
// Anyone is free to change, redistribute or copy parts of this code
// as long as it is not for commercial purposes / monetary gain
// Please be so kind to pay credit where due

#include "Amblone.h"

class TvPeriod {
  public:
    TvPeriod();        // Constructor
    float times[4];    // Defines four points in time (sec)
    COLORREF rgbT[4];  // Colours corresponding to intervals
};

class FakeTV {
  public:
    FakeTV();
    COLORREF GetColour(); // Get the colour of the fake TV
    void Tick(int fps);   // Call this method every program cycle
  private:
    COLORREF rgbCur;      // Current colour
    float etime;          // Elapsed time in the current TvPeriod (sec)
    TvPeriod *pPeriod;    // Pointer to the current TvPeriod
};
