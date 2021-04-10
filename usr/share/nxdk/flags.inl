NXDK_CFLAGS="-target i386-pc-win32 -march=pentium3 \
             -ffreestanding -nostdlib -fno-builtin \
             -I${NXDK_DIR}/lib -I${NXDK_DIR}/lib/xboxrt/libc_extensions \
             -I${NXDK_DIR}/lib/hal \
             -isystem ${NXDK_DIR}/lib/pdclib/include \
             -I${NXDK_DIR}/lib/pdclib/platform/xbox/include \
             -I${NXDK_DIR}/lib/winapi \
             -I${NXDK_DIR}/lib/xboxrt/vcruntime \
             -I${NXDK_DIR}/usr/include \
             -I${NXDK_DIR}/usr/local/include \
             -DNXDK -D__XBOX__=1 -D__STDC__=1"
NXDK_ASFLAGS="-target i386-pc-win32 -march=pentium3 \
              -nostdlib -I${NXDK_DIR}/lib -I${NXDK_DIR}/lib/xboxrt"
NXDK_CXXFLAGS="-I${NXDK_DIR}/lib/libcxx/include ${NXDK_CFLAGS} -fno-exceptions"
NXDK_LDFLAGS="-subsystem:windows -fixed -base:0x00010000 \
              -stack:65536 -safeseh:no -merge:.edata=.edataxb"
