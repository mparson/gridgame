uc = $(shell echo ${1} | tr '[:lower:]' '[:upper:]')
SHELL := bash
TARGET ?= c64
CFLAGS = --add-source -Osir
CONTROL ?= MOUSEC
NAME = gridgame
# make sure this location exists before a 'make install'
PREFIX = x:GAMES/$(call uc,$(NAME))
ALL_OBJS = $(patsubst %.c,%.o,$(wildcard *.c))
X16 = /home/mparson/Applications/X16/x16emu
AS = /home/mparson/Applications/cc65/bin/ca65
CC = /home/mparson/Applications/cc65/bin/cc65
LD = /home/mparson/Applications/cc65/bin/ld65
CL = /home/mparson/Applications/cc65/bin/cl65

ifeq ($(CONTROL),$(filter $(CONTROL),JOYC))
ALL_OBJS += joy.o
GAMETYPE = joy
else
GAMETYPE = mouse
endif

ifeq ($(TARGET),$(filter $(TARGET),c64 c128))
DISK = $(NAME)-$(TARGET).d64
else
DISK = ~/basic-progs.img
endif

ALL_BOARDS = $(wildcard boards/*.gb)

all: $(ALL_OBJS)
	$(CL) -t $(TARGET) -o $(NAME) -m $(NAME).mmap $(ALL_OBJS)

ifeq ($(CONTROL),$(filter $(CONTROL),JOYC))
joy.o:
	$(CC) $(CFLAGS) -t $(TARGET) -o joy.s c64/joy.c
	$(AS) -t $(TARGET) -o $@ joy.s
endif

%.o: %.c
	$(CC) $(CFLAGS) -t $(TARGET) -D $(CONTROL) -o $(patsubst %.o,%.s,$@) $<
	$(AS) -t $(TARGET) -o $@ $(patsubst %.o,%.s,$@)

ifeq ($(TARGET),$(filter $(TARGET),c64 c128))
newdisk:
	c1541 -format "$(NAME)-$(TARGET),64" d64 $(DISK)
endif

install: all
ifeq ($(TARGET),$(filter $(TARGET),c64 c128))
	c1541 -attach $(DISK) -delete $(NAME)
	c1541 -attach $(DISK) -write $(NAME) $(NAME)
	c1541 -attach $(DISK) -delete highscore
	c1541 -attach $(DISK) -write highscore highscore,s
	for board in $(ALL_BOARDS); do \
		c1541 -attach $(DISK) -write $${board} $$(basename $${board%.gb},s); \
	done;
else
	mcopy -Do -DO $(NAME) $(PREFIX)/$(call uc,$(NAME)).PRG;
	mcopy -Do -DO highscore $(PREFIX)/HIGHSCORE;
	for board in $(ALL_BOARDS); do \
		xboard=$${board%.gb}; \
		mcopy -Do -DO $${board} $(PREFIX)/$$(basename $${xboard^^}); \
	done;
endif

run:
ifeq ($(TARGET),c64)
	x64sc $(DISK)
else ifeq ($(TARGET),c128)
	x128 $(DISK)
else
	$(X16) -scale 2 -sdcard $(DISK) -bas runner
endif

clean:
	rm -f $(NAME).PRG *.mmap *.o *.obj *.s

