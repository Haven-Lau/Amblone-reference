
// Amblone code: Implementation file of the Amblone dialog
// Author: Bart van der Drift

// License:
// Anyone is free to change, redistribute or copy parts of this code
// as long as it is not for commercial purposes / monetary gain
// Please be so kind to pay credit where due

#include "stdafx.h"
#include "Amblone.h"
#include "AmbloneDlg.h"
#include "AmbloneHotkeys.h"
#include <string>
#include <tlhelp32.h>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAmbloneDlg dialog

DWORD WINAPI Thread(LPVOID pParam) {
  CAmbloneDlg * pAmbloneDlg;
  pAmbloneDlg = (CAmbloneDlg *) pParam;
  return pAmbloneDlg->MainLoopFunc();
}
//-------------------------------------------------------------------------------------------------------

 // Global pointer to the main class
CAmbloneDlg *thisDialog = NULL;

//-------------------------------------------------------------------------------------------------------

int Str2Int(CString str) {
  // Parse an integer in string format
  int MaxLength = 16;
  if (str.GetLength() >= MaxLength) return 0;
  int RetVal = 0;
  string strstr = str;
  const char * cchar = strstr.c_str();
  char * cstr = new char[MaxLength];
  strcpy(cstr, cchar);
  for (int i = 0; i < str.GetLength(); i++) {
    if (cstr[i] >= '0' && cstr[i] <= '9') {
      RetVal *= 10;
      RetVal += cstr[i] - '0';
    }
  }
  return RetVal;
}
//-------------------------------------------------------------------------------------------------------

CAmbloneDlg::CAmbloneDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAmbloneDlg::IDD, pParent)
{
  #if LOGGING
  InitLogging();
  #endif

  // Set pointers to NULL;
  pDC = NULL;
  pbPixels = NULL;
  pNew = NULL;
  pOld = NULL;
  DirectX = NULL;
  pXBMC = NULL;

  // Initialize Capture Modes
  WindowsFull.ID = 0;
  WindowsFull.Description = "Windows API - Fullscreen";
  WindowsFull.FullScreen = true;
  WindowsFull.DirectX = false;
  WindowsFull.XBMC = false;
  
  WindowsWin.ID = 1;
  WindowsWin.Description = "Windows API - Single window";
  WindowsWin.FullScreen = false;
  WindowsWin.DirectX = false;
  WindowsWin.XBMC = false;

  DirectXFull.ID = 2;
  DirectXFull.Description = "DirectX - Fullscreen";
  DirectXFull.FullScreen = true;
  DirectXFull.DirectX = true;
  DirectXFull.XBMC = false;

  /*DirectXWin.ID = 3;
  DirectXWin.Description = "DirectX - Single window";
  DirectXWin.FullScreen = false;
  DirectXWin.DirectX = true;
  DirectXWin.XBMC = false;

  XbmcWin.ID = 4;
  XbmcWin.Description = "XBMC HTTP API";
  XbmcWin.FullScreen = false;
  XbmcWin.DirectX = false;
  XbmcWin.XBMC = true;*/

  SelectedWin = -1;

  // Set a global pointer to this class
  thisDialog = this;

  // Create a new USB communication object
  pArduino = new ArduinoUSB();

  // Assign the pointer to the application in the settings object
  settings.theApp = &theApp;

  // Load the icon for the tray icon
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

  bool UpdateBalloon = false;
  if (settings.GetUpdate()) if (CheckUpdate(VERSION) == VRS_NEW) UpdateBalloon = true;

  if (UpdateBalloon) {
    // Show the tray icon, and add a balloon saying there is a new update
    TrayIcon.Create(pParent, WM_APP+10, "Amblone", m_hIcon, IDR_MENU1,FALSE,
      "A new version of Amblone is available!\nVisit http://amblone.com/download to get the new version", // balloon tip
                        "Amblone",                                                           // balloon title
                        NIIF_INFO,                                                           // balloon icon
                        15);
  }
  else if (settings.GetComPort() == -1) {
    // Show the tray icon, and add a balloon if the COM port hasn't been configured yet (on first time of execution)
    TrayIcon.Create(pParent, WM_APP+10, "Amblone", m_hIcon, IDR_MENU1,FALSE,
                        "Amblone has started\nRight click the tray icon for configuration!", // balloon tip
                        "Amblone",                                                           // balloon title
                        NIIF_INFO,                                                           // balloon icon
                        13);
  }
  else { // No balloon
    TrayIcon.Create(pParent, WM_APP+10, "Amblone", m_hIcon, IDR_MENU1,FALSE,
                        "",        // balloon tip
                        "",        // balloon title
                        NIIF_INFO, // balloon icon
                        13);
  }
  TrayIcon.SetTargetWnd(pParent);
}
//-------------------------------------------------------------------------------------------------------

CAmbloneDlg::~CAmbloneDlg() {
  delete pArduino;
  delete OutputDlg;
  
  #if LOGGING
  LogFile.close();
  #endif
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_COMBO1, CComboComs);
  DDX_Control(pDX, IDC_SLIDER_PRECISION, SliderPrecision);
  DDX_Control(pDX, IDC_CHECK_CNL2, CheckC2);
  DDX_Control(pDX, IDC_CHECK_CNL3, CheckC3);
  DDX_Control(pDX, IDC_CHECK_CNL4, CheckC4);
  DDX_Control(pDX, IDC_COMBO_CNL1, ComboC1);
  DDX_Control(pDX, IDC_COMBO_CNL2, ComboC2);
  DDX_Control(pDX, IDC_COMBO_CNL3, ComboC3);
  DDX_Control(pDX, IDC_COMBO_CNL4, ComboC4);
  DDX_Control(pDX, IDC_STATIC_LOOPS, StaticLoops);
  DDX_Control(pDX, IDC_SLIDER_THRESHOLD, SliderThreshold);
  DDX_Control(pDX, IDC_SLIDER_ACCENTUATION, SliderAccentuation);
  DDX_Control(pDX, IDC_CHECK_FLIPRB, CheckFlipRB);
  DDX_Control(pDX, IDC_SLIDER_RED, SliderRed);
  DDX_Control(pDX, IDC_SLIDER_GREEN, SliderGreen);
  DDX_Control(pDX, IDC_SLIDER_BLUE, SliderBlue);
  DDX_Control(pDX, IDC_SLIDER_INTENSITY, SliderBrightness);
  DDX_Control(pDX, IDC_SLIDER1, SliderPriority);
  DDX_Control(pDX, IDC_SLIDER_SWEEPSPEED, SliderSweepSpeed);
  DDX_Control(pDX, IDC_SLIDER_LIMIT_LPS, SliderLimitLps);
  DDX_Control(pDX, IDC_CHECK_LIMIT_LPS, CheckLimitLps);
  DDX_Control(pDX, IDC_STATIC_LOWLIMIT, LabelLowLimit);
  DDX_Control(pDX, IDC_STATIC_HIGHLIMIT, LabelHighLimit);
  DDX_Control(pDX, IDC_SLIDER_MINBRIGHTNESS, SliderMinBrightness);
  DDX_Control(pDX, IDC_CHECK_UPDATES, CheckUpdates);
  DDX_Control(pDX, IDC_COMBO_CAPTURE, CComboCapture);
  DDX_Control(pDX, IDC_COMBO_WINDOW, CComboWindow);
  DDX_Control(pDX, IDC_BUTTON_SHOWOUTPUT, BtnShowOutput);
  DDX_Control(pDX, IDC_BUTTON_HOTKEYS, BtnHotkeys);
  DDX_Control(pDX, IDC_STATIC_WINDOW, CStaticWindow);
  DDX_Control(pDX, IDC_STATIC_XBMC_IP, CStaticXbmcIP);
  DDX_Control(pDX, IDC_STATIC_XBMC_PORT, CStaticXbmcPort);
  DDX_Control(pDX, IDC_EDIT_XBMC_IP, CEditXbmcIP);
  DDX_Control(pDX, IDC_EDIT_XBMC_PORT, CEditXbmcPort);
}
//-------------------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CAmbloneDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_OK, &CAmbloneDlg::OnBnClickedOk)
  ON_WM_KEYDOWN()
  ON_CBN_SELCHANGE(IDC_COMBO1, &CAmbloneDlg::OnCbnSelchangeCombo1)
  ON_CBN_DROPDOWN(IDC_COMBO1, &CAmbloneDlg::OnCbnDropdownCombo1)
  ON_COMMAND(ID_CONFIG, &CAmbloneDlg::OnConfig)
  ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_PRECISION, &CAmbloneDlg::OnNMCustomdrawSliderPrecision)
  ON_BN_CLICKED(IDC_CHECK_CNL2, &CAmbloneDlg::OnBnClickedCheckCnl2)
  ON_BN_CLICKED(IDC_CHECK_CNL3, &CAmbloneDlg::OnBnClickedCheckCnl3)
  ON_BN_CLICKED(IDC_CHECK_CNL4, &CAmbloneDlg::OnBnClickedCheckCnl4)
  ON_WM_TIMER ( )
  ON_CBN_SELCHANGE(IDC_COMBO_CNL1, &CAmbloneDlg::OnCbnSelchangeComboCnl1)
  ON_CBN_SELCHANGE(IDC_COMBO_CNL2, &CAmbloneDlg::OnCbnSelchangeComboCnl2)
  ON_CBN_SELCHANGE(IDC_COMBO_CNL3, &CAmbloneDlg::OnCbnSelchangeComboCnl3)
  ON_CBN_SELCHANGE(IDC_COMBO_CNL4, &CAmbloneDlg::OnCbnSelchangeComboCnl4)
  ON_COMMAND(ID_APP_EXIT, &CAmbloneDlg::OnAppExit)
  ON_CBN_EDITCHANGE(IDC_COMBO1, &CAmbloneDlg::OnComboComsEditChange)
  ON_CBN_CLOSEUP(IDC_COMBO1, &CAmbloneDlg::OnComboComsCloseup)
  ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_THRESHOLD, &CAmbloneDlg::OnNMCustomdrawSliderThreshold)
  ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_ACCENTUATION, &CAmbloneDlg::OnNMCustomdrawSliderAccentuation)
  ON_BN_CLICKED(IDC_CHECK_FLIPRB, &CAmbloneDlg::OnBnClickedCheckFliprb)
  ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_INTENSITY, &CAmbloneDlg::OnNMCustomdrawSliderBrightness)
  ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_RED, &CAmbloneDlg::OnCustomdrawSliderRed)
  ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_GREEN, &CAmbloneDlg::OnCustomdrawSliderGreen)
  ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_BLUE, &CAmbloneDlg::OnCustomdrawSliderBlue)
  ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CAmbloneDlg::OnNMCustomdrawSlider1)
  ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_SWEEPSPEED, &CAmbloneDlg::OnNMCustomdrawSliderSweepspeed)
  ON_BN_CLICKED(IDC_CHECK_LIMIT_LPS, &CAmbloneDlg::OnBnClickedCheckLimitLps)
  ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_LIMIT_LPS, &CAmbloneDlg::OnNMCustomdrawSliderLimitLps)
  ON_MESSAGE(WM_HOTKEY,OnHotKey)
  ON_COMMAND(ID_POPUP_CHECKFORUPDATES, &CAmbloneDlg::OnPopupCheckforupdates)
  ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_MINBRIGHTNESS, &CAmbloneDlg::OnNMCustomdrawSliderMinbrightness)
  ON_BN_CLICKED(IDC_CHECK_UPDATES, &CAmbloneDlg::OnBnClickedCheckUpdates)
  ON_CBN_SELCHANGE(IDC_COMBO_CAPTURE, &CAmbloneDlg::OnCbnSelchangeComboCapture)
  ON_CBN_SELCHANGE(IDC_COMBO_WINDOW, &CAmbloneDlg::OnCbnSelchangeComboWindow)
  ON_CBN_DROPDOWN(IDC_COMBO_WINDOW, &CAmbloneDlg::OnCbnDropdownComboWindow)
  ON_BN_CLICKED(IDC_BUTTON_SHOWOUTPUT, &CAmbloneDlg::OnBnClickedButtonShowoutput)
  ON_BN_CLICKED(IDC_BUTTON_HOTKEYS, &CAmbloneDlg::OnBnClickedButtonHotkeys)
