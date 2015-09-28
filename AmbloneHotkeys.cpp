// AmbloneHotkeys.cpp : implementation file
//

#include "stdafx.h"
#include "Amblone.h"
#include "AmbloneHotkeys.h"

// AmbloneHotkeys dialog

IMPLEMENT_DYNAMIC(AmbloneHotkeys, CDialog)

AmbloneHotkeys::AmbloneHotkeys(CWnd* pParent /*=NULL*/)
	: CDialog(AmbloneHotkeys::IDD, pParent)
{
  block = true;
}
//-------------------------------------------------------------------------------------------------------

void AmbloneHotkeys::InitControls() {
  HotKeyMapping *hk;

  // Display 1
  hk = Parent->GetHotkey(HOTKEY_DISPLAY_1);
  CComboModDisp1.SelectString(0, ModifiersToString(hk->Modifiers));
  CEditKeyDisp1.SetWindowTextA((CString) (char) hk->Key);
  CEditKeyDisp1.SetLimitText(1);

  // Display 2
  hk = Parent->GetHotkey(HOTKEY_DISPLAY_2);
  CComboModDisp2.SelectString(0, ModifiersToString(hk->Modifiers));
  CEditKeyDisp2.SetWindowTextA((CString) (char) hk->Key);
  CEditKeyDisp2.SetLimitText(1);

  // Static colour
  hk = Parent->GetHotkey(HOTKEY_STATIC_COLOUR);
  CComboModStaticColour.SelectString(0, ModifiersToString(hk->Modifiers));
  CEditKeyStaticColour.SetWindowTextA((CString) (char) hk->Key);
  CEditKeyStaticColour.SetLimitText(1);

  // Colour sweep
  hk = Parent->GetHotkey(HOTKEY_SWEEP);
  CComboModSweep.SelectString(0, ModifiersToString(hk->Modifiers));
  CEditKeySweep.SetWindowTextA((CString) (char) hk->Key);
  CEditKeySweep.SetLimitText(1);

  // Fake TV
  hk = Parent->GetHotkey(HOTKEY_FAKE_TV);
  CComboModFakeTV.SelectString(0, ModifiersToString(hk->Modifiers));
  CEditKeyFakeTV.SetWindowTextA((CString) (char) hk->Key);
  CEditKeyFakeTV.SetLimitText(1);

  block = false;
}
//-------------------------------------------------------------------------------------------------------

AmbloneHotkeys::~AmbloneHotkeys()
{
}
//-------------------------------------------------------------------------------------------------------

