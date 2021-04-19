if(NXDK)
  # Hack for memory.h
  file(TOUCH ${CMAKE_CURRENT_BINARY_DIR}/memory.h)
  include_directories(SYSTEM "${CMAKE_CURRENT_BINARY_DIR}/")
endif()

set(ozz_build_tools OFF)
set(ozz_build_fbx OFF)
set(ozz_build_data OFF)
set(ozz_build_samples OFF)
set(ozz_build_howtos OFF)
set(ozz_build_tests OFF)
set(ozz_build_simd_ref OFF)
set(ozz_build_cpp11 OFF)
set(ozz_build_msvc_rt_dll OFF)
set(ozz_build_postfix ON)
add_subdirectory(ozz-animation EXCLUDE_FROM_ALL)

target_compile_options(ozz_base PUBLIC
  -Dcout=clog
)

find_package(SDL2 REQUIRED)

#add_custom_command(
#  DEPENDS $<$<BOOL:${ozz_build_fbx}>:BUILD_DATA>
#          "${ozz_media_directory}/bin/alain_skeleton.ozz"
#          "${ozz_media_directory}/bin/alain_crossarms.ozz"
#  OUTPUT  "${CMAKE_CURRENT_BINARY_DIR}/media/skeleton.ozz"
#          "${CMAKE_CURRENT_BINARY_DIR}/media/animation.ozz"
#  COMMAND ${CMAKE_COMMAND} -E make_directory media
#  COMMAND ${CMAKE_COMMAND} -E copy "${ozz_media_directory}/bin/alain_skeleton.ozz" "./media/skeleton.ozz"
#  COMMAND ${CMAKE_COMMAND} -E copy "${ozz_media_directory}/bin/alain_crossarms.ozz" "./media/animation.ozz"
#  VERBATIM)

add_executable(default
  main.cpp
#  "${CMAKE_CURRENT_BINARY_DIR}/media/skeleton.ozz"
#  "${CMAKE_CURRENT_BINARY_DIR}/media/animation.ozz"
)
target_link_libraries(default ozz_geometry ozz_animation_offline SDL2::SDL2)

if(NXDK)
  create_xiso("${CMAKE_BINARY_DIR}/ozz-animation.iso" "${CMAKE_BINARY_DIR}/")
endif()
