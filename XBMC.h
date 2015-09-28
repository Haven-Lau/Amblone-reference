

class XBMC {
  public:
    XBMC(CString IP, int Port);
    ~XBMC();
    COLORREF *GetQuarterColours();
    COLORREF GetQuarterColour(int Quarter);
    DWORD XBMC::RequesterThread();
  private:    
    void GetXbmcCapture();
    BITMAPFILEHEADER ParseBitmapFileHeader(unsigned char *Decoded);
    long ParseBitmapWidth(unsigned char *Decoded);
    long ParseBitmapHeight(unsigned char *Decoded);
    void Stop();
    // Shared variables
    CString sh_IP;
    int sh_Port;
    COLORREF sh_rgbQ[4];
    bool sh_running;
    bool sh_stopped;
};