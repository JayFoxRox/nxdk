#include <hal/xbox.h>
#include <hal/video.h>
#include <hal/debug.h>
#include <windows.h>

int main() {
  XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);

#if 0
  debugPrint("Launching client\n");
  Sleep(1000);
  XLaunchXBE("\\Device\\CdRom0\\client.xbe");
#else
  debugPrint("Launching server\n");
  Sleep(1000);
  XLaunchXBE("\\Device\\CdRom0\\server.xbe");
#endif

  // We should never reach this
  return 1;
}
