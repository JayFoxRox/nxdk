LIBSNDFILE_DIR = $(NXDK_DIR)/lib/libsndfile/libsndfile
LIBSNDFILE_SRCS = 
LIBSNDFILE_OBJS = $(addsuffix .obj, $(basename $(LIBSNDFILE_SRCS)))

NXDK_CFLAGS += -I $(LIBSNDFILE_DIR)

$(NXDK_DIR)/lib/libsndfile.lib: $(LIBSNDFILE_OBJS)

main.exe: $(NXDK_DIR)/lib/libsndfile.lib

CLEANRULES += clean-libsndfile

.PHONY: clean-libsndfile
clean-libsndfile:
	$(VE)rm -f $(LIBSNDFILE_OBJS) $(NXDK_DIR)/lib/libsndfile.lib
