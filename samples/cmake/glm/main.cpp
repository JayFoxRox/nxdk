#include <hal/debug.h>
#include <hal/video.h>
#include <glm/glm.hpp>

int main() {
  XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);

  glm::vec3 v = glm::vec3(0.0f, 1.0f, 2.0f);

  debugPrint("Hello World! %d", int(1000.0 * glm::length(v)));
  while(1);
  return 0;
}
