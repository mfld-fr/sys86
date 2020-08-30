# Makefile for target

CC = ia16-elf-gcc

AS = ia16-elf-as

LD = ia16-elf-ld
LDFLAGS = -T raw.ld

EXE = sys86.bin

OBJS= \
  int-hand.o \
  int-proc.o \
  # end of list

.PHONY : all clean test

all: $(EXE)

$(EXE): $(OBJS)
	$(LD) $(LDFLAGS) -M -o $(EXE) $(OBJS) > sys86.map

clean:
	rm -f $(EXE) $(OBJS) sys86.map

test: test-emu

test-emu:
	emu86 -w 0xf0000 -f sys86.bin -x 0xf000:0x0 &
	sleep 1
	pcat -t 1000 `cat emu86.pts` < ../test-sys86-in.txt > test-emu-out.txt
	diff ../test-sys86-ref.txt test-emu-out.txt
