# In-tree configured using:
#
# ./configure --target=x86-win32-gcc --disable-vp9 --disable-vp8-encoder --disable-sse2 --disable-runtime_cpu_detect --disable-multithread --disable-unit_tests --enable-examples --disable-docs --disable-webm_io --disable-postproc --disable-libyuv
#
# We'll probably want to switch to VP8 (optimized code for Pentium 3 exists).
# We'll have to re-introduce all removed MMX/SSE paths:
# - https://github.com/webmproject/libvpx/search?q=mmx&type=Commits
# - https://github.com/webmproject/libvpx/search?q=sse&type=Commits
# (Might have started with libvpx 1.6.1?)

LIBVPX_DIR := $(NXDK_DIR)/lib/libvpx/libvpx

CFLAGS += -I$(LIBVPX_DIR) -I$(LIBVPX_DIR)/.. -DWINAPI_FAMILY_PARTITION=
CXXFLAGS += -I$(LIBVPX_DIR) -I$(LIBVPX_DIR)/.. -DWINAPI_FAMILY_PARTITION=



#FIXME: Needs update; files are on old config



LIBVPX_SRCS := \
	$(LIBVPX_DIR)/vpx/src/vpx_decoder.c \
	$(LIBVPX_DIR)/vpx/src/vpx_encoder.c \
	$(LIBVPX_DIR)/vpx/src/vpx_codec.c \
	$(LIBVPX_DIR)/vpx/src/vpx_image.c \
	$(LIBVPX_DIR)/vpx/src/vpx_psnr.c \
	$(LIBVPX_DIR)/vpx_mem/vpx_mem.c \
	$(LIBVPX_DIR)/vpx_scale/generic/vpx_scale.c \
	$(LIBVPX_DIR)/vpx_scale/generic/yv12config.c \
	$(LIBVPX_DIR)/vpx_scale/generic/yv12extend.c \
	$(LIBVPX_DIR)/vpx_scale/generic/gen_scalers.c \
	$(LIBVPX_DIR)/vpx_scale/vpx_scale_rtcd.c \
	$(LIBVPX_DIR)/vpx_dsp/prob.c \
	$(LIBVPX_DIR)/vpx_dsp/bitreader.c \
	$(LIBVPX_DIR)/vpx_dsp/bitreader_buffer.c \
	$(LIBVPX_DIR)/vpx_dsp/intrapred.c \
	$(LIBVPX_DIR)/vpx_dsp/vpx_convolve.c \
	$(LIBVPX_DIR)/vpx_dsp/x86/vpx_asm_stubs.c \
	$(LIBVPX_DIR)/vpx_dsp/loopfilter.c \
	$(LIBVPX_DIR)/vpx_dsp/vpx_dsp_rtcd.c \
	$(LIBVPX_DIR)/vpx_util/vpx_thread.c \
	$(LIBVPX_DIR)/vp8/common/alloccommon.c \
	$(LIBVPX_DIR)/vp8/common/blockd.c \
	$(LIBVPX_DIR)/vp8/common/copy_c.c \
	$(LIBVPX_DIR)/vp8/common/debugmodes.c \
	$(LIBVPX_DIR)/vp8/common/dequantize.c \
	$(LIBVPX_DIR)/vp8/common/entropy.c \
	$(LIBVPX_DIR)/vp8/common/entropymode.c \
	$(LIBVPX_DIR)/vp8/common/entropymv.c \
	$(LIBVPX_DIR)/vp8/common/extend.c \
	$(LIBVPX_DIR)/vp8/common/filter.c \
	$(LIBVPX_DIR)/vp8/common/findnearmv.c \
	$(LIBVPX_DIR)/vp8/common/generic/systemdependent.c \
	$(LIBVPX_DIR)/vp8/common/idct_blk.c \
	$(LIBVPX_DIR)/vp8/common/idctllm.c \
	$(LIBVPX_DIR)/vp8/common/rtcd.c \
	$(LIBVPX_DIR)/vp8/common/vp8_loopfilter.c \
	$(LIBVPX_DIR)/vp8/common/loopfilter_filters.c \
	$(LIBVPX_DIR)/vp8/common/mbpitch.c \
	$(LIBVPX_DIR)/vp8/common/modecont.c \
	$(LIBVPX_DIR)/vp8/common/quant_common.c \
	$(LIBVPX_DIR)/vp8/common/reconinter.c \
	$(LIBVPX_DIR)/vp8/common/reconintra.c \
	$(LIBVPX_DIR)/vp8/common/reconintra4x4.c \
	$(LIBVPX_DIR)/vp8/common/setupintrarecon.c \
	$(LIBVPX_DIR)/vp8/common/swapyv12buffer.c \
	$(LIBVPX_DIR)/vp8/common/treecoder.c \
	$(LIBVPX_DIR)/vp8/common/x86/filter_x86.c \
	$(LIBVPX_DIR)/vp8/common/x86/vp8_asm_stubs.c \
	$(LIBVPX_DIR)/vp8/common/x86/loopfilter_x86.c \
	$(LIBVPX_DIR)/vp8/common/x86/idct_blk_mmx.c \
	$(LIBVPX_DIR)/vp8/vp8_dx_iface.c \
	$(LIBVPX_DIR)/vp8/decoder/dboolhuff.c \
	$(LIBVPX_DIR)/vp8/decoder/decodemv.c \
	$(LIBVPX_DIR)/vp8/decoder/decodeframe.c \
	$(LIBVPX_DIR)/vp8/decoder/detokenize.c \
	$(LIBVPX_DIR)/vp8/decoder/onyxd_if.c \
	$(LIBVPX_DIR)/vpx_config.c \
	$(LIBVPX_DIR)/vpx_ports/emms.asm \
	$(LIBVPX_DIR)/vpx_ports/x86_abi_support.asm \
	$(LIBVPX_DIR)/vpx_dsp/x86/intrapred_sse2.asm \
	$(LIBVPX_DIR)/vp8/common/x86/vp8_loopfilter_mmx.asm \
	$(LIBVPX_DIR)/vp8/common/x86/subpixel_mmx.asm \
	$(LIBVPX_DIR)/vp8/common/x86/dequantize_mmx.asm \
	$(LIBVPX_DIR)/vp8/common/x86/idctllm_mmx.asm \
	$(LIBVPX_DIR)/vp8/common/x86/iwalsh_mmx.asm \
	$(LIBVPX_DIR)/vp8/common/x86/recon_mmx.asm
# Bugs?
#	$(LIBVPX_DIR)/vpx_dsp/x86/loopfilter_sse2.c \ - Needs SSE2


LIBVPX_OBJS = $(addsuffix .obj, $(basename $(LIBVPX_SRCS)))

YASM=yasm

# Cancel built-in implicit rules
%: %.o
%.asm:
%.a:
%: %.cc

%.obj: %.asm
	@echo "[ YASM     ] $@"
	$(VE) $(YASM) -fwin32 -I$(LIBVPX_DIR) -I$(LIBVPX_DIR)/.. -o '$@' '$<'


$(NXDK_DIR)/lib/libvpx.lib: $(LIBVPX_OBJS)

main.exe: $(NXDK_DIR)/lib/libvpx.lib

CLEANRULES += clean-libvpx

.PHONY: clean-libvpx
clean-libvpx:
	$(VE)rm -f $(LIBVPX_OBJS) $(NXDK_DIR)/lib/libvpx.lib
