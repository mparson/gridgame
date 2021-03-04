uc = $(shell echo ${1} | tr '[:lower:]' '[:upper:]')
SHELL := bash
TARGET ?= cx16
CFLAGS = --add-source -Osir
NAME = gridgame
# make sure this location exists before a 'make install'
PREFIX = x:GAMES/$(call uc,$(NAME))
ALL_OBJS = $(patsubst %.c,%.o,$(wildcard *.c))
X16 = /home/mparson/Applications/X16/x16emu
AS = ca65
CC = cc65
LD = ld65
CL = cl65
ifeq ($(TARGET),$(filter $(TARGET),c64 c128))
ALL_OBJS += joy.o
DISK = $(NAME)-$(TARGET).d64
else
DISK = ~/basic-progs.img
endif
ALL_BOARDS = $(wildcard boards/*.gb)

all: $(ALL_OBJS)
	$(CL) -t $(TARGET) -o $(NAME) -m $(NAME).mmap $(ALL_OBJS)

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
	c1541 -attach $(DISK) -delete $(NAME)
	c1541 -attach $(DISK) -write $(NAME) $(NAME)
	c1541 -attach $(DISK) -write highscore
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
	x64sc $(DISK)-$(TARGET)
else ifeq ($(TARGET),c128)
	x128 $(DISK)-$(TARGET)
else
	$(X16) -scale 2 -sdcard $(DISK) -bas runner
endif

clean:
	rm -f $(NAME).PRG *.mmap *.o *.obj *.s

ifeq ($(TARGET),$(filter $(TARGET),c64 c128))
newdisk:
	c1541 -format "$(NAME)-$(TARGET),64" d64 $(DISK)
endif
