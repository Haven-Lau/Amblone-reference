// Amblone code: Header file of the Amblone dialog
// Author: Bart van der Drift

// License:
// Anyone is free to change, redistribute or copy parts of this code
// as long as it is not for commercial purposes / monetary gain
// Please be so kind to pay credit where due

#pragma once
#include <winsock.h>
#include <vector>
#include "afxwin.h"
#include "SystemTray.h"
#include "ArduinoUSB.h"
#include "AmbloneSettings.h"
#include "OutputDialog.h"
#include "DirectX.h"
#include "XBMC.h"
#include "afxcmn.h"
#include "Socket.h"
#include "faketv.h"
#include "HotKeyMapping.h"

using namespace std;

#define LOGGING 0
#if LOGGING
#include <fstream>
#include <ctime>
#define FunctionName __FUNCTION__
#endif

#define VERSION "1.3"
#define VRS_UPTODATE 0
#define VRS_NEW      1
#define VRS_ERROR    2

#define TEST 0
#define FILENAME "config.ini"

#define BITSPERPIXEL		32
#define	WINDOW_MODE			true				//Not in fullscreen mode

#define SW_RED_DOWN_GREEN_UP  0
#define SW_GREEN_DOWN_BLUE_UP 1
#define SW_BLUE_DOWN_RED_UP   2

#define KEY_1 '1'
#define KEY_2 '2'
#define KEY_Q 'Q'
#define KEY_W 'W'
#define KEY_E 'E'

#define T_LPS 1
#define T_DLG 2

struct WindowInfo {
  HWND Handle;
  HINSTANCE hInstance;
	char Name[255];
};

struct CaptureMethod {
  int ID;
  CString Description;
  bool FullScreen;
  bool DirectX;
  bool XBMC;
};

// CAmbloneDlg dialog
class CAmbloneDlg : public CDialog
{
// Construction
public:
	CAmbloneDlg(CWnd* pParent = NULL);	// standard constructor
  ~CAmbloneDlg();                     // destructor

// Dialog Data
	enum { IDD = IDD_Amblone_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
public:
  ArduinoUSB *pArduino; // USB communication class
  // Hotkeys
  vector<HotKeyMapping*> HotKeys;

protected:
	HICON m_hIcon; // Application Icon

	// Generated message map functions
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

  bool KeepRunning; // Worker thread loop control
  bool restart;     // Worker thread loop control
  
  CSystemTray TrayIcon; // TrayIcon of the application

  // Settings object
  AmbloneSettings settings;

  // DirectX object
  AmbloneDirectX *DirectX;

  // XBMC object
  XBMC *pXBMC;

  // Colour ouput visualization dialog
  OutputDialog *OutputDlg;

  // TV simulator object
  FakeTV iFakeTV;

  // The capture thread
  HANDLE hThread;

  // Variables used for screen capture
  COLORREF rgbQ[4];          // Average screen RGB of screen quarters
  COLORREF rgbC[4];          // Average screen RGB of channel 1 - 4
  COLORREF rgbSTATIC[4];     // RGB values if a static colour is selected
  COLORREF rgbSWEEP;         // RGB colours for sweep mode (only need 1 because all channels are similar)
  int SweepState;            // Determines how we are sweeping colours
  HDC hScrDC;                // The HDC of the screen
  HDC hMemDC;                // Temporarily hold HDC
  CDC *pDC;                  // CDC of window
  CRect rect;                // Rectangle to capture screen
	int xScrn, yScrn;          // screen resolution      
  DISPLAY_DEVICE dd;         // Display device of selected monitor
  bool FirstTime;            // To initialize cbBuffer
  BYTE* pbPixels;            // Pointer to screen pixels
  HBITMAP hBitmap;           // Bitmap handle of screen
  HGDIOBJ hOldBitmap;        // Handle to old bitmap
  BITMAP bmp;                // Actual bitmap of screen
  DWORD cbBuffer;            // Pixels from bmp
  unsigned int loopc;        // Count the amount of loops in 1 second
  CBrush *pNew;              // New brush
  CBrush *pOld;              // To save the current brush and restore it
  unsigned long r, g, b;     // Red, Green, Blue
  int BytesPerPixel;         // The amount of bytes of one pixel
  long mincount;             // Minimum amount of pixels for average
  unsigned int SleepTime;    // The time to sleep between every cycle to limit the max lps
  vector<WindowInfo> vWin;   // A list of windows
  int SelectedWin;           // The currently selected window to be captured
  bool UpdateOutputDlg;      // Indicates that the colour output dialogs needs redrawing
  CaptureMethod WindowsFull; // Capture method Windows API Fullscreen
  CaptureMethod WindowsWin;  // Capture method Windows API Window
  CaptureMethod DirectXFull; // Capture method DirectX Fullscreen
  //CaptureMethod DirectXWin;  // Capture method DirectX Window
  //CaptureMethod XbmcWin;     // Capture method XBMC
  CaptureMethod CurCapture;  // Currently selected capture mode
  bool WindowSelected;       // Have we selected a window to capture?
  bool HotKeyDlgOpen;        // Is the hotkey dialog opened?
  int CurDispDev;            // The currently selected display device
  #if LOGGING
  fstream LogFile;
  #endif

public:
  // Initialize the hotkeys
  void InitHotkeys();

  // Signal closing of hotkeys window
  void HotkeyWinClose();

  // Start the worker thread
  void StartRunning();

  // Stop the worker thread
  void StopRunning();

  // Restart the worker thread
  void Restart();

  // Check online if a new version is available. Returns one of the VRS_ defines
  int CheckUpdate(const char *current_version);

