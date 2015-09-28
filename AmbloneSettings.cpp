// Amblone code: Settings implementation
// Author: Bart van der Drift

// License:
// Anyone is free to change, redistribute or copy parts of this code
// as long as it is not for commercial purposes / monetary gain
// Please be so kind to pay credit where due

#include "stdafx.h"
#include "AmbloneSettings.h"

AmbloneSettings::AmbloneSettings() {
  ComPortLoaded = false;
  DisplayDeviceNumLoaded = false;
  PrecisionLoaded = false;
  ChannelsLoaded = false;
  ScreenSectionsLoaded = false;
  SkipPostProcessingLoaded = false;
  ThresholdLoaded = false;
  StaticColoursLoaded = false;
  AccentuationLoaded = false;
  FlipRBLoaded = false;
  CalibrationRedLoaded = false;
  CalibrationGreenLoaded = false;
  CalibrationBlueLoaded = false;
  BrightnessLoaded = false;
  ProcessPriorityLoaded = false;
  SweepSpeedLoaded = false;
  LpsLimitLoaded = false;
  DoLimitLpsLoaded = false;
  MinBrightnessLoaded = false;
  UpdateLoaded = false;
  CaptureModeIDLoaded = false;
  XbmcIPLoaded = false;
  XbmcPortLoaded = false;
}
//-------------------------------------------------------------------------------------------------------

int AmbloneSettings::GetComPort() {
  if (!ComPortLoaded) {
    ComPort = theApp->GetProfileIntA("GENERAL", "COM_PORT", -1);
    ComPortLoaded = true;
  }
  return ComPort;
}
//-------------------------------------------------------------------------------------------------------

void AmbloneSettings::SetComPort(int NewValue) {
  ComPort = NewValue;
  theApp->WriteProfileInt("GENERAL", "COM_PORT", ComPort);
}
//-------------------------------------------------------------------------------------------------------

int AmbloneSettings::GetDisplayDeviceNum() {
  if (!DisplayDeviceNumLoaded) {
    DisplayDeviceNum = theApp->GetProfileIntA("GENERAL", "DISPLAY_DEVICE_NUM", 0);
    DisplayDeviceNumLoaded = true;
  }
  return DisplayDeviceNum;
}
//-------------------------------------------------------------------------------------------------------

void AmbloneSettings::SetDisplayDeviceNum(int NewValue) {
  DisplayDeviceNum = NewValue;
  theApp->WriteProfileInt("GENERAL", "DISPLAY_DEVICE_NUM", DisplayDeviceNum);
}
//-------------------------------------------------------------------------------------------------------

int AmbloneSettings::GetPrecision() {
  if (!PrecisionLoaded) {
    Precision = theApp->GetProfileIntA("GENERAL", "PRECISION", 3);
    PrecisionLoaded = true;
  }
  return Precision;
}
//-------------------------------------------------------------------------------------------------------

void AmbloneSettings::SetPrecision(int NewValue) {
  Precision = NewValue;
  theApp->WriteProfileInt("GENERAL", "PRECISION", Precision);
}
//-------------------------------------------------------------------------------------------------------

int AmbloneSettings::GetThreshold() {
  if (!ThresholdLoaded) {
    Threshold = theApp->GetProfileIntA("GENERAL", "THRESHOLD", 50);
    ThresholdLoaded = true;
  }
  return Threshold;
}
//-------------------------------------------------------------------------------------------------------

void AmbloneSettings::SetThreshold(int NewValue) {
  Threshold = NewValue;
  theApp->WriteProfileInt("GENERAL", "THRESHOLD", Threshold);
}
//-------------------------------------------------------------------------------------------------------

int AmbloneSettings::GetChannels() {
  if (!ChannelsLoaded) {
    Channels = theApp->GetProfileIntA("GENERAL", "CHANNELS", 3);
    ChannelsLoaded = true;
  }
  return Channels;
}
//-------------------------------------------------------------------------------------------------------

void AmbloneSettings::SetChannels(int NewValue) {
  Channels = NewValue;
  theApp->WriteProfileInt("GENERAL", "CHANNELS", Channels);
}
//-------------------------------------------------------------------------------------------------------

