NXDK_RDT_DIR = $(NXDK_DIR)/lib/nxdk-rdt/nxdk-rdt
NXDK_RDT_SRCS += $(NXDK_RDT_DIR)/dbgd.c
NXDK_RDT_SRCS += $(NXDK_RDT_DIR)/net.c
NXDK_RDT_SRCS += $(NXDK_RDT_DIR)/../library.c
NXDK_RDT_OBJS = $(addsuffix .obj, $(basename $(NXDK_RDT_SRCS)))

$(NXDK_DIR)/lib/libnxdk-rdt.lib: $(NXDK_RDT_OBJS)

main.exe: $(NXDK_DIR)/lib/libnxdk-rdt.lib

CLEANRULES += clean-nxdk-rdt

.PHONY: clean-nxdk-rdt
clean-nxdk-rdt:
	$(VE)rm -f $(NXDK_RDT_OBJS) \
	           $(NXDK_DIR)/lib/libnxdk-rdt.lib
