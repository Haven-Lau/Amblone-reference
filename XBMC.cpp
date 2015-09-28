#include "stdafx.h"
#include "XBMC.h"
#include "Socket.h"
#include <string.h>
#include <fstream>
#include <algorithm>
#include <windows.h>
#include <stdio.h>
#include "base64.h"
using namespace std;

bool filter(char c) {
	return c=='\n' || c=='\r';
}
//-------------------------------------------------------------------------------------------------------

DWORD WINAPI XbmcThread(LPVOID pParam) {
  XBMC * pXBMC;
  pXBMC = (XBMC *) pParam;
  return pXBMC->RequesterThread();
}
//-------------------------------------------------------------------------------------------------------

XBMC::XBMC(CString IP, int Port) {
  sh_IP = IP;
  sh_Port = Port;
  sh_rgbQ[0] = 0;
  sh_rgbQ[1] = 0;
  sh_rgbQ[2] = 0;
  sh_rgbQ[3] = 0;
  HANDLE hThread = CreateThread(NULL, 0, XbmcThread, this, 0, NULL);
}
//-------------------------------------------------------------------------------------------------------

XBMC::~XBMC() {
  Stop();
}
//-------------------------------------------------------------------------------------------------------

COLORREF *XBMC::GetQuarterColours() {
  return sh_rgbQ;
}
//-------------------------------------------------------------------------------------------------------

COLORREF XBMC::GetQuarterColour(int Quarter) {
  return sh_rgbQ[Quarter - 1];
}
//-------------------------------------------------------------------------------------------------------

DWORD XBMC::RequesterThread() {
  sh_running = true;
  sh_stopped = false;
  while (sh_running) {
    GetXbmcCapture();
  }
  sh_stopped = true;
  return 0;
}
//-------------------------------------------------------------------------------------------------------

void XBMC::Stop() {
  sh_running = false;
  while (!sh_stopped);
}
//-------------------------------------------------------------------------------------------------------

void XBMC::GetXbmcCapture() {
  string Request = "GET /xbmcCmds/xbmcHttp?command=takescreenshot%28C:\\xbmcHERC.bmp;false;0;400;300;90;true%29 HTTP/1.1\n\rHost: localhost\n\r";
  string Base64Img = "";
  bool InHtmlTags = false;
  try {
    // Open a socket to the XBMC webserver
    SocketClient s((LPCTSTR) sh_IP, sh_Port);
    // Send request for a screenshot
    s.SendBytes(Request);
    
    while (true) {
      // Receive code
      string http_rec = s.ReceiveLine();
      // Remove the linefeeds and carriage returns
      http_rec.resize(std::remove_if(http_rec.begin(), http_rec.end(), filter) - http_rec.begin() );
      // The base64 encoded bitmap resides between the html tags, look for them
      if (InHtmlTags) {
        if (http_rec.find("</html>") != string.npos) {
          // End of html tags, we can exit the loop
          InHtmlTags = false;
          break;
        }
        else Base64Img += http_rec; // Add received base64 data to the string
      }
      else if (http_rec.find("<html>") != string.npos) InHtmlTags = true;
    }
    //AfxMessageBox(Base64Img.c_str());
    // Decode the base64 data
    int dlen;
    unsigned char *Decoded = base64_decode(Base64Img, dlen);
    // Check the validity of the bitmap image
    if ((Decoded[0] != 'B' && Decoded[0] != 'b') || (Decoded[1] != 'M' && Decoded[1] != 'm'))
      throw "Data does not start with 'BM'";
    // Extract the data of the Bitmap File Header
    BITMAPFILEHEADER bfh = ParseBitmapFileHeader(Decoded);    
    // We only read the width and height from the Bitmap Info Header
    long biWidth = ParseBitmapWidth(Decoded);
    long biHeight = ParseBitmapHeight(Decoded);
    // Now calculate the amount of bytes per pixel
    int ImgSize = bfh.bfSize - bfh.bfOffBits;
    long nPixels = biWidth * biHeight;
    int bpp = ImgSize / nPixels;
    // Now calculate the average colour of the quarters
    long tR = 0;
    long tG = 0;
    long tB = 0;
    for (int i = 0; i < ImgSize; i += bpp) {
      tR += Decoded[i+2];
      tG += Decoded[i+1];
      tB += Decoded[i];
    }
    tR /= nPixels;
    tG /= nPixels;
    tB /= nPixels;
    COLORREF avg = RGB(tR, tG, tB);
    sh_rgbQ[0] = avg;
    sh_rgbQ[1] = avg;
    sh_rgbQ[2] = avg;
    sh_rgbQ[3] = avg;
    // TODO: remove file output
    char *DecChar = new char[dlen];
    for (int i = 0; i < dlen; i++) DecChar[i] = Decoded[i];
    fstream outfile;
    outfile.open("C:\\test.bmp", ios::out | ios::binary);
    outfile.write(DecChar, dlen);
  }
  catch (const char *ex) {
    AfxMessageBox(ex);
  }
  catch (...) {
    AfxMessageBox("Exception");
  }
}
//-------------------------------------------------------------------------------------------------------

BITMAPFILEHEADER XBMC::ParseBitmapFileHeader(unsigned char *Decoded) {
  BITMAPFILEHEADER bfh;
  // bfType
  (*((unsigned char *)&(bfh.bfType)  )) = Decoded[0];
  (*((unsigned char *)&(bfh.bfType)+1)) = Decoded[1];
  // bfSize
  (*((unsigned char *)&(bfh.bfSize)  )) = Decoded[2];
  (*((unsigned char *)&(bfh.bfSize)+1)) = Decoded[3];
  (*((unsigned char *)&(bfh.bfSize)+2)) = Decoded[4];
  (*((unsigned char *)&(bfh.bfSize)+3)) = Decoded[5];
  // bfReserved1 and 2
  bfh.bfReserved1 = 0;
  bfh.bfReserved2 = 0;
  // bfOffBits
  (*((unsigned char *)&(bfh.bfOffBits)  )) = Decoded[10];
  (*((unsigned char *)&(bfh.bfOffBits)+1)) = Decoded[11];
  (*((unsigned char *)&(bfh.bfOffBits)+2)) = Decoded[12];
  (*((unsigned char *)&(bfh.bfOffBits)+3)) = Decoded[13];
  return bfh;
}
//-------------------------------------------------------------------------------------------------------

long XBMC::ParseBitmapWidth(unsigned char *Decoded) {
  long biWidth;
  (*((unsigned char *)&(biWidth)  )) = Decoded[18];
  (*((unsigned char *)&(biWidth)+1)) = Decoded[19];
  (*((unsigned char *)&(biWidth)+2)) = Decoded[20];
  (*((unsigned char *)&(biWidth)+3)) = Decoded[21];
  return biWidth;
}
//-------------------------------------------------------------------------------------------------------

long XBMC::ParseBitmapHeight(unsigned char *Decoded) {
  long biHeight;
  (*((unsigned char *)&(biHeight)  )) = Decoded[22];
  (*((unsigned char *)&(biHeight)+1)) = Decoded[23];
  (*((unsigned char *)&(biHeight)+2)) = Decoded[24];
  (*((unsigned char *)&(biHeight)+3)) = Decoded[25];
  return biHeight;
}
//-------------------------------------------------------------------------------------------------------
