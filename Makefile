#-------------------------------------------------------------------------------
# TARGET is the name of the output file
# BUILD is the directory where object files & intermediate files will be placed
# RELEASE is where the binary files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
#-------------------------------------------------------------------------------

TARGET		:=	libspec
BUILD		:=	obj
RELEASE     :=  lib
SOURCES		:=	src
INCLUDES	:=	include build 

#-------------------------------------------------------------------------------
# options for code generation
#-------------------------------------------------------------------------------

# This project really isn't large enough to have a seperate debug lib
DEBUG	    := -g
WARNING     := -Wall -Wpointer-arith -Wwrite-strings -Wuninitialized
CFLAGS	    := -std=c11 $(WARNING)
LDFLAGS     := -static-libgcc

ifdef SMALL
  CFLAGS  += -Os
  NODEBUG := 1
else
  CFLAGS  += -O2
endif
ifndef NODEBUG
  CFLAGS  += $(DEBUG)
endif

#-------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add 
# additional rules for different file extensions
#-------------------------------------------------------------------------------
CFLAGS	+=	$(INCLUDE)

ifneq ($(BUILD),$(notdir $(CURDIR)))
 
export OUTPUT	:=	$(CURDIR)/$(RELEASE)/$(TARGET)
export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir))
export CC		:=	clang
export AR		:=	llvm-ar
export OBJCOPY	:=	objcopy

ifdef GCC
  export CC		:=	gcc
  export AR		:=	ar
endif

#-------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#-------------------------------------------------------------------------------

CFILES		    := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
export OFILES	:= $(CFILES:.c=.o)
export INCLUDE	:= $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
                   -I$(CURDIR)/$(BUILD)

.PHONY: $(BUILD) clean rebuild default shared
 
#-------------------------------------------------------------------------------

$(BUILD):
	@[ -d $(CURDIR)/$(RELEASE) ] || mkdir -p $(CURDIR)/$(RELEASE)
	@[ -d $@ ] || mkdir -p $@
	@make --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
 
#-------------------------------------------------------------------------------

clean:
	@echo Cleaning... $(TARGET)
	@rm -rf $(BUILD) $(RELEASE)
 
rebuild: clean $(BUILD)

#-------------------------------------------------------------------------------

else
 
UNAME   := $(shell uname)
DEPENDS	:= $(OFILES:.o=.d)

ifeq ($(UNAME), Linux)
# A linux guy would be really helpful here
  SHARED  := $(OUTPUT).so
  CFLAGS  += -fPIC
else
  SHARED  := $(OUTPUT).dll
  LDFLAGS += -Wl,--add-stdcall-alias,--out-implib,$(OUTPUT).dll.a
endif
 
#-------------------------------------------------------------------------------
# main targets
#-------------------------------------------------------------------------------

ifdef SMALL
default : $(OUTPUT).a $(SHARED)
	@echo "Stripping Libraries"
	@strip --strip-all ../$(RELEASE)/*
else
default : $(OUTPUT).a $(SHARED)
endif

$(OUTPUT).a : $(OFILES)
	@echo Building static library
	@$(AR) rcs $@ $?

$(SHARED) : $(OFILES)
	@echo Building shared library
	@$(CC) -shared $(LDFLAGS) -o $@ $?
 
#-------------------------------------------------------------------------------
# Compile Targets for C/C++
#-------------------------------------------------------------------------------
 
#-------------------------------------------------------------------------------
%.o : %.cpp
	@echo Compiling $(notdir $<)
	@$(CXX) -MMD -MF $*.d -MP $(CFLAGS) -c $< -o $*.o

#-------------------------------------------------------------------------------
%.o : %.c
	@echo Compiling $(notdir $<)
	@$(CC) -MMD -MF $*.d -MP $(CFLAGS) -c $< -o $*.o

-include $(DEPENDS) 
#-------------------------------------------------------------------------------
endif