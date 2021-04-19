if(NXDK)
  # Hack for winsock2.h and mmsystem.h
  include_directories(SYSTEM "${NXDK_DIR}/lib/winapi/ws2_32;${NXDK_DIR}/lib/winapi/winmm")

  # Create lwip import lib
  add_subdirectory(${NXDK_DIR}/lib/net/lwip "${CMAKE_BINARY_DIR}/lwip/")
  get_target_property(LWIPCORE_INCLUDES lwipcore INCLUDE_DIRECTORIES)
  message("${LWIPCORE_INCLUDES}")
  set_target_properties(lwipcore
                        PROPERTIES INCLUDE_DIRECTORIES "${NXDK_DIR}/lib/net/lwip/src/include/;${NXDK_DIR}/lib/net/nforceif/include/")
  set_target_properties(lwipcore
                        PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${NXDK_DIR}/lib/net/lwip/src/include/;${NXDK_DIR}/lib/net/nforceif/include/")
  get_target_property(LWIPCORE_INCLUDES lwipcore INCLUDE_DIRECTORIES)
  message("${LWIPCORE_INCLUDES}")

  # Add pktdrv
  add_library(pktdrv ${NXDK_DIR}/lib/net/pktdrv/pktdrv.c)
  target_include_directories(pktdrv PUBLIC ${NXDK_DIR}/lib/net/pktdrv/)

  # Combine lwip and Xbox driver into libnet
  add_library(net
    "${NXDK_DIR}/lib/net/nforceif/src/driver.c"
    "${NXDK_DIR}/lib/net/nforceif/src/sys_arch.c"
  )
  target_link_libraries(net lwipcore pktdrv)
endif()

# Add enet and fix a bug in enet where include folder is not known to itself
add_subdirectory(enet)
set_target_properties(enet
                      PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/enet/include/")

if(NXDK)
  # Make enet depend on Xbox network
  set_target_properties(enet
                        PROPERTIES LINK_LIBRARIES "${NXDK_DIR}/lib/ws2_32.lib;${NXDK_DIR}/lib/winmm.lib;net")
endif()

# Build project
add_executable(server server.c)
target_link_libraries(server enet)
add_executable(client client.c)
target_link_libraries(client enet)
if(NXDK)
  add_executable(default main.c)
endif()

if(NXDK)
  create_xiso("${CMAKE_BINARY_DIR}/enet.iso" "${CMAKE_BINARY_DIR}/")
endif()
