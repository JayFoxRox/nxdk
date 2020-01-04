DEPS := \
  Common.h \
  Cxbx.h \
  Error.h \
  Exe.h \
  Xbe.h

OBJS := \
  Common.o \
  Error.o \
  Exe.o \
  OpenXDK.o \
  Xbe.o

all: cxbe cdxt

%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c -o '$@' '$<'

cxbe: Cxbe.cpp $(OBJS)
	$(CXX) -o '$@' $^

cdxt: Cdxt.cpp $(OBJS)
	$(CXX) -o '$@' $^

.PHONY: clean
clean:
	rm -f cxbe cdxt
