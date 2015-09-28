#pragma once
#include "afxwin.h"


// OutputDialog dialog

#define DIR_TOPLEFT     0
#define DIR_TOP         1
#define DIR_TOPRIGHT    2
#define DIR_LEFT        3
#define DIR_RIGHT       4
#define DIR_BOTTOMLEFT  5
#define DIR_BOTTOM      6
#define DIR_BOTTOMRIGHT 7

struct Placement {
  int left;
  int right;
  int top;
  int bottom;
};

class OutputDialog : public CDialog
{
	DECLARE_DYNAMIC(OutputDialog)

public:
	OutputDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~OutputDialog();

  void DrawColours();

  COLORREF TopLeft;
  COLORREF Top;
  COLORREF TopRight;
  COLORREF Left;
  COLORREF Middle;
  COLORREF Right;
  COLORREF BottomLeft;
  COLORREF Bottom;
  COLORREF BottomRight;

private:
  void DrawColour(COLORREF Colour, Placement Plc, int Direction);

  Placement p;
// Dialog Data
	enum { IDD = IDD_DIALOG_COLOURS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  CStatic CPictureTV;
  afx_msg void OnPaint();
  afx_msg void OnBnClickedCancel();
};
