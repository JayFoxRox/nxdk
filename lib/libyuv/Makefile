#FIXME: Document that we need an older version which still has MMX.

LIBYUV_DIR := $(NXDK_DIR)/lib/libyuv/libyuv

CXXFLAGS += -I$(LIBYUV_DIR)/include -I$(LIBYUV_DIR)/..
CFLAGS += -I$(LIBYUV_DIR)/include -I$(LIBYUV_DIR)/..



#FIXME: Needs update; files are on old config


LIBYUV_SRCS := \
	$(LIBYUV_DIR)/source/compare.cc \
	$(LIBYUV_DIR)/source/convert.cc \
	$(LIBYUV_DIR)/source/cpu_id.cc \
	$(LIBYUV_DIR)/source/format_conversion.cc \
	$(LIBYUV_DIR)/source/planar_functions.cc \
	$(LIBYUV_DIR)/source/row_common.cc \
	$(LIBYUV_DIR)/source/row_posix.cc \
	$(LIBYUV_DIR)/source/row_table.cc \
	$(LIBYUV_DIR)/source/scale.cc \
	$(LIBYUV_DIR)/source/video_common.cc
# Bugs..	$(LIBYUV_DIR)/source/rotate.cc \


LIBYUV_OBJS = $(addsuffix .obj, $(basename $(LIBYUV_SRCS)))


$(NXDK_DIR)/lib/libyuv.lib: $(LIBYUV_OBJS)

main.exe: $(NXDK_DIR)/lib/libyuv.lib

CLEANRULES += clean-libyuv

.PHONY: clean-libyuv
clean-libyuv:
	$(VE)rm -f $(LIBYUV_OBJS) $(NXDK_DIR)/lib/libyuv.lib
