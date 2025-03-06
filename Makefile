-include config.mk

PROJ_NAME=butterflies

BINARY_NAME ?= $(PROJ_NAME)

BUILD_MODE ?= RELEASE

PLATFORM = PLATFORM_DESKTOP
TARGET_PLATFORM = PLATFORM_DESKTOP_GLFW

RAYLIB_RELEASE_PATH  ?= $(RAYLIB_PATH)/src
RAYLIB_LIB_PATH ?= /usr/local/lib

SOURCE = src/butterflies.c

ROOT_DIR=$(shell pwd)
BUILD_DIR=$(ROOT_DIR)/build

RM      ?= rm
ECHO    ?= @echo
CP      ?= cp
MKDIR   ?= mkdir
SED     ?= sed

AS      ?= as
AR      ?= ar
GCC     ?= gcc
CC      ?= gcc
LD      ?= ld
OBJCOPY ?= objcopy
SIZE    ?= size
STRIP   ?= strip


CFLAGS = -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result
CFLAGS += -g -D_DEBUG

INCLUDE_PATHS = -I. -I$(RAYLIB_PATH)/src -I$(RAYLIB_PATH)/src/external

LDLIBS = -lraylib -lGL -lm -lpthread -ldl -lrt 
LDLIBS += -lX11

LDFLAGS = -L. -L$(RAYLIB_RELEASE_PATH) -L$(RAYLIB_PATH)/src
LDFLAGS += -L$(RAYLIB_LIB_PATH)


OBJ = 
OBJ += $(addprefix $(BUILD_DIR)/, $(SOURCE:.c=.o))


###################################################

all: $(ROOT_DIR)/$(BINARY_NAME)

vpath %.c . $(TOP)
$(BUILD_DIR)/%.o: %.c
	$(ECHO) "CC $<"
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -D$(PLATFORM) -D$(TARGET_PLATFORM) -c -o $@ $<

OBJ_DIRS = $(sort $(dir $(OBJ)))
$(OBJ): | $(OBJ_DIRS)
$(OBJ_DIRS):
	mkdir -p $@

print-cfg:
	$(ECHO) "OBJ              = $(OBJ)"
	$(ECHO) "BUILD_DIR        = $(BUILD_DIR)"
	$(ECHO) "INCLUDE_PATHS    = $(INCLUDE_PATHS)"
	$(ECHO) "CFLAGS           = $(CFLAGS)"
	$(ECHO) "DEFINES          = $(DEFINES)"
.PHONY: print-cfg


$(ROOT_DIR)/$(BINARY_NAME): $(OBJ)
	$(ECHO) "LINK $@"
	$(GCC) -o $@ $(OBJ) $(CFLAGS) $(LDFLAGS) $(LDLIBS)
	$(SIZE) $@

clean:
	$(RM) -rf $(BUILD_DIR)
	$(RM) -f $(ROOT_DIR)/$(BINARY_NAME)

.PHONY: all clean
