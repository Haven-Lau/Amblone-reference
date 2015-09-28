// OutputDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Amblone.h"
#include "OutputDialog.h"


// OutputDialog dialog

IMPLEMENT_DYNAMIC(OutputDialog, CDialog)

OutputDialog::OutputDialog(CWnd* pParent /*=NULL*/) : CDialog(OutputDialog::IDD, pParent) {
  const COLORREF InitColour = RGB(0, 0, 0);
  TopLeft = InitColour;
  Top = InitColour;
  TopRight = InitColour;
  Left = InitColour;
  Middle = InitColour;
  Right = InitColour;
  BottomLeft = InitColour;
  Bottom = InitColour;
  BottomRight = InitColour;
}

OutputDialog::~OutputDialog()
{
}

void OutputDialog::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_STATIC_PIC, CPictureTV);
}


BEGIN_MESSAGE_MAP(OutputDialog, CDialog)
  ON_WM_PAINT()
  ON_BN_CLICKED(IDCANCEL, &OutputDialog::OnBnClickedCancel)
END_MESSAGE_MAP()

// OutputDialog message handlers

void OutputDialog::OnPaint()
{
  CPaintDC dc(this); // device context for painting

  CDC *pDC = GetDC();
  WINDOWPLACEMENT PicWP; // Placement of the TV picture
  WINDOWPLACEMENT WinWP; // Placement of the dialog
  CPictureTV.GetWindowPlacement(&PicWP);
  GetWindowPlacement(&WinWP);
  p.left = PicWP.rcNormalPosition.left;
  p.right = PicWP.rcNormalPosition.right;
  p.top = PicWP.rcNormalPosition.top;
  p.bottom = PicWP.rcNormalPosition.bottom;

  // First draw a black rectangle
  CBrush *pNew = new CBrush(RGB(0, 0, 0));
  CBrush *pOld = pDC->SelectObject(pNew); // Save the old brush
  int Width = PicWP.rcNormalPosition.right + PicWP.rcNormalPosition.left;
  //MessageBox((CString)("Width = " + Width));
  pDC->PatBlt(
    0,
    0,
    Width,
    WinWP.rcNormalPosition.bottom,
    PATCOPY);
  pDC->SelectObject(pOld);    // Restore old brush
  delete pNew;

  DrawColours();
}

void OutputDialog::DrawColours() {
  int Width = 25;

  // Top  
  Placement Plc;
  Plc.left = p.left;
  Plc.right = p.right;
  Plc.top = p.top - Width;
  Plc.bottom = p.top;
  DrawColour(Top, Plc, DIR_TOP);
  // Left
  Plc.left = p.left - Width;
  Plc.right = p.left;
  Plc.top = p.top;
  Plc.bottom = p.bottom;
  DrawColour(Left, Plc, DIR_LEFT);
  // Right
  Plc.left = p.right;
  Plc.right = p.right + Width;
  Plc.top = p.top;
  Plc.bottom = p.bottom;
  DrawColour(Right, Plc, DIR_RIGHT);
  // Bottom
  Plc.left = p.left;
  Plc.right = p.right;
  Plc.top = p.bottom;
  Plc.bottom = p.bottom + Width;
  DrawColour(Bottom, Plc, DIR_BOTTOM);
  
  // Upper left corner
  Plc.left = p.left - Width - 1;
  Plc.right = p.left;
  Plc.top = p.top - Width - 1;
  Plc.bottom = p.top;
  DrawColour(TopLeft, Plc, DIR_TOPLEFT);
  // Upper right corner
  Plc.left = p.right;
  Plc.right = p.right + Width;
  Plc.top = p.top - Width - 1;
  Plc.bottom = p.top;
  DrawColour(TopRight, Plc, DIR_TOPRIGHT);
  // Lower left corner
  Plc.left = p.left - Width - 1;
  Plc.right = p.left;
  Plc.top = p.bottom;
  Plc.bottom = p.bottom + Width;
  DrawColour(BottomLeft, Plc, DIR_BOTTOMLEFT);
  // Lower right corner
  Plc.left = p.right;
  Plc.right = p.right + Width;
  Plc.top = p.bottom;
  Plc.bottom = p.bottom + Width;
  DrawColour(BottomRight, Plc, DIR_BOTTOMRIGHT);
}

