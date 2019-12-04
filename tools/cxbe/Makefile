INCLUDES := $(wildcard *.h)
SRCS := $(wildcard *.cpp)

GIT_VERSION := nxdk-$(shell git describe --dirty --always --tags)
VERSION := $(GIT_VERSION)

cxbe: $(SRCS) $(INCLUDES)
	$(CXX) -o '$@' $(SRCS) -DVERSION=\"$(VERSION)\"

.PHONY: clean
clean:
	rm -f cxbe