  // Get the screen and put it into a bitmap
  void CopyScreenToBitmap();

  // Get the average from the copied bitmap
  void GetAverageRGB(int precision, int colour_treshold);

  // Determine whether we need to get the colours from the screen (unless static colour/sweep)
  bool NeedScreenColours();

  // Capture the screen image
  void CaptureScreen();

  // Calculate the average for all 4 corners
  void GetAverageRGBquarters(int precision, int colour_treshold, int quarter);

  // Calculate the RGB values of the channels from the quarters
  void GetChannelsRGB(int channel);

  // Set the RGB values for the colour sweep mode
  void SetSweepRGB();

  // Perform some transformations on the colours we calculated
  void PostProcessRGB(int channel, int accentuation);

  // This function applies the calibration settings
  void ApplyCalibration();

  // Output to Arduino
  void OutputColour();

  // Load the screen sections from the saved settings
  void CAmbloneDlg::LoadScreenSectionSettings();

  // Make the Combo Boxes enabled or disabled according to the Check Boxes
  void SetComboCs();

  // Save the selected channels in the settings
  void UpdateChannelSettings();

  // Build a list of windows that are running
  void BuildWindowsList();
  BOOL CALLBACK AmbloneEnumWindowsProc(HWND hWnd, LPARAM lParam);

  // Set the capture mode to a given mode ID (fullscreen, DirectX etc.)
  void SetCaptureMode(int ModeID);

  // Windows that are running
  void ShowWindows();

  // See if a hotkey is already taken
  HotKeyMapping* GetHotkey(UINT Key, UINT Modifiers);

  // Get the hotkey mapping of a hotkey
  HotKeyMapping* GetHotkey(int HotkeyID);

  // Set a hotkey
  void SetHotkey(int HotkeyID, UINT Key, UINT Modifiers);

  // Main loop of the worker thread
  UINT MainLoopFunc();

  #if LOGGING
  void InitLogging();
  void Log(CString msg);
  #endif

  HRESULT Reset();
  HRESULT InitD3D(HWND hWnd);

  CButton CheckC2;
  CButton CheckC3;
  CButton CheckC4;
  CButton CheckLimitLps;
  CButton CheckFlipRB;
  CComboBox CComboComs;
  //CComboBox CComboDispDev;
  CComboBox ComboC1;
  CComboBox ComboC2;
  CComboBox ComboC3;
  CComboBox ComboC4;
  CSliderCtrl SliderAccentuation;
  CSliderCtrl SliderRed;
  CSliderCtrl SliderGreen;
  CSliderCtrl SliderBlue;
  CSliderCtrl SliderBrightness;
  CSliderCtrl SliderPrecision;
  CSliderCtrl SliderThreshold;
  CSliderCtrl SliderPriority;
  CSliderCtrl SliderSweepSpeed;
  CSliderCtrl SliderLimitLps;
  CStatic LabelLowLimit;
  CStatic LabelHighLimit;
  CStatic StaticLoops;

  afx_msg void OnBnClickedOk();
  afx_msg void OnNMCustomdrawSliderPrecision(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnCbnSelchangeCombo1();
  afx_msg void OnCbnDropdownCombo1();
  afx_msg void OnConfig();
  afx_msg void OnBnClickedCheckCnl2();
  afx_msg void OnBnClickedCheckCnl3();
  afx_msg void OnBnClickedCheckCnl4();
  afx_msg void OnCbnSelchangeComboCnl1();
  afx_msg void OnCbnSelchangeComboCnl2();
  afx_msg void OnCbnSelchangeComboCnl3();
  afx_msg void OnCbnSelchangeComboCnl4();  
  afx_msg void OnAppExit();
  afx_msg void OnNMCustomdrawSliderRed(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnNMCustomdrawSliderGreen(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnNMCustomdrawSliderBlue(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnEnChangeEdit1();
  afx_msg void OnBnClickedCheckStatic();
  afx_msg void OnComboComsEditChange();
  afx_msg void OnComboComsCloseup();
  afx_msg void OnNMCustomdrawSliderThreshold(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnPaint();
  afx_msg void OnTimer (UINT TimerVal);
  afx_msg void OnNMCustomdrawSliderAccentuation(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnBnClickedStaticAdvancd();
  afx_msg void OnBnClickedCheckFliprb();
  afx_msg void OnNMCustomdrawSliderBrightness(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnCustomdrawSliderRed(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnCustomdrawSliderGreen(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnCustomdrawSliderBlue(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnNMCustomdrawSliderSweepspeed(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnBnClickedCheckLimitLps();
  afx_msg void OnNMCustomdrawSliderLimitLps(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnPopupCheckforupdates();
  afx_msg HCURSOR OnQueryDragIcon();
  afx_msg LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);
  CSliderCtrl SliderMinBrightness;
  afx_msg void OnNMCustomdrawSliderMinbrightness(NMHDR *pNMHDR, LRESULT *pResult);
  CButton CheckUpdates;
  afx_msg void OnBnClickedCheckUpdates();
  CComboBox CComboCapture;
  afx_msg void OnCbnSelchangeComboCapture();
  afx_msg void OnCbnSelchangeComboWindow();
  CComboBox CComboWindow;
  afx_msg void OnCbnDropdownComboWindow();
  CButton BtnShowOutput;
  afx_msg void OnBnClickedButtonShowoutput();
  CButton BtnHotkeys;
  afx_msg void OnBnClickedButtonHotkeys();
  CStatic CStaticWindow;
  CStatic CStaticXbmcIP;
  CStatic CStaticXbmcPort;
  CEdit CEditXbmcIP;
  CEdit CEditXbmcPort;
};
