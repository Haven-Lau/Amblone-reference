// Amblone code: Settings header file
// Author: Bart van der Drift

// License:
// Anyone is free to change, redistribute or copy parts of this code
// as long as it is not for commercial purposes / monetary gain
// Please be so kind to pay credit where due

#include "Amblone.h"

#define PRECISION_MAX 10

// The screen sections. Need to correspond to the items in the combo boxes
// IT IS IMPORTANT TO KEEP THE ORDER OF THE ITEMS IN THE 
// COMBO BOXES CORRESPONDING WITH THESE VALUES
#define ENTIRE_SCREEN 0
#define UPPER_HALF    1
#define LOWER_HALF    2
#define LEFT_HALF     3
#define RIGHT_HALF    4
#define UPPER_LEFT    5
#define UPPER_RIGHT   6
#define LOWER_LEFT    7
#define LOWER_RIGHT   8
#define STATIC_COLOUR 9
#define SWEEP         10
#define FAKE_TV       11

class AmbloneSettings {
public:
  // Pointer to the main program
  CAmbloneApp *theApp;

  // Constructor
  AmbloneSettings();

  // Com Port
  int GetComPort();
  void SetComPort(int NewValue);

  // Display Device Number
  int GetDisplayDeviceNum();
  void SetDisplayDeviceNum(int NewValue);

  // Average calculation precision
  int GetPrecision();
  void SetPrecision(int NewValue);

  // Average calculation darkness threshold
  int GetThreshold();
  void SetThreshold(int NewValue);

  // The amount of RGB channels
  int GetChannels();
  void SetChannels(int NewValue);

  // The sections of the screen corresponding to the channels
  int GetScreenSection(int Channel);
  void SetScreenSection(int Channel, int NewValue);

  // The static output colours
  COLORREF GetStaticColour(int Channel);
  void SetStaticColour(int Channel, COLORREF NewColour);

  // Skip RGB post processing
  bool GetSkipPostProcessing();
  void SetSkipPostProcessing(bool NewValue);

  // The colour accentuation done by PostProcessing()
  int GetAccentuation();
  void SetAccentuation(int NewValue);

  // Switch the red and blue values around (for some systems)
  bool GetFlipRB();
  void SetFlipRB(bool NewValue);
  
  // Set the calibration values: 0-100
  int GetCalibrationRed();
  int GetCalibrationGreen();
  int GetCalibrationBlue();
  void SetCalibrationRed(int NewValue);
  void SetCalibrationGreen(int NewValue);
  void SetCalibrationBlue(int NewValue);

  // Caibration Brightness
  int GetBrightness();
  void SetBrightness(int NewValue);

  // The priority of the entire process
  int GetProcessPriority();
  void SetProcessPriority(int NewValue);

  // The speed of the colour sweep
  int GetSweepSpeed();
  void SetSweepSpeed(int NewValue);

  // Limit loops per second or not
  bool GetDoLimitLps();
  void SetDoLimitLps(bool NewValue);

  // The lps limit value
  int GetLpsLimit();
  void SetLpsLimit(int NewValue);

  // The minimal value that needs to be output on each channel
  int GetMinBrightness();
  void SetMinBrightness(int NewValue);

  // Automatically check for new versions option
  bool GetUpdate();
  void SetUpdate(bool NewValue);

  // The capture mode
  int GetCaptureModeID();
  void SetCaptureModeID(int NewValue);

  // HotKeys
  UINT GetHotkeyKey(int ID);
  UINT GetHotkeyMod(int ID);
  void SetHotkeyKey(int ID, UINT NewKey);
  void SetHotkeyMod(int ID, UINT Mod);

  // XBMC IP
  CString GetXbmcIP();
  void SetXbmcIP(CString NewValue);

  // XBMC Port
  int GetXbmcPort();
  void SetXbmcPort(int NewValue);

private:
  int ComPort;
  bool ComPortLoaded;

  int DisplayDeviceNum;
  bool DisplayDeviceNumLoaded;

  int Precision;
  bool PrecisionLoaded;

  int Threshold;
  bool ThresholdLoaded;

  int Channels;
  bool ChannelsLoaded;

  int ScreenSections[4];
  bool ScreenSectionsLoaded;

  COLORREF StaticColours[4];
  bool StaticColoursLoaded;

  bool SkipPostProcessing;
  bool SkipPostProcessingLoaded;

  int Accentuation;
  bool AccentuationLoaded;

  bool FlipRB;
  bool FlipRBLoaded;

  int CalibrationRed;
  int CalibrationGreen;
  int CalibrationBlue;
  bool CalibrationRedLoaded;
  bool CalibrationGreenLoaded;
  bool CalibrationBlueLoaded;

  int Brightness;
  bool BrightnessLoaded;

  int ProcessPriority;
  bool ProcessPriorityLoaded;

  int SweepSpeed;
  bool SweepSpeedLoaded;

  bool DoLimitLps;
  bool DoLimitLpsLoaded;

  int LpsLimit;
  bool LpsLimitLoaded;

  int MinBrightness;
  bool MinBrightnessLoaded;

  bool Update;
  bool UpdateLoaded;

  int CaptureModeID;
  bool CaptureModeIDLoaded;

  CString XbmcIP;
  bool XbmcIPLoaded;

  int XbmcPort;
  bool XbmcPortLoaded;
};
//-------------------------------------------------------------------------------------------------------

