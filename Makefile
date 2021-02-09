ALL_ASM = $(wildcard *.asm)
ALL_C = $(wildcard *.c) $(wildcard *.h)
# Run 'make TARGET=<target>', or set a TARGET env
# to build for another target system.
TARGET ?= cx16
CFLAGS = --add-source -Osir
AS = /home/mparson/Applications/cc65/bin/ca65
CC = /home/mparson/Applications/cc65/bin/cc65
LD = /home/mparson/Applications/cc65/bin/ld65
CL = /home/mparson/Applications/cc65/bin/cl65

ALL_OBJS = $(patsubst %.c,%.o,$(wildcard *.c)) $(patsubst %.asm,%.obj,$(wildcard *.asm))

all: $(ALL_OBJS)
	$(CL) -t $(TARGET) -o GRIDGAME -m gridgame.mmap $(ALL_OBJS)
	
%.o: %.c
	$(CC) $(CFLAGS) -t $(TARGET) -o $(patsubst %.o,%.s,$@) $<
	$(AS) -t $(TARGET) -o $@ $(patsubst %.o,%.s,$@)

%.obj: %.asm
	$(AS) -t $(TARGET) -o $@ $<

clean:
	rm -f GRIDGAME *.mmap *.o *.obj *.s
