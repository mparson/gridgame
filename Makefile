TARGET = cx16
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

all: $(ALL_OBJS)
	$(CL) -t $(TARGET) -o $(NAME).PRG -m $(NAME).mmap $(ALL_OBJS)
	
%.o: %.c
	$(CC) $(CFLAGS) -t $(TARGET) -o $(patsubst %.o,%.s,$@) $<
	$(AS) -t $(TARGET) -o $@ $(patsubst %.o,%.s,$@)

install: all
	mcopy -Do -DO $(NAME).PRG $(PREFIX)/
	
run: install
	$(X16) -scale 2 -sdcard ~/basic-progs.img -bas runner

clean:
	rm -f $(NAME).PRG *.mmap *.o *.obj *.s