END_MESSAGE_MAP()


// CAmbloneDlg message handlers

BOOL CAmbloneDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set large icon
	SetIcon(m_hIcon, FALSE);		// Set small icon  

  TrayIcon.MinimiseToTray(this);

  // Create new window to show the output colours
  OutputDlg = new OutputDialog(this);
  OutputDlg->Create(IDD_DIALOG_COLOURS, this);

  // Load some settings
  int cp = settings.GetComPort();
  if (cp != -1) {
    pArduino->OpenCOM(cp);
    CString COM_Cstr;
    COM_Cstr.Format("COM%d", cp);
    CComboComs.AddString(COM_Cstr);
    CComboComs.SetCurSel(0);
  }

  // Get the screen capturing method
  int CapModeID = settings.GetCaptureModeID();
  CComboCapture.AddString(WindowsFull.Description);
  CComboCapture.AddString(WindowsWin.Description);
  CComboCapture.AddString(DirectXFull.Description);
  //CComboCapture.AddString(DirectXWin.Description);
  //CComboCapture.AddString(XbmcWin.Description);
  SetCaptureMode(CapModeID);
  CComboCapture.SetCurSel(CapModeID);
  WindowSelected = false;
  OnCbnDropdownComboWindow();

  // Set the precision slider
  SliderPrecision.SetRangeMin(1);
  SliderPrecision.SetRangeMax(PRECISION_MAX);
  SliderPrecision.SetPos(settings.GetPrecision());

  // Set the threshold slider
  SliderThreshold.SetRangeMin(0);
  SliderThreshold.SetRangeMax(100);
  SliderThreshold.SetPos(settings.GetThreshold());

  // Set colour accentuation slider
  SliderAccentuation.SetRangeMin(0);
  SliderAccentuation.SetRangeMax(5);
  SliderAccentuation.SetPos(settings.GetAccentuation());

  // Set the calibration sliders
  SliderRed.SetRangeMin(0);
  SliderRed.SetRangeMax(100);
  SliderRed.SetPos(settings.GetCalibrationRed());
  SliderGreen.SetRangeMin(0);
  SliderGreen.SetRangeMax(100);
  SliderGreen.SetPos(settings.GetCalibrationGreen());
  SliderBlue.SetRangeMin(0);
  SliderBlue.SetRangeMax(100);
  SliderBlue.SetPos(settings.GetCalibrationBlue());
  // And brightness calibration
  SliderBrightness.SetRangeMin(0);
  SliderBrightness.SetRangeMax(100);
  SliderBrightness.SetPos(settings.GetBrightness());

  // Set the limit lps check
  CheckLimitLps.SetCheck(settings.GetDoLimitLps());

  // Set lps limit slider
  SliderLimitLps.SetRangeMin(5);
  SliderLimitLps.SetRangeMax(50);
  SliderLimitLps.SetPos(settings.GetLpsLimit());
  OnBnClickedCheckLimitLps();

  // Set process priority slider
  SliderPriority.SetRangeMin(1);
  SliderPriority.SetRangeMax(6);
  int prior = settings.GetProcessPriority();
  switch (prior) {
    case IDLE_PRIORITY_CLASS:
      SliderPriority.SetPos(1);
      break;
    case BELOW_NORMAL_PRIORITY_CLASS:
      SliderPriority.SetPos(2);
      break;
    case NORMAL_PRIORITY_CLASS:
      SliderPriority.SetPos(3);
      break;
    case ABOVE_NORMAL_PRIORITY_CLASS:
      SliderPriority.SetPos(4);
      break;
    case HIGH_PRIORITY_CLASS:
      SliderPriority.SetPos(5);
      break;
    case REALTIME_PRIORITY_CLASS:
      SliderPriority.SetPos(6);
      break;
  }
  SetPriorityClass(GetCurrentProcess(), prior);

  // Set colour sweep speed slider
  SliderSweepSpeed.SetRangeMin(-10);
  SliderSweepSpeed.SetRangeMax(10);
  SliderSweepSpeed.SetPos(settings.GetSweepSpeed());

  // Set minimal brightness slider
  SliderMinBrightness.SetRangeMin(0);
  SliderMinBrightness.SetRangeMax(100);
  SliderMinBrightness.SetPos(settings.GetMinBrightness());

  // Load  the channel settings
  switch (settings.GetChannels()) {
    case 1:
      break;
    case 2:
      CheckC2.SetCheck(true);
      CheckC3.EnableWindow(true);
      break;
    case 3:
      CheckC2.SetCheck(true);
      CheckC3.SetCheck(true);
      CheckC3.EnableWindow(true);
      CheckC4.EnableWindow(true);
      break;
    case 4:
      CheckC2.SetCheck(true);
      CheckC3.SetCheck(true);
      CheckC3.EnableWindow(true);
      CheckC4.SetCheck(true);
      CheckC4.EnableWindow(true);
      break;
  }
  SetComboCs();

  // Set the Flip red and blue values check box
  CheckFlipRB.SetCheck(settings.GetFlipRB());

  // Set the indexes for the ComboCs with the screen sections
  LoadScreenSectionSettings();

  // Load the static colours
  rgbSTATIC[0] = settings.GetStaticColour(1);
  rgbSTATIC[1] = settings.GetStaticColour(2);
  rgbSTATIC[2] = settings.GetStaticColour(3);
  rgbSTATIC[3] = settings.GetStaticColour(4);

  // Automatic check new version check
  CheckUpdates.SetCheck(settings.GetUpdate());

  // Initialize sweep colour to red
  rgbSWEEP = RGB(255, 0, 0);
  SweepState = SW_RED_DOWN_GREEN_UP;

  // Once everything is initialized, start the calculation thread
  StartRunning();

  // Set a timer to measure the performance
  loopc = 0;
  SetTimer(T_LPS, 1000, 0);

  // Set timer for colour output dialog
  SetTimer(T_DLG, 200, 0);

  // Register the hotkeys
  InitHotkeys();

  // Load the XBMC settings
  CEditXbmcIP.SetWindowTextA(settings.GetXbmcIP());
  int iport = settings.GetXbmcPort();
  char *port = new char[32];
  sprintf(port, "%d", iport);
  CString cport = port;
  CEditXbmcPort.SetWindowTextA(cport);

  return TRUE;  // return TRUE unless you set the focus to a control
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::InitHotkeys() {
  // Build up the hotkey vector
  // Order is important!!!
  UINT key;
  UINT mod;

  // Display 1
  key = settings.GetHotkeyKey(HOTKEY_DISPLAY_1);
  mod = settings.GetHotkeyMod(HOTKEY_DISPLAY_1);
  if (key == 0 && mod == 0) {
    key = '1';
    mod = MOD_CONTROL | MOD_SHIFT;
    // Set default value
    settings.SetHotkeyKey(HOTKEY_DISPLAY_1, key);
    settings.SetHotkeyMod(HOTKEY_DISPLAY_1, mod);
  }
  HotKeyMapping *HKM;
  HKM = new HotKeyMapping;
  HKM->ID = HOTKEY_DISPLAY_1;
  HKM->Modifiers = mod;
  HKM->Key = key;
  HKM->IsSet = true;
  HotKeys.push_back(HKM);

  // Display 2
  key = settings.GetHotkeyKey(HOTKEY_DISPLAY_2);
  mod = settings.GetHotkeyMod(HOTKEY_DISPLAY_2);
  if (key == 0 && mod == 0) {
    key = '2';
    mod = MOD_CONTROL | MOD_SHIFT;
    // Set default value
    settings.SetHotkeyKey(HOTKEY_DISPLAY_2, key);
    settings.SetHotkeyMod(HOTKEY_DISPLAY_2, mod);
  }
  HKM = new HotKeyMapping;
  HKM->ID = HOTKEY_DISPLAY_2;
  HKM->Modifiers = mod;
  HKM->Key = key;
  HKM->IsSet = true;
  HotKeys.push_back(HKM);

  // Static colour
  key = settings.GetHotkeyKey(HOTKEY_STATIC_COLOUR);
  mod = settings.GetHotkeyMod(HOTKEY_STATIC_COLOUR);
  if (key == 0 && mod == 0) {
    key = 'Q';
    mod = MOD_CONTROL | MOD_SHIFT;
    // Set default value
    settings.SetHotkeyKey(HOTKEY_STATIC_COLOUR, key);
    settings.SetHotkeyMod(HOTKEY_STATIC_COLOUR, mod);
  }
  HKM = new HotKeyMapping;
  HKM->ID = HOTKEY_STATIC_COLOUR;
  HKM->Modifiers = mod;
  HKM->Key = key;
  HKM->IsSet = true;
  HotKeys.push_back(HKM);

  // Colour sweep
  key = settings.GetHotkeyKey(HOTKEY_SWEEP);
  mod = settings.GetHotkeyMod(HOTKEY_SWEEP);
  if (key == 0 && mod == 0) {
    key = 'W';
    mod = MOD_CONTROL | MOD_SHIFT;
    // Set default value
    settings.SetHotkeyKey(HOTKEY_SWEEP, key);
    settings.SetHotkeyMod(HOTKEY_SWEEP, mod);
  }
  HKM = new HotKeyMapping;
  HKM->ID = HOTKEY_SWEEP;
  HKM->Modifiers = mod;
  HKM->Key = key;
  HKM->IsSet = true;
  HotKeys.push_back(HKM);

  // Fake TV
  key = settings.GetHotkeyKey(HOTKEY_FAKE_TV);
  mod = settings.GetHotkeyMod(HOTKEY_FAKE_TV);
  if (key == 0 && mod == 0) {
    key = 'E';
    mod = MOD_CONTROL | MOD_SHIFT;
    // Set default value
    settings.SetHotkeyKey(HOTKEY_FAKE_TV, key);
    settings.SetHotkeyMod(HOTKEY_FAKE_TV, mod);
  }
  HKM = new HotKeyMapping;
  HKM->ID = HOTKEY_FAKE_TV;
  HKM->Modifiers = mod;
  HKM->Key = key;
  HKM->IsSet = true;
  HotKeys.push_back(HKM);

  // Actually register the hotkeys
  for (int i = 0; i < HotKeys.size(); i++)
    RegisterHotKey(this->m_hWnd,
      HotKeys[i]->ID,
      HotKeys[i]->Modifiers,
      HotKeys[i]->Key
    );

  // Register some hotkeys
  /*RegisterHotKey(this->m_hWnd, KEY_1, MOD_CONTROL | MOD_SHIFT, '1');
  RegisterHotKey(this->m_hWnd, KEY_2, MOD_CONTROL | MOD_SHIFT, '2');
  RegisterHotKey(this->m_hWnd, KEY_Q, MOD_CONTROL | MOD_SHIFT, 'Q');
  RegisterHotKey(this->m_hWnd, KEY_W, MOD_CONTROL | MOD_SHIFT, 'W');
  RegisterHotKey(this->m_hWnd, KEY_E, MOD_CONTROL | MOD_SHIFT, 'E');*/
  HotKeyDlgOpen = false;
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::HotkeyWinClose() {
  HotKeyDlgOpen = false;
}
//-------------------------------------------------------------------------------------------------------

#if LOGGING
void CAmbloneDlg::InitLogging() {
  time_t now = time(NULL);
  char buffer[80];
  CString filename = (CString) "Log " + now.ToString() + ".txt";
  LogFile.open(filename, fstream::out);
  strftime(buffer, 80, "%c", localtime(&now));
  Log("Amblone log file, " + (CString) buffer);
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::Log(CString msg) {
  time_t now = time(NULL);
  char buffer[80];
  strftime(buffer, 80, "%x %X", localtime(&now));
  LogFile << (CString) buffer + (CString) " - " + msg << endl;
}
//-------------------------------------------------------------------------------------------------------
#endif

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAmbloneDlg::LoadScreenSectionSettings() {
  #if LOGGING
  Log(FunctionName);
  #endif
  ComboC1.SetCurSel(settings.GetScreenSection(1));
  ComboC2.SetCurSel(settings.GetScreenSection(2));
  ComboC3.SetCurSel(settings.GetScreenSection(3));
  ComboC4.SetCurSel(settings.GetScreenSection(4));
}

void CAmbloneDlg::OnPaint()
{
	CDialog::OnPaint();
}
//-------------------------------------------------------------------------------------------------------

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAmbloneDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnTimer(UINT_PTR nIDEvent) {
  if (nIDEvent == T_LPS) {
    #if LOGGING
    Log(FunctionName + " (LPS)");
    #endif
    CString Text;
    Text.Format("Loops per second: %d lps", loopc);
    StaticLoops.SetWindowTextA(Text);

    if (settings.GetDoLimitLps() && NeedScreenColours()) {
      // See if we need to increase or decrease the SleepTime value
      // First look for a large difference
      if (loopc < (settings.GetLpsLimit() - 5) || loopc > (settings.GetLpsLimit() + 5)) {
        int CurrentMsPerLoop = loopc ? (1000 / loopc) - SleepTime : 0;
        int WantedMsPerLoop = 1000 / settings.GetLpsLimit();
        SleepTime = WantedMsPerLoop - CurrentMsPerLoop;
        if (SleepTime > 500) SleepTime = 0;
      }
      // There's just a small difference, tweak the sleeptime if necessary
      if (loopc < (settings.GetLpsLimit() - 1) && SleepTime > 0) SleepTime--;
      else if (loopc > settings.GetLpsLimit()) SleepTime++;
    }
    loopc = 0;
  }
  else if (nIDEvent == T_DLG) {
    #if LOGGING
    Log(FunctionName + " (Colours)");
    #endif
    UpdateOutputDlg = true;
  }

  CDialog::OnTimer(nIDEvent);
}

void CAmbloneDlg::StartRunning() {
  #if LOGGING
  Log(FunctionName);
  #endif
  KeepRunning = true;
  hThread = CreateThread(NULL, 0, Thread, this, 0, NULL);
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::Restart() {
  #if LOGGING
  Log(FunctionName);
  #endif
  restart = true;
  KeepRunning = false;
  Sleep(100);
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::StopRunning() {
  #if LOGGING
  Log(FunctionName);
  #endif
  //TerminateThread(hThread, 0);
  restart = false;
  KeepRunning = false;
  Sleep(250);
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnBnClickedOk()
{
  TrayIcon.MinimiseToTray(this);
}
//-------------------------------------------------------------------------------------------------------

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam) {
  return thisDialog->AmbloneEnumWindowsProc(hWnd, lParam);
}
BOOL CALLBACK CAmbloneDlg::AmbloneEnumWindowsProc(HWND hWnd, LPARAM lParam) {
	char String[255];
  if (!hWnd)
	  return TRUE;		// Not a window
  if (!::IsWindowVisible(hWnd))
	  return TRUE;		// Not visible
  if (!::SendMessage(hWnd, WM_GETTEXT, sizeof(String), (LPARAM)String))
	  return TRUE;		// No window title
  WindowInfo NewInfo;
  NewInfo.Handle = hWnd;
  NewInfo.hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
  strcpy(NewInfo.Name, String);
  vWin.push_back(NewInfo);
  return TRUE;
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::BuildWindowsList() {
  vWin.clear();
  EnumWindows(EnumWindowsProc, NULL);
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::ShowWindows() {
  CString WinStr = "";
  for (int i = 0; i < vWin.size(); i++) {
    WinStr += vWin[i].Name;
    WinStr += "\n";
  }
  AfxMessageBox(WinStr);
}
//-------------------------------------------------------------------------------------------------------

UINT CAmbloneDlg::MainLoopFunc() {
  #if LOGGING
  Log(FunctionName);
  #endif
  int r = 0, g = 0, b = 0;
  int ExceptionCount = 0;

  do {
    try {
      ZeroMemory(&dd, sizeof(dd));
      dd.cb = sizeof(dd);
      CurDispDev = settings.GetDisplayDeviceNum();
      KeepRunning = true;

      if (EnumDisplayDevices(NULL, CurDispDev, &dd, NULL)) {
        // We found the monitor, now get the screen DC
        hScrDC = CreateDC(NULL,dd.DeviceName,NULL,NULL);

        // Get the screen resolution
        xScrn = GetDeviceCaps(hScrDC, HORZRES);     
        yScrn = GetDeviceCaps(hScrDC, VERTRES);

        // Create a bitmap compatible with the screen DC   
        hBitmap = CreateCompatibleBitmap(hScrDC, xScrn, yScrn);

        // Create a memory DC compatible to screen DC
        hMemDC = CreateCompatibleDC(hScrDC);

        // Create a rectangle
        GetClientRect(&rect);

        pDC = GetDC(); // Get the DC

        FirstTime = true;

        SleepTime = 0;

        do {
          // Calculate the average screen pixel colour for all corners
          if (NeedScreenColours()) GetAverageRGB(settings.GetPrecision(), settings.GetThreshold());
          else Sleep(35); // This will achieve an lps of around 28

          // Now extract the colours for the channels from the corners
          GetChannelsRGB(1);
          if (CheckC2.GetCheck()) GetChannelsRGB(2);
          if (CheckC3.GetCheck()) GetChannelsRGB(3);
          if (CheckC4.GetCheck()) GetChannelsRGB(4);

          // Perform some calculations on rgb to optimize the output
          if (!settings.GetSkipPostProcessing()) {

            // Don't do any PostProcessing if the channel is not selected, or if it is set to a static colour
            PostProcessRGB(1, settings.GetAccentuation());
            if (CheckC2.GetCheck()) PostProcessRGB(2, settings.GetAccentuation());
            if (CheckC3.GetCheck()) PostProcessRGB(3, settings.GetAccentuation());
            if (CheckC4.GetCheck()) PostProcessRGB(4, settings.GetAccentuation());
          }

          // Now output the colour
          OutputColour();

          loopc++;

          // Sleep to keep the lps low
          ExceptionCount = 3;
          bool DoSleep = settings.GetDoLimitLps() && SleepTime && NeedScreenColours();
          if (DoSleep) {
            #if LOGGING
            Log("Before sleep: " + SleepTime);
            #endif
            Sleep(SleepTime);
            #if LOGGING
            Log("After sleep");
            #endif
          }

          // Set exceptions to 0
          ExceptionCount = 0;

        } while (KeepRunning);
      }
      DeleteDC(hScrDC);
      DeleteDC(hMemDC);
      if (pbPixels) {
        delete pbPixels;
        pbPixels = NULL;
      }
    }
    catch (...) {
      ExceptionCount++;
      if (ExceptionCount >= 3) {
        // We only stop and show a message when something goes wrong multiple times
        MessageBox("An error occurred while running Amblone.\nChange your settings to try again.",
          "Amblone", MB_ICONERROR | MB_OK);
        while (KeepRunning) Sleep(100);
      }
      else Sleep(200);
    }
  } while (restart);
  return 0;
}
//-------------------------------------------------------------------------------------------------------

bool CAmbloneDlg::NeedScreenColours() {
  #if LOGGING
  Log(FunctionName);
  #endif
  // Check to see if we need to capture the screen Bitmap
  // Maybe all channels are set to static colour of sweep
  // in which case capturing the screen is not necessary
  
  if (ComboC1.GetCurSel() != STATIC_COLOUR && ComboC1.GetCurSel() != SWEEP && ComboC1.GetCurSel() != FAKE_TV) return true;

  if (CheckC2.GetCheck() && // channel 2 is used
    (ComboC2.GetCurSel() != STATIC_COLOUR && ComboC2.GetCurSel() != SWEEP && ComboC2.GetCurSel() != FAKE_TV)) return true;

  if (CheckC3.GetCheck() && // channel 3 is used
    (ComboC3.GetCurSel() != STATIC_COLOUR && ComboC3.GetCurSel() != SWEEP && ComboC3.GetCurSel() != FAKE_TV)) return true;

  if (CheckC4.GetCheck() && // channel 4 is used
    (ComboC4.GetCurSel() != STATIC_COLOUR && ComboC4.GetCurSel() != SWEEP && ComboC4.GetCurSel() != FAKE_TV)) return true;

  return false; // we do not need the screen bitmap
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::GetAverageRGB(int precision, int colour_treshold) {
  #if LOGGING
  Log(FunctionName);
  #endif
  // Capture the screen
  CaptureScreen();
  // Call GetAverageRGBquarters for all 4 quarters
  for (int i = 1; i <= 4; i++) GetAverageRGBquarters(precision, colour_treshold, i);
}
//-------------------------------------------------------------------------------------------------------


void CAmbloneDlg::CaptureScreen() {
  #if LOGGING
  Log(FunctionName);
  #endif
  if (CurCapture.XBMC) {
    if (pXBMC == NULL) {
      pXBMC = new XBMC("localhost", 80);
    }
  }
  else if (CurCapture.DirectX) {
    if (FirstTime) {
      if (DirectX) delete DirectX;
      DirectX = new AmbloneDirectX();
      DirectX->Initialize(CurDispDev);
      FirstTime = false;
      BytesPerPixel = 4;
    }
    pbPixels = DirectX->GetScreenBitmap();
  }
  else {    
    // Copy the screen into our Bitmap handle
    CopyScreenToBitmap();

    // Now get the actual Bitmap
    GetObject(hBitmap, sizeof(BITMAP), &bmp);
    
    // The first time this function is called, allocate the buffer memory
    if (FirstTime) {
      // Calculate the size the buffer needs to be
      cbBuffer = bmp.bmWidthBytes * bmp.bmHeight;
      // Allocate
      if (pbPixels) {
        delete pbPixels;
        pbPixels = NULL;
      }
      pbPixels = new BYTE[cbBuffer];

      // The amount of bytes per pixel is the amount of bits divided by 8
      BytesPerPixel = bmp.bmBitsPixel / 8;

      FirstTime = false;    
    }

    // Get the actual RGB data and put it into pbPixels
    GetBitmapBits(hBitmap, cbBuffer, pbPixels);
  }
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::GetAverageRGBquarters(int precision, int colour_treshold, int quarter) {
  #if LOGGING
  Log(FunctionName);
  #endif
  // This function calculates the average RGB value on the screen

  // precision: 1 in how many pixels is used to calculate the RGB average

  // colour_treshold: If R, G and B values of a pixel are under colour_treshold,
  //                  this pixel will not be taken into account

  // We calculate the average for every corner of the screen
  // so we can later add them to get upper half, left half etc...

  // +----------+----------+
  // |          |          |
  // |    Q1    |    Q2    |
  // |          |          |
  // +----------+----------+
  // |          |          |
  // |    Q3    |    Q4    |
  // |          |          |
  // +----------+----------+

  r=0, g=0, b=0; // Values to hold the RGB averages
  long count=0; // count the amount of pixels taken into account

  if (CurCapture.XBMC) {
    if (!settings.GetFlipRB()) {
      rgbQ[quarter-1] = pXBMC->GetQuarterColour(quarter);
    }
    else {
      COLORREF c = pXBMC->GetQuarterColour(quarter);
      rgbQ[quarter-1] = RGB(GetBValue(c), GetGValue(c), GetRValue(c));
    }
    return;
  }

  int nb, ng, nr; // The RGB values we are reading
  int index = 0; // Index of the selected pixel in pbPixels

  // Now determine which section of the screen we're going to loop over
  int jbegin, jend, ibegin, iend;
  switch (quarter) {
    case 1:
      jbegin = 0;
      jend = yScrn / 2;
      ibegin = 0;
      iend = xScrn / 2;
      break;
    case 2:
      jbegin = 0;
      jend = yScrn / 2;
      ibegin = xScrn / 2;
      iend = xScrn;
      break;
    case 3:
      jbegin = yScrn / 2;
      jend = yScrn;
      ibegin = 0;
      iend = xScrn / 2;
      break;
    case 4:
      jbegin = yScrn / 2;
      jend = yScrn;
      ibegin = xScrn / 2;
      iend = xScrn;
      break;
  }

  // This is where all the magic happens: calculate the average RGB
  for (int j = jbegin; j < jend; j += precision) {
    for (int i = ibegin; i < iend; i += precision) {
      // Calculate new index value
      index = (BytesPerPixel * j * xScrn) + (BytesPerPixel * i);

      //try {
        // Get RGB values (stored in reversed order)
        nb = pbPixels[index];
        ng = pbPixels[index+1];
        nr = pbPixels[index+2];
      /*}
      catch (...) {
        Restart();
        return;
      }*/

      if ((nb + ng + nr) > colour_treshold) {
        // The RGB value passed the treshold criterium, so add it to the average
        b += nb;
        g += ng;
        r += nr;
        count++; // Remember we have used this pixel
      }
    }
  }
  
  // If count is less than mincount, we will fill up
  // the remaining counts with black until we have mincount
  mincount = (yScrn / (8 * precision)) * (xScrn / (8 * precision));
  if (count < mincount) {
    b /= mincount;
    g /= mincount;
    r /= mincount;    
  }
  else {
    // Devide by the amount of pixels read
    // to get the actual average in the 0-255 range
    b /= count ? count : 1;
    g /= count ? count : 1;
    r /= count ? count : 1;
  }

  // Assign values to the rgb struct and return, flip Red and Blue if selected
  if (!settings.GetFlipRB()) rgbQ[quarter-1] = RGB(r, g, b);
  else rgbQ[quarter-1] = RGB(b, g, r);
  
  return;
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::CopyScreenToBitmap() {
  #if LOGGING
  Log(FunctionName);
  #endif
  // This function is largely copied from Barry's Screen Capture
  // http://www.codeproject.com/KB/GDI/barry_s_screen_capture.aspx
  // Thanks Barry!!!

  // If in single window mode, but no window selected, go to fullscreen
  if (CurCapture.FullScreen || !WindowSelected) {
    // select new bitmap into memory DC
    hOldBitmap = SelectObject (hMemDC, hBitmap);
    // BitBlt screen DC to memory DC
    BitBlt(hMemDC, 0, 0, xScrn, yScrn, hScrDC, 0, 0, SRCCOPY);
  }
  else {
    // Get the window name
    CString WinStr;
    CComboWindow.GetWindowTextA(WinStr);
    // See if the index doesn't exceed the size of the array
    if (SelectedWin >= vWin.size()) return;
    // Get handle to the window
    HWND SelWin = vWin[SelectedWin].Handle;
    RECT WinRect;
    ::GetWindowRect(SelWin, &WinRect);
    int xNew = WinRect.right - WinRect.left;
    int yNew = WinRect.bottom - WinRect.top;
    if (xNew != xScrn || yNew != yScrn) {
      yScrn = yNew;
      xScrn = xNew;
      FirstTime = true;
      // Create a bitmap compatible with the screen DC   
      hBitmap = CreateCompatibleBitmap(::GetWindowDC(SelWin), xScrn, yScrn);

      // Create a memory DC compatible to screen DC
      hMemDC = CreateCompatibleDC(::GetWindowDC(SelWin));
    }
    // select new bitmap into memory DC
    hOldBitmap = SelectObject (hMemDC, hBitmap);
    BitBlt(hMemDC, 0, 0, xScrn, yScrn, ::GetWindowDC(SelWin), 0, 0, SRCCOPY);
  }

  // Select old bitmap back into memory DC and get handle to     
  // bitmap of the screen   
  hBitmap = (HBITMAP) SelectObject(hMemDC, hOldBitmap);

  return;
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::GetChannelsRGB(int channel) {
  #if LOGGING
  Log(FunctionName);
  #endif
  CComboBox *Combo;

  switch (channel) {
    case 1:
      Combo = &ComboC1;
      break;
    case 2:
      Combo = &ComboC2;
      break;
    case 3:
      Combo = &ComboC3;
      break;
    case 4:
      Combo = &ComboC4;
      break;
  }

  switch (Combo->GetCurSel()) {
    case ENTIRE_SCREEN:
      // Average of all quarters
      rgbC[channel-1] = RGB(
        (GetRValue(rgbQ[0]) + GetRValue(rgbQ[1]) + GetRValue(rgbQ[2]) + GetRValue(rgbQ[3])) / 4,
        (GetGValue(rgbQ[0]) + GetGValue(rgbQ[1]) + GetGValue(rgbQ[2]) + GetGValue(rgbQ[3])) / 4,
        (GetBValue(rgbQ[0]) + GetBValue(rgbQ[1]) + GetBValue(rgbQ[2]) + GetBValue(rgbQ[3])) / 4);
      break;
    case UPPER_HALF:
      // Average of quarter 1 and 2
      rgbC[channel-1] = RGB(
        (GetRValue(rgbQ[0]) + GetRValue(rgbQ[1])) / 2,
        (GetGValue(rgbQ[0]) + GetGValue(rgbQ[1])) / 2,
        (GetBValue(rgbQ[0]) + GetBValue(rgbQ[1])) / 2);
      break;
    case LOWER_HALF:
      // Average of quarter 3 and 4
      rgbC[channel-1] = RGB(
        (GetRValue(rgbQ[2]) + GetRValue(rgbQ[3])) / 2,
        (GetGValue(rgbQ[2]) + GetGValue(rgbQ[3])) / 2,
        (GetBValue(rgbQ[2]) + GetBValue(rgbQ[3])) / 2);
      break;
    case LEFT_HALF:
      // Average of quarter 1 and 3
      rgbC[channel-1] = RGB(
        ((int)GetRValue(rgbQ[0]) + (int)GetRValue(rgbQ[2])) / 2,
        ((int)GetGValue(rgbQ[0]) + (int)GetGValue(rgbQ[2])) / 2,
        ((int)GetBValue(rgbQ[0]) + (int)GetBValue(rgbQ[2])) / 2);
      break;
    case RIGHT_HALF:
      // Average of quarter 2 and 4
      rgbC[channel-1] = RGB(
        ((int)GetRValue(rgbQ[1]) + (int)GetRValue(rgbQ[3])) / 2,
        ((int)GetGValue(rgbQ[1]) + (int)GetGValue(rgbQ[3])) / 2,
        ((int)GetBValue(rgbQ[1]) + (int)GetBValue(rgbQ[3])) / 2);
      break;
    case UPPER_LEFT:
      // Copy quarter 1
      rgbC[channel-1] = RGB(
        GetRValue(rgbQ[0]),
        GetGValue(rgbQ[0]),
        GetBValue(rgbQ[0]));
      break;
    case UPPER_RIGHT:
      // Copy quarter 2
      rgbC[channel-1] = RGB(
        GetRValue(rgbQ[1]),
        GetGValue(rgbQ[1]),
        GetBValue(rgbQ[1]));
      break;
    case LOWER_LEFT:
      // Copy quarter 3
      rgbC[channel-1] = RGB(
        GetRValue(rgbQ[2]),
        GetGValue(rgbQ[2]),
        GetBValue(rgbQ[2]));
      break;
    case LOWER_RIGHT:
      // Copy quarter 4
      rgbC[channel-1] = RGB(
        GetRValue(rgbQ[3]),
        GetGValue(rgbQ[3]),
        GetBValue(rgbQ[3]));
      break;
    case STATIC_COLOUR:
      // Use the selected colour
      rgbC[channel-1] = rgbSTATIC[channel-1];
      break;
    case SWEEP:
      SetSweepRGB();
      rgbC[channel-1] = rgbSWEEP;
      break;
    case FAKE_TV:
      if (channel == 1) iFakeTV.Tick(28);
      rgbC[channel-1] = iFakeTV.GetColour();
      break;
    default:
      rgbC[channel-1] = RGB(0, 0, 0);
      break;
  }
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::SetSweepRGB() {
  #if LOGGING
  Log(FunctionName);
  #endif
  // If the SweepSpeed is larger than 0, we will change the RGB value by that amount every loopcycle
  // If it is 0, we will test if the loop is module 2, and change the RGB values is it is
  // For everything smaller than 0, we do the loopcount modulo 2 - SweepSpeed

  bool SweepSpeedSlow = false;
  int StepSize = 1;
  int ModuloValue;
  int SweepSpeed = settings.GetSweepSpeed();
  if (SweepSpeed < 1) {
    SweepSpeedSlow = true;
    ModuloValue = 2 - SweepSpeed;
  }

  if (!SweepSpeedSlow || !(loopc % ModuloValue)) {
    switch (SweepState) {
      case SW_RED_DOWN_GREEN_UP:
        if (GetGValue(rgbSWEEP) < 256 - StepSize)
          rgbSWEEP = RGB(GetRValue(rgbSWEEP) - StepSize, GetGValue(rgbSWEEP) + StepSize, GetBValue(rgbSWEEP));
        else
          SweepState = SW_GREEN_DOWN_BLUE_UP;
        break;
      case SW_GREEN_DOWN_BLUE_UP:
        if (GetBValue(rgbSWEEP) < 256 - StepSize)
          rgbSWEEP = RGB(GetRValue(rgbSWEEP), GetGValue(rgbSWEEP) - StepSize, GetBValue(rgbSWEEP) + StepSize);
        else
          SweepState = SW_BLUE_DOWN_RED_UP;
        break;
      case SW_BLUE_DOWN_RED_UP:
        if (GetRValue(rgbSWEEP) < 256 - StepSize)
          rgbSWEEP = RGB(GetRValue(rgbSWEEP) + StepSize, GetGValue(rgbSWEEP), GetBValue(rgbSWEEP) - StepSize);
        else
          SweepState = SW_RED_DOWN_GREEN_UP;
        break;
      default:
        SweepState = SW_RED_DOWN_GREEN_UP;
        rgbSWEEP = RGB(255, 0, 0);
        break;
    }
  }
}

void CAmbloneDlg::PostProcessRGB(int channel, int accentuation) {
  #if LOGGING
  Log(FunctionName);
  #endif
  // Get the RGB values
  int r = GetRValue(rgbC[channel-1]);
  int g = GetGValue(rgbC[channel-1]);
  int b = GetBValue(rgbC[channel-1]);

  // Don't apply certain transformations in the case of static colours and colour sweep
  bool process = true;
  switch (channel) {
    case 1:
      if (ComboC1.GetCurSel() == STATIC_COLOUR || ComboC1.GetCurSel() == SWEEP) process = false;
      break;
    case 2:
      if (ComboC2.GetCurSel() == STATIC_COLOUR || ComboC2.GetCurSel() == SWEEP) process = false;
      break;
    case 3:
      if (ComboC3.GetCurSel() == STATIC_COLOUR || ComboC3.GetCurSel() == SWEEP) process = false;
      break;
    case 4:
      if (ComboC4.GetCurSel() == STATIC_COLOUR || ComboC4.GetCurSel() == SWEEP) process = false;
      break;
  }

  if (process) {
    // Only do the following if we haven't chosen a static colour or sweep

    // Here we want to enlarge the difference between the R, G and B value
    if (r >= g && r >= b) {
      // Red is the most dominant
      if (g >= b) {
        // Red > Green > Blue
        r += accentuation * (r - g);
        b -= accentuation * (g - b);
      }
      else {
        // Red > Blue > Green
        r += accentuation * (r - b);
        g -= accentuation * (b - g);
      }
    }
    else if (g >= b && g >= r) {
      // Green is the most dominant
      if (r >= b) {
        // Green > Red > Blue
        g += accentuation * (g - r);
        b -= accentuation * (r - b);
      }
      else {
        // Green > Blue > Red
        g += accentuation * (g - b);
        r -= accentuation * (b - r);
      }
    }
    else {
      // Blue is the most dominant
      if (r >= g) {
        // Blue > Red > Green
        b += accentuation * (b - r);
        g -= accentuation * (r - g);
      }
      else {
        // Blue > Green > Red
        b += accentuation * (b - g);
        r -= accentuation * (g - r);
      }
    }
  }

  // Now apply calibration
  r = (r * settings.GetCalibrationRed()) / 100;
  g = (g * settings.GetCalibrationGreen()) / 100;
  b = (b * settings.GetCalibrationBlue()) / 100;
  // And the brightness
  r = (r * settings.GetBrightness()) / 100;
  g = (g * settings.GetBrightness()) / 100;
  b = (b * settings.GetBrightness()) / 100;

  // Calculate minimal brightness of each colour
  // We do some rounding off here because of the possibly low values
  int MinRed = ((float) settings.GetMinBrightness() * (float) settings.GetCalibrationRed() / 100) + 0.5;
  int MinGreen = ((float) settings.GetMinBrightness() * (float) settings.GetCalibrationGreen() / 100) + 0.5;
  int MinBlue = ((float) settings.GetMinBrightness() * (float) settings.GetCalibrationBlue() / 100) + 0.5;

  // Check red bounds
  if (r > 255) r = 255;
  else if (process && r < MinRed) r = MinRed;
  else if (r < 0) r = 0;
  
  // Check green bounds
  if (g > 255) g = 255;
  else if (process && g < MinGreen) g = MinGreen;
  else if (g < 0) g = 0;
  
  // Check blue bounds
  if (b > 255) b = 255;
  else if (process && b < MinBlue) b = MinBlue;
  else if (b < 0) b = 0;

  // Store RGB values back
  rgbC[channel-1] = RGB(r, g, b);
  return;
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OutputColour() {
  #if LOGGING
  Log(FunctionName);
  #endif
  // This function outputs the calculated average RGB values  
  // Send colours to Dialog
  if (OutputDlg) {
    if (OutputDlg->IsWindowVisible() && UpdateOutputDlg) {
      OutputDlg->Left = RGB(
        GetRValue(rgbQ[0]) / 2 + GetRValue(rgbQ[2]) / 2,
        GetGValue(rgbQ[0]) / 2 + GetGValue(rgbQ[2]) / 2,
        GetBValue(rgbQ[0]) / 2 + GetBValue(rgbQ[2]) / 2
      );
      OutputDlg->Right = RGB(
        GetRValue(rgbQ[1]) / 2 + GetRValue(rgbQ[3]) / 2,
        GetGValue(rgbQ[1]) / 2 + GetGValue(rgbQ[3]) / 2,
        GetBValue(rgbQ[1]) / 2 + GetBValue(rgbQ[3]) / 2
      );
      OutputDlg->Top = RGB(
        GetRValue(rgbQ[0]) / 2 + GetRValue(rgbQ[1]) / 2,
        GetGValue(rgbQ[0]) / 2 + GetGValue(rgbQ[1]) / 2,
        GetBValue(rgbQ[0]) / 2 + GetBValue(rgbQ[1]) / 2
      );
      OutputDlg->Bottom = RGB(
        GetRValue(rgbQ[2]) / 2 + GetRValue(rgbQ[3]) / 2,
        GetGValue(rgbQ[2]) / 2 + GetGValue(rgbQ[3]) / 2,
        GetBValue(rgbQ[2]) / 2 + GetBValue(rgbQ[3]) / 2
      );
      OutputDlg->TopLeft = RGB(
        GetRValue(rgbQ[0]),
        GetGValue(rgbQ[0]),
        GetBValue(rgbQ[0])
      );
      OutputDlg->TopRight = RGB(
        GetRValue(rgbQ[1]),
        GetGValue(rgbQ[1]),
        GetBValue(rgbQ[1])
      );
      OutputDlg->BottomLeft = RGB(
        GetRValue(rgbQ[2]),
        GetGValue(rgbQ[2]),
        GetBValue(rgbQ[2])
      );
      OutputDlg->BottomRight = RGB(
        GetRValue(rgbQ[3]),
        GetGValue(rgbQ[3]),
        GetBValue(rgbQ[3])
      );
      /*OutputDlg->Left = RGB(
        0,
        255,
        0
      );
      OutputDlg->Right = RGB(
        0,
        255,
        0
      );
      OutputDlg->Top = RGB(
        0,
        255,
        0
      );
      OutputDlg->Bottom = RGB(
        0,
        255,
        0
      );
      OutputDlg->TopLeft = RGB(
        0,
        255,
        0
      );
      OutputDlg->TopRight = RGB(
        0,
        255,
        0
      );
      OutputDlg->BottomLeft = RGB(
        0,
        255,
        0
      );
      OutputDlg->BottomRight = RGB(
        0,
        255,
        0
      );*/
      OutputDlg->DrawColours();
      UpdateOutputDlg = false;
    }
  }

  // Write to USB Port
  pArduino->SendRGB(settings.GetChannels(), rgbC);
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnCbnSelchangeCombo1()
{
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnCbnDropdownCombo1()
{
  pArduino->CloseCOM();
  CString COM_Cstr;
  // Remember the currently selected COM
  CComboComs.GetWindowTextA(COM_Cstr);

  // Reset the content of the Combo Box
  CComboComs.ResetContent();

  // Set the selected COM back
  //CComboComs.AddString(COM_Cstr);
  CComboComs.SetWindowTextA(COM_Cstr);

  // Now search for other available COM ports
  for (int i = 1; i <= 64; i++) {
    COM_Cstr.Format("\\\\.\\COM%d", i);
    const HANDLE h = CreateFile(COM_Cstr,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);
    if (h != INVALID_HANDLE_VALUE) {
      COM_Cstr.Format("COM%d", i);
      CComboComs.AddString(COM_Cstr);
      CloseHandle(h);
    }
  }
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnConfig() {
  TrayIcon.MaximiseFromTray(this);
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnNMCustomdrawSliderPrecision(NMHDR *pNMHDR, LRESULT *pResult)
{
  LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
  
  // Save the precision
  settings.SetPrecision(SliderPrecision.GetPos());

  *pResult = 0;
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnBnClickedCheckCnl2()
{
  if (CheckC2.GetCheck()) {
    // The box is checked, enable the next Check Box
    CheckC3.EnableWindow(true);
  }
  else {
    // The box is not checked, next Check Boxes
    CheckC3.EnableWindow(false);
    CheckC3.SetCheck(false);
    CheckC4.EnableWindow(false);
    CheckC4.SetCheck(false);
  }
  SetComboCs();
  UpdateChannelSettings();
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnBnClickedCheckCnl3()
{
  if (CheckC3.GetCheck()) {
    // The box is checked, enable the Combo Box and the next Check Box
    CheckC4.EnableWindow(true);
  }
  else {
    // The box is not checked, disable the Combo Box and the next Check Box
    CheckC4.EnableWindow(false);
    CheckC4.SetCheck(false);
  }
  SetComboCs();
  UpdateChannelSettings();
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnBnClickedCheckCnl4()
{
  if (CheckC4.GetCheck()) {
    // The box is checked, enable the Combo Box
    ComboC4.EnableWindow(true);
  }
  else {
    // The box is not checked, disable the Combo Box
    ComboC4.EnableWindow(false);
  }
  SetComboCs();
  UpdateChannelSettings();
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::SetComboCs() {
  ComboC2.EnableWindow(CheckC2.GetCheck());
  ComboC3.EnableWindow(CheckC3.GetCheck());
  ComboC4.EnableWindow(CheckC4.GetCheck());
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::UpdateChannelSettings() {
  if (CheckC4.GetCheck()) settings.SetChannels(4);
  else if (CheckC3.GetCheck()) settings.SetChannels(3);
  else if (CheckC2.GetCheck()) settings.SetChannels(2);
  else settings.SetChannels(1);
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnCbnSelchangeComboCnl1()
{
  settings.SetScreenSection(1, ComboC1.GetCurSel());
  if (settings.GetScreenSection(1) == STATIC_COLOUR) {
    CColorDialog dlg;
    if (dlg.DoModal() == IDOK) {
      rgbSTATIC[0] = dlg.GetColor();
      settings.SetStaticColour(1, rgbSTATIC[0]);
    }
  }
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnCbnSelchangeComboCnl2()
{
  settings.SetScreenSection(2, ComboC2.GetCurSel());
  if (settings.GetScreenSection(2) == STATIC_COLOUR) {
    CColorDialog dlg; 
    if (dlg.DoModal() == IDOK) {
      rgbSTATIC[1] = dlg.GetColor();
      settings.SetStaticColour(2, rgbSTATIC[1]);
    }
  }
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnCbnSelchangeComboCnl3()
{
  settings.SetScreenSection(3, ComboC3.GetCurSel());
  if (settings.GetScreenSection(3) == STATIC_COLOUR) {
    CColorDialog dlg; 
    if (dlg.DoModal() == IDOK) {
      rgbSTATIC[2] = dlg.GetColor();
      settings.SetStaticColour(3, rgbSTATIC[2]);
    }
  }
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnCbnSelchangeComboCnl4()
{
  settings.SetScreenSection(4, ComboC4.GetCurSel());
  if (settings.GetScreenSection(4) == STATIC_COLOUR) {
    CColorDialog dlg; 
    if (dlg.DoModal() == IDOK){
      rgbSTATIC[3] = dlg.GetColor();
      settings.SetStaticColour(4, rgbSTATIC[3]);
    }
  }
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnAppExit()
{
  StopRunning();
  TrayIcon.RemoveIcon();
  exit(0);
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnComboComsEditChange()
{
  
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnComboComsCloseup()
{
  pArduino->CloseCOM();
  CString com;

  // Get the selected COM port
  CComboComs.GetWindowTextA(com);
  if (com != "") {
    // Get the 4th character and save get the int value
    int NewCom = Str2Int(com);

    // Try to open, and save the port if we succeed
    if (pArduino->OpenCOM(NewCom)) settings.SetComPort(NewCom);
  }
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnNMCustomdrawSliderThreshold(NMHDR *pNMHDR, LRESULT *pResult)
{
  LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
  
  settings.SetThreshold(SliderThreshold.GetPos());

  *pResult = 0;
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnNMCustomdrawSliderAccentuation(NMHDR *pNMHDR, LRESULT *pResult)
{
  LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
  
  settings.SetAccentuation(SliderAccentuation.GetPos());

  *pResult = 0;
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnBnClickedCheckFliprb()
{
  settings.SetFlipRB(CheckFlipRB.GetCheck());
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnNMCustomdrawSliderRed(NMHDR *pNMHDR, LRESULT *pResult)
{
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnNMCustomdrawSliderGreen(NMHDR *pNMHDR, LRESULT *pResult)
{
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnNMCustomdrawSliderBlue(NMHDR *pNMHDR, LRESULT *pResult)
{
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnNMCustomdrawSliderBrightness(NMHDR *pNMHDR, LRESULT *pResult)
{
  LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);

  settings.SetBrightness(SliderBrightness.GetPos());

  *pResult = 0;
}
//-------------------------------------------------------------------------------------------------------
void CAmbloneDlg::OnCustomdrawSliderRed(NMHDR *pNMHDR, LRESULT *pResult)
{
  LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
  
  settings.SetCalibrationRed(SliderRed.GetPos());

  *pResult = 0;
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnCustomdrawSliderGreen(NMHDR *pNMHDR, LRESULT *pResult)
{
  LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
  
  settings.SetCalibrationGreen(SliderGreen.GetPos());

  *pResult = 0;
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnCustomdrawSliderBlue(NMHDR *pNMHDR, LRESULT *pResult)
{
  LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
  
  settings.SetCalibrationBlue(SliderBlue.GetPos());

  *pResult = 0;
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
  LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
  
  // Get the position of the slider
  int prior = SliderPriority.GetPos();
  
  // Switch over the slider position
  switch (prior) {
    case 1:
      prior = IDLE_PRIORITY_CLASS;
      break;
    case 2:
      prior = BELOW_NORMAL_PRIORITY_CLASS;
      break;
    case 3:
      prior = NORMAL_PRIORITY_CLASS;
      break;
    case 4:
      prior = ABOVE_NORMAL_PRIORITY_CLASS;
      break;
    case 5:
      prior = HIGH_PRIORITY_CLASS;
      break;
    case 6:
      prior = REALTIME_PRIORITY_CLASS;
      break;
    default: // Set to normal if value is corrupted
      prior = NORMAL_PRIORITY_CLASS;
      break;
  }
  
  // Set process priority
  SetPriorityClass(GetCurrentProcess(), prior);

  // Save to settings
  settings.SetProcessPriority(prior);

  *pResult = 0;
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnNMCustomdrawSliderSweepspeed(NMHDR *pNMHDR, LRESULT *pResult)
{
  LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
  
  settings.SetSweepSpeed(SliderSweepSpeed.GetPos());

  *pResult = 0;
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnBnClickedCheckLimitLps()
{
  settings.SetDoLimitLps(CheckLimitLps.GetCheck());

  SliderLimitLps.EnableWindow(settings.GetDoLimitLps());
  LabelLowLimit.EnableWindow(settings.GetDoLimitLps());
  LabelHighLimit.EnableWindow(settings.GetDoLimitLps());

  CString Text;
  Text.Format("Limit loops per second (lps): %d", settings.GetLpsLimit());
  if (settings.GetDoLimitLps()) CheckLimitLps.SetWindowTextA(Text);
  else CheckLimitLps.SetWindowTextA("Limit loops per second (lps)");
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnNMCustomdrawSliderLimitLps(NMHDR *pNMHDR, LRESULT *pResult)
{
  LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
  
  settings.SetLpsLimit(SliderLimitLps.GetPos());
  OnBnClickedCheckLimitLps();

  *pResult = 0;
}
//-------------------------------------------------------------------------------------------------------

LRESULT CAmbloneDlg::OnHotKey(WPARAM wParam, LPARAM lParam) {
  if (wParam == HOTKEY_DISPLAY_1) {
    // Change to primary monitor
    LoadScreenSectionSettings();
    settings.SetDisplayDeviceNum(0);
    OnCbnDropdownComboWindow();
    // Restart the loop to apply changes
    Restart();
  }
  else if (wParam == HOTKEY_DISPLAY_2) {
    // Change to secondary monitor
    LoadScreenSectionSettings();
    settings.SetDisplayDeviceNum(1);
    OnCbnDropdownComboWindow();
    // Restart the loop to apply changes
    Restart();
  }
  else if (wParam == HOTKEY_STATIC_COLOUR) {
    // Static colour
    if (ComboC1.IsWindowEnabled()) ComboC1.SetCurSel(STATIC_COLOUR);
    if (ComboC2.IsWindowEnabled()) ComboC2.SetCurSel(STATIC_COLOUR);
    if (ComboC3.IsWindowEnabled()) ComboC3.SetCurSel(STATIC_COLOUR);
    if (ComboC4.IsWindowEnabled()) ComboC4.SetCurSel(STATIC_COLOUR);
  }
  else if (wParam == HOTKEY_SWEEP) {
    // Colour Sweep
    if (ComboC1.IsWindowEnabled()) ComboC1.SetCurSel(SWEEP);
    if (ComboC2.IsWindowEnabled()) ComboC2.SetCurSel(SWEEP);
    if (ComboC3.IsWindowEnabled()) ComboC3.SetCurSel(SWEEP);
    if (ComboC4.IsWindowEnabled()) ComboC4.SetCurSel(SWEEP);
  }
  else if (wParam == HOTKEY_FAKE_TV) {
    // Fake TV
    if (ComboC1.IsWindowEnabled()) ComboC1.SetCurSel(FAKE_TV);
    if (ComboC2.IsWindowEnabled()) ComboC2.SetCurSel(FAKE_TV);
    if (ComboC3.IsWindowEnabled()) ComboC3.SetCurSel(FAKE_TV);
    if (ComboC4.IsWindowEnabled()) ComboC4.SetCurSel(FAKE_TV);
  }
  return 0;
}
//-------------------------------------------------------------------------------------------------------

HotKeyMapping* CAmbloneDlg::GetHotkey(UINT Key, UINT Modifiers) {
  for (int i = 0; i < HotKeys.size(); i++) {
    if (HotKeys[i]->Key == Key && HotKeys[i]->Modifiers == Modifiers) return HotKeys[i];
  }
  return NULL;
}
//-------------------------------------------------------------------------------------------------------

HotKeyMapping* CAmbloneDlg::GetHotkey(int HotkeyID) {
  for (int i = 0; i < HotKeys.size(); i++) {
    if (HotKeys[i]->ID == HotkeyID) return HotKeys[i];
  }
  return NULL;
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::SetHotkey(int HotkeyID, UINT Key, UINT Modifiers) {
  for (int i = 0; i < HotKeys.size(); i++) {
    if (HotKeys[i]->ID == HotkeyID) {
      // We found the hotkey, first unregister it
      UnregisterHotKey(this->m_hWnd, HotkeyID);
      // Now save the new hotkey setting
      HotKeys[i]->Key = Key;
      HotKeys[i]->Modifiers = Modifiers;
      // Reregister the hotkey
      RegisterHotKey(this->m_hWnd,
        HotkeyID,
        Modifiers,
        Key
      );
      // Save to registry
      settings.SetHotkeyKey(HotkeyID, Key);
      settings.SetHotkeyMod(HotkeyID, Modifiers);
    }
  }
}
//-------------------------------------------------------------------------------------------------------

int CAmbloneDlg::CheckUpdate(const char *current_version) {
  char *new_vers = new char[10];

  try {
    SocketClient s("amblone.com", 80);
    s.SendLine("GET http://amblone.com/version/index.html");
    s.SendLine("");

    while (true) {
      string http_rec = s.ReceiveLine();
      if (http_rec.empty()) break;
      if (http_rec.npos != http_rec.find("amblone_version")) {
        // string http_rec now contains amblone_versionX.Y where X.Y is the version
        int pos = 0;
        while (http_rec[pos + 15] != '\0' && pos < 10) {
          new_vers[pos] = http_rec[pos + 15];
          pos++;
        }
        new_vers[pos] = '\0'; // You have been selected for termination!
        if (strcmp(VERSION, new_vers)) return VRS_NEW;
        else return VRS_UPTODATE;
      }
    }
  } 
  catch (...){;}
  return VRS_ERROR;
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnPopupCheckforupdates() {
  CString msg;
  switch (CheckUpdate(VERSION)) {
    case VRS_NEW:
      // New version
      MessageBox("A new version of Amblone is available!\nVisit http://amblone.com/download to get the new version");
      break;
    case VRS_UPTODATE:
      // Up to date
      msg.Format("You are running the latest version of Amblone. (v%s)", VERSION);
      MessageBox(msg);
      break;
    default:
      // Error
      msg.Format("Update information could not be retrieved.\nYou are running Amblone v%s", VERSION);
      MessageBox(msg);
      break;
  }
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnNMCustomdrawSliderMinbrightness(NMHDR *pNMHDR, LRESULT *pResult) {
  LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
  
  settings.SetMinBrightness(SliderMinBrightness.GetPos());

  *pResult = 0;
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnBnClickedCheckUpdates() {
  settings.SetUpdate(CheckUpdates.GetCheck());
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnCbnSelchangeComboCapture() {
  CString CaptStr;
  int CapModeID = CComboCapture.GetCurSel();

  // Get the text of the combo
  settings.SetCaptureModeID(CapModeID);
  SetCaptureMode(CapModeID);

  // Update the window dropdown
  OnCbnDropdownComboWindow();

  // Restart the loop to apply changes
  Restart();
}

//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnCbnSelchangeComboWindow() {
  if (CurCapture.FullScreen) {
    CString ComboText;
    CComboWindow.GetWindowTextA(ComboText);
    int NewDD = Str2Int(ComboText) - 1;
    // If there's no change, do nothing
    if (NewDD == settings.GetDisplayDeviceNum()) return;
    settings.SetDisplayDeviceNum(NewDD);
  }
  else {
    SelectedWin = CComboWindow.GetCurSel();
    WindowSelected = true;
  }
  
  // Restart the loop to apply changes
  Restart();
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnCbnDropdownComboWindow() {
  // Save current selection
  CString CurSelText;
  CComboWindow.GetWindowTextA(CurSelText);
  // Delete all content of the combo box
  for (int i = CComboWindow.GetCount(); i >= 0; i--)
    CComboWindow.DeleteString(i);

  // Get the text in the capturing method combo box
  CString CapText;
  CComboCapture.GetWindowTextA(CapText);
  if (CurCapture.FullScreen) {
    // Show the monitors
    int DeviceNum = 0;
    DISPLAY_DEVICE DisplayDevice;
    DISPLAY_DEVICE Monitor;
    ZeroMemory(&DisplayDevice, sizeof(DisplayDevice));
    DisplayDevice.cb = sizeof(DisplayDevice);
    ZeroMemory(&Monitor, sizeof(Monitor));
    Monitor.cb = sizeof(Monitor);
    while (EnumDisplayDevices(NULL, DeviceNum, &DisplayDevice, NULL)) {
      if (EnumDisplayDevices(DisplayDevice.DeviceName, 0, &Monitor, NULL)) {
        CComboWindow.AddString((CString)("Display " + (DeviceNum + 1)));
      }
      DeviceNum++;
    }
    CString SelDev = "Display " + (settings.GetDisplayDeviceNum() + 1);
    CComboWindow.SelectString(0, SelDev);
  }
  else {
    // Show the windows we can capture
    BuildWindowsList();
    for (int i = 0; i < vWin.size(); i++)
      CComboWindow.AddString(vWin[i].Name);
    CComboWindow.SetCurSel(CComboWindow.GetCount() - 1);
  }
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnBnClickedButtonShowoutput() {
  OutputDlg->ShowWindow(SW_SHOW);
  OutputDlg->UpdateWindow();
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::SetCaptureMode(int ModeID) {
  if (ModeID == WindowsFull.ID) CurCapture = WindowsFull;
  else if (ModeID == WindowsWin.ID) CurCapture = WindowsWin;
  else if (ModeID == DirectXFull.ID) CurCapture = DirectXFull;
  //else if (ModeID == DirectXWin.ID) CurCapture = DirectXWin;
  /*if (ModeID == XbmcWin.ID) {
    CurCapture = XbmcWin;
    // Hide the window selection combo box
    CComboWindow.ShowWindow(SW_HIDE);
    CStaticWindow.ShowWindow(SW_HIDE);
    // Show XBMC IP and Port selection
    CStaticXbmcIP.ShowWindow(SW_SHOW);
    CStaticXbmcPort.ShowWindow(SW_SHOW);
    CEditXbmcIP.ShowWindow(SW_SHOW);
    CEditXbmcPort.ShowWindow(SW_SHOW);
  }
  else {
    // Show the window selection combo box
    CComboWindow.ShowWindow(SW_SHOW);
    CStaticWindow.ShowWindow(SW_SHOW);
    // Hide XBMC IP and Port selection
    CStaticXbmcIP.ShowWindow(SW_HIDE);
    CStaticXbmcPort.ShowWindow(SW_HIDE);
    CEditXbmcIP.ShowWindow(SW_HIDE);
    CEditXbmcPort.ShowWindow(SW_HIDE);
  }*/
  Restart();
}
//-------------------------------------------------------------------------------------------------------

void CAmbloneDlg::OnBnClickedButtonHotkeys() {
  if (!HotKeyDlgOpen) {
    HotKeyDlgOpen = true;
    AmbloneHotkeys *HotkeysDlg;
    HotkeysDlg = new AmbloneHotkeys(this);
    HotkeysDlg->Parent = this;
    HotkeysDlg->Create(IDD_DIALOG_HOTKEYS, this);
    HotkeysDlg->InitControls();
    HotkeysDlg->ShowWindow(SW_SHOW);    
  }
}
