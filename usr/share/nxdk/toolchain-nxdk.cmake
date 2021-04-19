set(NXDK_DIR $ENV{NXDK_DIR})

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR Generic) #FIXME: Pentium 3
set(TOOLCHAIN_PREFIX nxdk)

set(WIN32 1)

set(NXDK 1)

# Keep depfiles to track header changes
if(NOT "${lang}" STREQUAL "ASM")
  set(CMAKE_DEPFILE_FLAGS_${lang} "-MD -MT <OBJECT> -MF <DEPFILE>")
endif()

#FIXME: Include stuff for MinGW / Windows-GNU / Windows-Clang?
#FIXME: How to set the ABI?

#set(NXDK_LINK_XBE "${NXDK_DIR}/usr/bin/${TOOLCHAIN_PREFIX}-exe2xbe -OUT:<TARGET>.xbe <TARGET>.exe")
set(NXDK_LINK_XBE "")

# cross compiler to use for C
set(CMAKE_C_COMPILER "${NXDK_DIR}/usr/bin/${TOOLCHAIN_PREFIX}-cc")
set(CMAKE_C_STANDARD_LIBRARIES "${NXDK_DIR}/lib/libwinapi.lib ${NXDK_DIR}/lib/xboxkrnl/libxboxkrnl.lib ${NXDK_DIR}/lib/libxboxrt.lib  ${NXDK_DIR}/lib/libpdclib.lib ${NXDK_DIR}/lib/libnxdk_hal.lib ${NXDK_DIR}/lib/libnxdk.lib ${NXDK_DIR}/lib/libnxdk_usb.lib") #"${CMAKE_CXX_STANDARD_LIBRARIES_INIT}"
set(CMAKE_C_LINK_EXECUTABLE "${NXDK_DIR}/usr/bin/${TOOLCHAIN_PREFIX}-link <FLAGS> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -out:<TARGET>.exe <LINK_LIBRARIES>; ${NXDK_LINK_XBE}")

# cross compiler to use for C++
set(CMAKE_CXX_COMPILER "${NXDK_DIR}/usr/bin/${TOOLCHAIN_PREFIX}-c++")
set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_C_STANDARD_LIBRARIES} ${NXDK_DIR}/lib/libc++.lib")
set(CMAKE_CXX_LINK_EXECUTABLE "${NXDK_DIR}/usr/bin/${TOOLCHAIN_PREFIX}-link <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -out:<TARGET>.exe <LINK_LIBRARIES>; ${NXDK_LINK_XBE}")


function(create_xiso output_file input_folder)
    add_custom_target(xiso ALL
        COMMAND "${NXDK_DIR}/usr/bin/${TOOLCHAIN_PREFIX}-xiso" "-c" "${input_folder}" "${output_file}"
        COMMENT "Creating XISO"
    )
endfunction()


set(CMAKE_SYSROOT "${NXDK_DIR}")
#message("TOOLCHAIN ${CMAKE_MODULE_PATH}")
#set(CMAKE_MODULE_PATH "${NXDK_DIR}/usr/share/cmake/Modules/")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
