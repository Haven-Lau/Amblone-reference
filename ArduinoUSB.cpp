// Amblone code: Implementation file for USB communication with Arduino
// Author: Bart van der Drift

// License:
// Anyone is free to change, redistribute or copy parts of this code
// as long as it is not for commercial purposes / monetary gain
// Please be so kind to pay credit where due

#include "StdAfx.h"
#include "ArduinoUSB.h"

ArduinoUSB::ArduinoUSB(void)
{
  ComOpened = false;
  CurrentCOM = 0;
}
//-------------------------------------------------------------------------------------------------------

ArduinoUSB::~ArduinoUSB(void)
{
  if (ComOpened) {
    CloseHandle(hUSB);
    ComOpened = false;
  }
}
//-------------------------------------------------------------------------------------------------------

bool ArduinoUSB::OpenCOM(int ComNum) {
  if (ComNum != CurrentCOM || !ComOpened) {
    CString com;
    com.Format("\\\\.\\COM%d", ComNum);

    // Create handle for usb communication
    hUSB = CreateFile(com,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if (hUSB != INVALID_HANDLE_VALUE) {
      // Handle is valid
      DCB iDCB = {0};
      if (GetCommState(hUSB, &iDCB)) {
        iDCB.BaudRate = CBR_256000;
        iDCB.ByteSize = 8;
        iDCB.StopBits = ONESTOPBIT;
        iDCB.Parity = NOPARITY;
        if (SetCommState(hUSB, &iDCB)) ComOpened = true;
        else {
          //AfxMessageBox("Error: Can't set CommState");
          ComOpened = false;
        }
        return true;
      }
      //else AfxMessageBox("Error: Can't get CommState");
      ComOpened = false;
    }
    else {
      //AfxMessageBox("Error: Invalid handle");
      ComOpened = false;
    }
  }
  return false;
}
//-------------------------------------------------------------------------------------------------------

void ArduinoUSB::SendRGB(int nChannels, COLORREF rgb[]) {
  if (ComOpened) {
    // Write the start flag
    unsigned char Send[32];
    switch (nChannels) {
      case 1:
        Send[0] = C_SF1; // Start flag 1
        break;
      case 2:
        Send[0] = C_SF2; // Start flag 2
        break;
      case 3:
        Send[0] = C_SF3; // Start flag 3
        break;
      case 4:
        Send[0] = C_SF4; // Start flag 4
        break;
    }

    // Now we start writing the RGB values
    int idx = 1; // The index for Send, where we are right now
    char r = GetRValue(rgb[0]);
    char g = GetGValue(rgb[0]);
    char b = GetBValue(rgb[0]);
    WriteCharToSend(r, Send, idx);
    WriteCharToSend(g, Send, idx);
    WriteCharToSend(b, Send, idx);
    if (nChannels > 1) {
      r = GetRValue(rgb[1]);
      g = GetGValue(rgb[1]);
      b = GetBValue(rgb[1]);
      WriteCharToSend(r, Send, idx);
      WriteCharToSend(g, Send, idx);
      WriteCharToSend(b, Send, idx);
    }
    if (nChannels > 2) {
      r = GetRValue(rgb[2]);
      g = GetGValue(rgb[2]);
      b = GetBValue(rgb[2]);
      WriteCharToSend(r, Send, idx);
      WriteCharToSend(g, Send, idx);
      WriteCharToSend(b, Send, idx);
    }
    if (nChannels > 3) {
      r = GetRValue(rgb[3]);
      g = GetGValue(rgb[3]);
      b = GetBValue(rgb[3]);
      WriteCharToSend(r, Send, idx);
      WriteCharToSend(g, Send, idx);
      WriteCharToSend(b, Send, idx);
    }

    // Now the end flag
    Send[idx] = C_END; // End flag
    idx++;
    Send[idx] = '\0';

    // Write to the COM Port
    DWORD w;
    /*CString cs;
    for (int i = 0; i < idx; i++) {
      CString app; app.Format("[%02x]",Send[i]);
      cs.Append(app);
    }
    AfxMessageBox(cs);*/
    WriteFile(hUSB, Send, idx, &w, NULL);
  }
}
//-------------------------------------------------------------------------------------------------------

bool ArduinoUSB::GetComOpened() {
  return ComOpened;
}
//-------------------------------------------------------------------------------------------------------

void ArduinoUSB::CloseCOM() {
  if (ComOpened) {
    ComOpened = false;
    CloseHandle(hUSB);
  }
}
//-------------------------------------------------------------------------------------------------------

bool ArduinoUSB::IsSpecialChar(unsigned char c) {
  if (c == C_SF1 ||
      c == C_SF2 ||
      c == C_SF3 ||
      c == C_SF4 ||
      c == C_END ||
      c == C_ESC)
    return true;
  return false;
}
//-------------------------------------------------------------------------------------------------------

void ArduinoUSB::WriteCharToSend(unsigned char c, unsigned char Send[], int &idx) {
  if (IsSpecialChar(c)) {
    Send[idx] = C_ESC; // Escape character
    idx++;
    Send[idx] = c;
    idx++;
  }
  else {
    Send[idx] = c;
    idx++;
  }
}
//-------------------------------------------------------------------------------------------------------
