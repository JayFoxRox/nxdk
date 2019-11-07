cmake_minimum_required(VERSION 3.15)
project(thot)
add_executable(meh
  main.c
  /home/fox/Data/Projects/XboxDev/nxdk/lib/libvpx/libvpx/tools_common.c
  /home/fox/Data/Projects/XboxDev/nxdk/lib/libvpx/libvpx/video_reader.c
  /home/fox/Data/Projects/XboxDev/nxdk/lib/libvpx/libvpx/ivfdec.c
)
target_link_libraries(meh -lvpx -lSDL2 -lm)
include_directories(
  #/home/fox/Data/Projects/XboxDev/nxdk/lib/libvpx
  #/home/fox/Data/Projects/XboxDev/nxdk/lib/libvpx/libvpx
  /usr/include/vpx
  /usr/include/SDL2
  thot
  thot/libvpx
)