void OutputDialog::DrawColour(COLORREF Colour, Placement Plc, int Direction) {
  const int FadeSize = 40;
  CDC *pDC = GetDC();
  // Draw the main rectangle
  CBrush *pNewBrush = new CBrush(Colour);
  CBrush *pOldBrush = pDC->SelectObject(pNewBrush); // Save the old brush  
  pDC->PatBlt(
    Plc.left,
    Plc.top,
    Plc.right - Plc.left,
    Plc.bottom - Plc.top,
    PATCOPY);
  pDC->SelectObject(pOldBrush);    // Restore old brush
  delete pNewBrush;

  for (int i = 0; i < FadeSize; i++) {
    COLORREF LineColour = RGB(
      (GetRValue(Colour) * (FadeSize - i)) / FadeSize,
      (GetGValue(Colour) * (FadeSize - i)) / FadeSize,
      (GetBValue(Colour) * (FadeSize - i)) / FadeSize
      );
    CPen *pNewPen = new CPen(PS_SOLID, 1, LineColour);
    CPen *pOldPen = pDC->SelectObject(pNewPen);
    switch (Direction) {
      case DIR_TOP:
        pDC->MoveTo(Plc.left, Plc.top - (i + 1));
        pDC->LineTo(Plc.right, Plc.top - (i + 1));
        break;
      case DIR_LEFT:
        pDC->MoveTo(Plc.left - (i + 1), Plc.top);
        pDC->LineTo(Plc.left - (i + 1), Plc.bottom);
        break;
      case DIR_RIGHT:
        pDC->MoveTo(Plc.right + i, Plc.top);
        pDC->LineTo(Plc.right + i, Plc.bottom);
        break;
      case DIR_BOTTOM:
        pDC->MoveTo(Plc.left, Plc.bottom + i);
        pDC->LineTo(Plc.right, Plc.bottom + i);
        break;
      case DIR_TOPLEFT:
        pDC->MoveTo(Plc.left - i, Plc.bottom - 1);
        pDC->LineTo(Plc.left - i, Plc.top - (i + 1));
        pDC->MoveTo(Plc.right - 1, Plc.top - i);
        pDC->LineTo(Plc.left - (i + 1), Plc.top - i);
        break;
      case DIR_TOPRIGHT:
        pDC->MoveTo(Plc.right + i, Plc.bottom - 1);
        pDC->LineTo(Plc.right + i, Plc.top - (i + 1));
        pDC->MoveTo(Plc.left, Plc.top - i);
        pDC->LineTo(Plc.right + i, Plc.top - i);
        break;
      case DIR_BOTTOMLEFT:
        pDC->MoveTo(Plc.left - i, Plc.top);
        pDC->LineTo(Plc.left - i, Plc.bottom + i);
        pDC->MoveTo(Plc.right - 1, Plc.bottom + i);
        pDC->LineTo(Plc.left - (i + 1), Plc.bottom + i);
        break;
      case DIR_BOTTOMRIGHT:
        pDC->MoveTo(Plc.right + i, Plc.top);
        pDC->LineTo(Plc.right + i, Plc.bottom + i);
        pDC->MoveTo(Plc.left, Plc.bottom + i);
        pDC->LineTo(Plc.right + i + 1, Plc.bottom + i);
        break;
    }
    pDC->SelectObject(pOldPen);    // Restore old pen
    delete pNewPen;
  }
}
void OutputDialog::OnBnClickedCancel()
{
  // TODO: Add your control notification handler code here
  OnCancel();
}