int AmbloneSettings::GetScreenSection(int Channel) {
  if (!ScreenSectionsLoaded) {
    ScreenSections[0] = theApp->GetProfileIntA("GENERAL", "SCREEN_SECTION_1", ENTIRE_SCREEN);
    ScreenSections[1] = theApp->GetProfileIntA("GENERAL", "SCREEN_SECTION_2", ENTIRE_SCREEN);
    ScreenSections[2] = theApp->GetProfileIntA("GENERAL", "SCREEN_SECTION_3", ENTIRE_SCREEN);
    ScreenSections[3] = theApp->GetProfileIntA("GENERAL", "SCREEN_SECTION_4", ENTIRE_SCREEN);
    ScreenSectionsLoaded = true;
  }
  return ScreenSections[Channel-1];
}
//-------------------------------------------------------------------------------------------------------

void AmbloneSettings::SetScreenSection(int Channel, int NewValue) {
  ScreenSections[Channel-1] = NewValue;
  switch (Channel) {
    case 1:
      theApp->WriteProfileInt("GENERAL", "SCREEN_SECTION_1", NewValue);
      break;
    case 2:
      theApp->WriteProfileInt("GENERAL", "SCREEN_SECTION_2", NewValue);
      break;
    case 3:
      theApp->WriteProfileInt("GENERAL", "SCREEN_SECTION_3", NewValue);
      break;
    case 4:
      theApp->WriteProfileInt("GENERAL", "SCREEN_SECTION_4", NewValue);
      break;
  }
}
//-------------------------------------------------------------------------------------------------------

bool AmbloneSettings::GetSkipPostProcessing() {
  if (!SkipPostProcessingLoaded) {
    SkipPostProcessing = theApp->GetProfileIntA("GENERAL", "SKIP_POST_PROCESSING", 0);
    SkipPostProcessingLoaded = true;
  }
  return SkipPostProcessing;
}
//-------------------------------------------------------------------------------------------------------

void AmbloneSettings::SetSkipPostProcessing(bool NewValue) {
  SkipPostProcessing = NewValue;
  theApp->WriteProfileInt("GENERAL", "SKIP_POST_PROCESSING", SkipPostProcessing);
}
//-------------------------------------------------------------------------------------------------------

COLORREF AmbloneSettings::GetStaticColour(int Channel) {
  // The default colour is a nice orange / yellowish light

  if (!StaticColoursLoaded) {
    // Channel 1
    int C1_Red = theApp->GetProfileIntA("COLOURS", "CHANNEL1_RED", 255);
    int C1_Green = theApp->GetProfileIntA("COLOURS", "CHANNEL1_GREEN", 150);
    int C1_Blue = theApp->GetProfileIntA("COLOURS", "CHANNEL1_BLUE", 50);
    StaticColours[0] = RGB(C1_Red, C1_Green, C1_Blue);

    // Channel 2
    int C2_Red = theApp->GetProfileIntA("COLOURS", "CHANNEL2_RED", 255);
    int C2_Green = theApp->GetProfileIntA("COLOURS", "CHANNEL2_GREEN", 150);
    int C2_Blue = theApp->GetProfileIntA("COLOURS", "CHANNEL2_BLUE", 50);
    StaticColours[1] = RGB(C2_Red, C2_Green, C2_Blue);

    // Channel 3
    int C3_Red = theApp->GetProfileIntA("COLOURS", "CHANNEL3_RED", 255);
    int C3_Green = theApp->GetProfileIntA("COLOURS", "CHANNEL3_GREEN", 150);
    int C3_Blue = theApp->GetProfileIntA("COLOURS", "CHANNEL3_BLUE", 50);
    StaticColours[2] = RGB(C3_Red, C3_Green, C3_Blue);

    // Channel 4
    int C4_Red = theApp->GetProfileIntA("COLOURS", "CHANNEL4_RED", 255);
    int C4_Green = theApp->GetProfileIntA("COLOURS", "CHANNEL4_GREEN", 150);
    int C4_Blue = theApp->GetProfileIntA("COLOURS", "CHANNEL4_BLUE", 50);
    StaticColours[3] = RGB(C4_Red, C4_Green, C4_Blue);
  }
  switch (Channel) {
    case 1:
      return StaticColours[0];
    case 2:
      return StaticColours[1];
    case 3:
      return StaticColours[2];
    case 4:
      return StaticColours[3];
    default:
      return StaticColours[0];
  }
}
//-------------------------------------------------------------------------------------------------------