void AmbloneHotkeys::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_COMBO_MOD_DISP1, CComboModDisp1);
  DDX_Control(pDX, IDC_COMBO_MOD_DISP2, CComboModDisp2);
  DDX_Control(pDX, IDC_EDIT_KEY_DISP1, CEditKeyDisp1);
  DDX_Control(pDX, IDC_EDIT_KEY_DISP2, CEditKeyDisp2);
  DDX_Control(pDX, IDC_COMBO_MOD_STATIC_COLOUR, CComboModStaticColour);
  DDX_Control(pDX, IDC_COMBO_MOD_SWEEP, CComboModSweep);
  DDX_Control(pDX, IDC_COMBO_MOD_FAKE_TV, CComboModFakeTV);
  DDX_Control(pDX, IDC_EDIT_KEY_STATIC_COLOUR, CEditKeyStaticColour);
  DDX_Control(pDX, IDC_EDIT_KEY_SWEEP, CEditKeySweep);
  DDX_Control(pDX, IDC_EDIT_KEY_FAKE_TV, CEditKeyFakeTV);
}
//-------------------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(AmbloneHotkeys, CDialog)
  ON_EN_SETFOCUS(IDC_EDIT_KEY_DISP1, &AmbloneHotkeys::OnEnSetfocusEditKeyDisp1)
  ON_EN_SETFOCUS(IDC_EDIT_KEY_DISP2, &AmbloneHotkeys::OnEnSetfocusEditKeyDisp2)
  ON_EN_UPDATE(IDC_EDIT_KEY_DISP1, &AmbloneHotkeys::OnEnUpdateEditKeyDisp1)
  ON_EN_KILLFOCUS(IDC_EDIT_KEY_DISP1, &AmbloneHotkeys::OnEnKillfocusEditKeyDisp1)
  ON_BN_CLICKED(IDOK, &AmbloneHotkeys::OnBnClickedOk)
  ON_WM_CLOSE()
  ON_EN_CHANGE(IDC_EDIT_KEY_DISP2, &AmbloneHotkeys::OnEnChangeEditKeyDisp2)
  ON_CBN_SELCHANGE(IDC_COMBO_MOD_DISP1, &AmbloneHotkeys::OnCbnSelchangeComboModDisp1)
  ON_CBN_SELCHANGE(IDC_COMBO_MOD_DISP2, &AmbloneHotkeys::OnCbnSelchangeComboModDisp2)
  ON_EN_CHANGE(IDC_EDIT_KEY_STATIC_COLOUR, &AmbloneHotkeys::OnEnChangeEditKeyStaticColour)
  ON_EN_CHANGE(IDC_EDIT_KEY_SWEEP, &AmbloneHotkeys::OnEnChangeEditKeySweep)
  ON_EN_CHANGE(IDC_EDIT_KEY_FAKE_TV, &AmbloneHotkeys::OnEnChangeEditKeyFakeTv)
  ON_CBN_SELCHANGE(IDC_COMBO_MOD_SWEEP, &AmbloneHotkeys::OnCbnSelchangeComboModSweep)
  ON_CBN_SELCHANGE(IDC_COMBO_MOD_STATIC_COLOUR, &AmbloneHotkeys::OnCbnSelchangeComboModStaticColour)
  ON_CBN_SELCHANGE(IDC_COMBO_MOD_FAKE_TV, &AmbloneHotkeys::OnCbnSelchangeComboModFakeTv)
END_MESSAGE_MAP()


// AmbloneHotkeys message handlers

CString AmbloneHotkeys::ModifiersToString(UINT Mod) {
  switch (Mod) {
    case MOD_CONTROL | MOD_SHIFT:
      return "Ctrl + Shift";
    case MOD_CONTROL | MOD_ALT:
      return "Ctrl + Alt";
    case MOD_ALT | MOD_SHIFT:
      return "Alt + Shift";  
  }
  return "";
}
//-------------------------------------------------------------------------------------------------------

UINT AmbloneHotkeys::StringToModifiers(CString ModStr) {
  if (ModStr == "Ctrl + Shift") return MOD_CONTROL | MOD_SHIFT;
  else if (ModStr == "Ctrl + Alt") return MOD_CONTROL | MOD_ALT;
  else if (ModStr == "Alt + Shift") return MOD_ALT | MOD_SHIFT;
  return 0;
}
//-------------------------------------------------------------------------------------------------------

void AmbloneHotkeys::UpdateHotkey(int HotkeyID, CEdit *EditBox, CComboBox *ComboBox) {
  if (!block) {
    // First get the current values of the hotkey (might have to put back)
    HotKeyMapping *CurrentMapping = Parent->GetHotkey(HotkeyID);
    // Start parsing the key
    CString NewKey;
    EditBox->GetWindowTextA(NewKey);
    if (NewKey.GetLength() == 1) {
      string sKey = NewKey;
      char cK = sKey.c_str()[0];
      if (cK >= 'a' && cK <= 'z') {
        // Make capital
        cK -= 'a' - 'A';
        CString Cap(cK);
        EditBox->SetWindowTextA(Cap);
      }
      CString ModStr;
      ComboBox->GetWindowTextA(ModStr);
      UINT Mod = StringToModifiers(ModStr);
      // First check if the hotkey is already taken
      HotKeyMapping *TakenMapping = Parent->GetHotkey(cK, Mod);
      if (TakenMapping == NULL)
        Parent->SetHotkey(HotkeyID, cK, Mod);
      else {
        // The hotkey is already taken
        if (TakenMapping == CurrentMapping) return;
        block = true;
        MessageBox("This hotkey is already taken", "Amblone", MB_ICONEXCLAMATION | MB_OK);
        EditBox->SetWindowTextA((CString) (char) CurrentMapping->Key);
        ComboBox->SelectString(0, ModifiersToString(CurrentMapping->Modifiers));      
        block = false;
      }
    }  
  }
}
//-------------------------------------------------------------------------------------------------------

