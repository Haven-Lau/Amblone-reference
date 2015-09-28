#pragma once
#include <vector>
#include "afxwin.h"
#include "Amblone.h"
#include "AmbloneDlg.h"
using namespace std;

// AmbloneHotkeys dialog

class AmbloneHotkeys : public CDialog
{
	DECLARE_DYNAMIC(AmbloneHotkeys)

public:
	AmbloneHotkeys(CWnd* pParent = NULL);   // standard constructor
	virtual ~AmbloneHotkeys();

// Dialog Data
	enum { IDD = IDD_DIALOG_HOTKEYS };

private:
  bool block;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  
	DECLARE_MESSAGE_MAP()

public:
  CAmbloneDlg *Parent;
  CComboBox CComboModDisp1;
  CComboBox CComboModDisp2;
  CEdit CEditKeyDisp1;
  CEdit CEditKeyDisp2;

  void InitControls();
  CString ModifiersToString(UINT Mod);
  UINT StringToModifiers(CString ModStr);
  void UpdateHotkey(int HotkeyID, CEdit *EditBox, CComboBox *ComboBox);

  afx_msg void OnEnSetfocusEditKeyDisp1();
  afx_msg void OnEnSetfocusEditKeyDisp2();
  afx_msg void OnEnUpdateEditKeyDisp1();
  afx_msg void OnEnKillfocusEditKeyDisp1();
  afx_msg void OnBnClickedOk();
  afx_msg void OnClose();
  afx_msg void OnEnChangeEditKeyDisp2();
  afx_msg void OnCbnSelchangeComboModDisp1();
  afx_msg void OnCbnSelchangeComboModDisp2();
  CComboBox CComboModStaticColour;
  CComboBox CComboModSweep;
  CComboBox CComboModFakeTV;
  CEdit CEditKeyStaticColour;
  CEdit CEditKeySweep;
  CEdit CEditKeyFakeTV;
  afx_msg void OnEnChangeEditKeyStaticColour();
  afx_msg void OnEnChangeEditKeySweep();
  afx_msg void OnEnChangeEditKeyFakeTv();
  afx_msg void OnCbnSelchangeComboModSweep();
  afx_msg void OnCbnSelchangeComboModStaticColour();
  afx_msg void OnCbnSelchangeComboModFakeTv();
};