void AmbloneSettings::SetStaticColour(int Channel, COLORREF NewColour) {
  switch (Channel) {
    case 1:
      StaticColours[0] = NewColour;
      theApp->WriteProfileInt("COLOURS", "CHANNEL1_RED", GetRValue(NewColour));
      theApp->WriteProfileInt("COLOURS", "CHANNEL1_GREEN", GetGValue(NewColour));
      theApp->WriteProfileInt("COLOURS", "CHANNEL1_BLUE", GetBValue(NewColour));
      break;
    case 2:
      StaticColours[1] = NewColour;
      theApp->WriteProfileInt("COLOURS", "CHANNEL2_RED", GetRValue(NewColour));
      theApp->WriteProfileInt("COLOURS", "CHANNEL2_GREEN", GetGValue(NewColour));
      theApp->WriteProfileInt("COLOURS", "CHANNEL2_BLUE", GetBValue(NewColour));
    case 3:
      StaticColours[2] = NewColour;
      theApp->WriteProfileInt("COLOURS", "CHANNEL3_RED", GetRValue(NewColour));
      theApp->WriteProfileInt("COLOURS", "CHANNEL3_GREEN", GetGValue(NewColour));
      theApp->WriteProfileInt("COLOURS", "CHANNEL3_BLUE", GetBValue(NewColour));
    case 4:
      StaticColours[3] = NewColour;
      theApp->WriteProfileInt("COLOURS", "CHANNEL4_RED", GetRValue(NewColour));
      theApp->WriteProfileInt("COLOURS", "CHANNEL4_GREEN", GetGValue(NewColour));
      theApp->WriteProfileInt("COLOURS", "CHANNEL4_BLUE", GetBValue(NewColour));
      break;
  }
}
//-------------------------------------------------------------------------------------------------------

int AmbloneSettings::GetAccentuation() {
  if (!AccentuationLoaded) {
    Accentuation = theApp->GetProfileIntA("GENERAL", "ACCENTUATION", 1);
    AccentuationLoaded = true;
  }
  return Accentuation;
}
//-------------------------------------------------------------------------------------------------------

void AmbloneSettings::SetAccentuation(int NewValue) {
  Accentuation = NewValue;
  theApp->WriteProfileInt("GENERAL", "ACCENTUATION", Accentuation);
}
//-------------------------------------------------------------------------------------------------------

bool AmbloneSettings::GetFlipRB() {
  if (!FlipRBLoaded) {
    FlipRB = theApp->GetProfileIntA("GENERAL", "FLIP_RB", 0);
    FlipRBLoaded = true;
  }
  return FlipRB;
}
//-------------------------------------------------------------------------------------------------------

void AmbloneSettings::SetFlipRB(bool NewValue) {
  FlipRB = NewValue;
  theApp->WriteProfileInt("GENERAL", "FLIP_RB", FlipRB);
}
//-------------------------------------------------------------------------------------------------------

int AmbloneSettings::GetCalibrationRed() {
  if (!CalibrationRedLoaded) {
    CalibrationRed = theApp->GetProfileIntA("CALIBRATION", "RED", 100);
    CalibrationRedLoaded = true;
  }
  return CalibrationRed;
}
//-------------------------------------------------------------------------------------------------------

int AmbloneSettings::GetCalibrationGreen() {
  if (!CalibrationGreenLoaded) {
    CalibrationGreen = theApp->GetProfileIntA("CALIBRATION", "GREEN", 100);
    CalibrationGreenLoaded = true;
  }
  return CalibrationGreen;
}
//-------------------------------------------------------------------------------------------------------

