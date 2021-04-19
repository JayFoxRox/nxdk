if(NXDK)
  # Hack for direct.h and friends
  include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/")
endif()

set(NEWTON_BUILD_SANDBOX_DEMOS "OFF")
set(NEWTON_BUILD_CORE_ONLY "ON")
set(NEWTON_BUILD_SHARED_LIBS "OFF")
set(NEWTON_USE_PLUGIN_DLL "OFF")
set(NEWTON_WITH_AVX_PLUGIN "OFF")
set(NEWTON_WITH_AVX2_PLUGIN OFF)
set(NEWTON_WITH_REFERENCE_GPU_PLUGIN "OFF")
add_subdirectory(newton-dynamics)
include_directories(newton-dynamics/sdk/dNewton/)
target_compile_definitions(dgCore PUBLIC -D_NEWTON_STATIC_LIB -DDG_USE_THREAD_EMULATION -DDG_SCALAR_VECTOR_CLASS)

#FIXME: float.h
target_compile_options(dgCore PUBLIC
  -DEM_INVALID=1
  -DEM_DENORMAL=1
  -DEM_ZERODIVIDE=1
  -D_clearfp\(...\)=0
  -D_controlfp\(...\)=0
  -D_finite\(...\)=0
  -D_isnan\(...\)=0
)

add_executable(default main.cpp)
target_link_libraries(default newton)

create_xiso("${CMAKE_BINARY_DIR}/newton-dynamics.iso" "${CMAKE_BINARY_DIR}/")
