INCLUDES := \
  Common.h \
  Cxbx.h \
  Error.h \
  Exe.h \
  Xbe.h
SRCS := \
  Common.cpp \
  Error.cpp \
  Exe.cpp \
  OpenXDK.cpp \
  Xbe.cpp

all: cxbe cdxt

cxbe: Cxbe.cpp $(SRCS) $(INCLUDES)
	$(CXX) -o '$@' $^

cdxt: Cdxt.cpp $(SRCS) $(INCLUDES)
	$(CXX) -o '$@' $^

.PHONY: clean
clean:
	rm -f cxbe cdxt