int AmbloneSettings::GetCalibrationBlue() {
  if (!CalibrationBlueLoaded) {
    CalibrationBlue = theApp->GetProfileIntA("CALIBRATION", "BLUE", 100);
    CalibrationBlueLoaded = true;
  }
  return CalibrationBlue;
}
//-------------------------------------------------------------------------------------------------------

void AmbloneSettings::SetCalibrationRed(int NewValue) {
  CalibrationRed = NewValue;
  theApp->WriteProfileInt("CALIBRATION", "RED", CalibrationRed);
}
//-------------------------------------------------------------------------------------------------------

void AmbloneSettings::SetCalibrationGreen(int NewValue) {
  CalibrationGreen = NewValue;
  theApp->WriteProfileInt("CALIBRATION", "GREEN", CalibrationGreen);
}
//-------------------------------------------------------------------------------------------------------

void AmbloneSettings::SetCalibrationBlue(int NewValue) {
  CalibrationBlue = NewValue;
  theApp->WriteProfileInt("CALIBRATION", "BLUE", CalibrationBlue);
}
//-------------------------------------------------------------------------------------------------------

int AmbloneSettings::GetBrightness() {
  if (!BrightnessLoaded) {
    Brightness = theApp->GetProfileIntA("CALIBRATION", "BRIGHTNESS", 100);
    BrightnessLoaded = true;
  }
  return Brightness;
}
//-------------------------------------------------------------------------------------------------------

void AmbloneSettings::SetBrightness(int NewValue) {
  Brightness = NewValue;
  theApp->WriteProfileInt("CALIBRATION", "BRIGHTNESS", Brightness);
}
//-------------------------------------------------------------------------------------------------------

int AmbloneSettings::GetProcessPriority() {
  if (!ProcessPriorityLoaded) {
    ProcessPriority = theApp->GetProfileIntA("GENERAL", "PROCESS_PRIORITY", BELOW_NORMAL_PRIORITY_CLASS);
    ProcessPriorityLoaded = true;
  }
  return ProcessPriority;
}
//-------------------------------------------------------------------------------------------------------

void AmbloneSettings::SetProcessPriority(int NewValue) {
  ProcessPriority = NewValue;
  theApp->WriteProfileInt("GENERAL", "PROCESS_PRIORITY", ProcessPriority);
}
//-------------------------------------------------------------------------------------------------------

int AmbloneSettings::GetSweepSpeed() {
  if (!SweepSpeedLoaded) {
    SweepSpeed = theApp->GetProfileIntA("GENERAL", "SWEEP_SPEED", 2);
    SweepSpeedLoaded = true;
  }
  return SweepSpeed;
}
//-------------------------------------------------------------------------------------------------------

void AmbloneSettings::SetSweepSpeed(int NewValue) {
  SweepSpeed = NewValue;
  theApp->WriteProfileInt("GENERAL", "SWEEP_SPEED", SweepSpeed);
}
//-------------------------------------------------------------------------------------------------------

int AmbloneSettings::GetLpsLimit() {
  if (!LpsLimitLoaded) {
    LpsLimit = theApp->GetProfileIntA("GENERAL", "LPS_LIMIT", 30);
    LpsLimitLoaded = true;
  }
  return LpsLimit;
}
//-------------------------------------------------------------------------------------------------------

void AmbloneSettings::SetLpsLimit(int NewValue) {
  LpsLimit = NewValue;
  theApp->WriteProfileInt("GENERAL", "LPS_LIMIT", LpsLimit);
}
//-------------------------------------------------------------------------------------------------------

bool AmbloneSettings::GetDoLimitLps() {
  if (!DoLimitLpsLoaded) {
    DoLimitLps = theApp->GetProfileIntA("GENERAL", "DO_LIMIT_LPS", 0);
    DoLimitLpsLoaded = true;
  }
  return DoLimitLps;
}
//-------------------------------------------------------------------------------------------------------

void AmbloneSettings::SetDoLimitLps(bool NewValue) {
  DoLimitLps = NewValue;
  theApp->WriteProfileInt("GENERAL", "DO_LIMIT_LPS", DoLimitLps);
}
//-------------------------------------------------------------------------------------------------------

