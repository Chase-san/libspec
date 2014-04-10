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
CFLAGS	    := $(DEBUG) -O2 -std=c11 $(WARNING)
LDFLAGS     := -static-libgcc

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

#-------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#-------------------------------------------------------------------------------

CFILES		    := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
export OFILES	:= $(CFILES:.c=.o)
export INCLUDE	:= $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
                   -I$(CURDIR)/$(BUILD)
 
.PHONY: $(BUILD) clean rebuild default
 
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
 
DEPENDS	:= $(OFILES:.o=.d)
 
#-------------------------------------------------------------------------------
# main targets
#-------------------------------------------------------------------------------

default : $(OUTPUT).a

$(OUTPUT).a : $(OFILES)
	@echo Building static library
	@$(AR) rcs $@ $?
	@echo Building dynamic library
	@$(CC) -shared -o $(OUTPUT).dll $(LDFLAGS) -Wl,--out-implib,$(OUTPUT)_dll.a
 
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
#-------------------------------------------------------------------------------