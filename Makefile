# SYS86 project
# Global Makefile

CC = ia16-elf-gcc
# ES only for segment please!
CFLAGS = -Os -ffreestanding -mcmodel=tiny -mprotected-mode

AS = ia16-elf-as

LD = ia16-elf-ld
LDFLAGS = -T raw.ld

EXE = sys86.bin

OBJS= \
	entry.o \
	lib.o \
	arch.o \
	task.o \
	int-dev.o \
	int-arch.o \
	int-proc.o \
	timer.o \
	queue.o \
	serial.o \
	system.o \
	main.o \
	# end of list

.PHONY : all clean test

all: $(EXE)

$(EXE): $(OBJS)
	$(LD) $(LDFLAGS) -M -o $(EXE) $(OBJS) > sys86.map

clean:
	rm -f $(EXE) $(OBJS) sys86.map

test: test-emu

test-emu:
	echo "*** WARNING : requires local emu86-sys ***"
	../emu86/emu86 -w 0x10000 -f sys86.bin -x 0x1000:0x0 &
	sleep 1
	../emu86/pcat -t 1000 -d 10 `cat emu86.pts` < test-sys86-in.txt > test-emu-out.txt
	diff test-sys86-ref.txt test-emu-out.txt
