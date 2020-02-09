.PHONY : build clean rebuild

SRC_ROOT = src

SUBDIRS := \
./ \
jsoncpp/ \

# Get all source dirs.
SRCDIRS  := $(foreach c, $(SUBDIRS), $(subst $(c), $(SRC_ROOT)/$(c), $(c)))
CPP_SRCFILES := $(foreach srcdir, $(SRCDIRS), $(wildcard $(srcdir)*.cpp))                    
C_SRCFILES := $(foreach srcdir, $(SRCDIRS), $(wildcard $(srcdir)*.c))   

DIR_LIB = ../../libs

# All the include pathes.
INCS += ../../include/


# The output file name.
TARGET = demo

# The C program compiler.
CC = g++

# The pre-processor and compiler options.
# Users can override those variables from the command line.
CFLAGS := $(foreach inc, $(INCS), -I$(inc))
CFLAGS +=  -fPIC -g -D_GLIBCXX_USE_CXX11_ABI=0

# The options used in linking as well as in any direct use of ld.
ifdef LINUX64
LDFLAGS := -L$(DIR_LIB)/x64
else
LDFLAGS := -L$(DIR_LIB)/x86 
endif
LDFLAGS += -fPIC -pthread -laiui -lmsc -lmsc_aiui -lasound

## Stable Section: usually no need to be changed. But you can add more.
##==========================================================================
SOURCEDIR  = $(sort  $(foreach subdir, $(dir $(CPP_SRCFILES)), $(patsubst %/, %, $(subdir))))
VPATH      = $(SOURCEDIR)
CPP_OBJS   = $(patsubst %.cpp, %.o, $(CPP_SRCFILES))
C_OBJS     = $(patsubst %.c, %.o, $(C_SRCFILES))     			 
BUILDDIR   = ./build
OBJECTDIR  = $(BUILDDIR)/objs
CPP_BUILDOBJS  = $(foreach obj, $(CPP_OBJS), $(subst $(dir $(obj)), $(OBJECTDIR)/, $(obj)))
C_BUILDOBJS  = $(foreach obj, $(C_OBJS), $(subst $(dir $(obj)), $(OBJECTDIR)/, $(obj)))
BUILDDEPS  = $(CPP_BUILDOBJS:.o=.d)

test:
	@echo ${PWD}


# Build target
#==========================================================================
build : prepare $(TARGET)

prepare :
	mkdir -p $(BUILDDIR)
	mkdir -p $(OBJECTDIR)


# Rules for generating object files (.o).
#.cpp_2_.o
#==========================================================================
$(OBJECTDIR)/%.o : %.c                                                 
	@$(CROSS_COMPILE)gcc -c $(CFLAGS) $< -o $@     

#.cpp_2_.o
#==========================================================================
$(OBJECTDIR)/%.o : %.cpp
	@$(CC) $(CFLAGS) -std=c++11 -o $@  -c $< -MD -MF $(patsubst %.o, %.d, $@)

# Rules for generating the executable.
#==========================================================================
$(TARGET) : $(C_BUILDOBJS) $(CPP_BUILDOBJS) 
	$(CC) -o $(BUILDDIR)/$@ $^ $(LDFLAGS)
	@echo "build success."

# Clean target
#==========================================================================
clean :
	rm -f $(CPP_BUILDOBJS) $(C_BUILDOBJS) $(BUILDDEPS) $(BUILDDIR)/$(TARGET) *.wav

# Rebuild target
#==========================================================================
rebuild : clean build
