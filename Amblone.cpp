// Amblone code: Base implementation file of the Amblone application
// Author: Bart van der Drift

// License:
// Anyone is free to change, redistribute or copy parts of this code
// as long as it is not for commercial purposes / monetary gain
// Please be so kind to pay credit where due

#include "stdafx.h"
#include "Amblone.h"
#include "AmbloneDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAmbloneApp

BEGIN_MESSAGE_MAP(CAmbloneApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CAmbloneDlg *AmbloneDlg;

// CAmbloneApp construction

CAmbloneApp::CAmbloneApp()
{
}


// The one and only CAmbloneApp object

CAmbloneApp theApp;


// CAmbloneApp initialization

BOOL CAmbloneApp::InitInstance()
{
	CWinApp::InitInstance();

	SetRegistryKey("Amblone");

  CAmbloneDlg dlg;
	
	m_pMainWnd = &dlg;
  AmbloneDlg = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