void AmbloneHotkeys::OnEnKillfocusEditKeyDisp1()
{
  CString Txt;
  CEditKeyDisp1.GetWindowTextA(Txt);
  if (Txt.GetLength() == 0) {
    char Key = Parent->HotKeys[HOTKEY_DISPLAY_1]->Key;
    CString cs(Key);
    CEditKeyDisp1.SetWindowTextA(cs);
  }
}
//-------------------------------------------------------------------------------------------------------

void AmbloneHotkeys::OnBnClickedOk()
{
  Parent->HotkeyWinClose();
  OnOK();
}
//-------------------------------------------------------------------------------------------------------

void AmbloneHotkeys::OnClose() {
  OnBnClickedOk();
}
//-------------------------------------------------------------------------------------------------------

void AmbloneHotkeys::OnEnSetfocusEditKeyDisp1() {
  CEditKeyDisp1.SetSel(0, -1);
}
//-------------------------------------------------------------------------------------------------------

void AmbloneHotkeys::OnEnSetfocusEditKeyDisp2() {
  CEditKeyDisp2.SetSel(0, -1);
}
//-------------------------------------------------------------------------------------------------------

void AmbloneHotkeys::OnEnUpdateEditKeyDisp1() {
  UpdateHotkey(HOTKEY_DISPLAY_1, &CEditKeyDisp1, &CComboModDisp1);
}
//-------------------------------------------------------------------------------------------------------

void AmbloneHotkeys::OnEnChangeEditKeyDisp2() {
  UpdateHotkey(HOTKEY_DISPLAY_2, &CEditKeyDisp2, &CComboModDisp2);
}
//-------------------------------------------------------------------------------------------------------

void AmbloneHotkeys::OnCbnSelchangeComboModDisp1() {
  UpdateHotkey(HOTKEY_DISPLAY_1, &CEditKeyDisp1, &CComboModDisp1);
}
//-------------------------------------------------------------------------------------------------------

void AmbloneHotkeys::OnCbnSelchangeComboModDisp2() {
  UpdateHotkey(HOTKEY_DISPLAY_2, &CEditKeyDisp2, &CComboModDisp2);
}
//-------------------------------------------------------------------------------------------------------

void AmbloneHotkeys::OnEnChangeEditKeyStaticColour() {
  UpdateHotkey(HOTKEY_STATIC_COLOUR, &CEditKeyStaticColour, &CComboModStaticColour);
}
//-------------------------------------------------------------------------------------------------------

void AmbloneHotkeys::OnEnChangeEditKeySweep() {
  UpdateHotkey(HOTKEY_SWEEP, &CEditKeySweep, &CComboModSweep);
}
//-------------------------------------------------------------------------------------------------------

void AmbloneHotkeys::OnEnChangeEditKeyFakeTv() {
  UpdateHotkey(HOTKEY_FAKE_TV, &CEditKeyFakeTV, &CComboModFakeTV);
}
//-------------------------------------------------------------------------------------------------------

void AmbloneHotkeys::OnCbnSelchangeComboModSweep() {
  UpdateHotkey(HOTKEY_SWEEP, &CEditKeySweep, &CComboModSweep);
}
//-------------------------------------------------------------------------------------------------------

void AmbloneHotkeys::OnCbnSelchangeComboModStaticColour() {
  UpdateHotkey(HOTKEY_STATIC_COLOUR, &CEditKeyStaticColour, &CComboModStaticColour);
}
//-------------------------------------------------------------------------------------------------------

void AmbloneHotkeys::OnCbnSelchangeComboModFakeTv() {
  UpdateHotkey(HOTKEY_FAKE_TV, &CEditKeyFakeTV, &CComboModFakeTV);
}
//-------------------------------------------------------------------------------------------------------
