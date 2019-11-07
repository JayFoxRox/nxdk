OGG_DIR := $(NXDK_DIR)/lib/ogg/ogg

CFLAGS += -isystem $(OGG_DIR)/include

OGG_SRCS := \
	$(OGG_DIR)/src/framing.c \
	$(OGG_DIR)/src/bitwise.c

OGG_OBJS = $(addsuffix .obj, $(basename $(OGG_SRCS)))

$(NXDK_DIR)/lib/libogg.lib: $(OGG_OBJS)

main.exe: $(NXDK_DIR)/lib/libogg.lib

CLEANRULES += clean-ogg

.PHONY: clean-ogg
clean-ogg:
	$(VE)rm -f $(OGG_OBJS) $(NXDK_DIR)/lib/libogg.lib
