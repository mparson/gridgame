lc = $(shell echo ${1} | tr '[:upper:]' '[:lower:]')
SHELL := bash
TARGET ?= cx16
CFLAGS = --add-source -Osir
NAME = GRIDGAME
# make sure this location exists before a 'make install'
PREFIX = x:GAMES/$(NAME)
ALL_OBJS = $(patsubst %.c,%.o,$(wildcard *.c))
X16 = /home/mparson/Applications/X16/x16emu
AS = ca65
CC = cc65
LD = ld65
CL = cl65
ifeq ($(TARGET),$(filter $(TARGET),c64 c128))
ALL_OBJS += joy.o
DISK = $(call lc,$(NAME).d64)
else
DISK = ~/basic-progs.img
endif
ALL_BOARDS = $(wildcard boards/*)

all: $(ALL_OBJS)
	$(CL) -t $(TARGET) -o $(NAME).PRG -m $(NAME).mmap $(ALL_OBJS)

ifeq ($(TARGET),$(filter $(TARGET),c64 c128))
joy.o:
	$(CC) $(CFLAGS) -t $(TARGET) -o joy.s c64/joy.c
	$(AS) -t $(TARGET) -o $@ joy.s
endif

%.o: %.c
	$(CC) $(CFLAGS) -t $(TARGET) -o $(patsubst %.o,%.s,$@) $<
	$(AS) -t $(TARGET) -o $@ $(patsubst %.o,%.s,$@)

install: all
ifeq ($(TARGET),$(filter $(TARGET),c64 c128))
	c1541 -attach $(DISK) -delete $(call lc,$(NAME))
	c1541 -attach $(DISK) -write $(NAME).PRG $(call lc,$(NAME))
	c1541 -attach $(DISK) -write HIGHSCORE $(call lc,HIGHSCORE)
else
	mcopy -Do -DO $(NAME).PRG $(PREFIX);
	mcopy -Do -DO HIGHSCORE $(PREFIX);
endif

install-boards: install
ifeq ($(TARGET),$(filter $(TARGET),c64 c128))
	for board in $(ALL_BOARDS); do \
		c1541 -attach $(DISK) -write $$board $$(basename $${board,,},s); \
	done;
else
	for board in $(ALL_BOARDS); do \
		mcopy -Do -DO $$board $(PREFIX); \
	done;
endif

run: install-boards
ifeq ($(TARGET),c64)
	x64sc $(DISK)
else ifeq ($(TARGET),c128)
	x128 $(DISK)
else
	$(X16) -scale 2 -sdcard $(DISK) -bas runner
endif

clean:
	rm -f $(NAME).PRG *.mmap *.o *.obj *.s

ifeq ($(TARGET),$(filter $(TARGET),c64 c128))
newdisk:
	c1541 -format "$(call lc,$(NAME)),64" d64 $(DISK)
endif
