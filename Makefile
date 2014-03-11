# I like the pretty error messages clang gives
CC := clang
AR := llvm-ar

ifdef GCC
    CC := gcc
    AR := ar
endif

CFLAGS := -std=c11 -Wpointer-arith -fms-extensions
LDFLAGS := -static-libgcc

DEBUG_CFLAGS := -g3 -Wall
DEBUG_LDFLAGS :=

RELEASE_CFLAGS := -g0 -O3
RELEASE_LDFLAGS := -s

NONLIBSRCS := main.c $(wildcard test*.c)

OBJ_PATH := obj
PROJECT_NAME := spec

##############################
# The rest is automagic
SRCS := $(wildcard *.c)
LIBSRCS := $(filter-out $(NONLIBSRCS),$(SRCS))

##############################
# DEBUG STUFF
DEBUG_ARTIFACT = $(PROJECT_NAME)_d.exe
DEBUG_STATIC_LIB = lib$(PROJECT_NAME)_d.a
DEBUG_OBJS = $(SRCS:%.c=$(OBJ_PATH)/%_d.o)
DEBUG_LIBOBJS = $(LIBSRCS:%.c=$(OBJ_PATH)/%_d.o)
DEBUG_DEPS = $(SRCS:%.c=$(OBJ_PATH)/%_d.d)

$(OBJ_PATH)/%_d.o: %.c
	@echo Compiling debug $<
	@$(CC) -c $(CFLAGS) $(DEBUG_CFLAGS) -MMD -o $@ $<
	
$(DEBUG_ARTIFACT): $(OBJ_PATH) $(DEBUG_OBJS)
	@echo Building $@
	@$(CC) $(DEBUG_OBJS) $(LDFLAGS) $(DEBUG_LDFLAGS) -o $@

$(DEBUG_STATIC_LIB): $(OBJ_PATH) $(DEBUG_LIBOBJS)
	@echo Packing $@
	@$(AR) rcs $@ $(DEBUG_LIBOBJS)

.PHONY: debug
debug: $(DEBUG_ARTIFACT)

.PHONY: dlib
dlib: debug_lib

.PHONY: debug_lib
debug_lib: $(DEBUG_STATIC_LIB)

##############################
# RELEASE STUFF
RELEASE_ARTIFACT = $(PROJECT_NAME).exe
RELEASE_STATIC_LIB = lib$(PROJECT_NAME).a
RELEASE_OBJS = $(SRCS:%.c=$(OBJ_PATH)/%.o)
RELEASE_LIBOBJS = $(LIBSRCS:%.c=$(OBJ_PATH)/%.o)
RELEASE_DEPS = $(SRCS:%.c=$(OBJ_PATH)/%.d)

$(OBJ_PATH)/%.o: %.c
	@echo Compiling $<
	@$(CC) -c $(CFLAGS) $(RELEASE_CFLAGS) -MMD -o $@ $<
	
$(RELEASE_ARTIFACT): $(OBJ_PATH) $(RELEASE_OBJS)
	@echo Building $@
	@$(CC) $(RELEASE_OBJS) $(LDFLAGS) $(RELEASE_LDFLAGS) -o $@

$(RELEASE_STATIC_LIB): $(OBJ_PATH) $(RELEASE_LIBOBJS)
	@echo Packing $@
	@$(AR) rcs $@ $(RELEASE_LIBOBJS)

.PHONY: release
release: $(RELEASE_ARTIFACT)

.PHONY: rlib
rlib: release_lib

.PHONY: release_lib
release_lib: $(RELEASE_STATIC_LIB)

##############################
# DEFAULT

$(OBJ_PATH):
	@mkdir $@

.PHONY: all
all: default

.PHONY: default
default: debug

.PHONY: lib
lib: debug_lib

.PHONY: clean
clean:
	@echo Cleaning up.
	@rm -rf $(DEBUG_OBJS) $(RELEASE_OBJS)
	@rm -rf $(DEBUG_LIBOBJS) $(RELEASE_LIBOBJS)
	@rm -rf $(DEBUG_STATIC_LIB) $(RELEASE_STATIC_LIB)
	@rm -rf $(DEBUG_ARTIFACT) $(RELEASE_ARTIFACT)
	@rm -rf $(DEBUG_DEPS) $(RELEASE_DEPS)
	@rm -rf $(OBJ_PATH)

.PHONY: style
style:
	@echo Styling all source and header files.
	@astyle -A2 -T -p -U -xe -k3 -W3 -j -n -S -Y -xC120  $(SRCS) $(wildcard *.h)

-include $(RELEASE_DEPS)
-include $(DEBUG_DEPS)