int AmbloneSettings::GetMinBrightness() {
  if (!MinBrightnessLoaded) {
    MinBrightness = theApp->GetProfileIntA("GENERAL", "MIN_BRIGHTNESS", 0);
    MinBrightnessLoaded = true;
  }
  return MinBrightness;
}
//-------------------------------------------------------------------------------------------------------

void AmbloneSettings::SetMinBrightness(int NewValue) {
  MinBrightness = NewValue;
  theApp->WriteProfileInt("GENERAL", "MIN_BRIGHTNESS", MinBrightness);
}
//-------------------------------------------------------------------------------------------------------

bool AmbloneSettings::GetUpdate() {
  if (!UpdateLoaded) {
    Update = theApp->GetProfileIntA("GENERAL", "UPDATE", 1);
    UpdateLoaded = true;
  }
  return Update;
}
//-------------------------------------------------------------------------------------------------------

void AmbloneSettings::SetUpdate(bool NewValue) {
  Update = NewValue;
  theApp->WriteProfileInt("GENERAL", "UPDATE", Update);
}
//-------------------------------------------------------------------------------------------------------

int AmbloneSettings::GetCaptureModeID() {
  if (!CaptureModeIDLoaded) {
    CaptureModeID = theApp->GetProfileIntA("GENERAL", "CAPTURE_MODE_ID", 0);
    CaptureModeIDLoaded = true;
  }
  return CaptureModeID;
}
//-------------------------------------------------------------------------------------------------------

void AmbloneSettings::SetCaptureModeID(int NewValue) {
  CaptureModeID = NewValue;
  theApp->WriteProfileInt("GENERAL", "CAPTURE_MODE_ID", CaptureModeID);
}
//-------------------------------------------------------------------------------------------------------

UINT AmbloneSettings::GetHotkeyKey(int ID) {
  CString Name;
  Name.Format("KEY_%d", ID);
  return theApp->GetProfileIntA("HOTKEYS", Name, 0);
}
//-------------------------------------------------------------------------------------------------------

UINT AmbloneSettings::GetHotkeyMod(int ID) {
  CString Name;
  Name.Format("MOD_%d", ID);
  return theApp->GetProfileIntA("HOTKEYS", Name, 0);
}
//-------------------------------------------------------------------------------------------------------

void AmbloneSettings::SetHotkeyKey(int ID, UINT NewKey) {
  CString Name;
  Name.Format("KEY_%d", ID);
  theApp->WriteProfileInt("HOTKEYS", Name, NewKey);
}
//-------------------------------------------------------------------------------------------------------

void AmbloneSettings::SetHotkeyMod(int ID, UINT Mod) {
  CString Name;
  Name.Format("MOD_%d", ID);
  theApp->WriteProfileInt("HOTKEYS", Name, Mod);
}
//-------------------------------------------------------------------------------------------------------

CString AmbloneSettings::GetXbmcIP() {
  if (!XbmcIPLoaded) {
    XbmcIP = theApp->GetProfileStringA("GENERAL", "XBMC_IP", "localhost");
    XbmcIPLoaded = true;
  }
  return XbmcIP;
}
//-------------------------------------------------------------------------------------------------------

void AmbloneSettings::SetXbmcIP(CString NewValue) {
  XbmcIP = NewValue;
  theApp->WriteProfileString("GENERAL", "XBMC_IP", XbmcIP);
}
//-------------------------------------------------------------------------------------------------------

int AmbloneSettings::GetXbmcPort() {
  if (!XbmcPortLoaded) {
    XbmcPort = theApp->GetProfileIntA("GENERAL", "XBMC_PORT", 80);
    XbmcPortLoaded = true;
  }
  return XbmcPort;
}
//-------------------------------------------------------------------------------------------------------

void AmbloneSettings::SetXbmcPort(int NewValue) {
  XbmcPort = NewValue;
  theApp->WriteProfileInt("GENERAL", "XBMC_PORT", XbmcPort);
}
//-------------------------------------------------------------------------------------------------------
