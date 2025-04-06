ifeq (,$(CROSS_COMPILE))
$(error missing CROSS_COMPILE for this toolchain)
endif

CURRENT_WORKING_DIR = $(shell pwd)

PLATFORM ?= tg5040
LD_LIBRARY_PATH = $(CURRENT_WORKING_DIR)/platform/$(PLATFORM)/lib/
PREFIX = $(CURRENT_WORKING_DIR)/platform/$(PLATFORM)

-include minui/workspace/$(PLATFORM)/platform/makefile.env
SDL?=SDL

TARGET = emit-key
PRODUCT = $(TARGET)

INCDIR = -I.  -Iinclude/
SOURCE = $(TARGET).c

CC = $(CROSS_COMPILE)gcc
CFLAGS   = $(ARCH) -fomit-frame-pointer
CFLAGS  += $(INCDIR) -DPLATFORM=\"$(PLATFORM)\" -DUSE_$(SDL) -Ofast -std=gnu99
FLAGS = -L$(LD_LIBRARY_PATH) -ldl $(LIBS) -l$(SDL) -l$(SDL)_image -l$(SDL)_ttf -lpthread -lm -lz

all: minui
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) $(CC) $(SOURCE) -o $(PRODUCT)-$(PLATFORM) $(CFLAGS) $(FLAGS)

setup: minui

clean:
	rm -rf $(PRODUCT)-$(PLATFORM)

minui:
	git clone https://github.com/shauninman/MinUI minui
