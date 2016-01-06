#ifdef WIN32

#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif



//#define _WIN32_WINNT 0x0501
//#define WIN32_LEAN_AND_MEAN  // WinSock.hスイッチ

#include "AkashaFrames.h"

//追加マニフェスト
#ifdef MSVC
#pragma comment(linker,"/manifestdependency:\"type='win32' \
  name='Microsoft.Windows.Common-Controls' \
  version='6.0.0.0' \
  processorArchitecture='x86' \
  publicKeyToken='6595b64144ccf1df' \
  language='*'\"") 
#endif


/*################################################################
  * アプリケーションエントリポイント
  */

//int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int){
int main(){


  //Start Win32++
  Win32AkashaApplication app;
	return app.Run();

  

}
