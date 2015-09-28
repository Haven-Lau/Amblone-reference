#include "Amblone.h"
#include <d3d9.h>
#include <d3dx9tex.h>

class AmbloneDirectX {
  public:
    AmbloneDirectX();
    ~AmbloneDirectX();
    void Initialize(int DeviceNum);
    BYTE* GetScreenBitmap();
  private:
    bool InitD3D(HWND hWnd, int DeviceNum);

    bool IsInitialized;
    IDirect3D9        *g_pD3D;
    IDirect3DDevice9  *g_pd3dDevice;
    IDirect3DSurface9 *g_pSurface;
    HWND hMainWnd;
    int ScreenWidth;
    int ScreenHeight;
    long DataLength;
    BYTE* pBits;
};