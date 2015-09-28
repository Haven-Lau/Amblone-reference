// Amblone code: Header file for USB communication with Arduino
// Author: Bart van der Drift

// License:
// Anyone is free to change, redistribute or copy parts of this code
// as long as it is not for commercial purposes / monetary gain
// Please be so kind to pay credit where due

#pragma once

#define C_SF1 0xF1 // Startflag for 1-channel mode (1 RGB channel)
#define C_SF2 0xF2 // Startflag for 2-channel mode (2 RGB channels)
#define C_SF3 0xF3 // Startflag for 3-channel mode (3 RGB channels)
#define C_SF4 0xF4 // Startflag for 4-channel mode (4 RGB channels)
#define C_END 0x33 // End flag
#define C_ESC 0x99 // Escape character

class ArduinoUSB
{
public:
  ArduinoUSB(void);
  ~ArduinoUSB(void);

  // Open the specified COM port
  bool OpenCOM(int ComNum);

  // Close the COM port if it is open
  void CloseCOM();

  // Send RGB values from rgbC to Arduino
  void SendRGB(int nChannels, COLORREF rgb[]);

  // Returns true of a COM port is open
  bool GetComOpened();
private:
  // Returns true if c is one of the flags
  bool IsSpecialChar(unsigned char c);

  // Writes character c to the array, and escapes if necessary
  void WriteCharToSend(unsigned char c, unsigned char Send[], int &idx);
private:
  // Variables for USB communication
  HANDLE hUSB;
  bool ComOpened;
  int CurrentCOM;
};
