-include config.mk

PROJ_NAME=butterflies

BINARY_NAME ?= $(PROJ_NAME).html

BUILD_MODE ?= RELEASE

PLATFORM = PLATFORM_WEB
TARGET_PLATFORM = PLATFORM_WEB

RAYLIB_RELEASE_PATH  ?= $(RAYLIB_PATH)/src
RAYLIB_LIB_PATH ?= /usr/local/lib

SOURCE = src/butterflies.c

ROOT_DIR=$(shell pwd)
BUILD_DIR=$(ROOT_DIR)/build_web

BUILD_WEB_ASYNCIFY    ?= FALSE
BUILD_WEB_SHELL       ?= $(ROOT_DIR)/src/shell.html
BUILD_WEB_HEAP_SIZE   ?= 134217728
BUILD_WEB_RESOURCES   ?= TRUE
BUILD_WEB_RESOURCES_PATH  ?= $(ROOT_DIR)/resources@resources


RM     ?= rm
ECHO   ?= @echo
CP     ?= cp
MKDIR  ?= mkdir
SED    ?= sed
CC     = emcc
GCC    = emcc

CFLAGS = -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result
ifeq ($(BUILD_MODE), DEBUG)
	CFLAGS += -g -D_DEBUG --profiling
else
	CFLAGS += -Os
endif

INCLUDE_PATHS = -I. -I$(RAYLIB_PATH)/src -I$(RAYLIB_PATH)/src/external

LDLIBS = $(RAYLIB_RELEASE_PATH)/libraylib.web.a

LDFLAGS = -L. -L$(RAYLIB_RELEASE_PATH) -L$(RAYLIB_PATH)/src
LDFLAGS += -sUSE_GLFW=3 -sTOTAL_MEMORY=$(BUILD_WEB_HEAP_SIZE) -sFORCE_FILESYSTEM=1
LDFLAGS += --preload-file $(BUILD_WEB_RESOURCES_PATH)
LDFLAGS += --shell-file $(BUILD_WEB_SHELL)
LDFLAGS += -sMIN_WEBGL_VERSION=2
LDFLAGS += -sMAX_WEBGL_VERSION=2

ifeq ($(BUILD_MODE), DEBUG)
	LDFLAGS += --profiling
endif

OBJ = 
OBJ += $(addprefix $(BUILD_DIR)/, $(SOURCE:.c=.o))


###################################################

all: $(BUILD_DIR)/$(BINARY_NAME)

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


$(BUILD_DIR)/$(BINARY_NAME): $(OBJ)
	$(ECHO) "LINK $@"
	$(CC) -o $@ $(OBJ) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -D$(PLATFORM)

index_html: $(BUILD_DIR)/$(BINARY_NAME)
	mv $(BUILD_DIR)/$(BINARY_NAME) $(BUILD_DIR)/index.html

zip_dist: index_html
	zip -j $(ROOT_DIR)/$(PROJ_NAME)_web.zip $(BUILD_DIR)/index.html $(BUILD_DIR)/$(PROJ_NAME).*

clean:
	$(RM) -rv $(BUILD_DIR)/*
	$(RM) $(ROOT_DIR)/$(PROJ_NAME)_web.zip

.PHONY: all clean
