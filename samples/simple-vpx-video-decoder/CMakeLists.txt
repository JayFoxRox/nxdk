cmake_minimum_required(VERSION 3.15)
project(thot)
add_executable(meh
  main.cpp
  video_reader.c
)
target_link_libraries(meh -lvpx -lSDL2 -lm)
include_directories(
  #/home/fox/Data/Projects/XboxDev/nxdk/lib/libvpx
  #/home/fox/Data/Projects/XboxDev/nxdk/lib/libvpx/libvpx
  /home/fox/Data/Projects/XboxDev/nxdk/lib/libyuv/libyuv/include
  /usr/include/vpx
  /usr/include/SDL2
  thot
  thot/libvpx
)
