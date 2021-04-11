#include <hal/debug.h>
#include <hal/video.h>

int main() {
  XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);

  debugPrint("Hello World!");
  while(1);
  return 0;
}
