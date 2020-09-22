#**********************************************************************
#
# Standard makefile
#
# written 2007 by Martin Steen
#
#**********************************************************************
#
# C++ : Name of C++ Compiler
#
C++       = g++
#
ifeq ($(OS),Windows_NT)
  OS_SYS   = Windows
else
  OS_SYS := $(shell uname -s)
endif

#
.SUFFIXES : .cpp .o
#
#
#
CPPFILES = ./source/main.cpp \
           ./source/NStringTool.cpp \
           ./source/WriteTga.cpp
#
CCFLAGS = -O2 -I./source
#
#
#
TARGET   = clmandelbrot
#
default : $(TARGET)
#
clean:
	rm -rf $(OBJECTS1) $(OBJECTS2)
	make
#
#
.cpp.o:
	$(C++) -c $(CCFLAGS) -o$@ $<

.c.o:
	gcc -c $(CCFLAGS) -o$@ $<

#
OBJECTS1 = $(CPPFILES:.cpp=.o)
OBJECTS2 = $(CFILES:.c=.o)
#
#
ifeq ($(OS_SYS),Darwin)
  LDFLAGS = -lm -framework OpenCL
endif
ifeq ($(OS_SYS),Linux)
  LDFLAGS = -lm -lOpenCL
endif

#
$(TARGET) : $(OBJECTS1) $(OBJECTS2)
	$(C++) -o $(TARGET) $(OBJECTS1) $(OBJECTS2) $(LIBPATH) $(LDFLAGS)
	strip $(TARGET)
#

