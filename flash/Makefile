# Makefile for target

CC = ia16-elf-gcc

AS = ia16-elf-as

LD = ia16-elf-ld
LDFLAGS = -T ../raw.ld

EXE = flash.bin

OBJS= \
  flash.o \
  # end of list

.PHONY : all clean test

all: $(EXE)

$(EXE): $(OBJS)
	$(LD) $(LDFLAGS) -M -o $(EXE) $(OBJS) > flash.map

clean:
	rm -f $(EXE) $(OBJS) flash.map

test: test-emu

test-emu:
	emu86 -w 0xf0000 -f flash.bin -x 0xf000:0x0 &
	sleep 1
	pcat -t 1000 `cat emu86.pts` < ../test-flash-in.txt > test-emu-out.txt
	diff ../test-flash-ref.txt test-emu-out.txt
