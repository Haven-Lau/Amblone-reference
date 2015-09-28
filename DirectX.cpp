// Standalone.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "DirectX.h"
#include <iostream>
using namespace std;

#define	WINDOW_MODE true
#define USE_BACKBUFFER false
#define BITSPERPIXEL 32

AmbloneDirectX::AmbloneDirectX() {
  IsInitialized = false;
}

AmbloneDirectX::~AmbloneDirectX() {
  if (!IsInitialized) return;
  if (g_pSurface) g_pSurface->Release();
  if (g_pd3dDevice) g_pd3dDevice->Release();
  if (g_pD3D) g_pD3D->Release();
  DestroyWindow(hMainWnd);
}

void AmbloneDirectX::Initialize(int DeviceNum) {
  if (IsInitialized) return;
  g_pD3D = NULL;
  g_pd3dDevice = NULL;
  g_pSurface = NULL;
  HINSTANCE hInstance;
  hMainWnd = CreateWindow("Amblone", "DirectX", WS_POPUPWINDOW|WS_CAPTION|WS_VISIBLE,
      480, 400, 320, 240, NULL, NULL, hInstance, NULL);
  InitD3D(hMainWnd, DeviceNum);
  IsInitialized = true;
}

bool AmbloneDirectX::InitD3D(HWND hWnd, int DeviceNum) {
	D3DDISPLAYMODE	ddm;
	D3DPRESENT_PARAMETERS	d3dpp;

	if((g_pD3D=Direct3DCreate9(D3D_SDK_VERSION))==NULL)
	{
		return false;
	}

	if(FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT + DeviceNum, &ddm)))
	{
		return false;
	}

	ZeroMemory(&d3dpp,sizeof(D3DPRESENT_PARAMETERS));
  RECT gScreenRect = {0, 0, 0, 0};

	d3dpp.Windowed=WINDOW_MODE;
	d3dpp.Flags=D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	d3dpp.BackBufferFormat=ddm.Format;
	d3dpp.BackBufferHeight=ScreenHeight=gScreenRect.bottom =ddm.Height;
	d3dpp.BackBufferWidth=ScreenWidth=gScreenRect.right =ddm.Width;
	d3dpp.MultiSampleType=D3DMULTISAMPLE_NONE;
	d3dpp.SwapEffect=D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow=hWnd;
	d3dpp.PresentationInterval=D3DPRESENT_INTERVAL_DEFAULT;
	d3dpp.FullScreen_RefreshRateInHz=D3DPRESENT_RATE_DEFAULT;

  DataLength = ScreenWidth * ScreenHeight * (BITSPERPIXEL / 8);
  pBits = new BYTE[DataLength];

  HRESULT CreateDiviceResult = g_pD3D->CreateDevice(D3DADAPTER_DEFAULT + DeviceNum,D3DDEVTYPE_HAL,hWnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING ,&d3dpp,&g_pd3dDevice);
	if(FAILED(CreateDiviceResult))
	{
    switch (CreateDiviceResult) {
      case D3DERR_DEVICELOST:
        break;
      case D3DERR_INVALIDCALL:
        break;
      case D3DERR_NOTAVAILABLE:
        break;
      case D3DERR_OUTOFVIDEOMEMORY:
        break;
      default:
        break;
    }
		return false;
	}

	if(FAILED(g_pd3dDevice->CreateOffscreenPlainSurface(ddm.Width, ddm.Height, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH/*D3DPOOL_SYSTEMMEM*/, &g_pSurface, NULL)))
	{
		return false;
	}

	return true;
}

BYTE* AmbloneDirectX::GetScreenBitmap() {
  if (USE_BACKBUFFER) {
    g_pd3dDevice->GetBackBuffer(1, 0, D3DBACKBUFFER_TYPE_MONO, &g_pSurface);
  }
  else {
    g_pd3dDevice->GetFrontBufferData(0, g_pSurface);
  }
  D3DLOCKED_RECT lockedRect;
  g_pSurface->LockRect(&lockedRect, NULL, D3DLOCK_NO_DIRTY_UPDATE|D3DLOCK_NOSYSLOCK|D3DLOCK_READONLY);
  memcpy(pBits, (BYTE*) lockedRect.pBits, DataLength);

  g_pSurface->UnlockRect();
  return pBits;
}
