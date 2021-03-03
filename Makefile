TARGET ?= cx16
CFLAGS = --add-source -Osir
NAME = GRIDGAME
# make sure this location exists before a 'make install'
PREFIX = x:GAMES/$(NAME)
X16 = /home/mparson/Applications/X16/x16emu
AS = /home/mparson/Applications/cc65/bin/ca65
CC = /home/mparson/Applications/cc65/bin/cc65
LD = /home/mparson/Applications/cc65/bin/ld65
CL = /home/mparson/Applications/cc65/bin/cl65

ALL_OBJS = $(patsubst %.c,%.o,$(wildcard *.c)) 
ifeq ($(TARGET),c64)
ALL_OBJS += joy.o
endif

all: $(ALL_OBJS)
	$(CL) -t $(TARGET) -o $(NAME).PRG -m $(NAME).mmap $(ALL_OBJS)
	
ifeq ($(TARGET),c64)
joy.o:
	$(CC) $(CFLAGS) -t $(TARGET) -o joy.s c64/joy.c
	$(AS) -t $(TARGET) -o $@ joy.s
endif

%.o: %.c
	$(CC) $(CFLAGS) -t $(TARGET) -o $(patsubst %.o,%.s,$@) $<
	$(AS) -t $(TARGET) -o $@ $(patsubst %.o,%.s,$@)

install: all
ifeq ($(TARGET),c64)
	c1541 -attach c64/gridgame.d64 -delete gridgame
	c1541 -attach c64/gridgame.d64 -write $(NAME).PRG gridgame
else
	mcopy -Do -DO $(NAME).PRG $(PREFIX)/
endif
	
run: install
ifeq ($(TARGET),c64)
	x64sc c64/gridgame.d64
else
	$(X16) -scale 2 -sdcard ~/basic-progs.img -bas runner
endif

clean:
	rm -f $(NAME).PRG *.mmap *.o *.obj *.s

newdisk:
	c1541 -format "gridgame,64" d64 c64/gridgame.d64
